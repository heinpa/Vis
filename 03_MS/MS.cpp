#include <cstdint>

#include "MS.h"
#include "MS.inl"
#include "Vec2.h"
#include "Vec3.h"


bool asymptoticDeciderBelowIso(Vec3 p1, Vec3 p2, Vec3 p3, Vec3 p4, uint8_t v) {
    float isovalue = static_cast<float>(v);
    float f = (p1.z * p3.z - p2.z * p4.z) / (p3.z + p1.z - p2.z - p4.z);
    return (f < isovalue);
}

Vec2 findIsolineIntersect(Vec3 p1, Vec3 p2, uint8_t isovalue) {
    // interpolate between two points, along either x or y axis.
    // return the point where the verex value == isovalue as isoline intersection.
    float v = static_cast<float>(isovalue);
    Vec2 result{};
    if(p1.x == p2.x) {
        // vertical
        float y = p1.y + (v - p1.z) * (p2.y - p1.y) / (p2.z - p1.z);
        result = {p1.x, y};
    } else if (p1.y == p2.y) {
        // horizontal
        float x = p1.x + (v - p1.z) * (p2.x - p1.x) / (p2.z - p1.z);
        result = {x, p1.y};
    }
    return result;
}

Isoline::Isoline(const Image& image, uint8_t isovalue,
                 bool useAsymptoticDecider) {

    Vec2ui step_size = {1,1};
    uint32_t rows = image.height / step_size.x;
    uint32_t cols = image.width / step_size.y;

    for(uint32_t i=0; i<rows; i++) {
    for(uint32_t j=0; j<cols; j++) {
        uint32_t x = i * step_size.x;
        uint32_t y = j * step_size.y;

        // convert positon to float [-1,1] for drawLines() function
        float h = static_cast<float>(image.height)/2;
        float w = static_cast<float>(image.width)/2;
        Vec2 ul = {(x-h)/h, (y-w)/w};                           // upper left
        Vec2 ur = {(x-h)/h, (y+step_size.y-w)/w};               // upper right
        Vec2 lr = {(x+step_size.x-h)/h, (y+step_size.y-w)/w};   // lower right
        Vec2 ll = {(x+step_size.x-h)/h, (y-w)/w};               // lower left

        // corner points (x positon, y positon, value)
        Vec3 p1 = {ul.x, ul.y, static_cast<float>(image.getLumiValue(x, y))};
        Vec3 p2 = {ur.x, ur.y, static_cast<float>(image.getLumiValue(x, y+step_size.y))};
        Vec3 p3 = {lr.x, lr.y, static_cast<float>(image.getLumiValue(x+step_size.x, y+step_size.y))};
        Vec3 p4 = {ll.x, ll.y, static_cast<float>(image.getLumiValue(x+step_size.x, y))};

        // determine the configuration, based on the corner values
        uint8_t c1, c2, c3, c4;
        if(p1.z >= static_cast<float>(isovalue)) {c1 = 0;} else {c1 = 1;}
        if(p2.z >= static_cast<float>(isovalue)) {c2 = 0;} else {c2 = 1;}
        if(p3.z >= static_cast<float>(isovalue)) {c3 = 0;} else {c3 = 1;}
        if(p4.z >= static_cast<float>(isovalue)) {c4 = 0;} else {c4 = 1;}
        uint8_t configuration = 8 * c1 + 4 * c2 + 2 * c3 + c4;

        // add isoline points depending on configuraion
        switch (configuration) {
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
    }
    }
}
