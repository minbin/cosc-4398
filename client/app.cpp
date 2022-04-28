#include "app.h"
#include "key.h"
#include "load.h"
#include "util.h"

#include <filesystem>
#include <fstream>
#include <string>

using namespace seal;
namespace fs = std::filesystem;

void
decrypt(std::string key_fp, std::string in_fp, std::string out_fp) {
    EncryptionParameters parms(scheme_type::ckks);

    size_t poly_modulus_degree = 32768;
    parms.set_poly_modulus_degree(poly_modulus_degree);
    parms.set_coeff_modulus(CoeffModulus::Create(poly_modulus_degree, { 60, 40, 40, 60 }));
    SEALContext context(parms);
    print_parameters(parms);

    std::ifstream is;

    std::cout << "Loading secret key from " << fs::path(key_fp) / "sk" << std::endl;
    is.open(fs::path(key_fp) / "sk", std::ifstream::binary);
    SecretKey secret_key;
    secret_key.unsafe_load(context, is);
    is.close();

    std::cout << "Loading encrypted result from " << in_fp << std::endl;
    Ciphertext ct_result;
    Plaintext pt_result;
    std::vector<double> result;
    Decryptor decryptor(context, secret_key);
    CKKSEncoder encoder(context);
    std::vector<double> ans;
    std::vector<double> proba;
    ans.resize(10);
    proba.resize(10);

    for (const auto &entry : fs::directory_iterator(in_fp)) {
        ct_result = load_ct(entry.path(), context);
        decryptor.decrypt(ct_result, pt_result);
        encoder.decode(pt_result, result);
        int n = std::stoi(entry.path().filename()) + 1;
        if (n > result.size()) {
            ans.resize(n);
            proba.resize(n);
        }
        proba[n-1] = result.front();
        if (result.front() > 0) ans[n-1] = 1;
        else ans[n-1] = 0;
    }

    print_vector(ans);
    print_json(out_fp, ans, proba);
}

void
encrypt_infile(std::string key_fp, std::string in_fp, std::string out_fp) {
    std::cout << "Creating output directory " << out_fp << std::endl;
    fs::create_directories(fs::path(out_fp));

    EncryptionParameters parms(scheme_type::ckks);
    size_t poly_modulus_degree = 32768;
    parms.set_poly_modulus_degree(poly_modulus_degree);
    parms.set_coeff_modulus(CoeffModulus::Create(poly_modulus_degree, { 60, 40, 40, 60 }));
    double scale = pow(2.0, 40);
    SEALContext context(parms);

    keys_t keys = load_keys(fs::path(key_fp), context);
    std::vector<std::vector<double>> queries = load_queries(in_fp);
    std::cout << "Finished loading " << in_fp << std::endl;

    Encryptor encryptor(context, keys.pk);
    Evaluator evaluator(context);
    CKKSEncoder encoder(context);

    fs::create_directories(fs::path(out_fp) / "query");
    fs::create_directories(fs::path(out_fp) / "keys");
    for (int i=0; i < queries.size(); i++) {
        Plaintext pt_query;
        std::cout << "Encoding "; print_vector(queries[i]);
        encoder.encode(queries[i], scale, pt_query);
        Ciphertext ct_query;
        encryptor.encrypt(pt_query, ct_query);
        std::string out_fn = fs::path(out_fp) / "query" / std::to_string(i);
        std::cout << "Saving output file " << out_fn << std::endl;
        std::filebuf fb;
        std::ostream os(&fb);
        fb.open(fs::path(out_fn), std::ios::out);
        ct_query.save(os);
        fb.close();
    }

    save_keys(fs::path(out_fp) / "keys", keys);
}

void
genkey(std::string base_fp) {
    EncryptionParameters parms(scheme_type::ckks);
    size_t poly_modulus_degree = 32768;
    parms.set_poly_modulus_degree(poly_modulus_degree);
    parms.set_coeff_modulus(CoeffModulus::Create(poly_modulus_degree, { 60, 40, 40, 60 }));
    SEALContext context(parms);

    // Generate keys
    KeyGenerator keygen(context);
    auto secret_key = keygen.secret_key();
    keys_t keys;
    keygen.create_public_key(keys.pk);
    keygen.create_relin_keys(keys.rk);
    keygen.create_galois_keys(keys.gk);

    // Save keys
    const fs::path out_fp = fs::path(base_fp);
    save_keys(out_fp, keys);
    std::filebuf fb;
    std::ostream os(&fb);
    fb.open(out_fp / "sk", std::ios::out);
    secret_key.save(os);
    fb.close();
}

int
main(int argc, char *argv[]) {
    if (argc == 1) {
        std::cout << "[genkey][encrypt][decrypt]" << std::endl;
        return 0;
    }
    std::vector<std::string> args(argv, argv + argc);
    args.erase(args.begin());

    if (args.front() == "genkey") {
        if (argc == 2) std::cout << "[-p path]" << std::endl;
        if (argc == 4) {
            genkey(args[2]);
        }
    } else if (args.front() == "encrypt") {
        if (argc == 2) {
            std::cout << "[-kf key_filepath][-if in_filename][-of out_filepath]" << std::endl;
        } else {
            std::string key_fp = "";
            std::string in_fp = "";
            std::string out_fp = "";
            for (int i=1; i < args.size(); i=i+2) {
                if (args[i] == "-kf") key_fp = args[i+1];
                else if (args[i] == "-if") in_fp = args[i+1];
                else if (args[i] == "-of") out_fp = args[i+1];
            }
            encrypt_infile(key_fp, in_fp, out_fp);
        }
    } else if (args.front() == "decrypt") {
        if (argc == 2) {
            std::cout << "[-kf key_filepath][-if in_filename][-of out_filepath]" << std::endl;
        } else {
            std::string key_fp = "";
            std::string in_fp = "";
            std::string out_fp = "";
            for (int i=1; i < args.size(); i=i+2) {
                if (args[i] == "-kf") key_fp = args[i+1];
                else if (args[i] == "-if") in_fp = args[i+1];
                else if (args[i] == "-of") out_fp = args[i+1];
            }
            decrypt(key_fp, in_fp, out_fp);
        }
    }

    return 0;
}
