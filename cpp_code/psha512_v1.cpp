/*
    PARALLEL_SHA512: This the first optimized version of the algorithm. The strings are now handled in C style,
    so I'm using unsigned char pointers. The function to compute the hash using SHA512 given a specific string
    has been replaced by the primitive SHA512() provided by the OpenSSL library. 
    memcpy is used for the string concatenation.
*/

#include <iostream>
#include <iomanip>
#include <vector>
#include <array>
#include <thread>
#include <openssl/sha.h>
#include <cstring> 

using namespace std;

// function to compute the Merkle Tree for each thread
void parallel_hash(const unsigned char* str, size_t str_len, unsigned int size, unsigned int start, unsigned int number,
        vector<array<unsigned char, SHA512_DIGEST_LENGTH>>& hashes) {

    vector<array<unsigned char, SHA512_DIGEST_LENGTH>> results(size);
    char buffer[256];
    unsigned char* unsigned_buffer = reinterpret_cast<unsigned char*>(buffer);
    memcpy(buffer, str, str_len);

    string index_str;
    for (size_t i = 0; i < size; ++i) {
        // Index to string conversion
        index_str = to_string(start + i + 1);
        memcpy(buffer + str_len, index_str.c_str(), index_str.size());
        SHA512(unsigned_buffer, str_len + index_str.size(), results[i].data());
    }

    // Loop to reduce the hashes to a single hash
    size_t active_hashes = size;
    while (active_hashes > 1) {
        for (size_t i = 0; i < active_hashes / 2; ++i) {
            SHA512_CTX ctx;
            SHA512_Init(&ctx);
            SHA512_Update(&ctx, results[2 * i].data(), SHA512_DIGEST_LENGTH);
            SHA512_Update(&ctx, results[2 * i + 1].data(), SHA512_DIGEST_LENGTH);
            SHA512_Final(results[i].data(), &ctx);
        }

        if (active_hashes % 2 != 0) {
            results[active_hashes / 2] = results[active_hashes - 1];
            active_hashes = active_hashes / 2 + 1;
        } else {
            active_hashes = active_hashes / 2;
        }
    }

    memcpy(hashes[number].data(), results[0].data(), SHA512_DIGEST_LENGTH);
}

int main(int argc, char* argv[]) {
    if (argc != 5) {
        cerr << "Usage: " << argv[0] << " <pwd> <salt> <n_cores> <n_leaves>" << endl;
        return 1;
    }

    const char* pwd = argv[1];
    const char* salt = argv[2];
    unsigned int n_cores = stoi(argv[3]);
    unsigned int n_leaves = stoi(argv[4]);

    vector<array<unsigned char, SHA512_DIGEST_LENGTH>> hashes(n_cores);
    vector<thread> threads;

    const size_t pwd_len = strlen(pwd), salt_len = strlen(salt);
    vector<unsigned char> pwd_salt(pwd_len + salt_len);
    memcpy(pwd_salt.data(), pwd, pwd_len);
    memcpy(pwd_salt.data() + pwd_len, salt, salt_len);

    // thread generation
    for (size_t i = 0; i < n_cores; ++i)
        threads.emplace_back(parallel_hash, pwd_salt, pwd_len + salt_len, n_leaves, i * n_leaves, i, ref(hashes));

    for (auto& thread : threads)
        thread.join();

    size_t active_hashes = n_cores;
    while (active_hashes > 1) {
        for (size_t i = 0; i < active_hashes / 2; ++i) {
            SHA512_CTX ctx;
            SHA512_Init(&ctx);
            SHA512_Update(&ctx, hashes[2 * i].data(), SHA512_DIGEST_LENGTH);
            SHA512_Update(&ctx, hashes[2 * i + 1].data(), SHA512_DIGEST_LENGTH);
            SHA512_Final(hashes[i].data(), &ctx);
        }

        if (active_hashes % 2 != 0) {
            hashes[active_hashes / 2] = hashes[active_hashes - 1];
            active_hashes = active_hashes / 2 + 1;
        } else {
            active_hashes = active_hashes / 2;
        }
    }

    // print the result in hexadecimal format
    char hex_output[SHA512_DIGEST_LENGTH * 2 + 1];
    static const char hex_digits[] = "0123456789abcdef";

    for (size_t i = 0; i < SHA512_DIGEST_LENGTH; ++i) {
        hex_output[i * 2]     = hex_digits[(hashes[0][i] >> 4) & 0x0F];
        hex_output[i * 2 + 1] = hex_digits[hashes[0][i] & 0x0F];
    }
    hex_output[SHA512_DIGEST_LENGTH * 2] = '\0';
    cout << "Hash: " << hex_output << endl;

    return 0;
}