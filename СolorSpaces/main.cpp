#include <iostream>
#include "util/convertion_util.h"

int main(int argc, char **argv) {
    try {
        if (argc != 11) {
            throw std::runtime_error("Wrong number of arguments. "
                                     "\nFormat: <lab4>.exe -f <from_color_space> -t <to_color_space> -i <count>"
                                     " <input_file_name> -o <count> <output_file_name>\n"
                                     "where:\n\t<color_space> - RGB / HSL / HSV / YCbCr.601 / YCbCr.709 / YCoCg / CMY\n"
                                     "\t<count> - 1 or 3\n"
                                     "\t<file_name>:\n"
                                     "\t\tfor count = 1: file name format .ppm\n"
                                     "\t\tfor count = 3: template <name.ext>, which corresponds files <name_1.ext>, \n"
                                     "\t\t\t<name_2.ext> and <name_3.ext> for each channel respectively. format .pgm"
                                     "\n\t\t\tthe order of the arguments (-f, -t, -i, -o) can be arbitrary");
        }

        size_t _f = 0, _t = 0, _i = 0, _o = 0, i_count = 0, o_count = 0;
        char *from_color_space = nullptr, *to_color_space = nullptr;
        char *file_in_name = nullptr, *file_out_name = nullptr;

        for (int i = 1; i < argc; ++i) {
            if (strlen(argv[i]) == 2) {
                if (strcmp(argv[i], "-f") == 0) {
                    _f = 1;
                    from_color_space = argv[++i];
                } else if (strcmp(argv[i], "-t") == 0) {
                    _t = 1;
                    to_color_space = argv[++i];
                } else if (strcmp(argv[i], "-i") == 0) {
                    _i = 1;
                    i_count = std::stoi(argv[++i]);
                    file_in_name = argv[++i];
                } else if (strcmp(argv[i], "-o") == 0) {
                    _o = 1;
                    o_count = std::stoi(argv[++i]);
                    file_out_name = argv[++i];
                } else {
                    throw std::runtime_error("Invalid arguments");
                }
            } else {
                throw std::runtime_error("Invalid arguments");
            }
        }

        if (_f + _t + _i + _o < 4 || (i_count != 1 && i_count != 3) || (o_count != 1 && o_count != 3)
            || file_in_name == nullptr || file_out_name == nullptr) {
            throw std::runtime_error("Wrong arguments");
        }

        convertion_util::instance(i_count, file_in_name, o_count, file_out_name, from_color_space,
                                  to_color_space).run();

    } catch (std::runtime_error &err) {
        std::cerr << err.what() << std::endl;
        return 1;
    }

}