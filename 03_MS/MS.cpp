#include<algorithm>
#include <cstdint>
#include <iostream>

#include "MS.h"
#include "MS.inl"
#include "Vec2.h"

Isoline::Isoline(const Image& image, uint8_t isovalue,
                 bool useAsymptoticDecider) {
  // TODO: compute isoline using the ms-algorithm and
  //       store the 2D points of the line list in the
  //       member variable "vertices"


    // Vec2 resolution = {0.5f/float(image.width), 0.5f/float(image.height)};
    Vec2 resolution = {1, 1};
    uint32_t rows = image.height / resolution.x;
    uint32_t cols = image.width / resolution.y;

    for(uint32_t i=0; i<rows-1; i++){ // TODO: offset ok?
    for(uint32_t j=0; j<cols-1; j++) {// there's a good chance you can just increment
                                      // by resolution in each direction
        float x = i * resolution.x;
        float y = j * resolution.y;

        float h = float(image.height)/2;
        float w = float(image.width)/2;

        // TODO: mid points - that's not quite how I remember this
        Vec2 a = {(x-h)/h, (y+resolution.y/2-w)/w};
        Vec2 b = {(x+resolution.x/2-h)/h,(y+resolution.y-w)/w};
        Vec2 c = {(x+resolution.x-h)/h, (y+resolution.y/2-w)/w};
        Vec2 d = {(x+resolution.x/2-h)/h, (y-w)/w};

        // std::cout << a << std::endl;
        // std::cout << b << std::endl;
        // std::cout << c << std::endl;
        // std::cout << d << std::endl;
        // std::cout << "====" << std::endl;

        // corner values (vertices)
        uint8_t c1, c2, c3, c4;

        if(image.getLumiValue(x, y) >= isovalue) {c1 = 0;} else {c1 = 1;}
        if(image.getLumiValue(x, y+resolution.y) >= isovalue) {c2 = 0;} else {c2 = 1;}
        if(image.getLumiValue(x+resolution.x, y+resolution.y) >= isovalue) {c3 = 0;} else {c3 = 1;}
        if(image.getLumiValue(x+resolution.x, y) >= isovalue) {c4 = 0;} else {c4 = 1;}

        uint8_t state = 8 * c1 + 4 * c2 + 2 * c3 + c4;
        // std::cout << "state" << state << std::endl;

        switch (state) {
           case 1:
               vertices.push_back(c);
               vertices.push_back(d);
               break;
           case 2:
               vertices.push_back(b);
               vertices.push_back(c);
               break;
           case 3:
               vertices.push_back(b);
               vertices.push_back(d);
               break;
           case 4:
               vertices.push_back(a);
               vertices.push_back(b);
               break;
           case 5:
               vertices.push_back(a);
               vertices.push_back(b);
               vertices.push_back(c);
               vertices.push_back(d);
               break;
           case 6:
               vertices.push_back(a);
               vertices.push_back(c);
               break;
           case 7:
               vertices.push_back(a);
               vertices.push_back(d);
               break;
           case 8:
               vertices.push_back(a);
               vertices.push_back(d);
               break;
           case 9:
               vertices.push_back(a);
               vertices.push_back(c);
               break;
           case 10:
               vertices.push_back(a);
               vertices.push_back(d);
               vertices.push_back(b);
               vertices.push_back(c);
               break;
           case 11:
               vertices.push_back(a);
               vertices.push_back(b);
               break;
           case 12:
               vertices.push_back(b);
               vertices.push_back(d);
               break;
           case 13:
               vertices.push_back(b);
               vertices.push_back(c);
               break;
           case 14:
               vertices.push_back(c);
               vertices.push_back(d);
               break;
           default:
               break;
        }
//    std::cout << "loop (" << i << ", " << j << ")" << std::endl;
    }
    }
    std::cout << "done" << std::endl;
}
