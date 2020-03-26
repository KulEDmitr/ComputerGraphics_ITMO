#include <iostream>

#include "util/line_draw_util.h"

int main(int argc, char **argv) {
    try {
        if (argc < 9) {
            throw std::runtime_error("9 or 10 arguments are needed! "
                                     "Format:\tprogram.exe <name_of_input_file> <name_of_output_file> "
                                     "<line_brightness> <line_thick> <x_start> <y_start> <x_end> <y_end> <gamma>"
                                     "where:\n\t<line_brightness> - integer in range [0, 255]\n"
                                     "\t<line_thick>  - positive fractional number\n"
                                     "\t(x, y) - coordinates inside the image, fractional numbers "
                                     "(integer values correspond to the center of pixels)\n"
                                     "\tgamma - (optional) positive real number "
                                     "(gamma correction with the entered value as gamma. "
                                     "In its absence, sRGB is used)");
        }

        line_draw_util::instance(argv[1], argv[2], atoi(argv[3]), atof(argv[4]), atof(argv[5]),
                                 atof(argv[6]), atof(argv[7]), atof(argv[8]), (argc == 9) ? 0 : atof(argv[9])).run();
    } catch (std::runtime_error &err) {
        std::cerr << err.what() << std::endl;
        return 1;
    }
}