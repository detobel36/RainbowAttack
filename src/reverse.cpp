#include <string>
#include <math.h>
#include <map>

// WARNING:
// Must made some tests to be sure that the distribution is correct.  It seems that the last letter
// is always compute with a small number ([0-9])

std::map<char,int> alpha_num_table;


void prepare_table() {
    for (int i = 0; i < rainbow::CHAR_POLICY.length(); ++i) {
        alpha_num_table[rainbow::CHAR_POLICY[i]] = i;
    }
}


std::string reverse(long long index, std::string hash, int pass_size){
    int primes[] = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97};
    int prime_sz = sizeof(primes)/sizeof(*primes);

    // int answer = 42;
    std::string result_password = "";
    long long char_idx;
    int nbr_char_policy = rainbow::CHAR_POLICY.length();
    std::string chars = rainbow::CHAR_POLICY;
    int len_hash = hash.length();

    long long total_int = 0;
    for (int i = 0; i < (pass_size+1); ++i) {
        total_int += alpha_num_table.find(hash[i])->second;
        total_int *= 36;
    }
    total_int += index;

    for (int i = 0; i < pass_size; ++i) {
        result_password += chars[total_int%nbr_char_policy];
        total_int /= nbr_char_policy;
    }


    // for (int i = 0; i < pass_size; ++i) {
    //     int first_char_value = ((alpha_num_table.find(hash[i*2])->second)*36 + 
    //                             (alpha_num_table.find(hash[i*2+1])->second) + 
    //                             index)%nbr_char_policy;


        // int first_char_value = (int) ((hash[i*2]*36) + (hash[i*2+1]) + index)%nbr_char_policy;
        // result_password += chars[first_char_value];

        // char_idx = (int) hash[(i+primes[i%prime_sz])%len_hash] + \
        //             (int) hash[(i+primes[(i*2)%prime_sz])%len_hash] + \
        //             (int) hash[(i+primes[(i*3)%prime_sz])%len_hash] + \
        //             (int) hash[(i+primes[(i*4)%prime_sz])%len_hash] + 
        //             (int)hash[(i+primes[(i*5)%prime_sz])%len_hash];
        // char_idx*=pow(nbr_char_policy,pass_size)+index;

        //char_idx = ((int)hash[(i+answer)%len_hash])*pow(index,i)+((int)hash[(i+index)%len_hash])*pow(answer,i) + ((int)hash[(i+answer+index)%len_hash])*pow(answer,index);
        //std::cout<<"char_idx: "<<char_idx<<std::endl;
        
        // result_password += chars[char_idx%nbr_char_policy];
    // }
    return result_password;
}