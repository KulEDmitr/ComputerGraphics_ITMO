#include <iostream>

#include "util/brightnessUtil.h"

int main(int argc, char **argv) {
    try {
        if (argc != 4) {
            throw std::runtime_error("4 or 6 arguments are needed! "
                                     "Format:\tprogram.exe <name_of_input_file> <name_of_output_file> <conversion> [<offset> <factor>]\n "
                                     "where <conversion> is an integer in range [0..5]\n"
                                     "\t\t0 - [<offset> <factor>] in RGB color space for each channel\n"
                                     "\t\t1 - [<offset> <factor>] in YCbCr.601 color space for Y channel\n"
                                     "\t\t2 - auto brightness in RGB. lite version: use all input pixels\n"
                                     "\t\t3 - auto brightness in YCbCr.601. lite version: use all input pixels\n"
                                     "\t\t4 - auto brightness in RGB. hard version: ignore 0,39% of lightest and darkest pixels\n"
                                     "\t\t5 - auto brightness in YCbCr.601. hard version: ignore 0,39% of lightest and darkest pixels\n"
                                     "\t<bit_rate>  - for 0 and 1 conversations only. integer in range [-255..255]\n"
                                     "\t<factor> - for 0 and 1 conversations only. positive real number in range [1/255..255]");
        }

        int conversation = atoi(argv[3]), offset = -1;
        double factor = -1.0;

        if (conversation == 0 || conversation == 1) {
            offset = atoi(argv[4]);
            factor = atof(argv[5]);
        }

        brightnessUtil::instance(argv[1], argv[2], conversation, offset, factor).run();
    } catch (std::runtime_error &err) {
        std::cerr << err.what() << std::endl;
        return 1;
    }
}
