#include <iostream>
#include "util/dithering_util.h"

int main(int argc, char **argv) {
    try {
        if (argc != 7) {
            throw std::runtime_error("7 arguments are needed! "
                                     "Format:\tprogram.exe <name_of_input_file> <name_of_output_file> "
                                     "<gradient> <dithering> <bit_rate> <gamma>"
                                     "where:\n\t<gradient> - 0 - canvas is the input img, 1 - draw horizontal gradient\n"
                                     "\t<dithering>  - integer in range [0..7] - dithering algorithm\n"
                                     "\t\t0 - Without dithering\n"
                                     "\t\t1 - Ordered 8x8\n"
                                     "\t\t2 - Random\n"
                                     "\t\t3 - Floyd-Steinberg\n"
                                     "\t\t4 - Jarvis, Judice, Ninke\n"
                                     "\t\t5 - Sierra 3\n"
                                     "\t\t6 - Atkinson\n"
                                     "\t\t7 - Halftone 4x4 orthogonal\n"
                                     "\t<bit_rate>  - integer in range [1..8]\n"
                                     "\tgamma - positive real number or 0\n"
                                     "\t\t0 - sRGB gamma correction\n"
                                     "\t\tother - gamma correction with the entered value as gamma.");
        }

        dithering_util::instance(argv[1], argv[2], atoi(argv[3]), atoi(argv[4]), atoi(argv[5]),
                atof(argv[6])).run();
    } catch (std::runtime_error &err) {
        std::cerr << err.what() << std::endl;
        return 1;
    }
}
