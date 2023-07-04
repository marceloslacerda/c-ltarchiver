#include <cstdint>
#include "schifra_galois_field.hpp"
#include "schifra_galois_field_polynomial.hpp"
#include "schifra_sequential_root_generator_polynomial_creator.hpp"
#include "schifra_reed_solomon_encoder.hpp"
#include "schifra_reed_solomon_decoder.hpp"
#include "schifra_reed_solomon_block.hpp"
#include "schifra_error_processes.hpp"
#include <filesystem>

template <
    int field_descriptor_t,
    std::size_t generator_polynomial_index,
    std::size_t fec_length_t,
    std::size_t code_length_t = (1 << field_descriptor_t) - 1,
    std::size_t data_length_t = code_length_t - fec_length_t>
class LTAVariables {
    public:
        static const std::size_t code_length = code_length_t;
        static const std::size_t data_length = data_length_t;
        static const std::size_t fec_length = fec_length_t;
        static const std::size_t field_descriptor = field_descriptor_t;
        const schifra::galois::field* field;
        schifra::galois::field_polynomial* generator_polynomial;
        typedef schifra::reed_solomon::encoder<code_length_t, fec_length_t, data_length_t> encoder_t;
        typedef schifra::reed_solomon::decoder<code_length_t, fec_length_t, data_length_t> decoder_t;
        const encoder_t* encoder;
        const decoder_t* decoder;
        schifra::reed_solomon::block<code_length,fec_length> block;
        LTAVariables(){
            field = new schifra::galois::field(
                field_descriptor,
                schifra::galois::primitive_polynomial_size06,
                schifra::galois::primitive_polynomial06);
            generator_polynomial = new schifra::galois::field_polynomial(*field);
            if (
                !schifra::make_sequential_root_generator_polynomial(*field,
                                                                    generator_polynomial_index,
                                                                    fec_length_t,
                                                                    *generator_polynomial)
            ) {
                std::cerr << "Error - Failed to create sequential root generator!" << std::endl;
                std::exit(1);
            }
            encoder = new encoder_t(*field, *generator_polynomial);
            decoder = new decoder_t(*field, generator_polynomial_index);
        }
        
};