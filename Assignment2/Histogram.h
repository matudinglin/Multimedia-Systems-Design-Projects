#pragma once
#include <array>
#include <iostream>
#include "Image.h"
#include <string>

enum COLORMODE {
	HSV, YUV, BOTH
};

// Convert RGB to YUV histogram
class Histogram
{
private:
	// Array used to store uv histogram
	// Two dimensions corresponding to U and V values, and each dimension has 256 bins to store the histogram counts
	std::array<std::array<int, 256>, 2> uvHistogram;
	// Array used to store h histogram
	// One dimension corresponding to H values, and each dimension has 256 bins to store the histogram counts
	std::array<int, 256> hHistogram;
	std::string imageName;


public:
	Histogram();
	Histogram(const MyImage*  rbgImage);
	~Histogram() {};
	friend std::ostream& operator<<(std::ostream& os, const Histogram& histogram);
	std::array<std::array<int, 256>, 2> getUVHistogram() const { return uvHistogram; };
	std::array<int, 256> getHHistogram() const { return hHistogram; };
	std::string getImageName() const { return imageName; };
};

double getHistogramSimilarity(const Histogram& aHistogram, const Histogram& bHistogram, COLORMODE colorMode);
double getPixelSimilarity(const MyImage* image, const Histogram& histogram);
void RGB2YUV(const MyImage* rgbImage, MyImage* yuvImage);
void YUV2RGB(const MyImage* yuvImage, MyImage* rgbImage);

