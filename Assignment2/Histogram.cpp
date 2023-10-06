#include "Histogram.h"
#include "Image.h"

// Convert RGB image to YUV image
void RGB2YUV(const MyImage* rgb, MyImage *yuvImage) {
	// Get image width and height
	int width = rgb->getWidth();
	int height = rgb->getHeight();

	// Get RGB data
	unsigned char* rgbData = rgb->getImageData();

	// Allocate memory for YUV data
	unsigned char* yuvData = new unsigned char[width * height * 3];

	// Convert RGB to YUV
	for (int i = 0; i < width * height * 3; i += 3) {
		// Get RGB values
		int r = rgbData[i];
		int g = rgbData[i + 1];
		int b = rgbData[i + 2];

		// Convert RGB to YUV
		yuvData[i] = (unsigned char)(0.299 * r + 0.587 * g + 0.114 * b);
		yuvData[i + 1] = (unsigned char)(-0.147 * r - 0.289 * g + 0.436 * b);
		yuvData[i + 2] = (unsigned char)(0.615 * r - 0.515 * g - 0.100 * b);
	}

	// Set YUV data
	yuvImage->setImageData(yuvData);
}

Histogram::Histogram(){
	// Initialize uv histogram
	for (int i = 0; i < 256; i++) {
		uvHistogram[0][i] = 0;
		uvHistogram[1][i] = 0;
	}
	imageName = "";
}

Histogram::Histogram(const MyImage* rgbImage) {
	// Initialize image name from rgbImage image path
	std::string imagePath = rgbImage->getImagePath();
	int lastSlashIndex = imagePath.find_last_of("/");
	imageName = imagePath.substr(lastSlashIndex + 1);

	// Initialize YUV image
	MyImage* yuvImage = new MyImage();
	int width = rgbImage->getWidth();
	int height = rgbImage->getHeight();
	yuvImage->setWidth(width);
	yuvImage->setHeight(height);

	// Convert RGB image to YUV image
	RGB2YUV(rgbImage, yuvImage);

	// Initialize uv histogram
	for (int i = 0; i < 256; i++) {
		uvHistogram[0][i] = 0;
		uvHistogram[1][i] = 0;
	}
	
	// Compute uv histogram
	unsigned char* yuvData = yuvImage->getImageData();
	unsigned char* rgbData = rgbImage->getImageData();
	for (int i = 0; i < width * height * 3; i += 3) {
		// Check if the pixel is pure green (0, 255, 0 in RGB)
		if (int(rgbData[i]) == 0 && int(rgbData[i + 1]) == 255 && int(rgbData[i + 2]) == 0) {
			continue;
		}
		
		// Get U and V values in 0-255 range
		int u = (int)yuvData[i + 1];
		int v = (int)yuvData[i + 2];

		// Increment uv histogram
		uvHistogram[0][u]++;
		uvHistogram[1][v]++;
	}

	delete yuvImage;
}

// Override << operator to print histogram
std::ostream& operator<<(std::ostream& os, const Histogram& histogram) {
	// Print U histogram
	os << "U Histogram" << std::endl;
	for (int i = 0; i < 256; i++) {
		os << histogram.uvHistogram[0][i] << " ";
	}
	os << std::endl;

	// Print V histogram
	os << "V Histogram" << std::endl;
	for (int i = 0; i < 256; i++) {
		os << histogram.uvHistogram[1][i] << " ";
	}
	os << std::endl;

	return os;
}

// Get color distribution similarity between two histograms using cosine similarity
double getHistogramSimilarity(const Histogram& aHistogram, const Histogram& bHistogram)
{
	// Get U and V histograms
	std::array<std::array<int, 256>, 2> aUVHistogram = aHistogram.getUVHistogram();
	std::array<std::array<int, 256>, 2> bUVHistogram = bHistogram.getUVHistogram();

	// Compute dot product
	double dotProduct = 0;
	double aNorm = 0;
	double bNorm = 0;
	for (int i = 0; i < 256; i++) {
		dotProduct += static_cast<double>(aUVHistogram[0][i]) * static_cast<double>(bUVHistogram[0][i]) +
			static_cast<double>(aUVHistogram[1][i]) * static_cast<double>(bUVHistogram[1][i]);
		aNorm += static_cast<double>(aUVHistogram[0][i]) * static_cast<double>(aUVHistogram[0][i]) +
			static_cast<double>(aUVHistogram[1][i]) * static_cast<double>(aUVHistogram[1][i]);
		bNorm += static_cast<double>(bUVHistogram[0][i]) * static_cast<double>(bUVHistogram[0][i]) +
			static_cast<double>(bUVHistogram[1][i]) * static_cast<double>(bUVHistogram[1][i]);
	}
	if (aNorm == 0 || bNorm == 0) {
		return 0;
	}
	// Compute cosine similarity
	double cosineSimilarity = dotProduct / (sqrt(aNorm) * sqrt(bNorm));

	return cosineSimilarity;
}