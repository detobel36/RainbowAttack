#include <string>
#include <math.h>
#include <map>

// WARNING:
// Must made some tests to be sure that the distribution is correct.  It seems that the last letter
// is always compute with a small number ([0-9])

std::map<char,int> alpha_num_table;
int nbr_char_policy;

void prepare_reverse_table() {
    char hash_possibility[] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 
                                'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', 
                                '1', '2', '3', '4', '5', '6', '7', '8', '9', '0'};

    nbr_char_policy = rainbow::CHAR_POLICY.length();

    for (int i = 0; i < sizeof(hash_possibility); ++i) {
        alpha_num_table[hash_possibility[i]] = i;
    }
}


std::string reverse(long long index, std::string hash, int pass_size){
    std::string result_password = "";

    unsigned long long int total_int = 0;
    for (int i = 0; i < pass_size+1; ++i) {
        total_int *= nbr_char_policy-1;
        total_int += (alpha_num_table.find(hash[i*2])->second + 
                        alpha_num_table.find(hash[i*2+1])->second)%nbr_char_policy;
    }
    total_int += index;

    for (int i = 0; i < pass_size; ++i) {
        result_password += rainbow::CHAR_POLICY[total_int%nbr_char_policy];
        total_int /= nbr_char_policy;
    }
    return result_password;
}