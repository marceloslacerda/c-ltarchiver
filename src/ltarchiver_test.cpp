#include <cstddef>
#include <iostream>
#include <string>
#include <fstream>
#include <iomanip>

#include "new_new_header.hpp"

int main()
{
    LTAVariables<8, 120, 32> variables;
    char * buffer = new char[variables.data_length];
    std::string ecc_code(variables.fec_length, 0x00);
    std::string buffer_str(variables.data_length, 0x00);
    std::string ecc_buffer_str(variables.fec_length, 0x00);
    std:: string message = "hello world";
    int test_data[223] = {104, 101, 108, 108, 111, 32, 119, 111, 114, 108, 100, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    int test_ecc[32] = {125, -52, 127, -77, -35, -51, -114, 107, -44, 3, -70, 107, -123, 99, 39, 76, -52, 85, 22, -20, 109, -30, 67, -42, 3, -100, 102, 114, -53, -111, -20, 2};
    std::cout << "Starting encoding" << std::endl;

    /* Transform message into Reed-Solomon encoded codeword */
    if (!variables.encoder->encode(std::string(buffer, variables.data_length), variables.block))
    {
        std::cerr << "Error - Critical encoding failure! "
                << "Msg: " << variables.block.error_as_string()  << std::endl;
        return 1;
    } else {
        variables.block.fec_to_string(ecc_code);
    }

    variables.block.reset();
    std::cout << "[";
    for(size_t i=0; i<variables.data_length; i++) {
        variables.block.data[i] = test_data[i];
        std::cout << variables.block.data[i] << ", ";
    }
    std::cout << "]" << std::endl;
    std::cout << "[";
    for(size_t i =variables.data_length; i<variables.code_length; i++) {
        variables.block.data[i] = test_ecc[i - variables.data_length];
        std::cout << variables.block.data[i] << ", ";
    }
    std::cout << "]" << std::endl;
    if (!(variables.decoder->decode(variables.block)))
    {
        std::cerr << "Error - Critical decoding failure! "
                  << "Msg: " << variables.block.error_as_string()  << std::endl;
        std::cerr << "Errors detected: " << variables.block.errors_detected << std::endl;
        return 1;
    }

    /*Move the data from the block into the buffers*/

    variables.block.data_to_string(buffer_str);
    variables.block.fec_to_string(ecc_buffer_str);
    std::cout << "Read from the block structure after decoding '" << buffer_str << "'" << std::endl;
    std::cout << "Success!" << std::endl;
    delete[] buffer;
    return 0;
}
