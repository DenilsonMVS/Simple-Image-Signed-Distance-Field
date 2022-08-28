
#ifndef DISTANCE_FIELD_HPP
#define DISTANCE_FIELD_HPP

#include <vector>
#include <cstddef>

/**
 * @brief Function that receives a bitmap image and generates a distance field from it.
 * The floating values of the distance field is the distance from the pixel to the closest pixel of different color
 * White pixels will have positive value, black pixels will have negative values.
 * 
 * @param bitmap Bitmap image
 * @param width Width of the image
 * @return std::vector<float> Signed distance field with the same dimensions as the bitmap
 */
extern std::vector<float> generateDistanceField(const std::vector<bool> &bitmap, const size_t width);

#endif
