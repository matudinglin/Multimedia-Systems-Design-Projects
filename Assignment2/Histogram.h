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
	// override << operator to print histogram
	friend std::ostream& operator<<(std::ostream& os, const Histogram& histogram);
};

