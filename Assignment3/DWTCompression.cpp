#include "DWTCompression.h"

unsigned char clip(int value) {
    if (value < 0) return 0;
    if (value > 255) return 255;
    return static_cast<unsigned char>(value);
}

// Constructors
DWTCompression::DWTCompression(MyImage* originalImage, int level) : originalImage(originalImage), level(level)
{
	compressedImage = new MyImage(originalImage);
    outputImage = new MyImage(originalImage);
}

// Compression
void DWTCompression::compress() {
    for (int channel = 0; channel < 3; channel++) {
        unsigned char* channelData = extractChannel(originalImage, channel);
        forwardDWT(channelData, 9);
        mergeChannel(compressedImage, channelData, channel);
        delete[] channelData; // Free up the memory allocated in extractChannel.
    }
}

void DWTCompression::decompress() {
    for (int channel = 0; channel < 3; channel++) {
        unsigned char* channelData = extractChannel(compressedImage, channel); // Decompression should work on the compressedImage
        zeroOutHighPassCoefficients(channelData, level);
        inverseDWT(channelData, level);
        mergeChannel(outputImage, channelData, channel);
        delete[] channelData; // Free up the memory allocated in extractChannel.
    }
}

void DWTCompression::forwardDWT(unsigned char* channelData, int currentLevel) {
    if (currentLevel <= level) return;

    int size = compressedImage->getWidth(); // Assuming square image

    // Applying Haar DWT on rows
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j += 2) {
            unsigned char a = channelData[i * size + j];
            unsigned char b = channelData[i * size + j + 1];

            // Calculate average and difference
            int avg = (static_cast<int>(a) + static_cast<int>(b)) / 2;
            int diff = static_cast<int>(a) - static_cast<int>(b);

            // Store results
            channelData[i * size + j / 2] = clip(avg);
            channelData[i * size + size / 2 + j / 2] = clip(diff);
        }
    }

    // Apply DWT to columns
    for (int j = 0; j < size; j++) {
        for (int i = 0; i < size; i += 2) {
            unsigned char a = channelData[i * size + j];
            unsigned char b = channelData[(i + 1) * size + j];

            int avg = (static_cast<int>(a) + static_cast<int>(b)) / 2;
            int diff = static_cast<int>(a) - static_cast<int>(b);

            channelData[j + (i / 2) * size] = clip(avg);
            channelData[j + (size / 2 + i / 2) * size] = clip(diff);
        }
    }


    // Recurse on the low-pass coefficients
    forwardDWT(channelData, currentLevel - 1);
}

void DWTCompression::inverseDWT(unsigned char* channelData, int currentLevel) {
    if (currentLevel > 9) return;

    int size = compressedImage->getWidth(); // Assuming square image

    // Applying inverse Haar DWT on rows
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size / 2; j++) {
            unsigned char avg = channelData[i * size + j];
            unsigned char diff = channelData[i * size + size / 2 + j];

            // Calculate original values
            int a = clip(static_cast<int>(avg) + static_cast<int>(diff) / 2);
            int b = clip(static_cast<int>(avg) - static_cast<int>(diff) / 2);

            // Store results
            channelData[i * size + 2 * j] = a;
            channelData[i * size + 2 * j + 1] = b;
        }
    }

    // Applying inverse Haar DWT on rows
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size / 2; j++) {
            unsigned char avg = channelData[i * size + j];
            unsigned char diff = channelData[i * size + size / 2 + j];

            int a = clip(static_cast<int>(avg) + static_cast<int>(diff) / 2);
            int b = clip(static_cast<int>(avg) - static_cast<int>(diff) / 2);

            channelData[i * size + 2 * j] = static_cast<unsigned char>(a);
            channelData[i * size + 2 * j + 1] = static_cast<unsigned char>(b);
        }
    }

    // Recurse on the next level
    inverseDWT(channelData, currentLevel + 1);
}

void DWTCompression::zeroOutHighPassCoefficients(unsigned char* channelData, int targetLevel) {
    int size = compressedImage->getWidth();
    int coeffToKeep = size / pow(2, targetLevel);

    // Zero out high-pass coefficients in rows
    for (int i = 0; i < size; i++) {
        for (int j = coeffToKeep; j < size; j++) {
            channelData[i * size + j] = 0;
        }
    }

    // Zero out high-pass coefficients in columns
    for (int j = 0; j < size; j++) {
        for (int i = coeffToKeep; i < size; i++) {
            channelData[i * size + j] = 0;
        }
    }
}

unsigned char* DWTCompression::extractChannel(MyImage *image, int channel) {
    int size = image->getWidth() * image->getHeight();
    unsigned char* channelData = new unsigned char[size];
    for (int i = 0; i < size; i++) {
        channelData[i] = image->getImageData()[3 * i + channel];
    }
    return channelData;
}

void DWTCompression::mergeChannel(MyImage* image, unsigned char* channelData, int channel) {
    int size = image->getWidth() * image->getHeight();
    for (int i = 0; i < size; i++) {
        image->getImageData()[3 * i + channel] = channelData[i];
    }
}