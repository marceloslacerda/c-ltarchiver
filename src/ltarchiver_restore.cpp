#include <cstddef>
#include <iostream>
#include <string>
#include <fstream>
#include <iomanip>

#include "ltarchiver.hpp"

int main(int argc, char *argv[])
{
   char * buffer = new char[ltarchiver::data_length];
   char * ecc_buffer = new char[ltarchiver::fec_length];
   std::string buffer_str(ltarchiver::data_length, '\0');
   std::string ecc_buffer_str(ltarchiver::fec_length, '\0');
   size_t errors_detected = 0;
   size_t errors_corrected = 0;


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

   if(!ltarchiver::make_sequential_root_generator_polinomial()) {
      return 1;
   }

   /* Instantiate Encoder and Decoder (Codec) */
   const ltarchiver::decoder_t decoder(ltarchiver::field, ltarchiver::generator_polynomial_index);
   std::cout << "Starting decoding" << std::endl;

   /* Instantiate RS Block For Codec */
   schifra::reed_solomon::block<ltarchiver::code_length, ltarchiver::fec_length> block;
   bool end_of_file = false;
   double bytes_read = 0;
   double file_size = get_file_size(std::string(argv[1]));
   uint last_percent = 0;
   std::streamsize last_read = 0;
   while (!end_of_file) {
      block.reset();
      inputFile.read(buffer, ltarchiver::data_length);
      last_read = inputFile.gcount();
      bytes_read += last_read;
      if (!inputFile) {
         for(size_t i = last_read ; i < ltarchiver::data_length; i++) {
            buffer[i] = 0x0; // clear part of the buffer that was not read
         }
         end_of_file = true;
      }
      /* Read ecc */
      eccFile.read(ecc_buffer, ltarchiver::fec_length);
      if (!eccFile && inputFile) {
         std::cerr << "Error reading the ecc file: Ecc file finished before the input file" << std::endl;
         return 1;
      }

      /* Transfer data to block*/
      for(size_t i =0; i<ltarchiver::data_length; i++) {
         block.data[i] = static_cast<schifra::galois::field_symbol>(buffer[i])  & 0xFF;
      }
      for(size_t i = ltarchiver::data_length; i<ltarchiver::code_length; i++) {
         block.data[i] = static_cast<schifra::galois::field_symbol>(ecc_buffer[i - ltarchiver::data_length])  & 0xFF;
      }

      /* Decode */
      if (!decoder.decode(block))
      {
         std::cerr << "Error - Critical decoding failure! "
                  << "Msg: " << block.error_as_string()  << std::endl;
         std::cerr << "Errors detected: " << block.errors_detected << std::endl;
         std::cerr << "Position of error on chunk: " << (bytes_read - last_read) << " Bytes read in chunk: " << last_read << std::endl;
         std::cerr << "Position of the ecc file: " << eccFile.tellg() << std::endl;
         std::cerr << "Data length: " << ltarchiver::data_length << " Ecc length: " << ltarchiver::fec_length << std::endl;
         return 1;
      }

      errors_detected += block.errors_detected;
      errors_corrected += block.errors_corrected;

      /*Move the data from the block into the buffers*/

      block.data_to_string(buffer_str);
      block.fec_to_string(ecc_buffer_str);
      
      /* Write buffers to respective files */
      outputFile.write(buffer_str.data(), last_read);
      eccOutputFile.write(ecc_buffer_str.data(), ltarchiver::fec_length);
      
      if((bytes_read / file_size)*100 > last_percent) {
         std::cout << "" << last_percent << "%" << std::endl;
         last_percent++;
      }
   }
   
   std::cout << "Finished decoding" << std::endl;
   std::cout << "Errors detected (estimated): " << errors_detected << std::endl;
   std::cout << "Errors corrected (estimated): " << errors_corrected << std::endl;
   inputFile.close();
   outputFile.flush();
   outputFile.close();
   eccOutputFile.flush();
   eccOutputFile.close();
   eccFile.close();
   delete[] buffer;
   delete[] ecc_buffer;
   return 0;
}