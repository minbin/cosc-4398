#pragma once

#include "key.h"
#include "util.h"

using namespace seal;

Ciphertext dot_product(keys_t keys, const seal::SEALContext context, Plaintext pt_model, Ciphertext ct_query) {
    Evaluator evaluator(context);
    Ciphertext ct_result;
    Ciphertext ct_tmp;

    // Multiply query * coef
    evaluator.multiply_plain(ct_query, pt_model, ct_result);
    evaluator.relinearize_inplace(ct_result, keys.rk);

    // Sum result of query * coef
    for (int i=0; i < 14; i++) {
        std::cout << "   Rotate " << pow(2, i) << std::endl;
        evaluator.rotate_vector(ct_result, pow(2, i), keys.gk, ct_tmp);
        evaluator.add_inplace(ct_result, ct_tmp);
        evaluator.relinearize_inplace(ct_result, keys.rk);
    }

    return ct_result;
}

Ciphertext mult_random_float(keys_t keys, const seal::SEALContext context, Ciphertext ct_result, double scale) {
    CKKSEncoder encoder(context);
    Evaluator evaluator(context);

    Plaintext pt_random_float;
    encoder.encode(random_float(), scale, pt_random_float);
    evaluator.multiply_plain_inplace(ct_result, pt_random_float);

    return ct_result;
}

