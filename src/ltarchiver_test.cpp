#include <cstddef>
#include <iostream>
#include <string>
#include <fstream>
#include <iomanip>

#include "schifra_galois_field.hpp"
#include "schifra_galois_field_polynomial.hpp"
#include "schifra_sequential_root_generator_polynomial_creator.hpp"
#include "schifra_reed_solomon_encoder.hpp"
#include "schifra_reed_solomon_decoder.hpp"
#include "schifra_reed_solomon_block.hpp"
#include "schifra_error_processes.hpp"

int main()
{
   /* Finite Field Parameters */
   const std::size_t field_descriptor                =   8;
   const std::size_t generator_polynomial_index      = 120;
   const std::size_t generator_polynomial_root_count =  32;

   /* Reed Solomon Code Parameters */
   const std::size_t code_length = 255;
   const std::size_t fec_length  =  32;
   const std::size_t data_length = code_length - fec_length;
   char * buffer = new char[data_length];
   std::string buffer_str(data_length, '\0');
   std::string ecc_buffer_str(fec_length, '\0');
   std:: string message = "hello world";
   int test_data[223] = {104, 101, 108, 108, 111, 32, 119, 111, 114, 108, 100, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
   int test_ecc[32] = {125, -52, 127, -77, -35, -51, -114, 107, -44, 3, -70, 107, -123, 99, 39, 76, -52, 85, 22, -20, 109, -30, 67, -42, 3, -100, 102, 114, -53, -111, -20, 2};

   
   /* Instantiate Finite Field and Generator Polynomials */
   const schifra::galois::field field(field_descriptor,
                                      schifra::galois::primitive_polynomial_size06,
                                      schifra::galois::primitive_polynomial06);

   schifra::galois::field_polynomial generator_polynomial(field);

   if (
        !schifra::make_sequential_root_generator_polynomial(field,
                                                            generator_polynomial_index,
                                                            generator_polynomial_root_count,
                                                            generator_polynomial)
      )
    {
       std::cerr << "Error - Failed to create sequential root generator!" << std::endl;
       return 1;
    }

    /* Instantiate Encoder and Decoder (Codec) */
    //typedef schifra::reed_solomon::encoder<code_length,fec_length,data_length> encoder_t;
    typedef schifra::reed_solomon::decoder<code_length,fec_length,data_length> decoder_t;
    const decoder_t decoder(field, generator_polynomial_index);
    //const encoder_t encoder(field, generator_polynomial);
    //std::cout << "Starting encoding" << std::endl;

    /* Instantiate RS Block For Codec */
    //schifra::reed_solomon::block<code_length,fec_length> block;
      /* Transform message into Reed-Solomon encoded codeword */
    /*if (!encoder.encode(std::string(buffer, data_length), block))
    {
        std::cerr << "Error - Critical encoding failure! "
                << "Msg: " << block.error_as_string()  << std::endl;
        return 1;
    } else {
        block.fec_to_string(ecc_code);
    }*/

    schifra::reed_solomon::block<code_length,fec_length> block2;
    block2.reset();
    std::cout << "[";
    for(size_t i=0; i<data_length; i++) {
        block2.data[i] = test_data[i];
        std::cout << block2.data[i] << ", ";
    }
    std::cout << "]" << std::endl;
    std::cout << "[";
    for(size_t i =data_length; i<code_length; i++) {
        block2.data[i] = test_ecc[i - data_length];
        std::cout << block2.data[i] << ", ";
    }
    std::cout << "]" << std::endl;
    if (!decoder.decode(block2))
    {
        std::cerr << "Error - Critical decoding failure! "
                  << "Msg: " << block2.error_as_string()  << std::endl;
        std::cerr << "Errors detected: " << block2.errors_detected << std::endl;
        return 1;
    }

    /*Move the data from the block into the buffers*/

    block2.data_to_string(buffer_str);
    block2.fec_to_string(ecc_buffer_str);
    std::cout << "Read from the block structure after decoding '" << buffer_str << "'" << std::endl;
    std::cout << "Success!" << std::endl;
    delete[] buffer;
    return 0;
}
