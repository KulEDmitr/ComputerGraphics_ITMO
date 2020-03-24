#include <iostream>

#include "util/bnm_file_util.h"

int main(int argc, char **argv) {
    try {
        if (argc != 4) {
            throw std::runtime_error("4 arguments are needed! "
                                     "Format:\tprogram.exe <name_of_input_file> <name_of_output_file> <action>\n "
                                     "where <action> is an integer in range [0, 4]\n main");
        }

        bnm_file_util::instance(argv[1], argv[2], atoi(argv[3])).run();
    } catch (std::runtime_error &err) {
        std::cerr << err.what() << std::endl;
        return 1;
    }
}
