#pragma once

#include "Image.h"

class DWTCompression
{
private:
    MyImage* originalImage;         // Store the original image
    MyImage* compressedImage;       // Store the compressed image
    MyImage* outputImage;           // Store the decompressed image
    int level;          // Desired compression level

public:
    // Constructors and Destructor
    DWTCompression(MyImage* originalImage, int level);
    ~DWTCompression() {};

    // Getters
    MyImage* getOutputImage() const { return outputImage; }
    MyImage* getCompressedImage() const { return compressedImage; }

    // Compression
    void compress();
    void decompress();
    unsigned char* extractChannel(MyImage* image, int channel);
    void mergeChannel(MyImage* image, unsigned char* channelData, int channel);
    void forwardDWT(unsigned char* channelData, int size);
    void inverseDWT(unsigned char* channelData, int currentLevel);
    void zeroOutHighPassCoefficients(unsigned char* channelData, int targetLevel);

};


