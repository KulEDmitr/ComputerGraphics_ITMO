cmake_minimum_required(VERSION 3.14)
project(Simple_photo_modify_util)

set(CMAKE_CXX_STANDARD 17)

add_executable(Simple_photo_modify_util main.cpp picture/picture.cpp util/bnm_file_util.cpp util/bnm_file_util.h picture/picture.h picture/gray_scale_pic.cpp picture/gray_scale_pic.h picture/color_pic.cpp picture/color_pic.h)