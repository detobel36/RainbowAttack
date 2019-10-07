#include <string>
#include <cstring>
#include <iostream>
#include <fstream>
#include <stdio.h>

#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>

#include "passwd-utils.hpp"
#include "reverse.cpp"

// Size of passwords
const int DEFAULT_PASS_SIZE = 6; // Didn't include '\0' at the end
const int pass_size = DEFAULT_PASS_SIZE;

// Number of passwords in the rainbow table
const int DEFAULT_NBR_PASS = 20;
const int nbr_pass = DEFAULT_NBR_PASS;

// Number of hash - reduce done before to have the tail
// Warning: begin at 1 and end include "NBR_LOOP" (so [1, NBR_LOOP])
const int DEFAULT_NBR_LOOP = 3;
const int nbr_loop = DEFAULT_NBR_LOOP;

// Display some debug message
const int DEBUG_LEVEL = 2; // 0 = no debug, 1 = some message, 2 = all message

// Max memore size used to store rainbow row
const int MAX_ELEMENT_PER_BATCH = 10;
const int MEMORY_SIZE = MAX_ELEMENT_PER_BATCH*((nbr_pass+1)*2); // Store 100 password per file
// Note, to have 1Go -> 1 000 000 000 char per file

int sort_array (const void *a, const void *b) {
    // TODO optimise here

    // Get the first element and must sort based on the second
    char * new_a = ((char *) a) + pass_size + 1;
    char * new_b = ((char *) b) + pass_size + 1;
    return strcmp(new_a, new_b);
}


// Used for debug
void read_all_table(std::string file_name) { // display data from file:
    std::ifstream table_file(file_name, std::ios::out | std::ios::binary);

    std::cout << std::endl;
    std::cout << "Read file " << file_name << ":" << std::endl;
    if (table_file) {
        std::cout << "Pass size: " << pass_size << std::endl;

        char test[pass_size+1];
        char test2[pass_size+1];
        while (table_file.read((char *) &test, (pass_size+1)*sizeof(char)) and
            table_file.read((char *) &test2, (pass_size+1)*sizeof(char))) {
            std::cout << test << " - " << test2 << std::endl;
        }

        table_file.close();
    }
    std::cout << std::endl;
}

std::string dichotomique_search(std::ifstream & file, int start, int end, std::string search_pass) {
    if (start > end) { // If we have search all elements
        return "";
    }

    // If we select only one element
    if (start == end) {
        file.seekg(start*2*(pass_size+1), file.beg);  // Return the head

        char pass_head[pass_size+1];
        char pass_tail[pass_size+1];

        file.read((char *) &pass_head, (pass_size+1)*sizeof(char));
        file.read((char *) &pass_tail, (pass_size+1)*sizeof(char));

        if (pass_tail == search_pass) {
            return pass_head;
        } else {
            return ""; // If not found, return empty
        }
    }

    // Else
    int middle = start + ((end-start)/2);
    file.seekg(middle*2*(pass_size+1)+(pass_size+1), file.beg); // Move to middle + one (to have tail)

    char pass_tail[pass_size+1];
    file.read((char *) &pass_tail, (pass_size+1)*sizeof(char));
    
    int comparison_value = strcmp(search_pass.c_str(), pass_tail);
    if (comparison_value == 0) {
        return dichotomique_search(file, middle, middle, search_pass);
    } else if (comparison_value < 0) {  // If search_pass is smaller than pass_tail
        return dichotomique_search(file, start, middle-1, search_pass);
    } else {
        return dichotomique_search(file, middle+1, end, search_pass);
    }
}

// Search in the table a specific hash to get the password
// Input: the table file name and the hash that need to be found
// Output: the password (which gives the hash_value passed as a parameter when it is hash) or NULL 
// if not found
std::string search_in_table(std::string table_file_name, std::string hash_value) {
    std::ifstream table_file(table_file_name, std::ios::out | std::ios::binary);

    if (table_file) {
        int index_hash_reduce = nbr_loop;
        
        std::string compute_pass = reverse(index_hash_reduce, hash_value, pass_size);
        while (index_hash_reduce >= 1) {
            std::string pass_head = dichotomique_search(table_file, 0, nbr_pass, compute_pass);
            if (pass_head != "") {  // If we have found
                for (int i = 1; i < index_hash_reduce; ++i) {
                    pass_head = reverse(i, sha256(pass_head), pass_size);
                }
                table_file.close();
                return pass_head;
            }

            // Else, compute new password
            --index_hash_reduce;
            if (index_hash_reduce > 0) {
                compute_pass = reverse(index_hash_reduce, hash_value, pass_size);
                for (int i = index_hash_reduce+1; i <= nbr_loop; ++i) {
                    compute_pass = reverse(i, sha256(compute_pass), pass_size);
                }
            }
        }
        table_file.close();
    }
    return "";
}

void store_batch_in_file(char batch_passwords[][2][pass_size+1], 
        int nbr_password_in_batch, std::string output_file, int total_batch) {

    // Sort here
    qsort(batch_passwords, nbr_password_in_batch, 2*(pass_size+1)*sizeof(char), sort_array);

    // Create work file
    std::ofstream table_file(output_file + std::to_string(total_batch) + ".txt", std::ios::out | std::ios::binary);
    // Write to file
    table_file.write((char *) batch_passwords, nbr_password_in_batch*2*(pass_size+1)*sizeof(char));
    // Close file
    table_file.close();
}


// TODO optimise here.  Maybe sort directly when password are fetch (with a heap)
// Method to have min element index in a sequence...
int index_min_element(char*** first_passwords, int total_password) {
    std::size_t index = 0;
    int min_index = -1;

    while (index < total_password) {
        std::cout << "Compare: " << "(index: " << index << ") " << (char *) (first_passwords+(index*2*(pass_size+1))+(pass_size+1)) << " - " << 
                " (min index: " << min_index << ") " << (char *) (first_passwords+(min_index*2*(pass_size+1))+(pass_size+1)) << std::endl;

        if ((char) *((char *) (first_passwords+(index*2*(pass_size+1))+(pass_size+1))) != '\0' and (min_index < 0 or 
            strcmp((char *) (first_passwords+(index*2*(pass_size+1))+(pass_size+1)), 
                (char *) (first_passwords+(min_index*2*(pass_size+1))+(pass_size+1))) < 0)) {
            std::cout << "index_min_element " << index << " = " << (char*) (first_passwords+(index*2*(pass_size+1))+(pass_size+1)) << std::endl;
            min_index = index;
        }
        ++index;
    }
    return min_index;
}

void generate_table(std::string output_file) {

    int max_batch_elements = MAX_ELEMENT_PER_BATCH;
    if (DEBUG_LEVEL > 0) {
        std::cout << "Max number of element in a batch: " << max_batch_elements << std::endl;
    }
    char batch_passwords[max_batch_elements][2][pass_size+1];

    int i = 0;
    int current_batch = 0;
    int total_batch = 0;
    while (i < nbr_pass) {
        // Generate password
        std::string generate_password = rainbow::generate_passwd(pass_size);
        if (DEBUG_LEVEL > 0 or i == nbr_pass/2) {
            std::cout << "Generated password (" << i << "): " << generate_password << std::endl;
        }

        // Compute tail password        
        std::string computed_pass_tail = generate_password;
        for (int j = 1; j <= nbr_loop; ++j) {

            if (i == nbr_pass/2) {
                std::cout << "Display hash and password to made some tests" << std::endl;
                std::cout << "pass: " << computed_pass_tail << " -> hash:" << sha256(computed_pass_tail) << std::endl;
            }

            computed_pass_tail = reverse(j, sha256(computed_pass_tail), pass_size);
            if (DEBUG_LEVEL > 1 and j != nbr_loop) {
                std::cout << "Computed intermediary password: " << computed_pass_tail << std::endl;
            }
        }

        if (DEBUG_LEVEL > 0 or i == nbr_pass/2) {
            std::cout << "Computed tail password: " << computed_pass_tail << std::endl;
            std::cout << std::endl; // Add space
        }

        // Create structure
        std::strncpy(batch_passwords[current_batch][0], generate_password.c_str() + '\0', (pass_size+1)*sizeof(char));
        std::strncpy(batch_passwords[current_batch][1], computed_pass_tail.c_str() + '\0', (pass_size+1)*sizeof(char));
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
    char*** first_passwords = (char***) malloc(total_batch*2*(pass_size+1));
    std::cout << "After malloc (" << first_passwords << " + " << (total_batch*2*(pass_size+1)) << ")" << std::endl;
    std::ifstream tmp_table_file[total_batch];
    std::cout << "total_batch: " << total_batch << std::endl;
    for (int file_index = 0; file_index < total_batch; ++file_index) {
        tmp_table_file[file_index] = std::ifstream(output_file + std::to_string(file_index) + ".txt", 
            std::ios::out | std::ios::binary);

        // Head
        tmp_table_file[file_index].read((char *) (first_passwords+(file_index*2*(pass_size+1))), (pass_size+1)*sizeof(char));
        // Tail
        tmp_table_file[file_index].read((char *) (first_passwords+(file_index*2*(pass_size+1))+(pass_size+1)), pass_size+1*sizeof(char));
    }
    std::cout << "Begin problem part ----" << std::endl;

    int index_pass = 0;
    int nbr_doublon = 0;
    char* last_tail_password = (char *) malloc(pass_size+1);
    while (index_pass < nbr_pass) {
        // std::cout << "Début boucle" << std::endl;
        // for (int file_index = 0; file_index < total_batch; ++file_index) {
        //     std::cout << "first_passwords (" << file_index << "): [" << 
        //         (char *) (first_passwords+(file_index*2*(pass_size+1))) << ", " <<
        //         (char *) (first_passwords+(file_index*2*(pass_size+1))+(pass_size+1)) <<
        //         "]" << std::endl;
        // }

        // std::cout << "Avant Compare (0): " << ((char *) first_passwords+(pass_size+1)) << " (addr: " << 
        // first_passwords << " - " << (pass_size+1) << ")" << std::endl;
        int min_element = index_min_element(first_passwords, total_batch);
        std::cout << "Min element: " << min_element << std::endl;

        if (last_tail_password != ((char *) first_passwords+(min_element*2*(pass_size+1))+(pass_size+1))) {
            memcpy(last_tail_password, first_passwords+(min_element*2*(pass_size+1))+(pass_size+1), pass_size+1);
            result_table_file.write((char *) (first_passwords+(min_element*2*(pass_size+1))), (pass_size+1)*sizeof(char));
            result_table_file.write((char *) (first_passwords+(min_element*2*(pass_size+1))+(pass_size+1)), (pass_size+1)*sizeof(char));
        } else {
            ++nbr_doublon;
        }
        // std::cout << "min_element: " << min_element << std::endl;

        // std::cout << "Pointer " << min_element << " (0) = " << ((int) tmp_table_file[min_element].tellg()) << std::endl;

        // Head
        tmp_table_file[min_element].read((char *) (first_passwords + (min_element*2*(pass_size+1))), (pass_size+1)*sizeof(char));
        // std::cout << "Pointer " << min_element << " (1) = " << 
        //     ((int) tmp_table_file[min_element].tellg()) << " (val: " << 
        //     (char  *) (first_passwords + (min_element*2*(pass_size+1))) << " - addr: " <<
        //     first_passwords << " - " << ((min_element*2*(pass_size+1))) << ")" << std::endl;
        // Tail
        bool result = (bool) tmp_table_file[min_element].read((char *) (first_passwords + (min_element*2*(pass_size+1)) + pass_size+1), (pass_size+1)*sizeof(char));
        // std::cout << "Pointer " << min_element << " (2) = " << 
        //     ((int) tmp_table_file[min_element].tellg()) << " (val: " <<
        //     (char *) (first_passwords + (min_element*2*(pass_size+1)) + pass_size+1) << " - addr: " << 
        //     first_passwords << " - " << ((min_element*2*(pass_size+1)) + pass_size+1) << ")" << std::endl;

        // If we couldn't read the next element, file is finish
        if (!result) {
            std::cout << "finish file" << std::endl;
            *((char *) first_passwords+(min_element*2*(pass_size+1))) = '\0';
            *((char *) first_passwords+(min_element*2*(pass_size+1))+pass_size+1) = '\0';
            std::cout << "end finish file" << std::endl;
        } else {
            std::cout << "Read: " << (char *) (first_passwords + (min_element*2*(pass_size+1)) + pass_size+1) << std::endl;
            std::cout << std::endl;
        }

        // std::cout << "Fin boucle" << std::endl;
        // for (int file_index = 0; file_index < total_batch; ++file_index) {
        //     std::cout << "first_passwords (" << file_index << "): [" << 
        //         (char *) (first_passwords+(file_index*2*(pass_size+1))) << ", " <<
        //         (char *) (first_passwords+(file_index*2*(pass_size+1))+(pass_size+1)) <<
        //         "] (addr: " << first_passwords << " - " << ((file_index*2*(pass_size+1))+(pass_size+1)) << ")" << std::endl;
        // }
        // std::cout << std::endl;

        ++index_pass;
    }
    std::cout << nbr_doublon << "/" << nbr_pass << std::endl;
    free(first_passwords);

    for (int file_index = 0; file_index < total_batch; ++file_index) {
        // Close file
        tmp_table_file[file_index].close();
        // Delete file
        std::string file_name = output_file + std::to_string(file_index) + ".txt";
        if (DEBUG_LEVEL > 0) {
            std::cout << "Delete file: " << file_name << std::endl;
        }
        remove(file_name.c_str());
    }
    result_table_file.close();
}


int main(int argc, char *argv[]) {

    // if (argc < 2) {
    //     std::cout << "No operation specified" << std::endl;
    // } else {
    //     char* generate_table_name = "";
    //     int nbr_loop = 10;

    //     int param;
    //     while ((param = getopt(argc, argv, "p:t:n:l:h")) != -1) {
    //         switch (param) {
    //             case 't':
    //                 if (optarg) {
    //                     generate_table_name = optarg;
    //                     if (DEBUG_LEVEL > 0) {
    //                         std::cout << "Generate table in file: " << generate_table_name << std::endl;
    //                     }
    //                 }
    //                 break;

    //             case 'p':
    //                 if (optarg) {
    //                     // pass_size = std::atoi(optarg);
    //                     if (DEBUG_LEVEL > 0) {
    //                         std::cout << "Password size: " << pass_size << std::endl;
    //                     }
    //                 }
    //                 break;

    //             case 'n':
    //                 if(optarg) {
    //                     // nbr_pass = std::atoi(optarg);
    //                     if (DEBUG_LEVEL > 0) {
    //                         std::cout << "Number of password that must be generated: " << nbr_pass << std::endl;
    //                     }
    //                 }
    //                 break;

    //             case 'l':
    //                 if(optarg) {
    //                     // nbr_loop = std::atoi(optarg);
    //                     if (DEBUG_LEVEL > 0) {
    //                         std::cout << "Number of hash/reduce: " << nbr_loop << std::endl;
    //                     }
    //                 }
    //                 break;

    //             case 'h':
    //                 std::cout << "Help to execute: " << argv[0] << std::endl;
    //                 std::cout << "\t-h\t\tDisplay this help" << std::endl;
    //                 std::cout << "\t-t <file name>\tGenerate table and store in file" << std::endl;
    //                 std::cout << "\t-p <size>\tSet password size (default: " << DEFAULT_PASS_SIZE << ")" << std::endl;
    //                 std::cout << "\t-n <number>\tNumber of password to generate (default: " << DEFAULT_NBR_PASS << ")" << std::endl;
    //                 std::cout << "\t-l <number>\tNumber of hash/reduce (default: " << DEFAULT_NBR_LOOP << ")" << std::endl;
    //                 return 0;

    //             case '?':
    //                 if (isprint(optopt))
    //                   fprintf(stderr, "Unknown option `-%c'.\n", optopt);
    //                 else
    //                   fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt);
    //                 return 1;

    //             default:
    //                 std::cout << "No option define" << std::endl;
    //         }
    //     }

    //     for (int index = optind; index < argc; ++index) {
    //         printf("Non-option argument %s\n", argv[index]);
    //     }

    //     if (generate_table_name != "") {
    //         generate_table(generate_table_name);
    //     }

    // }
    // return 0;

    std::string file_name = "test_size";

    generate_table(file_name);
    read_all_table(file_name + ".txt");

    /*
    Genrated password: 16UutT

    Compute hash: 387efcb493828b653163d832e584cc007cb226b364612812075e7c02b4a92741
    Computed intermediary password: 99w68r

    Compute hash: 2f031890be64b43f79402106df0cb1787a32cd80733017cb65b95bdeab04a94a
    Computed intermediary password: kr4jnz

    Compute hash: f8a11b5a282a865bfe750f43bb55b5de345e1a7be9db184c238edbb4ace2e63b
    Computed tail password: nxDlmz
    */


    // TODO ne retourne pas le bon résultat !
    // d0ab54c4fffe32871b1557d5d424f69391998b98253b3342d394cd29ef58ff5b
    // std::string result = search_in_table(file_name + ".txt", "d0ab54c4fffe32871b1557d5d424f69391998b98253b3342d394cd29ef58ff5b");
    // std::cout << "Result: " << result << std::endl;

    return 0;

}
