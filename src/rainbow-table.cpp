#include <string>
#include <cstring>
#include <iostream>
#include <fstream>
#include "passwd-utils.hpp"
#include "reverse.cpp"

const int PASS_SIZE = 6;
const int NBR_PASS = 3;
const int NBR_LOOP = 3;
const bool DEBUG = true;

struct Rainbow_row
{
    char pass_head[PASS_SIZE+1];
    char pass_tail[PASS_SIZE+1];
};

// Used for debug
void read_all_table(std::string file_name) // display data from file:
{
    std::ifstream table_file(file_name, std::ios::out | std::ios::binary);

    std::cout << std::endl;
    std::cout << "Content of Rainbow table:" << std::endl;
    if(table_file)
    {

        Rainbow_row result_row;
        while(table_file.read((char *) &result_row, sizeof(Rainbow_row)))
        {
            std::cout << result_row.pass_head << " - " << result_row.pass_tail << std::endl;
        }
        
        table_file.close();
    }
    std::cout << std::endl;
}

// Search in the table a specific hash to get the password
// Input: the table file name and the hash that need to be found
// Output: the password (which gives the hash_value passed as a parameter when it is hash) or NULL 
// if not found
std::string search_in_table(std::string table_file_name, std::string hash_value)
{
    std::ifstream table_file(table_file_name, std::ios::out | std::ios::binary);

    if(table_file)
    {
        int index = 0;

        Rainbow_row result_row;
        while(table_file.read((char *) &result_row, sizeof(Rainbow_row)))
        {
            // We begin to check the tail
            if(reverse(index, hash_value, PASS_SIZE) == result_row.pass_tail)
            {
                if(DEBUG)
                {
                    std::cout << "Match pass: " << result_row.pass_tail << " (need to recover previous password)" << std::endl;
                }
                // TODO recover previous password

                table_file.close();
                return result_row.pass_head;
            }

            // If not found, we compute from the first one
            int i = 0; // number of
            while(i < NBR_LOOP)
            {

                return ;
            }

        }
        
        table_file.close();
    }
    return NULL;
}

void generate_table(std::string output_file)
{
    // Create file
    std::ofstream table_file(output_file, std::ios::out | std::ios::binary);

    int i = 0;
    while(i < NBR_PASS)
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
        
        // TODO add a loop to do it multiple time
        std::string computed_pass_tail = generate_password;
        for(int i = 0; i < NBR_LOOP; ++i)
        {
            computed_pass_tail = reverse(i, sha256(computed_pass_tail), PASS_SIZE);
        }

        std::strncpy(new_row.pass_tail, computed_pass_tail.c_str(), sizeof(new_row.pass_tail)-1);
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
    read_all_table(file_name);

    reverse(1, "tEsaxa", PASS_SIZE);
    reverse(1, "tsxaxa", PASS_SIZE);
    reverse(1, "Oa2axa", PASS_SIZE);
    reverse(1, "wEsaxa", PASS_SIZE);
    reverse(1, "tZEaxa", PASS_SIZE);
    reverse(1, "Oa2axa", PASS_SIZE);
    reverse(1, "wEsaxa", PASS_SIZE);
}
