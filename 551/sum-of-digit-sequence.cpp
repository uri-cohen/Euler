//
// Project Euler problem 551
//
// Let a0, a1, a2, ... be an integer sequence defined by:
//
// a[0] = 1;
// for n ≥ 1, an is the sum of the digits of all preceding terms.
// The sequence starts with 1, 1, 2, 4, 8, 16, 23, 28, 38, 49, ...
// You are given a[10^6] = 31054319.
//
// Find a[10^15].
//
// Uri Cohen, uri.l.cohen@gmail.com
//
// Solution outline:
// * note that a[n+1] = a[n]+ sumOfDigits(a[n])
// * observation: if a[n1] and a[n2] are of the form
//     <prefix1>0*<common-suffix> and <prefix2>0*<common-suffix>
//   and if sumOfDigits(<prefix1>) == sumOfDigits(<prefix2>
//   then the increment in a[ni...] is the same in both subsequences as long as
//   the prefix is unchanged.  this allows to calculate and save per prefix increments
//   and reuse it later (for different values) once the <common-suffix> is re-encountered.
//   the longer the 0* sequence is the higher the gain is.

#include <stdlib.h>

#include <iostream>
#include <map>
#include <vector>
using namespace std;

#include <boost/program_options.hpp>
namespace po = boost::program_options;

typedef long long int Number;

class ShortcutDB
{
public:
	struct Data
	{
		Data(void) : steps(0), delta(0) {}
		Data(Number s, Number d) : steps(s), delta(d) {}
		Data(const Data& o) : steps(o.steps), delta(o.delta) {}
		virtual ~Data(void) {}

		Data& operator=(const Data& o) {
			steps = o.steps;
			delta = o.delta;
			return *this;
		}

		Number steps;
		Number delta;
	};

public:
	ShortcutDB(unsigned int suffixSize, unsigned int nOutputDigits);
	virtual ~ShortcutDB(void) {}

	const Data& get(unsigned int zeros, unsigned int prefixSoD, unsigned int suffix) const;
	void set(unsigned int zeros, unsigned int prefixSoD, unsigned int suffix, const Data& data);

private:
	vector<Data>*		_db;
	unsigned int		_nZeros;
	unsigned int 		_nPrefixes;
	unsigned int 		_nSuffixes;
	Number				_zeroSize;

	static Data			_nullData;
};

ShortcutDB::Data ShortcutDB::_nullData(0,0);


ShortcutDB::ShortcutDB(unsigned int suffixSize, unsigned int nOutputDigits)
{
	_nZeros = nOutputDigits - suffixSize;
	_nPrefixes = 9 * _nZeros;
	_nSuffixes = 9 * nOutputDigits;
	_db = new vector<Data>(_nZeros*_nPrefixes*_nSuffixes);
	if (!_db) {
		cout << "failed to allocate db(" << (_nZeros*_nPrefixes*_nSuffixes) << "=" <<
				_nZeros << "*" << _nPrefixes << "*" << _nSuffixes << ")" << endl;
		exit(1);
	}
	_zeroSize = _nPrefixes * _nSuffixes;
}

const ShortcutDB::Data&
ShortcutDB::get(unsigned int zeros, unsigned int prefixSoD, unsigned int suffix) const
{
	if (!prefixSoD) {
		return _nullData;
	}
	if (!(zeros<_nZeros && prefixSoD<_nPrefixes && suffix<_nSuffixes)) {
		return _nullData;
	}
	Number idx = zeros*_zeroSize + prefixSoD*_nSuffixes + suffix;
	return (*_db)[idx];
}

void
ShortcutDB::set(unsigned int zeros, unsigned int prefixSoD, unsigned int suffix, const ShortcutDB::Data& data)
{
	if (!(zeros<_nZeros && prefixSoD<_nPrefixes && suffix<_nSuffixes)) {
		cout << "ShortcutDB::set failed on: " <<
				zeros << "<" << _nZeros << " && " <<
				prefixSoD << "<" << _nPrefixes << " && " <<
				suffix << "<" << _nSuffixes << endl;
		exit(1);
	}
	Number idx = zeros*_zeroSize + prefixSoD*_nSuffixes + suffix;
	(*_db)[idx] = data;
}

///////////////////////////////////////////////////////////////////////////////

class SumOfDigitsSequence
{
public:
	// Base is used to capture the counting state upon entering a new suffix cycle.
	// On last change in such a cycle this info is used to add a shortcut
	struct Base
	{
		Base() : prefixSoD(0), suffix(0), step(0), value(0) {}
		Base(int p, int s, Number i, Number v) : prefixSoD(p), suffix(s), step(i), value(v) {}
		virtual ~Base(void) {}
		Base& operator=(const Base& o) {
			prefixSoD = o.prefixSoD;
			suffix = o.suffix;
			step = o.step;
			value = o.value;
			return *this;
		}

		int prefixSoD;
		int suffix;
		Number step;
		Number value;
	};

public:
	SumOfDigitsSequence(unsigned int nSuffixDigits, unsigned int nOutputDigits);
	virtual ~SumOfDigitsSequence(void) {}

	Number calc(Number n);

private:
	void initCalc(Number _n);
	bool checkAndUpdateShortcutDB(void);
	bool checkAndDoShortcut(void);

private:
	static Number sumOfDigits(Number n) {
		Number sod = 0;
		for(; n; n/=10) {
			sod += (n%10);
		}
		return sod;
	}

	static int numberOfZeros(Number n) {
		int z = 0;
		for(; !(n%10); n/=10) {
			++z;
		}
		return z;
	}

private:
	Number			_a;		// _a is the sequence value at _i
	Number			_i;
	Number 			_n;

	Number			_prefix;
	unsigned int	_prefixSoD;
	unsigned int	_nZeros;
	unsigned int	_suffix;

	ShortcutDB 		_shortcuts;

	unsigned int 	_suffixSize;
	vector<int>  	_suffixToSoD;
	vector<Base> 	_state;

	Number			_nJumps;
	Number			_nIncr;
};

SumOfDigitsSequence::SumOfDigitsSequence(unsigned int nSuffixDigits, unsigned int nOutputDigits)
	: _i(0),
	  _prefix(0), _prefixSoD(0), _nZeros(0), _suffix(1),
	  _shortcuts(nSuffixDigits, nOutputDigits)
{
	_suffixSize = 1;
	while(nSuffixDigits--) {
		_suffixSize *= 10;
	}
	_suffixToSoD.resize(_suffixSize);
	for(unsigned int i=0; i<_suffixSize; ++i) {
		_suffixToSoD[i] = sumOfDigits(i);
	}
}

// initialized also in constructor
// but allows recalcing
void
SumOfDigitsSequence::initCalc(Number n)
{
	_a = 1;
	_i = 1;
	_n = n;
	_prefix = _prefixSoD = _nZeros = 0;
	_suffix = 1;
	_nJumps = _nIncr = 0;
}

// check whether the next [+1] step changes the prefix.
// is so then update the required shortcut entry.
// assumes _a, _i, prefix, _prefixSoD, _nZeros, _suffix are all consistent
// returns true iff next increment changes prefix
bool
SumOfDigitsSequence::checkAndUpdateShortcutDB()
{
	Number a1 = _a + _prefixSoD + _suffixToSoD[_suffix];
	Number prefix = a1 / _suffixSize;
	if (prefix == _prefix || !_prefix) {
		return prefix != _prefix;
	}
	int z = numberOfZeros(prefix);
	if (z >= int(_state.size())) {
		z = int(_state.size()-1);
	}

	for(; z>=0; --z) {
		Base& b = _state[z];
		if (!b.step) {
			// this means this level and lower were invalidated by a shortcut jump
			break;
		}

		_shortcuts.set(z, b.prefixSoD, b.suffix, ShortcutDB::Data(_i-b.step, _a-b.value));
		b.step = 0;
	}
	return true;
}

bool
SumOfDigitsSequence::checkAndDoShortcut(void)
{
	bool hasShortcut = false;
	for(int z=_nZeros; z>=0; --z) {
		const ShortcutDB::Data& d = _shortcuts.get(z, _prefixSoD, _suffix);
		if (hasShortcut) {
			_state[z].step = 0;
			continue;
		}
		if (d.steps && (_i + d.steps) <= _n) {
			_i += d.steps;
			_a += d.delta;
			_state[z].step = 0;
			hasShortcut = true;		// note prefix changes if nzeros>0
		}
	}
	return hasShortcut;
}

Number
SumOfDigitsSequence::calc(Number n)
{
	initCalc(n);
	while(_i < n) {
		bool expectPrefixChange = checkAndUpdateShortcutDB();
		bool hasShortcut = checkAndDoShortcut();

		if (!hasShortcut) {
			_a += _prefixSoD + _suffixToSoD[_suffix];
			++_i;
			++_nIncr;
		} else {
			++_nJumps;
		}

		_suffix = _a % _suffixSize;
		if (expectPrefixChange || hasShortcut) {
			_prefix = _a / _suffixSize;
			_prefixSoD = sumOfDigits(_prefix);
			_nZeros = numberOfZeros(_prefix);
		}

		if (expectPrefixChange) {
			if (_state.size() <= _nZeros) {
				_state.resize(_nZeros+1);
			}
			for(unsigned int z=0; z<=_nZeros; ++z) {
				_state[z] = Base(_prefixSoD, _suffix, _i, _a);
			}
		}
	}
	cout << "nIncr = " << _nIncr << ";   nJumps = " << _nJumps << endl;
	return _a;
}

///////////////////////////////////////////////////////////////////////////////


int
main(int argc, char* argv[])
{
	Number n;
	unsigned int suffixDigits;
	unsigned int outputDigits;

	po::options_description desc("Allowed options");
	desc.add_options()
	    ("help", "produce help message")
	    ("n", po::value<Number>(&n)->default_value(1000000), "")
		("suffix-digits", po::value<unsigned int>(&suffixDigits)->default_value(3), "number of digits in common suffix")
		("output-digits", po::value<unsigned int>(&outputDigits)->default_value(24), "number of digits in output number");

	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);
	po::notify(vm);

	SumOfDigitsSequence s(suffixDigits, outputDigits);
	cout << "SumOfDigitSequence(" << n << ") = " << s.calc(n) << endl;

	return 0;
}
