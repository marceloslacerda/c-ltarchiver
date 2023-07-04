#include <cstdint>
#include "schifra_galois_field.hpp"
#include "schifra_galois_field_polynomial.hpp"
#include "schifra_sequential_root_generator_polynomial_creator.hpp"
#include "schifra_reed_solomon_encoder.hpp"
#include "schifra_reed_solomon_decoder.hpp"
#include "schifra_reed_solomon_block.hpp"
#include "schifra_error_processes.hpp"
#include <filesystem>

# ifndef reed_me
# define reed_me 1
# define reed_vars(fd, gpi, rc, fecl) \
  const std::size_t field_descriptor = fd; \
  const std::size_t generator_polynomial_index = gpi; \
  const std::size_t generator_polynomial_root_count = rc; \
  const std::size_t code_length = (2 << fd) - 1; \
  const std::size_t fec_length = fecl; \
  const std::size_t data_length = code_length - fec_length; \
  const schifra::galois::field field(field_descriptor, \
                                     schifra::galois::primitive_polynomial_size06, \
                                     schifra::galois::primitive_polynomial06); \
  schifra::galois::field_polynomial generator_polynomial(field); \
  typedef schifra::reed_solomon::encoder<code_length, fec_length, data_length> encoder_t; \
  typedef schifra::reed_solomon::decoder<code_length, fec_length, data_length> decoder_t;

std::uintmax_t get_file_size(std::string path) {
   std::filesystem::path filepath(path);
   return std::filesystem::file_size(filepath);
}

# define make_sequential_root_generator_polinomial() \
      if ( \
          !schifra::make_sequential_root_generator_polynomial( \
              field, \
              generator_polynomial_index, \
              generator_polynomial_root_count, \
              generator_polynomial) \
      ) { \
          std::cerr << "Error - Failed to create sequential root generator!" << std::endl; \
          return 1; \
      }

#endif
