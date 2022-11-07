
#ifndef CIPHER_H_
#define CIPHER_H_

#include "mbedtls/aes.h"
#include "string.h"
#include <string>

#define CIPHER_DEBUG

class Cipher
{
public:
    Cipher();

    Cipher(char *key);

    virtual ~Cipher();

    void setKey(char *key);

    char *getKey();

    void encrypt(char *plainText, char *key, unsigned char *outputBuffer);

    void encrypt(char *plainText, unsigned char *outputBuffer);

    void decrypt(unsigned char *cipherText, char *key, unsigned char *outputBuffer);

    void decrypt(unsigned char *cipherText, unsigned char *outputBuffer);

    std::string encryptBuffer(char *plainText, char *key);

    std::string encryptBuffer(char *plainText);

    std::string decryptBuffer(std::string cipherText, char *key);

    std::string decryptBuffer(std::string cipherText);

    std::string encryptstring(std::string plainText, char *key);

    std::string encryptstring(std::string plainText);

    std::string decryptstring(std::string cipherText, char *key);

    std::string decryptstring(std::string cipherText);

private:
    char *privateCipherKey;
};

#endif /* CIPHER_H_ */