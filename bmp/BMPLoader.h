#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

struct pixel_u8 {
    uint8_t r, g, b;
};

const int BMP_RED   = 0;
const int BMP_GREEN = 1;
const int BMP_BLUE  = 2;

struct BMPImage {

    struct {
        char     signature[3];
        uint32_t size;
        char     reserved[4];
        uint32_t image_offset;
    } header;

    struct { // device-independent bitmap info
        uint32_t header_size;
        uint32_t width;
        uint32_t height;
        uint16_t planes;
        uint16_t bpp;          // bits per pixel
        uint32_t compression;
        uint32_t imagesize;    // compressed image size
        uint32_t xpixelsmeter; // horizontal pixels per meter
        uint32_t ypixelsmeter; // vertical pixels per meter
        uint32_t colorsused;
        uint32_t importantcolors;
    } dib;

    struct {
        std::vector<pixel_u8> colortable;
        std::vector<pixel_u8> data;
    } pixels;

    const uint32_t width(void) const { return dib.width; }
    const uint32_t height(void) const { return dib.height; }
    size_t size(void) const { return pixels.data.size(); }

    pixel_u8& at(int y, int x) {
        return pixels.data[width()*y + x];
    }

    uint8_t& at(int y, int x, int c) {
        switch(c) {
            case 0: return this->at(y, x).r; break;
            case 1: return this->at(y, x).g; break;
            case 2: return this->at(y, x).b; break;
            default:
                throw std::runtime_error("invalid color index in BMPImage");
        }
    }

    auto begin(void) -> std::vector<pixel_u8>::iterator {
        return pixels.data.begin();
    }

    auto end(void) -> std::vector<pixel_u8>::iterator {
        return pixels.data.end();
    }

    friend std::ostream& operator<<(std::ostream& os, BMPImage& bmp) {
        os << "header\n";
        os << "\tsignature:       " << bmp.header.signature << "\n";
        os << "\tsize:            " << bmp.header.size << " bytes\n";
        os << "\tpixel address:   " << bmp.header.image_offset << "\n";
        
        os << "\nDIB\n"; // device-independent bitmap format header
        os << "\theader size:  " << bmp.dib.header_size << " bytes\n";
        os << "\twidth:        " << bmp.dib.width << "\n";
        os << "\theight:       " << bmp.dib.height << "\n";
        os << "\tplanes:       " << bmp.dib.planes << "\n";
        os << "\tbits / pixel: " << bmp.dib.bpp << " ";
        switch(bmp.dib.bpp) {
            case 1: os << "(monochrome)";         break;
            case 4: os << "(16 colors)";          break;
            case 8: os << "(up to 256 colors)";   break;
            case 16: os << "(65536 colors)";      break;
            case 24: os << "(colors not in LUT)"; break;
            default: break;
        }
        os << "\n";

        os << "\tcompression:  ";
        switch(bmp.dib.compression) {
            case 0: os << "BI_RGB (no compression)";     break;
            case 1: os << "BI_RLE8 (8bit RLE encoding)"; break;
            case 2: os << "BI_RLE4 (4bit RLE encoding)"; break;
            default: break;
        }
        os << std::endl;

        os << "\timage size:   " << bmp.dib.imagesize << " bytes\n";
        os << "\tpixels/meter\n";
        os << "\t\th: " << bmp.dib.xpixelsmeter << "\n";
        os << "\t\tv: " << bmp.dib.ypixelsmeter << "\n";
        os << "\ttotal colors:     " << bmp.dib.colorsused << "\n";
        os << "\timportant colors: " << bmp.dib.importantcolors << "\n";

        return os;
    }

};

struct __BMPLoader {
private:

    union {
        char c2[2];
        uint16_t u16;
        int16_t  i16;
    };

    union {
        char     c4[4];
        uint32_t u32;
        int32_t  i32;
        float    f32;
    };

    union {
        char c8[8];
        uint64_t u64;
        int64_t  i64;
        double   f64;
    };

    uint32_t read_u32(std::ifstream& fs) { fs.read(c4, 4); return u32; }
    int32_t  read_i32(std::ifstream& fs) { fs.read(c4, 4); return i32; }
    float    read_f32(std::ifstream& fs) { fs.read(c4, 4); return f32; }

    uint64_t read_u64(std::ifstream& fs) { fs.read(c8, 8); return u64; }
    int64_t  read_i64(std::ifstream& fs) { fs.read(c8, 8); return i64; }
    double   read_f64(std::ifstream& fs) { fs.read(c8, 8); return f64; }

    uint16_t read_u16(std::ifstream& fs) { fs.read(c2, 2); return u16; }
    int16_t  read_i16(std::ifstream& fs) { fs.read(c2, 2); return i16; }

public:

    BMPImage parseFile(std::ifstream& fs) {
        BMPImage bmpimage;

        // read the header information
        {
            fs.read(bmpimage.header.signature, 2);
            bmpimage.header.signature[2] = 0x00;

            bmpimage.header.size = this->read_u32(fs);

            fs.read(bmpimage.header.reserved, 4);

            bmpimage.header.image_offset = this->read_u32(fs);
        }

        // read the DIB 
        {
            bmpimage.dib.header_size     = this->read_u32(fs);
            bmpimage.dib.width           = this->read_u32(fs);
            bmpimage.dib.height          = this->read_u32(fs);
            bmpimage.dib.planes          = this->read_u16(fs);
            bmpimage.dib.bpp             = this->read_u16(fs);
            bmpimage.dib.compression     = this->read_u32(fs);
            bmpimage.dib.imagesize       = this->read_u32(fs);
            bmpimage.dib.xpixelsmeter    = this->read_u32(fs);
            bmpimage.dib.ypixelsmeter    = this->read_u32(fs);
            bmpimage.dib.colorsused      = this->read_u32(fs);
            bmpimage.dib.importantcolors = this->read_u32(fs);
        }

        // read the actual pixel data
        switch(bmpimage.dib.bpp) {
            case 1:
            case 4:
            case 8:
            case 16:
                throw std::runtime_error("unrecognized bpp for image: " + 
                    std::to_string(bmpimage.dib.bpp));
                break;
            case 24: // dont need a color table
                {
                    bmpimage.pixels.data.resize(bmpimage.dib.width * bmpimage.dib.height);
                    for(int j = bmpimage.height()-1; j >= 0; j--) {

                        union {
                            // blue green red
                            uint8_t u8[3];
                            char    c8[3];
                        };

                        for(int i = 0; i < bmpimage.width(); i++) {

                            fs.read(c8, 3);
                            //bmpimage.pixels.data.push_back({ u8[2], u8[1], u8[0] });
                            bmpimage.at(j, i) = { u8[2], u8[1], u8[0] };

                        }

                        // grab the padding bytes
                        int row_pixels = bmpimage.width() * 3;
                        switch(row_pixels % 4) {
                            case 0: break; // nothing to do
                            case 1: fs.read(c8, 3); break;
                            case 2: fs.read(c8, 2); break;
                            case 3: fs.read(c8, 1); break;
                            default:
                                throw std::runtime_error("BMPLoader : internal error");
                        }

                    }
                }
                break;
            default:
                throw std::runtime_error("unrecognized bpp for image: " + 
                    std::to_string(bmpimage.dib.bpp));
                break;
        }

        std::cout << bmpimage << std::endl;
        return bmpimage;
    }

};

// singleton
__BMPLoader BMPLoader;
