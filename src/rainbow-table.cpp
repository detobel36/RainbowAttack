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
int pass_size = DEFAULT_PASS_SIZE;

// Number of passwords in the rainbow table
const int DEFAULT_NBR_PASS = 1000000;
int nbr_pass = DEFAULT_NBR_PASS;

// Number of hash - reduce done before to have the tail
// Warning: begin at 1 and end include "NBR_LOOP" (so [1, NBR_LOOP])
const int DEFAULT_NBR_LOOP = 50000;
int nbr_loop = DEFAULT_NBR_LOOP;

// Display some debug message
int debug_level = 0; // 0 = no debug, 1 = some message, 2 = all message

// Max memore size used to store rainbow row
const int MAX_ELEMENT_PER_BATCH = 100000;
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

void store_batch_in_file(char* batch_passwords, int nbr_password_in_batch, std::string output_file, 
    std::size_t total_batch) {

    // Sort here
    qsort(batch_passwords, nbr_password_in_batch, 2*(pass_size+1)*sizeof(char), sort_array);

    // Create work file
    std::ofstream table_file(output_file + std::to_string(total_batch) + ".txt", std::ios::out | std::ios::binary);
    // Write to file
    table_file.write(batch_passwords, nbr_password_in_batch*2*(pass_size+1)*sizeof(char));
    // Close file
    table_file.close();
}


// TODO optimise here.  Maybe sort directly when password are fetch (with a heap)
// Method to have min element index in a sequence...
int index_min_element(char*** first_passwords, std::size_t total_password) {
    std::size_t index = 0;
    int min_index = -1;

    while (index < total_password) {
        if ((char) *((char *) (first_passwords+(index*2*(pass_size+1))+(pass_size+1))) != '\0' and (min_index < 0 or 
            strcmp((char *) (first_passwords+(index*2*(pass_size+1))+(pass_size+1)), 
                (char *) (first_passwords+(min_index*2*(pass_size+1))+(pass_size+1))) < 0)) {
            min_index = index;
        }
        ++index;
    }
    return min_index;
}

void generate_table(std::string output_file) {

    int max_batch_elements = MAX_ELEMENT_PER_BATCH;
    if (debug_level > 0) {
        std::cout << "Max number of element in a batch: " << max_batch_elements << std::endl;
    }
    char* batch_passwords = (char*) malloc(max_batch_elements*2*(pass_size+1));

    int i = 0;
    int current_batch = 0;
    std::size_t total_batch = 0;
    while (i < nbr_pass) {
        // Generate password
        std::string generate_password = rainbow::generate_passwd(pass_size);
        if (debug_level > 0 or i == nbr_pass/2) {
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
            if (debug_level > 1 and j != nbr_loop) {
                std::cout << "Computed intermediary password: " << computed_pass_tail << std::endl;
            }
        }

        if (debug_level > 0 or i == nbr_pass/2) {
            std::cout << "Computed tail password: " << computed_pass_tail << std::endl;
            std::cout << std::endl; // Add space
        }

        // Create structure
        std::strncpy((char*) (batch_passwords+(current_batch*2*(pass_size+1))), 
            generate_password.c_str() + '\0', (pass_size+1)*sizeof(char));
        std::strncpy((char*) (batch_passwords+(current_batch*2*(pass_size+1))+pass_size+1), 
            computed_pass_tail.c_str() + '\0', (pass_size+1)*sizeof(char));
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
    free(batch_passwords);

    if (debug_level > 1) {
        std::cout << " === Read all files === " << std::endl;
        for (std::size_t i = 0; i < total_batch; ++i) {
            read_all_table(output_file + std::to_string(i) + ".txt");
        }
        std::cout << " ====================== " << std::endl;
    }

    // Open result file
    std::ofstream result_table_file(output_file + ".txt", std::ios::out | std::ios::binary);
    
    // Open all file and read first pass head and tail
    char*** first_passwords = (char***) malloc(total_batch*2*(pass_size+1));
    std::ifstream tmp_table_file[total_batch];
    for (std::size_t file_index = 0; file_index < total_batch; ++file_index) {
        tmp_table_file[file_index] = std::ifstream(output_file + std::to_string(file_index) + ".txt", 
            std::ios::out | std::ios::binary);

        // Head
        tmp_table_file[file_index].read((char *) (first_passwords+(file_index*2*(pass_size+1))), (pass_size+1)*sizeof(char));
        // Tail
        tmp_table_file[file_index].read((char *) (first_passwords+(file_index*2*(pass_size+1))+(pass_size+1)), pass_size+1*sizeof(char));
    }

    int index_pass = 0;
    int nbr_doublon = 0;
    char* last_tail_password = (char *) malloc(pass_size+1);
    while (index_pass < nbr_pass) {

        int min_element = index_min_element(first_passwords, total_batch);

        if (last_tail_password != ((char *) first_passwords+(min_element*2*(pass_size+1))+(pass_size+1))) {
            memcpy(last_tail_password, first_passwords+(min_element*2*(pass_size+1))+(pass_size+1), pass_size+1);
            result_table_file.write((char *) (first_passwords+(min_element*2*(pass_size+1))), (pass_size+1)*sizeof(char));
            result_table_file.write((char *) (first_passwords+(min_element*2*(pass_size+1))+(pass_size+1)), (pass_size+1)*sizeof(char));
        } else {
            ++nbr_doublon;
        }

        // Head
        tmp_table_file[min_element].read((char *) (first_passwords + (min_element*2*(pass_size+1))), (pass_size+1)*sizeof(char));
        // Tail
        bool result = (bool) tmp_table_file[min_element].read((char *) (first_passwords + (min_element*2*(pass_size+1)) + pass_size+1), (pass_size+1)*sizeof(char));

        // If we couldn't read the next element, file is finish
        if (!result) {
            std::strcpy((char*) (first_passwords+(min_element*2*(pass_size+1))), "\0");
            std::strcpy((char*) (first_passwords+(min_element*2*(pass_size+1) + pass_size+1)), "\0");
        }
        ++index_pass;
    }
    std::cout << nbr_doublon << "/" << nbr_pass << std::endl;
    free(first_passwords);

    for (std::size_t file_index = 0; file_index < total_batch; ++file_index) {
        // Close file
        tmp_table_file[file_index].close();
        // Delete file
        std::string file_name = output_file + std::to_string(file_index) + ".txt";
        if (debug_level > 0) {
            std::cout << "Delete file: " << file_name << std::endl;
        }
        remove(file_name.c_str());
    }
    result_table_file.close();
}

void display_help(char* first_argument) {
    std::cout << "Help to execute: " << first_argument << std::endl
        << "\t-h\t\t\tDisplay this help" << std::endl
        << "\t-d <level>\t\tSet debug level message (0 to disable)" << std::endl
        << "\t-t <file name>\t\tGenerate table and store in file " << 
            "(WITHOUT extention)" << std::endl
        << "\t-p <size>\t\tSet password size (default: " << DEFAULT_PASS_SIZE << ")" << 
            std::endl
        << "\t-n <number>\t\tNumber of password to generate " << 
            "(default: " << DEFAULT_NBR_PASS << ")" << std::endl
        << "\t-l <number>\t\tNumber of hash/reduce (default: " << DEFAULT_NBR_LOOP << ")" << 
            std::endl
        << "\t-r <file name>\t\tRead table (used for debug)" << std::endl
        << "\t-s <table file name>\tTry to find password of hash using the table passed " << 
            "as a parameter (WITHOUT extention) (required '-f')" << std::endl
        << "\t-f <hash file name>\tFile that contain hash that must be tested (required '-s')" << 
            std::endl 
        << "\t-o <file name>\tOutput name of results (required '-f' & '-s')" << std::endl;
}


int main(int argc, char *argv[]) {

    if (argc < 2) {
        std::cout << "No operation specified" << std::endl;
        display_help(argv[0]);
    } else {
        char* generate_table_name = NULL;
        char* search_table_name = NULL;
        char* hash_file_name = NULL;
        char* output_file_name = "output.txt";
        int nbr_loop = 10;

        int param;
        while ((param = getopt(argc, argv, "p:t:n:l:r:d:s:f:o:h")) != -1) {
            switch (param) {
                case 'd':
                    if (optarg) {
                        debug_level = std::atoi(optarg);
                    }
                    break;

                case 't':
                    if (optarg) {
                        generate_table_name = optarg;
                        if (debug_level > 0) {
                            std::cout << "Generate table in file: " << generate_table_name << std::endl;
                        }
                    }
                    break;

                case 'p':
                    if (optarg) {
                        pass_size = std::atoi(optarg);
                        if (debug_level > 0) {
                            std::cout << "Password size: " << pass_size << std::endl;
                        }
                    }
                    break;

                case 'n':
                    if (optarg) {
                        nbr_pass = std::atoi(optarg);
                        if (debug_level > 0) {
                            std::cout << "Number of password that must be generated: " << nbr_pass << std::endl;
                        }
                    }
                    break;

                case 'l':
                    if (optarg) {
                        nbr_loop = std::atoi(optarg);
                        if (debug_level > 0) {
                            std::cout << "Number of hash/reduce: " << nbr_loop << std::endl;
                        }
                    }
                    break;

                case 'r':
                    if (optarg) {
                        read_all_table(optarg);
                    }
                    break;

                case 's':
                    if (optarg) {
                        search_table_name = optarg;
                    }
                    break;

                case 'f':
                    if (optarg) {
                        hash_file_name = optarg;
                    }
                    break;

                case 'o':
                    if (optarg) {
                        output_file_name = optarg;
                    }
                    break;

                case 'h':
                    display_help(argv[0]);
                    return 0;

                case '?':
                    if (isprint(optopt)) {
                        fprintf(stderr, "Unknown option `-%c'.\n", optopt);
                    } else {
                        fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt);
                    }
                    return 1;

                default:
                    std::cout << "No option define" << std::endl;
            }
        }

        for (int index = optind; index < argc; ++index) {
            printf("Non-option argument %s\n", argv[index]);
        }

        ///////////////////////////////
        // GENERATE RAINBOW TABLE
        if (generate_table_name != NULL) {
            generate_table(generate_table_name);
        }

        ///////////////////////////////
        // SEARCH PASS USING TABLE
        // If search or hash is define (and not the other), display a warning
        if ((search_table_name == NULL or hash_file_name == NULL) and 
            search_table_name != hash_file_name) {
            std::cout << "WARNING: you must define a file that contain hash (using '-f') that " << 
                "must be search by the table (define in file '-s')" << std::endl;
        } else if (search_table_name != NULL and hash_file_name != NULL) {
            if (generate_table_name != NULL and search_table_name != generate_table_name) {
                std::cout << "INFO: you generate a table in '" << generate_table_name << "' and " <<
                    "you use another table to search ('" << search_table_name << "')" << std::endl;
            }

            std::ifstream hash_file(hash_file_name);
            std::ofstream output_file(output_file_name);
            std::string hash_str;
            std::string result;
            while (std::getline(hash_file, hash_str)) {
                result = search_in_table(std::string(search_table_name) + ".txt", hash_str);
                std::cout << "Result: " << result << std::endl;
                output_file << result + "\n";
            }
            hash_file.close();
            output_file.close();
        }

    }
    return 0;
}
