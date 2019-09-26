#include <string>
#include <cstring>
#include <iostream>
#include <fstream>

struct Rainbow_row
{
    // TODO adapt to have fix size
    char pass_head[9];
    char pass_tail[9];
};

void read_table(std::string file_name) // display data from file:
{
    std::ifstream table_file(file_name, std::ios::out | std::ios::binary);

    std::cout << "coucou read" << std::endl;
    if(table_file)
    {

        Rainbow_row result_row[1];
        table_file.read((char *) &result_row[0], sizeof(Rainbow_row));
        std::cout << "Head: " << result_row[0].pass_head << std::endl;
        std::cout << "Tail: " << result_row[0].pass_tail << std::endl;
        
        table_file.close();
    }
}

void write_table(std::string file_name)
{

    std::ofstream table_file(file_name, std::ios::out | std::ios::binary);

    std::cout << "coucou write" << std::endl;

    Rainbow_row new_row[1];
    std::strncpy(new_row[0].pass_head, "12345678", sizeof(new_row[0].pass_head)-1);
    new_row[0].pass_head[8] = '\0';
    std::strncpy(new_row[0].pass_tail, "01234567", sizeof(new_row[0].pass_tail)-1);
    new_row[0].pass_tail[8] = '\0';

    if(table_file)
    {
        std::cout << "Save: " << &new_row[0] << std::endl;
        table_file.write((char *) &new_row[0], sizeof(Rainbow_row));
        // table_file << "test" << std::endl;
    }

    table_file.close();
}


int main(int argc, char *argv[])
{
    std::string file_name = "Employe.txt";
    write_table(file_name);
    read_table(file_name);
}
