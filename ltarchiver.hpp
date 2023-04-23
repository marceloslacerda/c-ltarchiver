#include "schifra_galois_field.hpp"
#include "schifra_galois_field_polynomial.hpp"
#include "schifra_sequential_root_generator_polynomial_creator.hpp"
#include "schifra_reed_solomon_encoder.hpp"
#include "schifra_reed_solomon_decoder.hpp"
#include "schifra_reed_solomon_block.hpp"
#include "schifra_error_processes.hpp"

namespace ltarchiver
{
    /* Finite Field Parameters */
    const std::size_t field_descriptor                =   8;
    const std::size_t generator_polynomial_index      = 120;
    const std::size_t generator_polynomial_root_count =  32;

    /* Reed Solomon Code Parameters */
    const std::size_t code_length = 255;
    const std::size_t fec_length  =  32;
    const std::size_t data_length = code_length - fec_length;
    /* Instantiate Finite Field and Generator Polynomials */
    const schifra::galois::field field(field_descriptor,
                                      schifra::galois::primitive_polynomial_size06,
                                      schifra::galois::primitive_polynomial06);
    schifra::galois::field_polynomial generator_polynomial(field);
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