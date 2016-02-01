#pragma once

#include <openssl/evp.h>
#include <iostream>
#include <string>
#include <exception>

class encryption
{
public:
    typedef int (*evp_init_ex_t)(EVP_CIPHER_CTX*, const EVP_CIPHER*, ENGINE*, const unsigned char*, const unsigned char*);
    typedef int (*evp_update_t)(EVP_CIPHER_CTX*, unsigned char*, int*, const unsigned char*, int);
    typedef int (*evp_final_t)(EVP_CIPHER_CTX*, unsigned char*, int*);

    static const size_t plaintext_size = 128;
    static const int block_size = 128 / 8;
    static const int key_iterations = 65535;

    static void create_salt(std::string& salt, size_t size);
    static void create_iv(unsigned char* iv_128);

    class error : public std::exception
    {
    public:
        explicit error(const char* what) : exception(what) {}
    };
private:
    static bool random_initalized;
    EVP_CIPHER_CTX* ctx;
    unsigned char plaintext[plaintext_size];
    unsigned char ciphertext[plaintext_size + block_size];

    void handle_errors();
    int make_encryption(
        evp_init_ex_t, evp_update_t, evp_final_t, 
        std::istream&, size_t, const unsigned char*, const unsigned char*, std::ostream&);
public:
    encryption();

    int encrypt(
        std::istream& plaintext_stream, size_t plaintext_len, const unsigned char* key_256, 
        const unsigned char* iv, std::ostream& ciphertext_stream);
    int decrypt(
        std::istream& ciphertext_stream, size_t ciphertext_len, const unsigned char* key_256,
        const unsigned char* iv, std::ostream& plaintext_stream);
    void derive_key(const char* password, int passlen, const std::string& salt, unsigned char* out_key_256);
};