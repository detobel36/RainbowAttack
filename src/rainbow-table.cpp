#include <string>
#include <cstring>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include "passwd-utils.hpp"
#include "reverse.cpp"

// Size of passwords
const int PASS_SIZE = 6; // Didn't include '\0' at the end
// Number of passwords in the rainbow table
const int NBR_PASS = 20;
// Number of hash - reduce done before to have the tail
// Warning: begin at 1 and end include "NBR_LOOP" (so [1, NBR_LOOP])
const int NBR_LOOP = 3;
// Display some debug message
const int DEBUG_LEVEL = 1; // 0 = no debug, 1 = some message, 2 = all message
// Max memore size used to store rainbow row
const int MEMORY_SIZE = 9*((PASS_SIZE+1)*2); // Store 50 password per file
// Note, to have 1Go -> 1 000 000 000 char per file

int sort_array (const void *a, const void *b) {
    // TODO optimise here

    // Get the first element and must sort based on the second
    char * new_a = ((char *) a) + PASS_SIZE + 1;
    char * new_b = ((char *) b) + PASS_SIZE + 1;
    return strcmp(new_a, new_b);
}


// Used for debug
void read_all_table(std::string file_name) { // display data from file:
    std::ifstream table_file(file_name, std::ios::out | std::ios::binary);

    std::cout << std::endl;
    std::cout << "Read file " << file_name << ":" << std::endl;
    if (table_file) {
        std::cout << "Pass size: " << PASS_SIZE << std::endl;

        char test[PASS_SIZE+1];
        char test2[PASS_SIZE+1];
        while (table_file.read((char *) &test, PASS_SIZE+1*sizeof(char)) and
            table_file.read((char *) &test2, PASS_SIZE+1*sizeof(char))) {
            std::cout << test << " - " << test2 << std::endl;
        }

        table_file.close();
    }
    std::cout << std::endl;
}

// Search in the table a specific hash to get the password
// Input: the table file name and the hash that need to be found
// Output: the password (which gives the hash_value passed as a parameter when it is hash) or NULL 
// if not found
// std::string search_in_table(std::string table_file_name, std::string hash_value) {
//     std::ifstream table_file(table_file_name, std::ios::out | std::ios::binary);

//     if (table_file) {

//         char result_row[2*(PASS_SIZE+1)];
//         while (table_file.read((char *) &result_row, 2*PASS_SIZE*sizeof(char))) {

//             int index = NBR_LOOP;
//             std::string compute_pass = reverse(index, hash_value, PASS_SIZE);
//             // TODO inverser les boucles ici.  Pour profiter de la recherche "rapide"
//             while (compute_pass != result_row.pass_tail and index > 1) {
//                 --index;
//                 compute_pass = reverse(index, sha256(compute_pass), PASS_SIZE);
//             }

//             if (compute_pass == result_row.pass_tail) { // If we correctly found the pass
//                 if (DEBUG) {
//                     std::cout << "Match pass: " << result_row.pass_tail << " after " << index << " matches" << std::endl;
//                 }
//                 table_file.close();
//                 return compute_pass;
//             }
//         }
        
//         table_file.close();
//     }
//     return NULL;
// }

void store_batch_in_file(char batch_passwords[][2][PASS_SIZE+1], 
        int nbr_password_in_batch, std::string output_file, int total_batch) {

    // Sort here
    qsort(batch_passwords, nbr_password_in_batch, 2*(PASS_SIZE+1)*sizeof(char), sort_array);

    // Create work file
    std::ofstream table_file(output_file + std::to_string(total_batch) + ".txt", std::ios::out | std::ios::binary);
    // Write to file
    table_file.write((char *) batch_passwords, nbr_password_in_batch*2*(PASS_SIZE+1)*sizeof(char));
    // Close file
    table_file.close();
}

// TODO optimise here.  Maybe sort directly when password are fetch (with a heap)
// Method to have min element index in a sequence...
int index_min_element(char first_passwords[][2][PASS_SIZE+1], int total_password) {
    std::size_t index = 0;
    int min_index = -1;

    while (index < total_password) {
        if (first_passwords[index][1][0] != '\0' and (min_index < 0 or 
            strcmp(first_passwords[index][1], first_passwords[min_index][1]) < 0)) {
            min_index = index;
        }
        ++index;
    }
    return min_index;
}

void generate_table(std::string output_file) {

    int max_batch_elements = MEMORY_SIZE/(2*(PASS_SIZE+1));
    if (DEBUG_LEVEL > 0) {
        std::cout << "Max number of element in a batch: " << max_batch_elements << std::endl;
    }
    char batch_passwords[max_batch_elements][2][PASS_SIZE+1];

    int i = 0;
    int current_batch = 0;
    int total_batch = 0;
    while (i < NBR_PASS) {
        // Generate password
        std::string generate_password = rainbow::generate_passwd(PASS_SIZE);
        if (DEBUG_LEVEL > 0) {
            std::cout << "Genrated password: " << generate_password << std::endl;
        }

        // Compute tail password        
        std::string computed_pass_tail = generate_password;
        for (int i = 1; i <= NBR_LOOP; ++i) {
            computed_pass_tail = reverse(i, sha256(computed_pass_tail), PASS_SIZE);
            if (DEBUG_LEVEL > 1 and i != NBR_LOOP) {
                std::cout << "Computed intermediary password: " << computed_pass_tail << std::endl;
            }
        }

        if (DEBUG_LEVEL > 0) {
            std::cout << "Computed tail password: " << computed_pass_tail << std::endl;
            std::cout << std::endl; // Add space
        }

        // Create structure
        std::strncpy(batch_passwords[current_batch][0], generate_password.c_str() + '\0', (PASS_SIZE+1)*sizeof(char));
        std::strncpy(batch_passwords[current_batch][1], computed_pass_tail.c_str() + '\0', (PASS_SIZE+1)*sizeof(char));
        ++current_batch;

        if (current_batch >= max_batch_elements) {
            store_batch_in_file(batch_passwords, current_batch, output_file, total_batch);
            // Reset batch information
            ++total_batch;
            current_batch = 0;
        }

        ++i;
    }

    // Save last batch
    if (current_batch > 0) {
        store_batch_in_file(batch_passwords, current_batch, output_file, total_batch);
        ++total_batch;
    }

    if (DEBUG_LEVEL > 1) {
        std::cout << " === Read all files === " << std::endl;
        for (int i = 0; i < total_batch; ++i) {
            read_all_table(output_file + std::to_string(i) + ".txt");
        }
        std::cout << " ====================== " << std::endl;
    }

    // Open result file
    std::ofstream result_table_file(output_file + ".txt", std::ios::out | std::ios::binary);
    
    // Open all file and read first pass head and tail
    char first_passwords[total_batch][2][PASS_SIZE+1];
    std::ifstream tmp_table_file[total_batch];
    for (int file_index = 0; file_index < total_batch; ++file_index) {
        tmp_table_file[file_index] = std::ifstream(output_file + std::to_string(file_index) + ".txt", 
            std::ios::out | std::ios::binary);

        // Head
        tmp_table_file[file_index].read((char *) &first_passwords[file_index][0], PASS_SIZE+1*sizeof(char));
        // Tail
        tmp_table_file[file_index].read((char *) &first_passwords[file_index][1], PASS_SIZE+1*sizeof(char));
    }

    int index_pass = 0;
    while (index_pass < NBR_PASS) {
        int min_element = index_min_element(first_passwords, total_batch);

        result_table_file.write((char *) first_passwords[min_element], 2*(PASS_SIZE+1)*sizeof(char));

        // Head
        tmp_table_file[min_element].read((char *) &first_passwords[min_element][0], PASS_SIZE+1*sizeof(char));
        // Tail
        bool result = (bool) tmp_table_file[min_element].read((char *) &first_passwords[min_element][1], PASS_SIZE+1*sizeof(char));

        // If we couldn't read the next element, file is finish
        if (!result) {
            first_passwords[min_element][0][0] = '\0';
            first_passwords[min_element][1][0] = '\0';
        }
        ++index_pass;
    }

    for (int file_index = 0; file_index < total_batch; ++file_index) {
        // Close file
        tmp_table_file[file_index].close();
        // Delete file
        std::string file_name = output_file + std::to_string(file_index) + ".txt";
        remove(file_name.c_str());
    }
    result_table_file.close();
}


int main(int argc, char *argv[]) {
    std::string file_name = "result";

    generate_table(file_name);
    read_all_table(file_name + ".txt");

    // reverse(1, "tEsaxa", PASS_SIZE);
    // reverse(1, "tsxaxa", PASS_SIZE);
    // reverse(1, "Oa2axa", PASS_SIZE);
    // reverse(1, "wEsaxa", PASS_SIZE);
    // reverse(1, "tZEaxa", PASS_SIZE);
    // reverse(1, "Oa2axa", PASS_SIZE);
    // reverse(1, "wEsaxa", PASS_SIZE);
}
