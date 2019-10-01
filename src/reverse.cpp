#include <string>
#include <math.h>

// WARNING:
// Must made some tests to be sure that the distribution is correct.  It seems that the last letter
// is always compute with a small number ([0-9])

unsigned int string_to_int(const char* s, const int nbr_char_policy, const long long p) {
    long long int_result = 0;
    long long p_plus_one = p*nbr_char_policy;

    for (const char *car = s; *car; ++car) {
        int_result *= nbr_char_policy;
        int_result += (int)(*car);
        int_result %= p_plus_one;
    }
    return int_result % p;
}

std::string int_to_string(long long int_hash, const int nbr_char_policy, const int string_size) {
    std::string result = "";

    while(result.length() < string_size)
    {
        // Get new letter and add it to the final result
        result += rainbow::CHAR_POLICY[int_hash % nbr_char_policy];
        // Go to the next number
        int_hash /= nbr_char_policy;
    }
    return result;
}

std::string reverse(int index, std::string hash_value, int pass_size)
{
    std::string new_hash = sha256(hash_value);
    int nbr_char_policy = rainbow::CHAR_POLICY.length();
    long long modulo = pow(nbr_char_policy, pass_size);
    // Convert new hash in integer
    // TODO need to be enhance (it is maybe possible to directly cast string into an integer/binary)
    long long int_hash = string_to_int(new_hash.c_str(), nbr_char_policy, modulo);
    std::string result_password = int_to_string(int_hash, nbr_char_policy, pass_size);
    return result_password;
}
