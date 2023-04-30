#include <cstddef>
#include <iostream>
#include <string>
#include <fstream>
#include <iomanip>

#include "ltarchiver.hpp"

const uint chunks = 20; // for bufferized read/write
char * largeBuffer = new char[ltarchiver::data_length * chunks];
char * largeECCBuffer = new char[ltarchiver::fec_length * chunks];

/* Instantiate RS Block For Codec */
ltarchiver::block_t block;
int encode_chunk(const ltarchiver::encoder_t& encoder, size_t index) {
   char * buffer = &largeBuffer[index * ltarchiver::data_length];
   char * ecc = &largeECCBuffer[index * ltarchiver::fec_length];
   /* Transform message into Reed-Solomon encoded codeword */
   if (!encoder.encode(buffer, block))
   //if (!encoder.encode(std::string(buffer, ltarchiver::data_length), block))
   {
      std::cerr << "Error - Critical encoding failure! "
               << "Msg: " << block.error_as_string()  << std::endl;
      return 1;
   } else {
      // Write ecc back into the largeECCBuffer
      for(size_t i = 0; i<ltarchiver::fec_length; i++) {
         ecc[i] = static_cast<char>(block.data[ltarchiver::data_length + i]);
      }
      return 0;
   }
}

int main(int argc, char *argv[])
{

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
   std::cout << "Starting encoding" << std::endl;
   bool end_of_file = false;
   double bytes_read = 0;
   double file_size = get_file_size(std::string(argv[1]));
   uint last_percent = 0;
   std::streamsize last_read = 0;
   const ltarchiver::encoder_t encoder(ltarchiver::field, ltarchiver::generator_polynomial);

   while (!end_of_file) {
      inputFile.read(largeBuffer, ltarchiver::data_length * chunks);
      last_read = inputFile.gcount();
      bytes_read += last_read;
      uint timesToLoop = last_read / chunks;
      uint remainderBytes = last_read % ltarchiver::data_length;
      outputFile.write(largeBuffer, last_read);
      if (!inputFile) {
         for(size_t i = last_read ; i < ltarchiver::data_length * chunks; i++) {
            largeBuffer[i] = 0x0; // clear part of the buffer that was not read
         }
         end_of_file = true;
      }
      for(size_t i = 0; i <= timesToLoop; i++) {
         if(encode_chunk(encoder, i)) {
            return 1;
         }
      }
      if(remainderBytes > 0) {
         if(encode_chunk(encoder, timesToLoop)) {
            return 1;
         }
         timesToLoop++;
      }
      size_t bytes_to_write = ltarchiver::fec_length * timesToLoop;
      eccFile.write(largeECCBuffer, bytes_to_write);
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
   return 0;
}
