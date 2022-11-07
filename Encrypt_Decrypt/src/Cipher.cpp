#include "Cipher.h"


Cipher::Cipher()
{
    setKey("abcdefghijklmnop");
}

Cipher::Cipher(char *key)
{
    setKey(key);
}

Cipher::~Cipher()
{
    delete privateCipherKey;
}

void Cipher::setKey(char *key)
{

    if (strlen(key) > 16)
    {
        privateCipherKey = new char[17];
        (std::string(key).substr(0, 16)).c_str();
    }
    else if (strlen(key) < 16)
    {
        privateCipherKey = "abcdefghijklmnop";

#ifdef CIPHER_DEBUG
#endif
    }
    else
    {
#ifdef CIPHER_DEBUG
#endif
        privateCipherKey = key;
    }
}

char *Cipher::getKey()
{
    return privateCipherKey;
}

void Cipher::encrypt(char *plainText, char *key, unsigned char *outputBuffer)
{
    // encrypt plainText buffer of length 16 characters
    mbedtls_aes_context aes;

    mbedtls_aes_init(&aes);
    mbedtls_aes_setkey_enc(&aes, (const unsigned char *)key, strlen(key) * 8);
    mbedtls_aes_crypt_ecb(&aes, MBEDTLS_AES_ENCRYPT, (const unsigned char *)plainText, outputBuffer);
    mbedtls_aes_free(&aes);
}

void Cipher::encrypt(char *plainText, unsigned char *outputBuffer)
{
    encrypt(plainText, getKey(), outputBuffer);
}

void Cipher::decrypt(unsigned char *cipherText, char *key, unsigned char *outputBuffer)
{
    // encrypt ciphered chipherText buffer of length 16 characters to plain text
    mbedtls_aes_context aes;

    mbedtls_aes_init(&aes);
    mbedtls_aes_setkey_dec(&aes, (const unsigned char *)key, strlen(key) * 8);
    mbedtls_aes_crypt_ecb(&aes, MBEDTLS_AES_DECRYPT, (const unsigned char *)cipherText, outputBuffer);
    mbedtls_aes_free(&aes);
}

void Cipher::decrypt(unsigned char *cipherText, unsigned char *outputBuffer)
{
    decrypt(cipherText, getKey(), outputBuffer);
}

std::string Cipher::encryptBuffer(char *plainText, char *key)
{
    std::string cipherTextString = "";
    unsigned char cipherTextOutput[16];

    encrypt(plainText, key, cipherTextOutput);

    for (int i = 0; i < 16; i++)
    {
        cipherTextString = cipherTextString + (char)cipherTextOutput[i];
    }

    return cipherTextString;
}

std::string Cipher::encryptBuffer(char *plainText)
{
    return encryptBuffer(plainText, getKey());
}

std::string Cipher::decryptBuffer(std::string cipherText, char *key)
{
    std::string decipheredTextString = "";
    unsigned char cipherTextOutput[16];
    unsigned char decipheredTextOutput[16];

    for (int i = 0; i < 16; i++)
    {
        cipherTextOutput[i] = (char)cipherText[i];
    }

    decrypt(cipherTextOutput, key, decipheredTextOutput);

    for (int i = 0; i < 16; i++)
    {
        decipheredTextString = decipheredTextString + (char)decipheredTextOutput[i];

        if (decipheredTextString[i] == '\0')
        {
            break;
        }
    }

    return decipheredTextString;
}

std::string Cipher::decryptBuffer(std::string cipherText)
{
    return decryptBuffer(cipherText, getKey());
}

std::string Cipher::encryptstring(std::string plainText, char *key)
{
    constexpr int BUFF_SIZE = 16;
    std::string buffer = "";
    std::string cipherTextString = "";
    int index = plainText.length() / BUFF_SIZE;

    for (int block = 0; block < plainText.length() / BUFF_SIZE; block++)
    {
        for (int j = block * BUFF_SIZE; j < (block + 1) * BUFF_SIZE; j++)
        {
            buffer += plainText[j];
        }

        cipherTextString += encryptBuffer(const_cast<char *>(buffer.c_str()), key);
        buffer = "";
    }

    buffer = "";

    if (plainText.length() % BUFF_SIZE > 0)
    {
        for (int bytes_read = (index * BUFF_SIZE); bytes_read <= (index * BUFF_SIZE) + plainText.length() % BUFF_SIZE; bytes_read++)
        {
            buffer += plainText[bytes_read];
        };
        cipherTextString += encryptBuffer(const_cast<char *>(buffer.c_str()), key);
    }

    return cipherTextString;
}

std::string Cipher::encryptstring(std::string plainText)
{
    return encryptstring(plainText, getKey());
}

std::string Cipher::decryptstring(std::string cipherText, char *key)
{
    constexpr int BUFF_SIZE = 16;
    std::string buffer = "";
    std::string decipheredTextString = "";

    for (int block = 0; block < cipherText.length() / BUFF_SIZE; block++)
    {
        for (int j = block * BUFF_SIZE; j < (block + 1) * BUFF_SIZE; j++)
        {
            buffer += cipherText[j];
        }

        decipheredTextString += decryptBuffer(buffer, key);
        buffer = "";
    }

    return decipheredTextString;
}

std::string Cipher::decryptstring(std::string cipherText)
{
    return decryptstring(cipherText, getKey());
}