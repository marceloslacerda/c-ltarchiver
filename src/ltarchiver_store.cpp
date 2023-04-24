#include <cstddef>
#include <iostream>
#include <string>
#include <fstream>
#include <iomanip>

#include "ltarchiver.hpp"

int main(int argc, char *argv[])
{
   char * buffer = new char[ltarchiver::data_length];
   std::string ecc_code(ltarchiver::fec_length, '\0');

   if (argc != 4) {
      std::cerr << "usage ltarchiver_store <input_file> <output_file> <ecc_file>" << std::endl;
      return 1;
   }

   /* Initialize file streams */
   std::ifstream inputFile(argv[1], std::ios_base::binary);
   std::ofstream outputFile(argv[2], std::ios_base::binary);
   std::ofstream eccFile(argv[3], std::ios_base::binary);

   if(!ltarchiver::make_sequential_root_generator_polinomial()) {
      return 1;
   }

   /* Instantiate Encoder and Decoder (Codec) */
   const ltarchiver::encoder_t encoder(ltarchiver::field, ltarchiver::generator_polynomial);
   std::cout << "Starting encoding" << std::endl;

   /* Instantiate RS Block For Codec */
   schifra::reed_solomon::block<ltarchiver::code_length,ltarchiver::fec_length> block;
   bool end_of_file = false;
   double bytes_read = 0;
   double file_size = get_file_size(std::string(argv[1]));
   uint last_percent = 0;
   std::streamsize last_read = 0;

   while (!end_of_file) {
      inputFile.read(buffer, ltarchiver::data_length);
      last_read = inputFile.gcount();
      bytes_read += last_read;
      outputFile.write(buffer, last_read);
      if (!inputFile) {
         for(size_t i = last_read ; i < ltarchiver::data_length; i++) {
            buffer[i] = 0x0; // clear part of the buffer that was not read
         }
         end_of_file = true;
      }
      /* Transform message into Reed-Solomon encoded codeword */
      if (!encoder.encode(std::string(buffer, ltarchiver::data_length), block))
      {
         std::cerr << "Error - Critical encoding failure! "
                  << "Msg: " << block.error_as_string()  << std::endl;
         return 1;
      } else {
         // write block data into ecc
         block.fec_to_string(ecc_code);
         eccFile.write(ecc_code.data(), ltarchiver::fec_length);
         std::string buff_str(buffer, ltarchiver::data_length);
      }
      if((bytes_read / file_size)*100 > last_percent) {
         std::cout << "" << last_percent << "%" << std::endl;
         last_percent++;
      }
   }
   std::cout << "Success!" << std::endl;
   inputFile.close();
   outputFile.flush();
   eccFile.flush();
   outputFile.close();
   eccFile.close();
   delete[] buffer;
   return 0;
}
