#include<algorithm>
#include <cstdint>
#include <iostream>

#include "MS.h"
#include "MS.inl"
#include "Vec2.h"
#include "Vec3.h"


// TODO: solve offset on edges
// TODO: check float conversion -> better to do at different spot?


bool asymptoticDeciderBelowIso(Vec3 p1, Vec3 p2, Vec3 p3, Vec3 p4, uint8_t v) {
    float isovalue = float(v)/255;
    float f = (p1.z * p3.z - p2.z * p4.z) / (p3.z + p1.z - p2.z - p4.z);
    return (f < isovalue);
}

Vec2 findIsolineIntersect(Vec3 p1, Vec3 p2, uint8_t v) {
    float isovalue = float(v)/255;
    Vec2 result{};
    if(p1.x == p2.x) {
        // vertical
        float y = p1.y + (isovalue - p1.z) * (p2.y - p1.y) / (p2.z - p1.z);
        result = {p1.x, y};
    } else if (p1.y == p2.y) {
        // horizontal
        float x = p1.x + (isovalue - p1.z) * (p2.x - p1.x) / (p2.z - p1.z);
        result = {x, p1.y};
    }
    return result;
}

Isoline::Isoline(const Image& image, uint8_t isovalue,
                 bool useAsymptoticDecider) {

    Vec2 resolution = {1,1};
    uint32_t rows = image.height / resolution.x;
    uint32_t cols = image.width / resolution.y;

    for(uint32_t i=0; i<rows; i++) { 
    for(uint32_t j=0; j<cols; j++) {
        float x = i * resolution.x;
        float y = j * resolution.y;

        float h = float(image.height)/2;
        float w = float(image.width)/2;

        Vec2 a = {(x-h)/h, (y-w)/w};
        Vec2 b = {(x-h)/h, (y+resolution.y-w)/w};
        Vec2 c = {(x+resolution.x-h)/h, (y+resolution.y-w)/w};
        Vec2 d = {(x+resolution.x-h)/h, (y-w)/w};

        // determine the case, based on the scalar vertex values compared to the isovalue
        Vec3 p1 = {a.x, a.y, float(image.getLumiValue(x, y))/255};
        Vec3 p2 = {b.x, b.y, float(image.getLumiValue(x, y+resolution.y))/255};
        Vec3 p3 = {c.x, c.y, float(image.getLumiValue(x+resolution.x, y+resolution.y))/255};
        Vec3 p4 = {d.x, d.y, float(image.getLumiValue(x+resolution.x, y))/255};

        // corner values (vertices)
        uint8_t c1, c2, c3, c4;

        if(p1.z >= float(isovalue)/255) {c1 = 0;} else {c1 = 1;}
        if(p2.z >= float(isovalue)/255) {c2 = 0;} else {c2 = 1;}
        if(p3.z >= float(isovalue)/255) {c3 = 0;} else {c3 = 1;}
        if(p4.z >= float(isovalue)/255) {c4 = 0;} else {c4 = 1;}

        uint8_t state;
        state = 8 * c1 + 4 * c2 + 2 * c3 + c4;
        switch (state) {
           case 1:
               vertices.push_back(findIsolineIntersect(p3, p4, isovalue));
               vertices.push_back(findIsolineIntersect(p4, p1, isovalue));
               break;
           case 2:
               vertices.push_back(findIsolineIntersect(p2, p3, isovalue));
               vertices.push_back(findIsolineIntersect(p3, p4, isovalue));
               break;
           case 3:
               vertices.push_back(findIsolineIntersect(p2, p3, isovalue));
               vertices.push_back(findIsolineIntersect(p4, p1, isovalue));
               break;
           case 4:
               vertices.push_back(findIsolineIntersect(p1, p2, isovalue));
               vertices.push_back(findIsolineIntersect(p2, p3, isovalue));
               break;
           case 5:
               if(useAsymptoticDecider && asymptoticDeciderBelowIso(p1, p2, p3, p4, isovalue)) {
                   vertices.push_back(findIsolineIntersect(p2, p3, isovalue));
                   vertices.push_back(findIsolineIntersect(p3, p4, isovalue));
                   vertices.push_back(findIsolineIntersect(p4, p1, isovalue));
                   vertices.push_back(findIsolineIntersect(p1, p2, isovalue));
               } else {
                   vertices.push_back(findIsolineIntersect(p1, p2, isovalue));
                   vertices.push_back(findIsolineIntersect(p2, p3, isovalue));
                   vertices.push_back(findIsolineIntersect(p3, p4, isovalue));
                   vertices.push_back(findIsolineIntersect(p4, p1, isovalue));
               }
               break;
           case 6:
               vertices.push_back(findIsolineIntersect(p1, p2, isovalue));
               vertices.push_back(findIsolineIntersect(p3, p4, isovalue));
               break;
           case 7:
               vertices.push_back(findIsolineIntersect(p1, p2, isovalue));
               vertices.push_back(findIsolineIntersect(p4, p1, isovalue));
               break;
           case 8:
               vertices.push_back(findIsolineIntersect(p4, p1, isovalue));
               vertices.push_back(findIsolineIntersect(p1, p2, isovalue));
               break;
           case 9:
               vertices.push_back(findIsolineIntersect(p1, p2, isovalue));
               vertices.push_back(findIsolineIntersect(p3, p4, isovalue));
               break;
           case 10:
               if(useAsymptoticDecider && asymptoticDeciderBelowIso(p1, p2, p3, p4, isovalue)) {
                   vertices.push_back(findIsolineIntersect(p1, p2, isovalue));
                   vertices.push_back(findIsolineIntersect(p2, p3, isovalue));
                   vertices.push_back(findIsolineIntersect(p3, p4, isovalue));
                   vertices.push_back(findIsolineIntersect(p4, p1, isovalue));
               } else {
                   vertices.push_back(findIsolineIntersect(p1, p2, isovalue));
                   vertices.push_back(findIsolineIntersect(p4, p1, isovalue));
                   vertices.push_back(findIsolineIntersect(p2, p3, isovalue));
                   vertices.push_back(findIsolineIntersect(p3, p4, isovalue));
               }
               break;
           case 11:
               vertices.push_back(findIsolineIntersect(p1, p2, isovalue));
               vertices.push_back(findIsolineIntersect(p2, p3, isovalue));
               break;
           case 12:
               vertices.push_back(findIsolineIntersect(p2, p3, isovalue));
               vertices.push_back(findIsolineIntersect(p4, p1, isovalue));
               break;
           case 13:
               vertices.push_back(findIsolineIntersect(p2, p3, isovalue));
               vertices.push_back(findIsolineIntersect(p3, p4, isovalue));
               break;
           case 14:
               vertices.push_back(findIsolineIntersect(p3, p4, isovalue));
               vertices.push_back(findIsolineIntersect(p4, p1, isovalue));
               break;
           default:
               break;
        }

        // depending on the case, interpolate along the relevant edges to find isoline points
        // x = x_i + (y - y_i) * (x_j - x_i) / (y_j - y_i)
        // for y = isovalue

        // for ambigious cases, use decider for decision in which order to append vertices 
        // (i.e. what set of lines to draw)
    }
    }
}
