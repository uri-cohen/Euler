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
// * use the following database:
//     (#zeros, prefixSumOfDigits, suffix) -> (#steps, increment)
// * use the following state tracking:
//     vector[#zeros] -> (prefixSumOfDigits, baseStep, baseValue)

#include <stdlib.h>

#include <iostream>
#include <map>
#include <vector>

using namespace std;

typedef long long int Number;

// suffix is obtained by n%commonSuffixSize
// note that a 3 digits suffix (1000) covers for a[] values of 109 digits...
const Number commonSuffixSize = 10000;

struct Shortcut
{
public:
	Shortcut(void) : steps(0), delta(0) {}
	Shortcut(Number s, Number d) : steps(s), delta(d) {}
	Shortcut(const Shortcut& o) : steps(o.steps), delta(o.delta) {}
	virtual ~Shortcut(void) {}

	Shortcut& operator=(const Shortcut& o) {
		steps = o.steps;
		delta = o.delta;
		return *this;
	}
public:
	Number steps;
	Number delta;
};

struct DatabaseKey
{
	DatabaseKey(void) : nZeros(0), prefix(0), suffix(0) {}
	DatabaseKey(int z, int p, int s) : nZeros(z), prefix(p), suffix(s) {}
	DatabaseKey(const DatabaseKey& o) : nZeros(o.nZeros), prefix(o.prefix), suffix(o.suffix) {}
	virtual ~DatabaseKey(void) {}

	DatabaseKey& operator=(const DatabaseKey& o) {
		nZeros = o.nZeros;
		prefix = o.prefix;
		suffix = o.suffix;
		return *this;
	}

	bool operator==(const DatabaseKey& o) const {
		return (nZeros == o.nZeros) && (prefix == o.prefix) && (suffix == o.suffix);
	}
	bool operator<(const DatabaseKey& o) const {
		if (nZeros < o.nZeros) return true;
		if (nZeros == o.nZeros) {
			if (prefix < o.prefix) return true;
			return (prefix == o.prefix) && (suffix < o.suffix);
		}
		return false;
	}
public:
	int nZeros;
	int prefix;			// sum of digits
	int suffix;
};

struct BaseState
{
public:
	BaseState(void) : prefix(0), step(0), value(0) {}
	BaseState(int p, Number s, Number i, Number v) : prefix(p), suffix(s), step(i), value(v) {}
	BaseState(const BaseState& o) : prefix(o.prefix), suffix(o.suffix), step(o.step), value(o.value) {}
	virtual ~BaseState(void) {}

	BaseState& operator=(const BaseState& o) {
		prefix = o.prefix;
		suffix = o.suffix;
		step = o.step;
		value = o.value;
		return *this;
	}
public:
	int prefix;
	int suffix;
	Number step;
	Number value;
};

void
initSffixSumOfDigits(vector<Number>& suffixSumOfDigits)
{
	for(auto i=0; i<commonSuffixSize; ++i) {
		int d = i;
		int s = 0;
		while (d) {
			s += d%10;
			d /= 10;
		}
		suffixSumOfDigits[i] = s;
	}
}

Number
sumOfDigitsSequence(Number n)
{
	map<DatabaseKey, Shortcut> shortcuts;
	vector<BaseState> state;

	// cache suffix to sum of digits
	vector<Number> suffixSumOfDigits(commonSuffixSize);
	initSffixSumOfDigits(suffixSumOfDigits);

	Number a = 1;				// 'a' holds the current elem value. initial value a[0]=1
	int prevPrefix = 0;
	int prevNZeros = 0;

	for(Number i=1; i<n; ++i) {
		// calculate prefix, nZeros and suffix
		int suffix = a%commonSuffixSize;
		Number b = a/commonSuffixSize;
		int nZeros = 0;
		while(b && !(b%10)) {
			++nZeros;
			b /= 10;
		}
		int prefix = 0;
		while (b) {
			prefix += (b%10);
			b /= 10;
		}

		if (prefix == prevPrefix) {
			a += prefix + suffixSumOfDigits[suffix];
			continue;
		}

		// cout << ":: i=" << i << " a=" << a << " prefix=" << prefix << " suffix=" << suffix << " nZero=" << nZeros << endl;

		prevPrefix = prefix;
		// save for post shortcut usage in state update
		Number i0 = i;
		Number a0 = a;

		// we entered a new suffix cycle
		// check if we have a shortcut for this and update the state
		bool shortcutTaken = false;
 		for (DatabaseKey k(nZeros, prefix, suffix); k.nZeros>=0; --k.nZeros) {
			map<DatabaseKey, Shortcut>::const_iterator it = shortcuts.find(k);
			if (it == shortcuts.end()) {
				continue;
			}
			const Shortcut& sc = it->second;
			if (i+sc.steps < n) {
//				cout << "take a shortcut at step " << i << ", " << a
//						<< " : [" << k.nZeros << ", " << prefix << ", " << suffix << "] --> (" <<
//						(i + sc.steps) << ", " << (a + sc.delta) << ")" << endl;
				a += sc.delta;
				i += sc.steps;
				shortcutTaken = true;
				break;
			}
		}
 		if (!shortcutTaken) {
			a += prefix + suffixSumOfDigits[suffix];
		}

 		while (state.size() <= nZeros) {
 			state.push_back(BaseState());
 		}
 		for (DatabaseKey k(nZeros, prefix, suffix); k.nZeros>=0; --k.nZeros) {
 			k.prefix = state[k.nZeros].prefix;
			map<DatabaseKey, Shortcut>::const_iterator it = shortcuts.find(k);
			if (k.prefix && it == shortcuts.end()) {
//				cout << "add shortcut at step " << i <<
//						" : [" << k.nZeros << ", " << k.prefix << ", " << k.suffix << "] --> (steps=" <<
//						(i0-state[k.nZeros].step) << ", delta=" << (a0-state[k.nZeros].value) << ")" << endl;
				shortcuts[k] = Shortcut(i0-state[k.nZeros].step, a0-state[k.nZeros].value);
			}
			state[k.nZeros] = BaseState(prefix, suffix, i0, a0);
 		}
	}

	return a;
}

int
main(int argc, char* argv[])
{
	Number n = 200;
	if (argc > 1) {
		n = atoll(argv[1]);
	}
	cout << "sumOfDigitsSequence(" << n << ") = " <<  sumOfDigitsSequence(n) << endl;
	return 0;
}
