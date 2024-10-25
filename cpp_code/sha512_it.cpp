/*
    Iterative version of SHA512 in C
*/

#include <iostream>
#include <cstring>
#include <openssl/sha.h>

using namespace std;

int main(int argc, char* argv[]) {
    if (argc != 4) {
        cerr << "Usage: " << argv[0] << " <pwd> <salt> <rounds>" << endl;
        return 1;
    }

    const char* pwd = argv[1];
    const char* salt = argv[2];
    unsigned int rounds = stoi(argv[3]);

    unsigned char pwd_salt[256];
    const size_t pwd_len = strlen(pwd), salt_len = strlen(salt);
    memcpy(pwd_salt, pwd, pwd_len);
    memcpy(pwd_salt + pwd_len, salt, salt_len);
    unsigned char buffer[SHA512_DIGEST_LENGTH];

    // first hash calculation
    SHA512(reinterpret_cast<const unsigned char*>(pwd), pwd_len + salt_len, buffer);

    // hashing rounds - 1 times
    for (unsigned long i = 1; i < rounds; ++i)
        SHA512(buffer, SHA512_DIGEST_LENGTH, buffer);

    // print the result in hexadecimal format
    char hex_output[SHA512_DIGEST_LENGTH * 2 + 1];
    static const char hex_digits[] = "0123456789abcdef";

    for (size_t i = 0; i < SHA512_DIGEST_LENGTH; ++i) {
        hex_output[i * 2]     = hex_digits[(buffer[i] >> 4) & 0x0F];
        hex_output[i * 2 + 1] = hex_digits[buffer[i] & 0x0F];
    }
    hex_output[SHA512_DIGEST_LENGTH * 2] = '\0';
    cout << "Final hash: " << hex_output << endl;

    return 0;
}