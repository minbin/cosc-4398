#pragma once

#include "key.h"
#include "util.h"

using namespace seal;

inline Ciphertext
decision_function(keys_t keys, const SEALContext context, Ciphertext ct_query, std::vector<double> model) {
    CKKSEncoder encoder(context);
    Evaluator evaluator(context);
    Ciphertext ct_result;
    Ciphertext ct_tmp;

    // Model to plaintext
    Plaintext pt_model;
    encoder.encode(model, ct_query.scale(), pt_model);

    // Multiply query * coef
    evaluator.multiply_plain(ct_query, pt_model, ct_result);
    evaluator.relinearize_inplace(ct_result, keys.rk);

    // Sum result of query * coef
    for (int i=0; i < 14; i++) {
        evaluator.rotate_vector(ct_result, pow(2, i), keys.gk, ct_tmp);
        evaluator.add_inplace(ct_result, ct_tmp);
        evaluator.relinearize_inplace(ct_result, keys.rk);
    }

    return ct_result;
}

inline Ciphertext
mult_random_float(keys_t keys, const SEALContext context, Ciphertext ct, double scale) {
    CKKSEncoder encoder(context);
    Evaluator evaluator(context);

    Plaintext pt;
    encoder.encode(random_float(), scale, pt);
    evaluator.multiply_plain_inplace(ct, pt);
    evaluator.relinearize_inplace(ct, keys.rk);
    return ct;
}
