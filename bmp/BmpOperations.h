#pragma once

#include "BMPLoader.h"

auto greyscale(BMPImage bmp) -> BMPImage {

    for(auto& c : bmp.pixels.data) {
            int sum = c.r + c.g + c.b;
            sum /= 3;
            c = { (uint8_t)sum, (uint8_t)sum, (uint8_t)sum };
    }

    return bmp;
}

auto maximize_contrast(BMPImage bmp) -> BMPImage {

    pixel_u8 max, min;

    max = bmp.at(0, 0);
    min = max;

    // find the max and mins of every rgb member
    for(auto& p : bmp) {
        if(p.r > max.r) { max.r = p.r; }
        if(p.g > max.g) { max.g = p.g; }
        if(p.b > max.b) { max.b = p.b; }

        if(p.r < min.r) { min.r = p.r; }
        if(p.g < min.g) { min.g = p.g; }
        if(p.b < min.b) { min.b = p.b; }
    }

    auto linear_map = [](float a1, float a2, float b1, float b2, float a) -> float {
        return a1 + ((a2 - a1) / (b2 - b1)) * (a - a1);
    };

    BMPImage newbmp = bmp;

    for(auto& p : newbmp) {

        p.r = linear_map(min.r, max.r, 0, 255, p.r);
        p.g = linear_map(min.g, max.g, 0, 255, p.g);
        p.b = linear_map(min.b, max.b, 0, 255, p.b);
    }

    return newbmp;
}

auto downsample(BMPImage bmp, int sample_rate) -> BMPImage {

    int effective_width = bmp.width()   / sample_rate;
    int effective_height = bmp.height() / sample_rate;

    BMPImage new_img;
    new_img.dib.height = effective_height;
    new_img.dib.width  = effective_width;

    for(int j = 0; j < bmp.height(); j += sample_rate) {
        for(int i = 0; i < bmp.width(); i += sample_rate) {

            new_img.pixels.data.push_back( bmp.at(j, i) );

        }
    }

    return new_img;

}


