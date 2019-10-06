#include <string>
#include <math.h>

// WARNING:
// Must made some tests to be sure that the distribution is correct.  It seems that the last letter
// is always compute with a small number ([0-9])

std::string reverse(int index, std::string hash, int pass_size){
    int primes[] = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97};
    int answer = 42;
    std::string result_password = "";
    long long char_idx;
    int nbr_char_policy = rainbow::CHAR_POLICY.length();
    std::string chars = rainbow::CHAR_POLICY;
    int sub_value_idx=0;
    int a;
    int len_hash = hash.length();
    for(int i=0; i<pass_size; i++){

        char_idx = (int)hash[i+primes[i]] + (int)hash[i+primes[i*2]] + (int)hash[i+primes[i*3]] + (int)hash[i+primes[i*4]] + (int)hash[i+primes[i*5]] ; 
        char_idx*=pow(nbr_char_policy,pass_size)+index;

        //char_idx = ((int)hash[(i+answer)%len_hash])*pow(index,i)+((int)hash[(i+index)%len_hash])*pow(answer,i) + ((int)hash[(i+answer+index)%len_hash])*pow(answer,index);
        //std::cout<<"char_idx: "<<char_idx<<std::endl;
        result_password += chars[char_idx%nbr_char_policy];
    }
    return result_password;
}