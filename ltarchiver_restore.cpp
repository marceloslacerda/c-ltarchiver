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

/*std::uintmax_t get_file_size(std::string path) {
   std::filesystem::path filepath(path);
   return std::filesystem::file_size(filepath);

}*/

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
   char * ecc_buffer = new char[fec_length];
   std::string buffer_str(data_length, '\0');
   std::string ecc_buffer_str(fec_length, '\0');


   if (argc != 5) {
      std::cerr << "usage ltarchiver_restore <input_file> <output_file> <ecc_file> <new_ecc_file>" << std::endl;
      return 1;
   }

   /* Initialize file streams */
   std::ifstream inputFile(argv[1], std::ios_base::binary);
   std::ofstream outputFile(argv[2], std::ios_base::binary);
   std::ifstream eccFile(argv[3], std::ios_base::binary);
   std::ofstream eccOutputFile(argv[4], std::ios_base::binary);
   //std::uintmax_t filesize = get_file_size(std::string(argv));

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
   typedef schifra::reed_solomon::decoder<code_length,fec_length,data_length> decoder_t;

   const decoder_t decoder(field, generator_polynomial_index);
   std::cout << "Starting decoding" << std::endl;

   /* Instantiate RS Block For Codec */
   schifra::reed_solomon::block<code_length,fec_length> block;
   bool end_of_file = false;
   std::uintmax_t bytes_read = 0;
   std::streamsize last_read = 0;
   while (!end_of_file) {
      block.reset();
      inputFile.read(buffer, data_length);
      last_read = inputFile.gcount();
      bytes_read += last_read;
      if (!inputFile) {
         for(size_t i = last_read ; i < data_length; i++) {
            buffer[i] = 0; // clear part of the buffer that was not read
         }
         end_of_file = true;
      }
      /* Read ecc */
      eccFile.read(ecc_buffer, fec_length);
      if (!eccFile && inputFile) {
         std::cerr << "Error reading the ecc file: Ecc file finished before the input file" << std::endl;
         return 1;
      }

      /* Transfer data to block*/
      for(size_t i =0; i<data_length; i++) {
         block.data[i] = static_cast<schifra::galois::field_symbol>(buffer[i])  & 0xFF;
      }
      for(size_t i =data_length; i<code_length; i++) {
         block.data[i] = static_cast<schifra::galois::field_symbol>(ecc_buffer[i - data_length])  & 0xFF;
      }

      /* Decode */
      if (!decoder.decode(block))
      {
         std::cerr << "Error - Critical decoding failure! "
                  << "Msg: " << block.error_as_string()  << std::endl;
         std::cerr << "Errors detected: " << block.errors_detected << std::endl;
         return 1;
      }

      /*Move the data from the block into the buffers*/

      block.data_to_string(buffer_str);
      block.fec_to_string(ecc_buffer_str);
      
      /* Write buffers to respective files */
      outputFile.write(buffer_str.data(), last_read);
      eccOutputFile.write(ecc_buffer_str.data(), fec_length);
   }
   
   std::cout << "Success!" << std::endl;
   inputFile.close();
   outputFile.flush();
   outputFile.close();
   eccOutputFile.flush();
   eccOutputFile.close();
   eccFile.close();
   delete[] buffer;
   return 0;
}