COMPILER         = zig c++
OPTIMIZATION_OPT = -O3
OPTIONS          =  -Isrc/schifra -Isrc/ -ansi -std=c++17 -pedantic-errors -Wall -Wextra -Wno-long-long $(OPTIMIZATION_OPT)
LINKER_OPTS      = -lstdc++ -lm -lstdc++fs

BUILD_LIST+=ltarchiver_store
BUILD_LIST+=ltarchiver_restore
BUILD_LIST+=ltarchiver_test

all: $(BUILD_LIST)

$(BUILD_LIST) : %: src/%.cpp
	$(COMPILER) $(OPTIONS) -o out/$@ src/$@.cpp $(LINKER_OPTS)

run_tests : clean all
	./schifra_reed_solomon_codec_validation
	./schifra_reed_solomon_speed_evaluation

strip_bin :
	@for f in $(BUILD_LIST); do if [ -f $$f ]; then strip -s $$f; echo $$f; fi done;

clean:
	rm -f core.* *.o *.bak *stackdump *~

.PHONY: all clean valgrind run_tests