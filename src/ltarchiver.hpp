#include "schifra_galois_field.hpp"
#include "schifra_galois_field_polynomial.hpp"
#include "schifra_sequential_root_generator_polynomial_creator.hpp"
#include "schifra_reed_solomon_encoder.hpp"
#include "schifra_reed_solomon_decoder.hpp"
#include "schifra_reed_solomon_block.hpp"
#include "schifra_error_processes.hpp"
#include <cstdint>
#include <filesystem>

namespace ltarchiver
{
    /* Finite Field Parameters */
    const std::size_t field_descriptor                =  8; // Field size = 2^field_descriptor - 1
    const std::size_t generator_polynomial_index      = 120; // Must be smaller than field size
    const std::size_t generator_polynomial_root_count =  2; // Plus index must be smaller than field size

    /* Reed Solomon Code Parameters */
    const std::size_t code_length = 255; // Must be the same as field size
    const std::size_t fec_length  =  2;
    const std::size_t data_length = code_length - fec_length;
    /* Instantiate Finite Field and Generator Polynomials */
    const schifra::galois::field field(field_descriptor,
                                      schifra::galois::primitive_polynomial_size06,
                                      schifra::galois::primitive_polynomial06);
    schifra::galois::field_polynomial generator_polynomial(field); // The alpha that generates all elements of the field
    typedef schifra::reed_solomon::encoder<code_length, fec_length, data_length> encoder_t;
    typedef schifra::reed_solomon::decoder<ltarchiver::code_length, ltarchiver::fec_length, ltarchiver::data_length> decoder_t;

    bool make_sequential_root_generator_polinomial() {
        if (
            !schifra::make_sequential_root_generator_polynomial(field,
                                                                generator_polynomial_index,
                                                                generator_polynomial_root_count,
                                                                generator_polynomial)
        ) {
            std::cerr << "Error - Failed to create sequential root generator!" << std::endl;
            return false;
        }
        return true;
    }
}

std::uintmax_t get_file_size(std::string path) {
   std::filesystem::path filepath(path);
   return std::filesystem::file_size(filepath);
}
