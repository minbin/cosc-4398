#include "app.h"

#include <filesystem>
#include <fstream>
#include <string>

using namespace seal;
namespace fs = std::filesystem;

std::string get_uuid() {
    static std::random_device dev;
    static std::mt19937 rng(dev());

    std::uniform_int_distribution<int> dist(0, 15);

    const char *v = "0123456789abcdef";
    const bool dash[] = { 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0 };

    std::string res;
    for (int i = 0; i < 16; i++) {
        if (dash[i]) res += "-";
        res += v[dist(rng)];
        res += v[dist(rng)];
    }
    return res;
}

void decrypt(std::string filepath, std::string filename) {
  EncryptionParameters parms(scheme_type::ckks);

  size_t poly_modulus_degree = 8192;
  parms.set_poly_modulus_degree(poly_modulus_degree);
  parms.set_coeff_modulus(CoeffModulus::Create(poly_modulus_degree, { 60, 40, 40, 60 }));
  SEALContext context(parms);

  std::ifstream is;

  is.open(fs::path(filepath) / "secret.key", std::ifstream::binary);
  SecretKey secret_key;
  secret_key.unsafe_load(context, is);
  is.close();

  is.open(fs::path(filepath) / filename, std::ifstream::binary);
  Ciphertext enc_res;
  enc_res.unsafe_load(context, is);
  is.close();

  Decryptor decryptor(context, secret_key);
  CKKSEncoder encoder(context);

  Plaintext plain_res;
  decryptor.decrypt(enc_res, plain_res);
  std::vector<double> res;
  encoder.decode(plain_res, res);
  std::cout << res[0] << std::endl;
}

void encrypt(std::string filepath, std::string num) {
  EncryptionParameters parms(scheme_type::ckks);

  size_t poly_modulus_degree = 8192;
  parms.set_poly_modulus_degree(poly_modulus_degree);
  parms.set_coeff_modulus(CoeffModulus::Create(poly_modulus_degree, { 60, 40, 40, 60 }));
  double scale = pow(2.0, 40);
  SEALContext context(parms);

  PublicKey public_key;
  RelinKeys relin_keys;
  GaloisKeys gal_keys;
  std::ifstream is;

  is.open(fs::path(filepath) / "pub.key", std::ifstream::binary);
  public_key.unsafe_load(context, is);
  is.close();

  is.open(fs::path(filepath) / "relin.key", std::ifstream::binary);
  relin_keys.unsafe_load(context, is);
  is.close();

  is.open(fs::path(filepath) / "gal.key", std::ifstream::binary);
  gal_keys.unsafe_load(context, is);
  is.close();

  Encryptor encryptor(context, public_key);
  Evaluator evaluator(context);
  CKKSEncoder encoder(context);
  std::vector<double> input;
  input.push_back(std::stoi(num));

  Plaintext x_plain;
  encoder.encode(input, scale, x_plain);
  Ciphertext x_cipher;
  encryptor.encrypt(x_plain, x_cipher);

  std::string uuid;
  uuid = get_uuid();

  std::filebuf fb;
  std::ostream os(&fb);
  fb.open(fs::path(filepath) / uuid, std::ios::out);
  x_cipher.save(os);
  fb.close();

  std::ofstream of;
  of.open(fs::path(filepath) / "enc.conf", std::ios::app);
  of << uuid << std::endl;
  of.close();
}

void genkey() {
  std::string uuid;
  uuid = get_uuid();
  std::cout << uuid << std::endl;

  fs::create_directory(uuid);

  EncryptionParameters parms(scheme_type::ckks);
  size_t poly_modulus_degree = 8192;
  parms.set_poly_modulus_degree(poly_modulus_degree);
  parms.set_coeff_modulus(CoeffModulus::Create(poly_modulus_degree, { 60, 40, 40, 60 }));
  SEALContext context(parms);

  print_parameters(uuid, context);

  KeyGenerator keygen(context);
  auto secret_key = keygen.secret_key();
  PublicKey public_key;
  RelinKeys relin_keys;
  GaloisKeys gal_keys;
  keygen.create_public_key(public_key);
  keygen.create_relin_keys(relin_keys);
  keygen.create_galois_keys(gal_keys);

  std::filebuf fb;
  std::ostream os(&fb);

  fb.open(fs::path(uuid) / "secret.key", std::ios::out);
  secret_key.save(os);
  fb.close();
  fb.open(fs::path(uuid) / "pub.key", std::ios::out);
  public_key.save(os);
  fb.close();
  fb.open(fs::path(uuid) / "relin.key", std::ios::out);
  relin_keys.save(os);
  fb.close();
  fb.open(fs::path(uuid) / "gal.key", std::ios::out);
  gal_keys.save(os);
  fb.close();
}

int main(int argc, char *argv[]) {
  if (std::strcmp(argv[1], "encrypt") == 0) {
    if (argc == 4) {
      encrypt(argv[2], argv[3]);
    }
  } else if (std::strcmp(argv[1], "genkey") == 0) {
    genkey();
  } else if (std::strcmp(argv[1], "decrypt") == 0) {
    std::string line;
    std::ifstream ifs (fs::path(argv[2]) / "enc.conf");
    if (ifs.is_open()) {
      while(std::getline(ifs, line)) {
        decrypt(argv[2], line + ".res");
      }
    }
  }
  else if (std::strcmp(argv[1], "pack") == 0) {
    std::string sk = fs::path(argv[2]) / "secret.key";
    std::string cmd = "tar -cvzf " + std::string(argv[2]) + ".tar.gz --exclude-from=" + sk + " " + std::string(argv[2]);
    std::cout << cmd << std::endl;
    system(cmd.c_str());
  }
  else if (std::strcmp(argv[1], "unpack") == 0) {
    std::string cmd = "tar -zxf " + std::string(argv[2]);
    std::cout << cmd << std::endl;
    system(cmd.c_str());
  }

  return 0;
}
