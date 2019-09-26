#include <string>
#include <math.h>

unsigned int string_to_int(const char* s, const int nbr_char_policy, const long long p) {
    long long int_result = 0;
    for (const char *car = s; *car; car++) {
        int_result *= nbr_char_policy;
        int_result += (int)(*car);
        int_result %= p;
    }
    return int_result;
}

std::string int_to_string(long long int_hash, const int nbr_char_policy, const int string_size) {
    std::string result = "";

    std::cout << "nbr char policy: " << nbr_char_policy << std::endl;
    while(result.length() < string_size)
    {
        result += rainbow::CHAR_POLICY[int_hash % nbr_char_policy];
        std::cout << "init: " << int_hash << std::endl;
        std::cout << "mod: " << (int_hash % nbr_char_policy) << std::endl;
        std::cout << "div: " << (int_hash / nbr_char_policy) << std::endl;
        std::cout << std::endl;
        int_hash /= nbr_char_policy;
    }
    return result;
}

std::string reverse(int index, std::string hash_value)
{
    std::cout << "Hash: " << hash_value << std::endl;
    std::string new_hash = sha256(hash_value);
    std::cout << "New hash: " << new_hash << std::endl;

    int pass_size = 6;

    int nbr_char_policy = rainbow::CHAR_POLICY.length();
    long long modulo = pow(nbr_char_policy, pass_size+1);
    std::cout << "Pour 6: " << modulo << std::endl;
    long long int_hash = string_to_int(new_hash.c_str(), nbr_char_policy, modulo);
    std::cout << "Int of hash: " << int_hash << std::endl;
    std::cout << "Generated password: " << int_to_string(int_hash, nbr_char_policy, pass_size) << std::endl;


    // int int_hash = std::stoi(new_hash);
    // std::cout << "Integer: " << (new_hash.c_str()) << std::endl;

    // TODO
    return hash_value;
}

