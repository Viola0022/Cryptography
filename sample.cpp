#ifndef __PROGTEST__
#include <assert.h>
#include <ctype.h>
#include <limits.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

#include <openssl/evp.h>
#include <openssl/rand.h>

using namespace std;

#endif /* __PROGTEST__ */

//check zero bits in binary
bool checkZeroBits(const unsigned char *hash, int bits) {
    //nulové byte
    int bytes = bits/8;
    //nulové bity
    int rest_bits = bits%8;

    for (int i = 0; i < bytes; ++i) {
        if (hash[i] != 0) return false;
    }

    if (rest_bits > 0) {
        unsigned char mask = 0xFF << (8-rest_bits);
        if ((hash[bytes] & mask) != 0) return false;
    }

    return true;
}

//byte to hexadecimal
string Hexa(const unsigned char *data, size_t length) {
    stringstream ss;
    ss << hex << setfill('0');
    for (size_t i = 0; i < length; ++i) {
        ss << setw(2) << static_cast<int>(data[i]);
    }
    return ss.str();
}


int findHash (int numberZeroBits, string & outputMessage, string & outputHash) {
    /* TODO: Your code here */
    if (numberZeroBits < 0 || numberZeroBits > 512) {
        return 0;
    }

    const int messageLength = 32; //random message lenght
    unsigned char message[messageLength];
    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int hashLength;

    EVP_MD_CTX *mdctx = EVP_MD_CTX_new();
    const EVP_MD *md = EVP_get_digestbyname("SHA512");

    if (!mdctx || !md) {
        return 0;
    }

    while (true) {
        if (RAND_bytes(message, messageLength) != 1) {
            EVP_MD_CTX_free(mdctx);
            return 0;
        }

        if (EVP_DigestInit_ex(mdctx, md, NULL) != 1 ||
            EVP_DigestUpdate(mdctx, message, messageLength) != 1 ||
            EVP_DigestFinal_ex(mdctx, hash, &hashLength) != 1) {
            EVP_MD_CTX_free(mdctx);
            return 0;
        }

        if (checkZeroBits(hash, numberZeroBits)) {
            outputMessage = Hexa(message, messageLength);
            outputHash = Hexa(hash, hashLength);
            EVP_MD_CTX_free(mdctx);
            return 1;
        }
    }
}

int findHashEx (int numberZeroBits, string & outputMessage, string & outputHash, string_view hashType) {
    /* TODO or use dummy implementation */
    return 1;
}

#ifndef __PROGTEST__

//check zero bits in hexa
int checkHash(int bits, const string & hash) {
    // DIY
    int bytes = bits/8;
    int restBits = bits%8;
    for (int i = 0; i < bytes; ++i) {
        if (hash[i] != '0' || hash[i+1] != '0') return 0;
    }
    if (restBits > 0) {
        int value = stoi(hash.substr(2*bytes, 2), nullptr, 16);
        if ((value & (0xFF << (8-restBits))) != 0) return 0;
    }
    return 1;
}

int main (void) {
    string hash, message;
    assert(findHash(0, message, hash) == 1);
    assert(!message.empty() && !hash.empty() && checkHash(0, hash));
    message.clear();
    hash.clear();
    assert(findHash(1, message, hash) == 1);
    assert(!message.empty() && !hash.empty() && checkHash(1, hash));
    message.clear();
    hash.clear();
    assert(findHash(2, message, hash) == 1);
    assert(!message.empty() && !hash.empty() && checkHash(2, hash));
    message.clear();
    hash.clear();
    assert(findHash(3, message, hash) == 1);
    assert(!message.empty() && !hash.empty() && checkHash(3, hash));
    message.clear();
    hash.clear();
    assert(findHash(-1, message, hash) == 0);
    return EXIT_SUCCESS;
}

#endif /* __PROGTEST__ */

