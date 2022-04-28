#pragma once

#include "key.h"
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

keys_t
load_keys(std::string key_fp, const seal::SEALContext context) {
    keys_t keys;
    std::ifstream is;

    std::cout << "Loading public key from " << fs::path(key_fp) / "pk" << std::endl;
    is.open(fs::path(key_fp) / "pk", std::ifstream::binary);
    keys.pk.unsafe_load(context, is);
    is.close();

    std::cout << "Loading relin key from " << fs::path(key_fp) / "rk" << std::endl;
    is.open(fs::path(key_fp) / "rk", std::ifstream::binary);
    keys.rk.unsafe_load(context, is);
    is.close();

    std::cout << "Loading galois key from " << fs::path(key_fp) / "gk" << std::endl;
    is.open(fs::path(key_fp) / "gk", std::ifstream::binary);
    keys.gk.unsafe_load(context, is);
    is.close();

    std::cout << "Keys loaded " << std::endl;
    return keys;
}

std::vector<double>
load_pt_singleclass_model(std::string model_fp) {
    std::ifstream is;
    std::string s;
    std::string delim = ",";

    is.open(model_fp);
    size_t pos = 0;

    std::cout << "Loading model" << std::endl;
    std::vector<double> model;
    double intercept;
    while (is) {
        is >> s;
        while ((pos = s.find(delim)) != std::string::npos) {
            model.push_back(std::stof(s.substr(0, pos)));
            s.erase(0, pos + delim.length());
        }
        model.push_back(std::stof(s));
    }
    is.close();

    std::cout << "    Coefficients: ";
    for(int i=0; i < model.size()-1; i++) {
        std::cout << model[i] << " ";
    }
    std::cout << "\n";
    std::cout << "    Intercept: " << model.back() << std::endl;

    return model;
}

std::vector<Plaintext>
load_pt_multiclass_model(std::string model_fp, const SEALContext context, double scale) {
    std::vector<Plaintext> model;
    std::cout << "Loading multiclass model from " << model_fp << std::endl;
    std::ifstream is(model_fp);
    std::string delim = ",";
    CKKSEncoder encoder(context);
    if (is.is_open()) {
        std::string s;
        while(std::getline(is, s)) {
            size_t pos = 0;
            std::vector<double> tmp;
            Plaintext pt_tmp;
            while((pos = s.find(delim)) != std::string::npos) {
                tmp.push_back(std::stof(s.substr(0, pos)));
                s.erase(0, pos + delim.length());
            }
            encoder.encode(tmp, scale, pt_tmp);
            model.push_back(pt_tmp);
        }
    }
    is.close();
    return model;
}

std::vector<std::vector<double>>
load_multiclass_model(std::string model_fp) {
    std::vector<std::vector<double>> model;
    std::cout << "Loading multiclass model from " << model_fp << std::endl;
    std::ifstream is(model_fp);
    std::string delim = ",";
    if (is.is_open()) {
        std::string s;
        while(std::getline(is, s)) {
            size_t pos = 0;
            std::vector<double> tmp;
            while((pos = s.find(delim)) != std::string::npos) {
                tmp.push_back(std::stof(s.substr(0, pos)));
                s.erase(0, pos + delim.length());
            }
            model.push_back(tmp);
        }
    }
    is.close();

    return model;
}

Ciphertext
load_ct(std::string in_fp, const SEALContext context) {
    Ciphertext ct;
    std::cout << "Loading ciphertext from " << in_fp << std::endl;
    std::ifstream is(in_fp, std::ifstream::binary);
    ct.unsafe_load(context, is);
    is.close();
    return ct;
}
