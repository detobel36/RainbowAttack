#include <string>
#include <cstring>
#include <iostream>
#include <fstream>
#include "passwd-utils.hpp"

const int PASS_SIZE = 6;
const bool DEBUG = true;

struct Rainbow_row
{
    char pass_head[PASS_SIZE+1];
    char pass_tail[PASS_SIZE+1];
};

void read_table(std::string file_name) // display data from file:
{
    std::ifstream table_file(file_name, std::ios::out | std::ios::binary);

    std::cout << "coucou read" << std::endl;
    if(table_file)
    {

        Rainbow_row result_row;
        while(table_file.read((char *) &result_row, sizeof(Rainbow_row)))
        {
            std::cout << "Head: " << result_row.pass_head << std::endl;
            std::cout << "Tail: " << result_row.pass_tail << std::endl;
        }
        
        table_file.close();
    }
}

void generate_table(std::string output_file)
{
    // Create file
    std::ofstream table_file(output_file, std::ios::out | std::ios::binary);

    int i = 0;
    while(i < 3)
    {
        // Generate password
        std::string generate_password = rainbow::generate_passwd(PASS_SIZE);
        if(DEBUG)
        {
            std::cout << "Genrated password: " << generate_password << std::endl;
        }

        // Create structure
        Rainbow_row new_row;
        std::strncpy(new_row.pass_head, generate_password.c_str(), sizeof(new_row.pass_head)-1);
        new_row.pass_head[PASS_SIZE] = '\0';

        // TODO hash - reduce - hash ...

        std::strncpy(new_row.pass_tail, generate_password.c_str(), sizeof(new_row.pass_tail)-1);
        new_row.pass_tail[PASS_SIZE] = '\0';

        if(table_file)
        {
            table_file.write((char *) &new_row, sizeof(Rainbow_row));
        }

        ++i;
    }

    table_file.close();
}


int main(int argc, char *argv[])
{
    std::string file_name = "Employe.txt";

    generate_table(file_name);
    read_table(file_name);
}
