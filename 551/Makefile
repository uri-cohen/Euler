
.PHONY: all clean

P := sum-of-digit-sequence
LIBS := -lboost_program_options

CPPFLAGS 	:= -std=c++11 -O6 -Wall -fPIC

all: $P
	./$P 
	
clean:
	rm -rf *.o $P

%: %.cpp
	g++ $(CPPFLAGS) -o $@ $< $(LIBS)


