#include "convertUtil.h"
#include <iostream>
#include <cmath>

convertUtil &convertUtil::instance(size_t iCount, char *inp, size_t oCount, char *outp, char *fromColorSpace,
                                   char *toColorSpace) {
    ColorSpaces from, to;

    if (strcmp(fromColorSpace, "RGB") == 0) {
        from = RGB;
    } else if (strcmp(fromColorSpace, "HSL") == 0) {
        from = HSL;
    } else if (strcmp(fromColorSpace, "HSV") == 0) {
        from = HSV;
    } else if (strcmp(fromColorSpace, "YCbCr.601") == 0) {
        from = YCbCr_601;
    } else if (strcmp(fromColorSpace, "YCbCr.709") == 0) {
        from = YCbCr_709;
    } else if (strcmp(fromColorSpace, "YCoCg") == 0) {
        from = YCoCg;
    } else if (strcmp(fromColorSpace, "CMY") == 0) {
        from = CMY;
    } else {
        throw std::runtime_error("Wrong arguments");
    }

    if (strcmp(toColorSpace, "RGB") == 0) {
        to = RGB;
    } else if (strcmp(toColorSpace, "HSL") == 0) {
        to = HSL;
    } else if (strcmp(toColorSpace, "HSV") == 0) {
        to = HSV;
    } else if (strcmp(toColorSpace, "YCbCr.601") == 0) {
        to = YCbCr_601;
    } else if (strcmp(toColorSpace, "YCbCr.709") == 0) {
        to = YCbCr_709;
    } else if (strcmp(toColorSpace, "YCoCg") == 0) {
        to = YCoCg;
    } else if (strcmp(toColorSpace, "CMY") == 0) {
        to = CMY;
    } else {
        throw std::runtime_error("Wrong arguments");
    }

    static convertUtil util(iCount, inp, oCount, outp, from, to);
    return util;
}

convertUtil::~convertUtil() {
    free(name_of_input_file);
    free(name_of_output_file);
    if (canvas != nullptr) {
        canvas->~colorPicture();
    }
}

void convertUtil::run() {
    try {
        (iCount == 1) ? treatmentOneDataFile() : treatmentAnyDataFiles();
        convert();
        (oCount == 1) ? writeOneFile() : writeAnyFile();

    } catch (std::runtime_error &err) {
        if (canvas != nullptr) {
            canvas->~colorPicture();
        }
        throw std::runtime_error(err.what());
    }
}

void convertUtil::treatmentOneDataFile() {
    FILE *f = nullptr;
    try {
        if ((f = fopen(name_of_input_file, "rb")) == nullptr) {
            throw std::runtime_error("Can't open input file");
        }
        char pattern[3] = "P6";
        checkFile(f, pattern);
        read_canvas(f);
    } catch (std::runtime_error &err) {
        fclose(f);
        throw std::runtime_error(err.what());
    }

    if (fclose(f) == EOF) {
        throw std::runtime_error("Input file can't be closed");
    }
}

void convertUtil::checkFile(FILE *f, char *pattern) {
    char *header = new char[2];

    if (fscanf(f, "%s", header) == EOF) {
        delete[](header);
        throw std::runtime_error("Bad picture file");
    }

    if (strcmp(header, pattern) != 0) {
        delete[](header);
        throw std::runtime_error("Unknown picture type");
    }

//    delete[](header);
}

void convertUtil::read_canvas(FILE *f) {
    int wide, height, grade;
    /*
     * On success, the function returns the number of items of the argument list successfully filled.
     * This count can match the expected number of items or be less (even zero) due to a matching failure,
     * a reading error, or the reach of the end-of-file.
     * If a reading error happens or the end-of-file is reached while reading, the proper indicator
     * is set (feof or ferror). And, if either happens before any data could be successfully read, EOF is returned.
     * If an encoding error happens interpreting wide characters, the function sets errno to EILSEQ.
     */
    int count = fscanf(f, "%i %i\n%i\n", &wide, &height, &grade);
    if (feof(f) != 0 || ferror(f) || errno == EILSEQ || count < 3) {
        throw std::runtime_error("Bad picture file. Size isn't an integer");
    }

    if (wide <= 0 || height <= 0 || grade > 255) {
        throw std::runtime_error("Bad picture file");
    }

    u_char *file_data = nullptr;
    int picture_size = 3 * wide * height;

    try {
        file_data = new u_char[picture_size];

        if (fread(file_data, sizeof(u_char), picture_size, f) != picture_size || feof(f) != 0) {
            free(file_data);
            throw std::runtime_error("Bad picture file");
        }

        canvas = new colorPicture(wide, height, grade, file_data);

    } catch (std::bad_alloc &err) {
        delete[] (file_data);
        throw std::runtime_error("Not enough memory");
    }
    delete[] (file_data);
}

void convertUtil::treatmentAnyDataFiles() {
    FILE *fA = nullptr, *fB = nullptr, *fC = nullptr;
    std::vector<FILE *> files = {fA, fB, fC};

    try {
        for (int i = 1; i <= 3; ++i) {
            std::string tmp_file_name(name_of_input_file);
            std::string idx("_");
            idx.append(1, '0' + i);
            tmp_file_name.insert(tmp_file_name.begin() + tmp_file_name.find_last_of('.'), idx.begin(), idx.end());

            FILE *tmp;
            if ((tmp = fopen(tmp_file_name.data(), "rb")) == nullptr) {
                throw std::runtime_error("Can't open input file");
            }
            std::swap(files[i - 1], tmp);

            char pattern[3] = "P5";
            checkFile(files[i - 1], pattern);
        }

        read_canvas(files);
    } catch (std::runtime_error &err) {
        for (int i = 0; i < 3; ++i) {
            if (files[i] != nullptr) {
                fclose(files[i]);
            }
        }
        throw std::runtime_error(err.what());
    }

    for (int i = 0; i < 3; ++i) {
        if (fclose(files[i]) == EOF) {
            throw std::runtime_error("Input file can't be closed");
        }
    }
}

void convertUtil::read_canvas(std::vector<FILE *> &files) {
    int wide = -1, height = -1, grade = -1, pictureSize = -1;

    for (int i = 0; i < 3; ++i) {
        int wid, hei, gra;
        /*
        * On success, the function returns the number of items of the argument list successfully filled.
        * This count can match the expected number of items or be less (even zero) due to a matching failure,
        * a reading error, or the reach of the end-of-file.
        * If a reading error happens or the end-of-file is reached while reading, the proper indicator
        * is set (feof or ferror). And, if either happens before any data could be successfully read, EOF is returned.
        * If an encoding error happens interpreting wide characters, the function sets errno to EILSEQ.
        */
        int count = fscanf(files[i], "%i %i\n%i\n", &wid, &hei, &gra);
        if (feof(files[i]) != 0 || ferror(files[i]) || errno == EILSEQ || count < 3) {
            throw std::runtime_error("Bad picture file. Size isn't an integer");
        }
        if (wid <= 0 || hei <= 0 || gra > 255) {
            throw std::runtime_error("Bad picture file");
        }

        int picture_size1 = wid * hei;

        if (pictureSize != -1) {
            if (wide != wid || height != hei || picture_size1 != pictureSize) {
                throw std::runtime_error("Bad picture file. Channels files have different parameters");

            }
        } else {
            wide = wid;
            height = hei;
            grade = gra;
            pictureSize = picture_size1;
        }
    }

    u_char *fileData1 = nullptr, *fileData2 = nullptr, *fileData3 = nullptr;
    try {
        fileData1 = new u_char[pictureSize];
        fileData2 = new u_char[pictureSize];
        fileData3 = new u_char[pictureSize];

        if (fread(fileData1, sizeof(u_char), pictureSize, files[0]) != pictureSize || feof(files[0]) != 0) {
            free(fileData1);
            throw std::runtime_error("Bad first picture file");
        }
        if (fread(fileData2, sizeof(u_char), pictureSize, files[1]) != pictureSize || feof(files[1]) != 0) {
            free(fileData2);
            throw std::runtime_error("Bad second picture file");
        }
        if (fread(fileData3, sizeof(u_char), pictureSize, files[2]) != pictureSize || feof(files[2]) != 0) {
            free(fileData3);
            throw std::runtime_error("Bad third picture file");
        }

        canvas = new colorPicture(wide, height, grade, fileData1, fileData2, fileData3);

    } catch (std::bad_alloc &err) {
        delete[] (fileData1);
        delete[] (fileData2);
        delete[] (fileData3);
        throw std::runtime_error("Not enough memory");
    }

    delete[] (fileData1);
    delete[] (fileData2);
    delete[] (fileData3);
}

void convertUtil::convert() {
    if (fromSpace == toSpace) {
        return;
    }

    switch (fromSpace) {
        case RGB:
            break;
        case HSL:
            HSLtoRGB();
            break;
        case HSV:
            HSVtoRGB();
            break;
        case YCbCr_601:
            YCbCr_601toRGB();
            break;
        case YCbCr_709:
            YCbCr_709toRGB();
            break;
        case YCoCg:
            YCoCgtoRGB();
            break;
        case CMY:
            CMYtoRGB();
            break;
        default:
            break;
    }
    switch (toSpace) {
        case RGB:
            break;
        case HSL:
            RGBtoHSL();
            break;
        case HSV:
            RGBtoHSV();
            break;
        case YCbCr_601:
            RGBtoYCbCr_601();
            break;
        case YCbCr_709:
            RGBtoYCbCr_709();
            break;
        case YCoCg:
            RGBtoYCoCg();
            break;
        case CMY:
            RGBtoCMY();
            break;
        default:
            break;
    }
}

u_char convertUtil::correctNoise(double pixel) {
    return (u_char) round(255.0 * fmax(fmin(pixel, 1.0), 0.0));
}

//from RGB convertion
void convertUtil::RGBtoHSL() {
    for (size_t i = 0; i < canvas->getSize(); ++i) {
        rgb_pixel *cur = canvas->getPixel(i);

        double R = cur->A / 255.0,
                G = cur->B / 255.0,
                B = cur->C / 255.0;

        double cMax = fmax(R, fmax(G, B)),
                cMin = fmin(R, fmin(G, B));

        double delta = cMax - cMin, L = (cMin + cMax) / 2.0, H = 0;

        double Sat = (delta / (1.0 - fabs(1.0 - 2.0 * L)));

        if (delta == 0) {
            H = 0;
        } else if (cMax == R) {
            H = (G - B) / delta;
        } else if (cMax == G) {
            H = (B - R) / delta + 2.0;
        } else {
            H = (R - G) / delta + 4.0;
        }
        H *= 60;

        if (H < 0) {
            H += 360;
        }
        // H:
        cur->A = correctNoise(H / 360.0);
        // S:
        cur->B = correctNoise(Sat);
        // L:
        cur->C = correctNoise(L);
    }
}

void convertUtil::RGBtoHSV() {
    for (size_t i = 0; i < canvas->getSize(); ++i) {
        rgb_pixel *cur = canvas->getPixel(i);

        double R = cur->A / 255.0,
                G = cur->B / 255.0,
                B = cur->C / 255.0;

        double cMax = fmax(R, fmax(G, B)),
                cMin = fmin(R, fmin(G, B));

        double delta = cMax - cMin, H = 0;

        if (delta == 0) {
            H = 0;
        } else if (cMax == R) {
            H = (G - B) / delta;
        } else if (cMax == G) {
            H = (B - R) / delta + 2.0;
        } else {
            H = (R - G) / delta + 4.0;
        }
        H *= 60;

        if (H < 0) {
            H += 360;
        }

        double Sat;
        if (cMax == 0) {
            Sat = 0;
        } else {
            Sat = delta / cMax;
        }

        // H:
        cur->A = correctNoise(H / 360.0);
        // S:
        cur->B = correctNoise(Sat);
        // V:
        cur->C = correctNoise(cMax);
    }
}

void convertUtil::RGBtoYCbCr(double kR, double kG, double kB) {
    for (size_t i = 0; i < canvas->getSize(); ++i) {
        rgb_pixel *cur = canvas->getPixel(i);

        double R = cur->A / 255.0,
                G = cur->B / 255.0,
                B = cur->C / 255.0;

        double Y = kR * R + kG * G + kB * B,
                Cb = ((B - Y) / (1.0 - kB)) / 2.0,
                Cr = ((R - Y) / (1.0 - kR)) / 2.0;

        // Y
        cur->A = correctNoise(Y);
        // Cb
        cur->B = correctNoise(Cb + 0.5);
        // Cr
        cur->C = correctNoise(Cr + 0.5);
    }
}
void convertUtil::RGBtoYCbCr_601() {
    double kR = 0.299, kG = 0.587, kB = 0.114;
    RGBtoYCbCr(kR, kG, kB);
}
void convertUtil::RGBtoYCbCr_709() {
    double kR = 0.0722, kG = 0.2126, kB = 0.7152;
    RGBtoYCbCr(kR, kG, kB);
}

void convertUtil::RGBtoYCoCg() {
    for (size_t i = 0; i < canvas->getSize(); ++i) {
        rgb_pixel *cur = canvas->getPixel(i);

        double R = cur->A / 255.0,
                G = cur->B / 255.0,
                B = cur->C / 255.0;

        // Y
        cur->A = correctNoise(((R + B) / 4.0 + G / 2.0));
        // Co
        cur->B = correctNoise((R - B) / 2.0 + 0.5);
        // Cg
        cur->C = correctNoise(G / 2.0 - (B + R) / 4.0 + 0.5);
    }
}

void convertUtil::RGBtoCMY() {
    for (size_t i = 0; i < canvas->getSize(); ++i) {
        rgb_pixel *cur = canvas->getPixel(i);

        double R = cur->A / 255.0,
                G = cur->B / 255.0,
                B = cur->C / 255.0;

        // C
        cur->A = correctNoise(1 - R);
        // M
        cur->B = correctNoise(1 - G);
        // Y
        cur->C = correctNoise(1 - B);
    }
}

//to RGB convertion
void convertUtil::HSLtoRGB() {
    for (size_t i = 0; i < canvas->getSize(); ++i) {
        rgb_pixel *cur = canvas->getPixel(i);

        double H = cur->A / 255.0 * 360.0,
                Sat = cur->B / 255.0,
                L = cur->C / 255.0;

        double _H = H / 60.0;
        double C = (1 - fabs(2 * L - 1)) * Sat,
                X = C * (1 - fabs(fmod(_H, 2.0) - 1)),
                m = L - C / 2.0;

        double R = 0, G = 0, B = 0;

        if (_H - 1.0 < 0.001) {
            R = C;
            G = X;
            B = 0;
        } else if (_H - 2.0 < 0.001) {
            R = X;
            G = C;
            B = 0;
        } else if (_H - 3.0 < 0.001) {
            R = 0;
            G = C;
            B = X;
        } else if (_H - 4.0 < 0.001) {
            R = 0;
            G = X;
            B = C;
        } else if (_H - 5.0 < 0.001) {
            R = X;
            G = 0;
            B = C;
        } else if (_H - 6.0 < 0.001) {
            R = C;
            G = 0;
            B = X;
        }

        // R:
        cur->A = correctNoise(R + m);
        // G
        cur->B = correctNoise(G + m);
        // B
        cur->C = correctNoise(B + m);
    }
}
void convertUtil::HSVtoRGB() {
    for (size_t i = 0; i < canvas->getSize(); ++i) {
        rgb_pixel *cur = canvas->getPixel(i);

        double H = cur->A / 255.0 * 360.0,
                Sat = cur->B / 255.0,
                V = cur->C / 255.0;


        double _H = H;
        if (_H > 360.0) {
            _H = 0.0;
        }
        _H /= 60.0;

        double C = V * Sat,
                X = C * (1 - fabs(fmod(_H, 2.0) - 1)),
                m = V - C;

        double R = 0, G = 0, B = 0;

        if (_H - 1.0 < 0.001) {
            R = C;
            G = X;
            B = 0;
        } else if (_H - 2.0 < 0.001) {
            R = X;
            G = C;
            B = 0;
        } else if (_H - 3.0 < 0.001) {
            R = 0;
            G = C;
            B = X;
        } else if (_H - 4.0 < 0.001) {
            R = 0;
            G = X;
            B = C;
        } else if (_H - 5.0 < 0.001) {
            R = X;
            G = 0;
            B = C;
        } else if (_H - 6.0 < 0.001) {
            R = C;
            G = 0;
            B = X;
        }

        // R:
        cur->A = correctNoise(R + m);
        // G
        cur->B = correctNoise(G + m);
        // B
        cur->C = correctNoise(B + m);
    }
}

void convertUtil::YCbCrtoRGB(double kR, double kG, double kB) {
    for (size_t i = 0; i < canvas->getSize(); ++i) {
        rgb_pixel *cur = canvas->getPixel(i);

        double Y = cur->A / 255.0,
                Cb = cur->B / 255.0 - 0.5,
                Cr = cur->C / 255.0 - 0.5;

        double R = Y + 2.0 * (1.0 - kR) * Cr;
        double G = Y - 2.0 * kB * (1.0 - kB) * Cb / kG - 2.0 * kR * (1.0 - kR) * Cr / kG;
        double B = Y + 2.0 * (1.0 - kB) * Cb;

        // R:
        cur->A = correctNoise(R);
        // G
        cur->B = correctNoise(G);
        // B
        cur->C = correctNoise(B);
    }
}

void convertUtil::YCbCr_601toRGB() {
    double kR = 0.299, kG = 0.587, kB = 0.114;
    YCbCrtoRGB(kR, kG, kB);
}

void convertUtil::YCbCr_709toRGB() {
    double kR = 0.0722, kG = 0.2126, kB = 0.7152;
    YCbCrtoRGB(kR, kG, kB);
}

void convertUtil::YCoCgtoRGB() {
    for (size_t i = 0; i < canvas->getSize(); ++i) {
        rgb_pixel *cur = canvas->getPixel(i);

        double Y = cur->A / 255.0,
                Co = cur->B / 255.0 - 0.5,
                Cg = cur->C / 255.0 - 0.5;

        // R
        cur->A = correctNoise(Y - Cg + Co);
        // G
        cur->B = correctNoise(Y + Cg);
        // B
        cur->C = correctNoise(Y - Cg - Co);
    }
}

void convertUtil::CMYtoRGB() {
    for (size_t i = 0; i < canvas->getSize(); ++i) {
        rgb_pixel *cur = canvas->getPixel(i);

        double C = cur->A / 255.0,
                M = cur->B / 255.0,
                Y = cur->C / 255.0;

        // R
        cur->A = correctNoise(1 - C);
        // G
        cur->B = correctNoise(1 - M);
        // B
        cur->C = correctNoise(1 - Y);
    }
}

void convertUtil::writeOneFile() {
    FILE *f;
    if ((f = fopen(name_of_output_file, "wb")) == nullptr) {
        throw std::runtime_error("result file didn't open");
    }

    try {
        canvas->write(f);
    } catch (std::runtime_error &err) {
        fclose(f);
        throw std::runtime_error(err.what());
    }

    if (fclose(f) == EOF) {
        throw std::runtime_error("Output file can't be closed");
    }
}

void convertUtil::writeAnyFile() {
    FILE *fA = nullptr, *fB = nullptr, *fC = nullptr;
    std::vector<FILE *> files = {fA, fB, fC};

    for (int i = 1; i <= 3; ++i) {
        std::string tmp_file_name(name_of_output_file);
        std::string idx("_");
        idx.append(1, '0' + i);
        tmp_file_name.insert(tmp_file_name.begin() + tmp_file_name.find_last_of('.'), idx.begin(), idx.end());

        FILE *tmp;
        if ((tmp = fopen(tmp_file_name.data(), "wb")) == nullptr) {
            throw std::runtime_error("result file didn't open");
        }

        std::swap(files[i - 1], tmp);
    }

    try {
        canvas->write(files[0], files[1], files[2]);
    } catch (std::runtime_error &err) {
        for (int i = 0; i < 3; ++i) {
            fclose(files[i]);
        }
        throw std::runtime_error(err.what());
    }

    for (int i = 0; i < 3; ++i) {
        if (fclose(files[i]) == EOF) {
            throw std::runtime_error("Input file can't be closed");
        }
    }
}
