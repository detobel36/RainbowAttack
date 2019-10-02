#include <iostream>
#include <fstream>
#include <string>
#include <cstring>

const int PASS_SIZE = 2;

int main(int argc, char *argv[]) {
    char pass[2][3];
    pass[0][0] = 'a';
    pass[0][1] = 'b';
    pass[0][2] = '\0';

    pass[1][0] = 'c';
    pass[1][1] = 'd';
    pass[1][2] = '\0';

    std::string test = "ok";
    int index = 0;
    std::string new_test = test + std::to_string(index) + " new";
    std::cout << "Test: " << new_test << std::endl;

    std::ofstream table_file("test12.txt", std::ios::out | std::ios::binary);
    table_file.write((char *) &pass, 6*sizeof(char));
    std::cout << "Write: " << &pass << std::endl;
    // table_file.write((char *) &pass_2, 2*sizeof(char));
    table_file.close();
    std::cout << "end compute" << std::endl;

    // Test read
    char result_row[3];
    std::ifstream table_file_out("test12.txt", std::ios::out | std::ios::binary);
    while (table_file_out.read((char *) &result_row, 3*sizeof(char))) {
        std::cout << "res: " << result_row << std::endl;
    }
    table_file_out.close();

    // Test read all and split after
    char result_one[3];
    char result_two[3];
    std::ifstream table_file_out2("test12.txt", std::ios::out | std::ios::binary);
    table_file_out2.read((char *) &result_one, 3*sizeof(char));
    table_file_out2.read((char *) &result_two, 3*sizeof(char));
    std::cout << "res2: " << result_one << " - " << result_two << std::endl;
    
    table_file_out2.close();

}
