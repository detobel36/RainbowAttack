#include <iostream>

#include <map>


#include "passwd-utils.hpp"
#include "reverse.cpp"
#include "sha256.h"


int main(int argc, char *argv[]) {
    // std::string hash_char ("abcdefghijklmnopqrstuvwxy0123456789");

    // std::map<std::string, int> corresponding_table;
    // int total_encode = 0;
    // int compteur_num_char_policy = 0;
    // for(int i = 0; i < 36; ++i) {
    //     for(int j = 0; j < 36; ++j) {
    //         std::string lettre_one (1, hash_char[i]);
    //         std::string lettre_two (1, hash_char[j]);

    //         std::string two_hash_char = lettre_one + lettre_two;

    //         if (total_encode < 1240) {
    //             corresponding_table[two_hash_char] = rainbow::CHAR_POLICY[compteur_num_char_policy];
    //             std::cout << compteur_num_char_policy << " - " << two_hash_char << " - " << rainbow::CHAR_POLICY[compteur_num_char_policy] << std::endl;
    //             std::cout << "Match: " << ("80" == two_hash_char) << std::endl;
    //             ++compteur_num_char_policy;
    //             compteur_num_char_policy %= rainbow::CHAR_POLICY.length();
    //         } else {
    //             corresponding_table[two_hash_char] = -1;
    //         }

    //         total_encode += 1;
    //     }
    // }
    // std::cout << "total: " << total_encode << std::endl;

    int nbr_tests = 100000;
    int pass_size = 10;
    int index = 0;
    std::map<char, int> counterDict;

    for(int i = 0; i < rainbow::CHAR_POLICY.length(); ++i) {
        counterDict[rainbow::CHAR_POLICY[i]] = 0;
    }

    prepare_reverse_table();

    std::string init_str = sha256("passwo");
    for (long long i = 0; i < nbr_tests; ++i) {
        // std::cout << i << " - " << init_str << std::endl;
        init_str = reverse(index++, sha256(init_str), pass_size);
        std::cout << init_str << std::endl;
        for (int lettre = 0; lettre < pass_size; ++lettre) {
            counterDict[init_str[lettre]] +=1;
        }
    }

    std::cout << "Result:" << std::endl;
    int max_value = 0;
    int min_value = nbr_tests;

    for (auto& x: counterDict){
        std::cout << "Character distribution: " << x.first << " - " << x.second <<  " <=> " << 
            x.second/(float(nbr_tests)*pass_size) << "%" << std::endl;
        if (x.second > max_value) {
            max_value = x.second;
        } else if (x.second < min_value) {
            min_value = x.second;
        }
    }
    std::cout << std::endl;
    std::cout << "Min: " << min_value << " - Max: " << max_value << " (delta: " << (max_value - min_value) << ")" << std::endl;
    std::cout << "Percent difference: " << (max_value/(float(nbr_tests)*pass_size)) - (min_value/(float(nbr_tests)*pass_size)) << std::endl;


    // int index = 0;

    // Table with:
    // letter
    // position
    // number
    // Example: letter A, in position 2, with 56 occurance
    // int* statistiques[62][pass_size];

    // Init statistiques
    // for (int i = 0; i < 62; ++i) {
    //     for (int j = 0; j < pass_size; ++j) {
    //         statistiques[i][j] = 0;
    //     }
    // }

    // for (int i = 0; i < 62; ++i) {
    //     std::cout << rainbow::CHAR_POLICY[i] << "\t" << (int) rainbow::CHAR_POLICY[i] << "\t" <<  
    //         ((int) rainbow::CHAR_POLICY[i])%62 << std::endl;
    // }

    // for (int i = 0; i < nbr_tests; ++i) {
    //     std::string result = reverse(index, sha256(i), pass_size);
    //     for (int lettre = 0; lettre < pass_size; ++lettre) {
    //         statistiques[];
    //     }
    // }

}
