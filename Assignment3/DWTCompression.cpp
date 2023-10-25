#include "DWTCompression.h"
#include <cmath>

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
    int size = compressedImage->getWidth();
    for (int i = 0; i < 9 - level; ++i) {
        for (int channel = 0; channel < 3; channel++) {
            unsigned char* channelData = extractChannel(originalImage, channel);
            forwardDWT(channelData, size);
            mergeChannel(compressedImage, channelData, channel);
            delete[] channelData;
        }
        // Copy data from compressedImage to originalImage
        for (int j = 0; j < size * size * 3; ++j) {
			originalImage->getImageData()[j] = compressedImage->getImageData()[j];
		}
	}
}

void DWTCompression::decompress() {
    int size = compressedImage->getWidth();
    // Zero out high-pass coefficients at the target level
    for (int channel = 0; channel < 3; channel++) {
		unsigned char* channelData = extractChannel(compressedImage, channel);
		zeroOutHighPassCoefficients(channelData, level);
		mergeChannel(compressedImage, channelData, channel);
		delete[] channelData;
	}

    for (int i = 0; i < 9 - level; ++i) {
        for (int channel = 0; channel < 3; channel++) {
            unsigned char* channelData = extractChannel(compressedImage, channel);
            inverseDWT(channelData, size);
            mergeChannel(outputImage, channelData, channel);
            delete[] channelData;
        }
        // Copy data from outputImage to compressedImage
        for (int j = 0; j < size * size * 3; ++j) {
            compressedImage->getImageData()[j] = outputImage->getImageData()[j];
        }
    }
}

void DWTCompression::forwardDWT(unsigned char* channelData, int size) {
    // Temporary buffers to store the transformed values
    double* tempRow = new double[size];
    double* tempCol = new double[size];

    // Applying Haar DWT on rows
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size / 2; ++j) {
            int index1 = i * size + 2 * j;
            int index2 = index1 + 1;

            double a = static_cast<double>(channelData[index1]);
            double b = static_cast<double>(channelData[index2]);

            tempRow[j] = (a + b) / 2.0;
            tempRow[size / 2 + j] = a - b;
        }
        // Copy back to channelData with clipping and rounding
        for (int j = 0; j < size; ++j) {
            channelData[i * size + j] = clip(std::round(tempRow[j]));
        }
    }

    // Applying Haar DWT on columns
    for (int j = 0; j < size; ++j) {
        for (int i = 0; i < size / 2; ++i) {
            int index1 = 2 * i * size + j;
            int index2 = (2 * i + 1) * size + j;

            double a = static_cast<double>(channelData[index1]);
            double b = static_cast<double>(channelData[index2]);

            tempCol[i] = (a + b) / 2.0;
            tempCol[size / 2 + i] = a - b;
        }
        // Copy back to channelData with clipping and rounding
        for (int i = 0; i < size; ++i) {
            channelData[i * size + j] = clip(std::round(tempCol[i]));
        }
    }

    // Free up the allocated memory
    delete[] tempRow;
    delete[] tempCol;
}

void DWTCompression::inverseDWT(unsigned char* channelData, int size) {
    // Temporary buffers to store the transformed values
    int* tempRow = new int[size];
    int* tempCol = new int[size];

    // Applying inverse Haar DWT on rows
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size / 2; j++) {
            int avg = static_cast<int>(channelData[i * size + j]);
            int diff = static_cast<int>(channelData[i * size + size / 2 + j]);

            // Calculate original values
            int a = clip(avg + (diff / 2));
            int b = clip(avg - (diff / 2));

            // Store results
            tempRow[2 * j] = a;
            tempRow[2 * j + 1] = b;
        }
        // Copy back to channelData
        for (int j = 0; j < size; j++) {
            channelData[i * size + j] = static_cast<unsigned char>(tempRow[j]);
        }
    }

    // Applying inverse Haar DWT on columns
    for (int j = 0; j < size; j++) {
        for (int i = 0; i < size / 2; i++) {
            int avg = static_cast<int>(channelData[i * size + j]);
            int diff = static_cast<int>(channelData[(i + size / 2) * size + j]);

            int a = clip(avg + (diff / 2));
            int b = clip(avg - (diff / 2));

            tempCol[2 * i] = a;
            tempCol[2 * i + 1] = b;
        }
        // Copy back to channelData
        for (int i = 0; i < size; i++) {
            channelData[i * size + j] = static_cast<unsigned char>(tempCol[i]);
        }
    }

    // Free up the allocated memory
    delete[] tempRow;
    delete[] tempCol;
}

void DWTCompression::zeroOutHighPassCoefficients(unsigned char* channelData, int targetLevel) {
    int size = compressedImage->getWidth();
    int coeffToKeep = size / pow(2, 9-targetLevel);

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