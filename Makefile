as-cal: as-cal.cc
	clang++ -std=c++20 -Wall -Wextra $< -o $@

.PHONY: run
run: as-cal example.local
	cat example.local | ./as-cal

example.local: ~/terminy
	ln $< $@
