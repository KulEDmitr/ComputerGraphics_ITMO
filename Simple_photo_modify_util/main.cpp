#include <iostream>

#include "util/bnm_file_util.h"

/*Аргументы программе передаются через командную строку:
program.exe <name_of_input_file> <name_of_output_file> <action>

где <action>:
  0 - инверсия,
  1 - зеркальное отображение по горизонтали,
  2 - зеркальное отражение по вертикали,
  3 - поворот на 90 градусов по часовой стрелке,
  4 - поворот на 90 градусов против часовой стрелки.
  5 - копировать исходную картинку
*/

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
    }
}
