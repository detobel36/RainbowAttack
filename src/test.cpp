#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <stdio.h>
#include <stdlib.h>

const int PASS_SIZE = 2;

int main(int argc, char *argv[]) {
    int total_batch = 2;
    int pass_size = 6;

    char*** first_passwords = (char***) malloc(total_batch*2*(pass_size+1));

    std::ifstream fichier = std::ifstream("result.txt",  std::ios::out | std::ios::binary);
    fichier.read((char *) first_passwords, (pass_size+1)*sizeof(char));
    std::cout << "Read: " << (char *) first_passwords << std::endl;

    fichier.read((char *) (first_passwords+pass_size+1), (pass_size+1)*sizeof(char));
    std::cout << "Read 2: " << (char *) (first_passwords+pass_size+1) << std::endl;


    std::cout << std::endl;
    std::cout << "all First password" << std::endl;                                                 
    for(int num_pass = 0; num_pass < total_batch; ++num_pass) {

        std::cout << "-- " << (char*) (first_passwords+(num_pass*2*(pass_size+1))+pass_size+1) << std::endl;

        std::cout << (char*) (first_passwords+(num_pass*2*(pass_size+1))) <<
            ", " << (char*) (first_passwords+((num_pass*2*(pass_size+1))+(pass_size+1))) << std::endl;

        // Marche: (char*) (first_passwords+(num_pass*2*(pass_size+1))+pass_size+1)
        // Marche pas: ((char*) first_passwords+((num_pass*2*(pass_size+1))+(pass_size+1)))
    }


    // char pass[2][3];
    // pass[0][0] = 'a';
    // pass[0][1] = 'b';
    // pass[0][2] = '\0';

    // pass[1][0] = 'c';
    // pass[1][1] = 'd';
    // pass[1][2] = '\0';

    // std::string test = "ok";
    // int index = 0;
    // std::string new_test = test + std::to_string(index) + " new";
    // std::cout << "Test: " << new_test << std::endl;

    // std::ofstream table_file("test12.txt", std::ios::out | std::ios::binary);
    // table_file.write((char *) &pass, 6*sizeof(char));
    // std::cout << "Write: " << &pass << std::endl;
    // // table_file.write((char *) &pass_2, 2*sizeof(char));
    // table_file.close();
    // std::cout << "end compute" << std::endl;

    // // Test read
    // char result_row[3];
    // std::ifstream table_file_out("test12.txt", std::ios::out | std::ios::binary);
    // while (table_file_out.read((char *) &result_row, 3*sizeof(char))) {
    //     std::cout << "res: " << result_row << std::endl;
    // }
    // table_file_out.close();

    // // Test read all and split after
    // char result_one[3];
    // char result_two[3];
    // std::ifstream table_file_out2("test12.txt", std::ios::out | std::ios::binary);
    // table_file_out2.read((char *) &result_one, 3*sizeof(char));
    // table_file_out2.read((char *) &result_two, 3*sizeof(char));
    // std::cout << "res2: " << result_one << " - " << result_two << std::endl;
    
    // table_file_out2.close();

}
