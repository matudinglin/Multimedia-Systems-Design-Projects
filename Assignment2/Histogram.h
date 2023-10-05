#pragma once
#include <array>
#include <iostream>
#include "Image.h"

// Convert RGB to YUV histogram
class Histogram
{
private:
	// Array used to store uv histogram
	// Two dimensions corresponding to U and V values, and each dimension has 256 bins to store the histogram counts
	std::array<std::array<int, 256>, 2> uvHistogram;


public:
	Histogram();
	Histogram(const MyImage*  rbgImage);
	~Histogram() {};
	friend std::ostream& operator<<(std::ostream& os, const Histogram& histogram);
	std::array<std::array<int, 256>, 2> getUVHistogram() const { return uvHistogram; };
};

double getHistogramSimilarity(const Histogram& aHistogram, const Histogram& bHistogram);
void RGB2YUV(const MyImage* rgbImage, MyImage* yuvImage);