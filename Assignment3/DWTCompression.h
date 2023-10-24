#pragma once

#include "Image.h"

class DWTCompression
{
private:
    MyImage* originalImage;         // Store the original image
    MyImage* compressedImage;       // Store the compressed image
    int compressionLevel;          // Desired compression level

public:
    // Constructors and Destructor
    DWTCompression(MyImage* originalImage, int compressionLevel);
    ~DWTCompression() {};

    // Getters
    MyImage* getCompressedImage() const { return compressedImage; }

    // Compression
    void compress();
};


