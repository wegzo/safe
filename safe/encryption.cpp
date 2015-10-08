#include "encryption.h"
#include <openssl/err.h>
#include <openssl/sha.h>
#include <openssl/crypto.h>
#include <time.h>
#include <cstdlib>

encryption::encryption()
{
    srand((unsigned int)time(NULL));
}

void encryption::handle_errors()
{
    // TODO: cleanup everything when switching to throw
    ERR_print_errors_fp(stderr);
    system("pause");
    exit(0);
}

// TODO: streams may leak info
int encryption::make_encryption(
    evp_init_ex_t evp_init_ex, evp_update_t evp_update, evp_final_t evp_final,
    std::istream& plaintext_stream, size_t plaintext_len, const unsigned char* key, 
    const unsigned char* iv, std::ostream& ciphertext_stream)
{
    int len, ciphertext_len = 0;

    // create and init context
    if(!(this->ctx = EVP_CIPHER_CTX_new()))
        handle_errors();
    // init the encryption operation
    if(evp_init_ex(this->ctx, EVP_aes_256_cbc(), NULL, key, iv) != 1)
        handle_errors();

    // cipher
    for(size_t pos = 0; pos < plaintext_len; pos += sizeof(this->plaintext))
    {
        if((pos + sizeof(this->plaintext)) <= plaintext_len)
            // read full next block
            plaintext_stream.read((char*)this->plaintext, sizeof(this->plaintext));
        else
            // read the last partial block
            plaintext_stream.read((char*)this->plaintext, plaintext_len - pos);

        if(evp_update(this->ctx, this->ciphertext, &len, this->plaintext, (int)plaintext_stream.gcount()) != 1)
            handle_errors();
        ciphertext_len += len;
        ciphertext_stream.write((char*)this->ciphertext, len);
    }
    // finalize(pads the last block if input wasn't sufficient)
    if(evp_final(this->ctx, this->ciphertext, &len) != 1)
        handle_errors();
    if(len > 0)
        ciphertext_stream.write((char*)this->ciphertext, len);
    ciphertext_len += len;

    memset(this->plaintext, 0, sizeof(this->plaintext));
    memset(this->ciphertext, 0, sizeof(this->ciphertext));
    EVP_CIPHER_CTX_cleanup(this->ctx);
    EVP_CIPHER_CTX_free(this->ctx);

    return ciphertext_len;
}

int encryption::encrypt(
    std::istream& plaintext_stream, size_t plaintext_len, 
    const unsigned char* key_256, const unsigned char* iv, std::ostream& ciphertext_stream)
{
    return this->make_encryption(
        EVP_EncryptInit_ex, EVP_EncryptUpdate, EVP_EncryptFinal_ex, 
        plaintext_stream, plaintext_len, key_256, iv, ciphertext_stream);
}

int encryption::decrypt(
    std::istream& ciphertext_stream, size_t ciphertext_len, 
    const unsigned char* key_256, const unsigned char* iv, std::ostream& plaintext_stream)
{
    return this->make_encryption(
        EVP_DecryptInit_ex, EVP_DecryptUpdate, EVP_DecryptFinal_ex, 
        ciphertext_stream, ciphertext_len, key_256, iv, plaintext_stream);
}

void encryption::derive_key(const char* password, const std::string& salt, unsigned char* out_key_256)
{
    if(PKCS5_PBKDF2_HMAC(
        password, strlen(password), (unsigned char*)salt.c_str(), salt.length(), 
        key_iterations, EVP_sha256(), 256 / 8, out_key_256) != 1)
        handle_errors();
}

void encryption::create_salt(std::string& salt, size_t size)
{
    // TODO: iv currently deducable from salt
    // create salt from hash of iv
    for(size_t i = 0; i < size; i++)
        salt.push_back(rand() % 256);
}

void encryption::create_iv(unsigned char* iv_128)
{
    for(size_t i = 0; i < (128 / 8); i++)
        iv_128[i] = rand() % 256;
}