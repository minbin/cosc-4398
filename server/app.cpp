#include "app.h"
#include "calc.h"
#include "key.h"
#include "load.h"
#include "save.h"
#include "util.h"

#include <cmath>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <string>

using namespace seal;
namespace fs = std::filesystem;

void
singleclass(std::string key_fp, std::string in_fp, std::string out_fp, std::string model_fp) {
    EncryptionParameters parms(scheme_type::ckks);

    size_t poly_modulus_degree = 32768;
    parms.set_poly_modulus_degree(poly_modulus_degree);
    parms.set_coeff_modulus(CoeffModulus::Create(poly_modulus_degree, { 60, 40, 40, 60 }));
    double scale = pow(2.0, 40);
    SEALContext context(parms);
    print_parameters(parms);

    keys_t keys = load_keys(key_fp, context);
    Plaintext pt_model = load_pt_singleclass_model(model_fp, context, scale);
    Ciphertext ct_query;
    Ciphertext ct_result;
    for (const auto &entry : fs::directory_iterator(in_fp)) {
        ct_query = load_ct(entry.path(), context);
        std::cout << "Calculating dot product" << std::endl;
        ct_result = dot_product(keys, context, pt_model, ct_query);
        save_ct(fs::path(out_fp) / entry.path().filename(), ct_result);
    }
}

int
main(int argc, char *argv[]) {
    if (argc == 1) {
        std::cout << "[singleclass]" << std::endl;
        return 0;
    }
    std::vector<std::string> args(argv, argv + argc);
    args.erase(args.begin());

    if (args.front() == "singleclass") {
        if (argc == 2) {
            std::cout << "[-kf key_filepath][-if in_filepath][-of out_filepath][-m model_filename]" << std::endl;
        } else {
            std::string key_fp = "";
            std::string in_fp = "";
            std::string out_fp = "";
            std::string model_fp = "";
            for (int i=1; i < args.size(); i=i+2) {
                if (args[i] == "-kf") key_fp = args[i+1];
                else if (args[i] == "-if") in_fp = args[i+1];
                else if (args[i] == "-of") out_fp = args[i+1];
                else if (args[i] == "-m") model_fp = args[i+1];
            }
            singleclass(key_fp, in_fp, out_fp, model_fp);
        }
    }

    return 0;
}
