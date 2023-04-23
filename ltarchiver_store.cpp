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

void print_char_array(const char* array, int size) {
   for(int i=0; i< size; i++) {
      std::cout << std::setfill('0') << std::setw(2) << std::hex << (int)array[i] << " ";
   }
   std::cout << std::endl;
}

int main(int argc, char *argv[])
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
   std::string ecc_code(fec_length, '\0');

   if (argc != 4) {
      std::cerr << "usage ltarchiver_store <input_file> <output_file> <ecc_file>" << std::endl;
      return 1;
   }

   /* Initialize file streams */
   std::ifstream inputFile(argv[1], std::ios_base::binary);
   std::ofstream outputFile(argv[2], std::ios_base::binary);
   std::ofstream eccFile(argv[3], std::ios_base::binary);

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
   typedef schifra::reed_solomon::encoder<code_length,fec_length,data_length> encoder_t;
   //typedef schifra::reed_solomon::decoder<code_length,fec_length,data_length> decoder_t;

   const encoder_t encoder(field, generator_polynomial);
   std::cout << "Starting encoding" << std::endl;

   /* Instantiate RS Block For Codec */
   schifra::reed_solomon::block<code_length,fec_length> block;
   bool end_of_file = false;
   while (!end_of_file) {
      inputFile.read(buffer, data_length);
      outputFile.write(buffer, inputFile.gcount());
      if (!inputFile) {
         for(size_t i = inputFile.gcount() ; i < data_length; i++) {
            buffer[i] = 0; // clear part of the buffer that was not read
         }
         end_of_file = true;
      }
      //std::cout << "Buffer contents before encoding " << buffer << std::endl;
      /* Transform message into Reed-Solomon encoded codeword */
      if (!encoder.encode(std::string(buffer, data_length), block))
      {
         std::cerr << "Error - Critical encoding failure! "
                  << "Msg: " << block.error_as_string()  << std::endl;
         return 1;
      } else {
         // write block data into ecc
         block.fec_to_string(ecc_code);
         /*std::cout << "[";
         for(size_t i = 0; i< fec_length; i++) {
            std::cout << (static_cast<int>(ecc_code[i]) & 0xFF) << ", ";
         }
         std::cout << "]" << std::endl;*/
         eccFile.write(ecc_code.data(), fec_length);
         std::string buff_str(buffer, data_length);
         //schifra::reed_solomon::block<code_length,fec_length> block_tmp(buff_str, ecc_code);
         /*const decoder_t decoder(field, generator_polynomial_index);
         if (!decoder.decode(block))
         {
            std::cerr << "Error - Critical encoding failure! "
                  << "Msg: " << block.error_as_string()  << std::endl;
            return 1;
         } else {
            block.data_to_string(buff_str);
            std::cout << "Restored message " << buff_str << std::endl;
            std::cout << "Block contents " << block << std::endl;
         }*/
         //print_char_array(buffer, data_length);
         //print_char_array(ecc_code.data(), fec_length);
      }
   }
   /*std::cout << "Encoder Parameters [" << encoder_t::trait::code_length << ","
                                       << encoder_t::trait::data_length << ","
                                       << encoder_t::trait::fec_length  << "]" << std::endl;*/

   std::cout << "Success!" << std::endl;
   inputFile.close();
   outputFile.flush();
   eccFile.flush();
   outputFile.close();
   eccFile.close();
   delete[] buffer;
   return 0;
}
