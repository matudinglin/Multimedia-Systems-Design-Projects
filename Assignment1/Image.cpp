//*****************************************************************************
//
// Image.cpp : Defines the class operations on images
//
// Author - Parag Havaldar
// Code used by students as starter code to display and modify images
//
//*****************************************************************************

#include "Image.h"
#include <iostream>
using namespace std;

// Constructor and Desctructors
MyImage::MyImage()
{
	Data = NULL;
	Width = -1;
	Height = -1;
	ImagePath[0] = 0;
}

MyImage::~MyImage()
{
	if (Data)
		delete[] Data;
}


// Copy constructor
MyImage::MyImage(MyImage* otherImage)
{
	Height = otherImage->Height;
	Width = otherImage->Width;
	Data = new char[Width * Height * 3];
	strcpy(otherImage->ImagePath, ImagePath);

	for (int i = 0; i < (Height * Width * 3); i++)
	{
		Data[i] = otherImage->Data[i];
	}


}



// = operator overload
MyImage& MyImage::operator= (const MyImage& otherImage)
{
	Height = otherImage.Height;
	Width = otherImage.Width;
	Data = new char[Width * Height * 3];
	strcpy((char*)otherImage.ImagePath, ImagePath);

	for (int i = 0; i < (Height * Width * 3); i++)
	{
		Data[i] = otherImage.Data[i];
	}

	return *this;

}


// MyImage::ReadImage
// Function to read the image given a path
bool MyImage::ReadImage()
{

	// Verify ImagePath
	if (ImagePath[0] == 0 || Width < 0 || Height < 0)
	{
		fprintf(stderr, "Image or Image properties not defined");
		fprintf(stderr, "Usage is `Image.exe Imagefile w h`");
		return false;
	}

	// Create a valid output file pointer
	FILE* IN_FILE;
	IN_FILE = fopen(ImagePath, "rb");
	if (IN_FILE == NULL)
	{
		fprintf(stderr, "Error Opening File for Reading");
		return false;
	}

	// Create and populate RGB buffers
	int i;
	char* Rbuf = new char[Height * Width];
	char* Gbuf = new char[Height * Width];
	char* Bbuf = new char[Height * Width];

	for (i = 0; i < Width * Height; i++)
	{
		Rbuf[i] = fgetc(IN_FILE);
	}
	for (i = 0; i < Width * Height; i++)
	{
		Gbuf[i] = fgetc(IN_FILE);
	}
	for (i = 0; i < Width * Height; i++)
	{
		Bbuf[i] = fgetc(IN_FILE);
	}

	// Allocate Data structure and copy
	Data = new char[Width * Height * 3];
	for (i = 0; i < Height * Width; i++)
	{
		Data[3 * i] = Bbuf[i];
		Data[3 * i + 1] = Gbuf[i];
		Data[3 * i + 2] = Rbuf[i];
	}

	originalData = new char[Width * Height * 3];
	size_t dataSize = Width * Height * 3;
	memcpy(originalData, Data, dataSize);
	originalWidth = Width;
	originalHeight = Height;

	overlayData = new char[200 * 200 * 3];
	// make overlay data all black
	for (int i = 0; i < 200 * 200 * 3; i++) {
		overlayData[i] = 0;
	}

	// Clean up and return
	delete Rbuf;
	delete Gbuf;
	delete Bbuf;
	fclose(IN_FILE);

	return true;

}



// MyImage functions defined here
bool MyImage::WriteImage()
{
	// Verify ImagePath
	// Verify ImagePath
	if (ImagePath[0] == 0 || Width < 0 || Height < 0)
	{
		fprintf(stderr, "Image or Image properties not defined");
		return false;
	}

	// Create a valid output file pointer
	FILE* OUT_FILE;
	OUT_FILE = fopen(ImagePath, "wb");
	if (OUT_FILE == NULL)
	{
		fprintf(stderr, "Error Opening File for Writing");
		return false;
	}

	// Create and populate RGB buffers
	int i;
	char* Rbuf = new char[Height * Width];
	char* Gbuf = new char[Height * Width];
	char* Bbuf = new char[Height * Width];

	for (i = 0; i < Height * Width; i++)
	{
		Bbuf[i] = Data[3 * i];
		Gbuf[i] = Data[3 * i + 1];
		Rbuf[i] = Data[3 * i + 2];
	}


	// Write data to file
	for (i = 0; i < Width * Height; i++)
	{
		fputc(Rbuf[i], OUT_FILE);
	}
	for (i = 0; i < Width * Height; i++)
	{
		fputc(Gbuf[i], OUT_FILE);
	}
	for (i = 0; i < Width * Height; i++)
	{
		fputc(Bbuf[i], OUT_FILE);
	}

	// Clean up and return
	delete Rbuf;
	delete Gbuf;
	delete Bbuf;
	fclose(OUT_FILE);

	return true;

}


bool MyImage::resizeImage(float scalingFactor) {
	int newWidth = static_cast<int>(Width * scalingFactor);
	int newHeight = static_cast<int>(Height * scalingFactor);

	// Create a new buffer for the resampled image
	char* newData = new char[newWidth * newHeight * 3];
	for (int y = 0; y < newHeight; y++) {
		for (int x = 0; x < newWidth; x++) {
			// Calculate the corresponding position in the original image
			int origX = static_cast<int>(x / scalingFactor);
			int origY = static_cast<int>(y / scalingFactor);

			// Check bounds to avoid segmentation faults
			origX = min(max(origX, 0), Width - 1);
			origY = min(max(origY, 0), Height - 1);

			// Copy the pixel value from the original image to the resampled image
			newData[(y * newWidth + x) * 3] = Data[(origY * Width + origX) * 3];
			newData[(y * newWidth + x) * 3 + 1] = Data[(origY * Width + origX) * 3 + 1];
			newData[(y * newWidth + x) * 3 + 2] = Data[(origY * Width + origX) * 3 + 2];
		}
	}

	// Update the image properties
	setWidth(newWidth);
	setHeight(newHeight);

	// Delete the old image data and set the new data
	if (Data)
		delete[] Data;

	setImageData(newData);
	return true;
}

bool MyImage::blurImage(float filterSize) {
	// Create a new buffer for the blurred image
	char* newData = new char[Width * Height * 3];

	// Calculate the filter radius
	int filterRadius = static_cast<int>(filterSize / 2);

	// Loop through all pixels in the image
	for (int y = 0; y < Height; y++) {
		for (int x = 0; x < Width; x++) {

			// Calculate the average pixel value for the filter
			int avgR = 0;
			int avgG = 0;
			int avgB = 0;
			int numPixels = 0;

			// Loop through all pixels in the filter
			for (int filterY = -filterRadius; filterY <= filterRadius; filterY++) {
				for (int filterX = -filterRadius; filterX <= filterRadius; filterX++) {

					// Calculate the corresponding position in the original image
					int origX = x + filterX;
					int origY = y + filterY;

					// Check bounds to avoid segmentation faults
					origX = min(max(origX, 0), Width - 1);
					origY = min(max(origY, 0), Height - 1);

					// Add the pixel value to the average	
					avgB += static_cast<unsigned char>(Data[(origY * Width + origX) * 3]);
					avgG += static_cast<unsigned char>(Data[(origY * Width + origX) * 3 + 1]);
					avgR += static_cast<unsigned char>(Data[(origY * Width + origX) * 3 + 2]);

					numPixels++;
				}
			}

			// Calculate the average pixel value
			avgB /= numPixels;
			avgG /= numPixels;
			avgR /= numPixels;

			// Set the pixel value in the new image
			newData[(y * Width + x) * 3] = avgB;
			newData[(y * Width + x) * 3 + 1] = avgG;
			newData[(y * Width + x) * 3 + 2] = avgR;
		}
	}

	// Delete the old image data and set the new data
	if (Data)
		delete[] Data;

	setImageData(newData);

	return true;
}


// Here is where you would place your code to modify an image
// eg Filtering, Transformation, Cropping, etc.
bool MyImage::Modify(float scalingFactor, bool antiAliasing)
{
	int filterSize = 5;

	if (scalingFactor <= 0 || filterSize <= 0) {
		fprintf(stderr, "Invalid scaling factor or filter size");
		return false;
	}

	if (antiAliasing) blurImage(filterSize);

	resizeImage(scalingFactor);

	return true;
}

char* MyImage::getOverlayData(int overlayWindowSize, int left, int top, float scalingFactor)
{
	if (!Data || overlayWindowSize <= 0.0f)
	{
		return NULL; // Return NULL if the original data is not available or the window size is invalid
	}

	// Check if the overlay window is within image bounds
	if (left >= 0 && top >= 0 && (left + overlayWindowSize) <= Width && (top + overlayWindowSize) <= Height)
	{
		// output state
		cout << "Get result" << endl;
		// Copy the overlay data from the original data
		for (int y = 0; y < overlayWindowSize; y++)
		{
			for (int x = 0; x < overlayWindowSize; x++)
			{
				int srcX = left + x;
				int srcY = top + y;
				int destIndex = (y * overlayWindowSize + x) * 3;
				int srcIndex = (srcY * Width + srcX) * 3;

				// Copy pixel data (R, G, B components)
				overlayData[destIndex] = 0;
				overlayData[destIndex + 1] = 0;
				overlayData[destIndex + 2] = 0;
			}
		}

		return overlayData; // Return the cropped overlay data
	}
	else
	{
		return NULL; // Overlay window is out of image bounds
	}
}