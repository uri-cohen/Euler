
.PHONY: all clean

PROBLEM = 551-sum-of-digit-sequence

all:
	make -C $(PROBLEM) all

clean:
	make -C $(PROBLEM) clean
