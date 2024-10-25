/*
    PARALLEL_SHA512: this is the high level implementation of the algorithm. Here I use high level
    C++ constructs such as string and vector. This is not good performance wise, but it gives an idea 
    on how the algorithm works without complicating the code so much. 
*/

#include <iostream>
#include <iomanip>
#include <vector>
#include <thread>
#include <openssl/sha.h>

using namespace std;

// funtion to compute the hash of a string using SHA512
string sha512(const string& s) {
    unsigned char hash[SHA512_DIGEST_LENGTH];
    const unsigned char* data = reinterpret_cast<const unsigned char *>(s.c_str());
    const size_t length = s.length();

    SHA512(data, length, hash);

    ostringstream oss;
    for (size_t i = 0; i < SHA512_DIGEST_LENGTH; ++i) {
        oss << hex << setw(2) << setfill('0') << static_cast<int>(hash[i]);
    }

    return oss.str();
}

// function to compute the Merkle Tree for each thread
void parallel_hash(const string& s, unsigned int size, unsigned int start, unsigned int number,
        vector<string>& hashes) {
            
    vector<string> leaves(size, s), results;

    for(size_t i = 0; i < leaves.size(); ++i) {
        leaves[i] += to_string(start+i+1);
        results.emplace_back(sha512(leaves[i]));
    }

    while(results.size() > 1) {
        vector<string> tmp;
        for(size_t i = 0; i+1 < results.size(); i = i + 2)
            tmp.emplace_back(sha512(results[i] + results[i+1]));

        if(results.size() % 2 != 0) 
            tmp.emplace_back(results.back());

        results = move(tmp);
    }

    hashes[number] = results.front();
}

int main(int argc, char* argv[]) {
    if (argc != 5) {
        cerr << "Usage: " << argv[0] << " <pwd> <salt> <n_cores> <n_leaves>" << endl;
        return 1;
    }

    unsigned int n_cores = stoi(argv[3]);
    unsigned int n_leaves = stoi(argv[4]);
    const string pwd = argv[1];
    const string salt = argv[2];
    vector<string> hashes(n_cores);
    vector<thread> threads;

    for(size_t i = 0; i < n_cores; ++i)
        threads.emplace_back(parallel_hash, pwd+salt, n_leaves, i*n_leaves, i, ref(hashes));

    for (auto& thread : threads) 
        thread.join();

    while(hashes.size() > 1) {
        vector<string> tmp;
        for(size_t i = 0; i+1 < hashes.size(); i = i + 2)
            tmp.emplace_back(sha512(hashes[i] + hashes[i+1]));

        if(hashes.size() % 2 != 0)
            tmp.emplace_back(hashes.back());

        hashes = move(tmp);
    }

    cout << "Hash: " << hashes.front() << endl;
    return 0;
}