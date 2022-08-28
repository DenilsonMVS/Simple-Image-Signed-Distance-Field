

#include "distanceField.hpp"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <memory>

// Example of generating a distance field
int main() {

    // Load image
    int width, height, channels;
    unsigned char *image = stbi_load("a.png", &width, &height, &channels, 0);

    // Convert image to bitmap
    const size_t numPixels = width * height;
    auto bitmap = std::vector<bool>(numPixels);
    for(size_t i = 0; i < numPixels; i++)
        bitmap[i] = image[i] >= 128;
    
    // Generate distance field
    const auto distanceField = generateDistanceField(bitmap, width);

    // Calculate maximum absolute distance
    float maxDistance = 0;
    for(auto &distance : distanceField)
        maxDistance = std::max(maxDistance, std::abs(distance));
    
    // Convert distance field as float to byte
    const float conversionMult = 127.5f / maxDistance;
    auto distanceFieldAsBytes = std::unique_ptr<unsigned char []>(new unsigned char[numPixels]);
    for(size_t i = 0; i < numPixels; i++)
        distanceFieldAsBytes[i] = std::round(distanceField[i] * conversionMult + 127.5f);

    // Write image
    stbi_write_png("a_dist.png", width, height, 1, distanceFieldAsBytes.get(), width);

    // Free image
    stbi_image_free(image);

    return 0;
}
