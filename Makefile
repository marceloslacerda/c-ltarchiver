.PHONY: make_out

COMPILER         = gcc
OPTIMIZATION_OPT = -O3
OPTIONS          =  -Isrc/schifra -Isrc/ -ansi -std=c++17 -pedantic-errors -Wall -Wextra -Wno-long-long $(OPTIMIZATION_OPT)
LINKER_OPTS      = -lstdc++ -lm -lstdc++fs

all: out/ltarchiver_test out/ltarchiver_restore out/ltarchiver_store

make_out:
	mkdir -p out

out/ltarchiver_test: src/ltarchiver_test.cpp
	$(COMPILER) $(OPTIONS) -o out/ltarchiver_test src/ltarchiver_test.cpp $(LINKER_OPTS)

out/ltarchiver_restore: src/ltarchiver_restore.cpp
	$(COMPILER) $(OPTIONS) -o out/ltarchiver_restore src/ltarchiver_restore.cpp $(LINKER_OPTS)

out/ltarchiver_store: src/ltarchiver_store.cpp
	$(COMPILER) $(OPTIONS) -o out/ltarchiver_store src/ltarchiver_store.cpp $(LINKER_OPTS)

clean:
	rm -f out/*
