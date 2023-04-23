#
# **************************************************************************
# *                                                                        *
# *                                Schifra                                 *
# *                Reed-Solomon Error Correcting Code Library              *
# *                                                                        *
# * Release Version 0.0.1                                                  *
# * http://www.schifra.com                                                 *
# * Copyright (c) 2000-2020 Arash Partow, All Rights Reserved.             *
# *                                                                        *
# * The Schifra Reed-Solomon error correcting code library and all its     *
# * components are supplied under the terms of the General Schifra License *
# * agreement. The contents of the Schifra Reed-Solomon error correcting   *
# * code library and all its components may not be copied or disclosed     *
# * except in accordance with the terms of that agreement.                 *
# *                                                                        *
# * URL: http://www.schifra.com/license.html                               *
# *                                                                        *
# **************************************************************************
#


COMPILER         = -c++
OPTIMIZATION_OPT = -O3
OPTIONS          = -ansi -std=c++11 -pedantic-errors -Wall -Wextra -Wno-long-long $(OPTIMIZATION_OPT)
LINKER_OPTS      = -lstdc++ -lm


HPP_SRC+=schifra_ecc_traits.hpp
HPP_SRC+=schifra_error_processes.hpp
HPP_SRC+=schifra_galois_field.hpp
HPP_SRC+=schifra_galois_field_element.hpp
HPP_SRC+=schifra_galois_field_polynomial.hpp
HPP_SRC+=schifra_reed_solomon_block.hpp
HPP_SRC+=schifra_reed_solomon_codec_validator.hpp
HPP_SRC+=schifra_reed_solomon_decoder.hpp
HPP_SRC+=schifra_reed_solomon_encoder.hpp
HPP_SRC+=schifra_reed_solomon_file_decoder.hpp
HPP_SRC+=schifra_reed_solomon_file_encoder.hpp
HPP_SRC+=schifra_reed_solomon_product_code.hpp
HPP_SRC+=schifra_reed_solomon_speed_evaluator.hpp
HPP_SRC+=schifra_sequential_root_generator_polynomial_creator.hpp
HPP_SRC+=ltarchiver.hpp

BUILD_LIST+=ltarchiver_store
BUILD_LIST+=ltarchiver_restore
BUILD_LIST+=ltarchiver_test


all: $(BUILD_LIST)

$(BUILD_LIST) : %: %.cpp $(HPP_SRC)
	$(COMPILER) $(OPTIONS) -o $@ $@.cpp $(LINKER_OPTS)

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
