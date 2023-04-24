COMPILER         = -c++
OPTIMIZATION_OPT = -O3
OPTIONS          =  -Isrc/schifra -Isrc/ -ansi -std=c++11 -pedantic-errors -Wall -Wextra -Wno-long-long $(OPTIMIZATION_OPT)
LINKER_OPTS      = -lstdc++ -lm

BUILD_LIST+=ltarchiver_store
BUILD_LIST+=ltarchiver_restore
BUILD_LIST+=ltarchiver_test


all: $(BUILD_LIST)

$(BUILD_LIST) : %: src/%.cpp
	$(COMPILER) $(OPTIONS) -o out/$@ src/$@.cpp $(LINKER_OPTS)

run_tests : clean all
	./schifra_reed_solomon_codec_validation
	./schifra_reed_solomon_speed_evaluation

schifra_reed_solomon_threads_example01: schifra_reed_solomon_threads_example01.cpp $(HPP_SRC)
	$(COMPILER) $(OPTIONS) -o schifra_reed_solomon_threads_example01 schifra_reed_solomon_threads_example01.cpp $(LINKER_OPTS) -pthread -lboost_thread -lboost_system

schifra_reed_solomon_threads_example02: schifra_reed_solomon_threads_example02.cpp $(HPP_SRC)
	$(COMPILER) $(OPTIONS) -o schifra_reed_solomon_threads_example02 schifra_reed_solomon_threads_example02.cpp $(LINKER_OPTS) -pthread -lboost_thread -lboost_system

strip_bin :
	@for f in $(BUILD_LIST); do if [ -f $$f ]; then strip -s $$f; echo $$f; fi done;

valgrind :
	@for f in $(BUILD_LIST); do \
		if [ -f $$f ]; then \
			cmd="valgrind --leak-check=full --show-reachable=yes --track-origins=yes --log-file=$$f.log -v ./$$f"; \
			echo $$cmd; \
			$$cmd; \
		fi done;

clean:
	rm -f core.* *.o *.bak *stackdump *~
