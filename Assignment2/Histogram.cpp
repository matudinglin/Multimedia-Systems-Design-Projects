#include "Histogram.h"
#include "Image.h"

// Convert RGB image to YUV image
void RGB2YUV(const MyImage* rgbImage, MyImage* yuvImage) {
	// Get image width and height
	int width = rgbImage->getWidth();
	int height = rgbImage->getHeight();

	// Get RGB data
	unsigned char* rgbData = rgbImage->getImageData();

	// Allocate memory for YUV data
	unsigned char* yuvData = new unsigned char[width * height * 3];

	// Convert RGB to YUV
	for (int i = 0; i < width * height * 3; i += 3) {
		// Get RGB values
		int b = rgbData[i];
		int g = rgbData[i + 1];
		int r = rgbData[i + 2];

		// Convert RGB to YUV
		yuvData[i] = (unsigned char)(0.299 * r + 0.587 * g + 0.114 * b);
		yuvData[i + 1] = (unsigned char)(-0.147 * r - 0.289 * g + 0.436 * b);
		yuvData[i + 2] = (unsigned char)(0.615 * r - 0.515 * g - 0.100 * b);
	}

	// Set YUV data
	yuvImage->setImageData(yuvData);
}

// Convert RGB image to HSV image
void RGB2HSV(const MyImage* rgbImage, MyImage* hsvImage) {
	// Get image width and height
	int width = rgbImage->getWidth();
	int height = rgbImage->getHeight();

	// Get RGB data
	unsigned char* rgbData = rgbImage->getImageData();

	// Allocate memory for HSV data
	unsigned char* hsvData = new unsigned char[width * height * 3];

	// Convert RGB to HSV
	for (int i = 0; i < width * height * 3; i += 3) {
		// Get RGB values
		int r = rgbData[i];
		int g = rgbData[i + 1];
		int b = rgbData[i + 2];

		// Normalize RGB values to the range [0, 1]
		float r_norm = static_cast<float>(r) / 255.0;
		float g_norm = static_cast<float>(g) / 255.0;
		float b_norm = static_cast<float>(b) / 255.0;

		// Find the maximum and minimum values among R, G, and B
		float cmax = max(max(r_norm, g_norm), b_norm);
		float cmin = min(min(r_norm, g_norm), b_norm);

		// Calculate the difference between the maximum and minimum values
		float delta = cmax - cmin;

		// Calculate hue (H)
		float hue = 0.0;
		if (delta != 0.0) {
			if (cmax == r_norm)
				hue = 60.0 * fmod((g_norm - b_norm) / delta, 6.0);
			else if (cmax == g_norm)
				hue = 60.0 * ((b_norm - r_norm) / delta + 2.0);
			else if (cmax == b_norm)
				hue = 60.0 * ((r_norm - g_norm) / delta + 4.0);
		}

		if (hue < 0.0)
			hue += 360.0;

		// Calculate saturation (S)
		float saturation = (cmax == 0.0) ? 0.0 : (delta / cmax);

		// Calculate value (V)
		float value = cmax;

		// Scale hue, saturation, and value to appropriate ranges
		hsvData[i] = static_cast<unsigned char>((hue / 360.0) * 255); // Hue is in range [0, 255]
		hsvData[i + 1] = static_cast<unsigned char>(saturation * 255); // Saturation is in range [0, 255]
		hsvData[i + 2] = static_cast<unsigned char>(value * 255); // Value is in range [0, 255]
	}

	// Set HSV data
	hsvImage->setImageData(hsvData);
}

Histogram::Histogram() {
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
	int width = rgbImage->getWidth();
	int height = rgbImage->getHeight();
	unsigned char* rgbData = rgbImage->getImageData();

	// Initialize YUV image
	MyImage* yuvImage = new MyImage();
	yuvImage->setWidth(width);
	yuvImage->setHeight(height);
	RGB2YUV(rgbImage, yuvImage);
	for (int i = 0; i < 256; i++) {
		uvHistogram[0][i] = 0;
		uvHistogram[1][i] = 0;
	}
	// Compute uv histogram
	unsigned char* yuvData = yuvImage->getImageData();
	for (int i = 0; i < width * height * 3; i += 3) {
		// Check if the pixel is pure green (0, 255, 0 in RGB)
		if (int(rgbData[i]) == 0 && int(rgbData[i + 1]) == 255 && int(rgbData[i + 2]) == 0) {
			continue;
		}
		int u = (int)yuvData[i + 1];
		int v = (int)yuvData[i + 2];
		uvHistogram[0][u]++;
		uvHistogram[1][v]++;
	}
	// Get max U and V number in uv histogram
	int maxUNumber = 0;
	int maxVNumber = 0;
	for (int i = 0; i < 256; i++) {
		if (uvHistogram[0][i] > maxUNumber) {
			maxUNumber = uvHistogram[0][i];
		}
		if (uvHistogram[1][i] > maxVNumber) {
			maxVNumber = uvHistogram[1][i];
		}
	}
	// Delete pixels that are not in the top 10% of the histogram
	int uThreshold = maxUNumber * 0.1;
	int vThreshold = maxVNumber * 0.1;
	for (int i = 0; i < 256; i++) {
		if (uvHistogram[0][i] < uThreshold) {
			uvHistogram[0][i] = 0;
		}
		if (uvHistogram[1][i] < vThreshold) {
			uvHistogram[1][i] = 0;
		}
	}
	delete yuvImage;

	// Initalize HSV image
	MyImage* hsvImage = new MyImage();
	hsvImage->setWidth(width);
	hsvImage->setHeight(height);
	RGB2HSV(rgbImage, hsvImage);
	for (int i = 0; i < 256; i++) {
		hHistogram[i] = 0;
	}
	// Compute h histogram
	unsigned char* hsvData = hsvImage->getImageData();
	for (int i = 0; i < width * height * 3; i += 3) {
		// Check if the pixel is pure green (0, 255, 0 in RGB)
		if (int(rgbData[i]) == 0 && int(rgbData[i + 1]) == 255 && int(rgbData[i + 2]) == 0) {
			continue;
		}
		int h = (int)hsvData[i];
		hHistogram[h]++;
	}
	// Get max H number in h histogram
	int maxHNumber = 0;
	for (int i = 0; i < 256; i++) {
		if (hHistogram[i] > maxHNumber) {
			maxHNumber = hHistogram[i];
		}
	}
	// Delete pixels that are not in the top 10% of the histogram
	int hThreshold = maxHNumber * 0.1;
	for (int i = 0; i < 256; i++) {
		if (hHistogram[i] < hThreshold) {
			hHistogram[i] = 0;
		}
	}
	delete hsvImage;
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
double getHistogramSimilarity(const Histogram& aHistogram, const Histogram& bHistogram, COLORMODE colorMode){
	if (colorMode == YUV){
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
		// Check for division by zero
		if (aNorm == 0 || bNorm == 0) {
			return 0;
		}
		// Compute cosine similarity
		double cosineSimilarity = dotProduct / (sqrt(aNorm) * sqrt(bNorm));

		return cosineSimilarity;
	}
	else if (colorMode == HSV){
		// Get H histograms
		std::array<int, 256> aHHistogram = aHistogram.getHHistogram();
		std::array<int, 256> bHHistogram = bHistogram.getHHistogram();
		
		// Compute dot product
		double dotProduct = 0;
		double aNorm = 0;
		double bNorm = 0;
		for (int i = 0; i < 256; i++) {
			dotProduct += static_cast<double>(aHHistogram[i]) * static_cast<double>(bHHistogram[i]);
			aNorm += static_cast<double>(aHHistogram[i]) * static_cast<double>(aHHistogram[i]);
			bNorm += static_cast<double>(bHHistogram[i]) * static_cast<double>(bHHistogram[i]);
		}
		// Check for division by zero
		if (aNorm == 0 || bNorm == 0) {
			return 0;
		}
		// Compute cosine similarity
		double cosineSimilarity = dotProduct / (sqrt(aNorm) * sqrt(bNorm));

		return cosineSimilarity;
	}
	else if (colorMode == BOTH){

	}
}

