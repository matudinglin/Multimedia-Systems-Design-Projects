//*****************************************************************************
//
// Image.h : Defines the class operations on images
//
// Author - Parag Havaldar
// Main Image class structure 
//
//*****************************************************************************

#pragma once
#ifndef IMAGE_DISPLAY
#define IMAGE_DISPLAY

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"
#include "afxwin.h"

// C RunTime Header Files
#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>


// Class structure of Image 
// Use to encapsulate an RGB image
class MyImage
{

private:
	int		Width;					// Width of Image
	int		Height;					// Height of Image
	char	ImagePath[_MAX_PATH];	// Image location
	unsigned char*   Data;			// RGB data of the image

public:
	// Constructor
	MyImage();
	// Copy Constructor
	MyImage::MyImage(MyImage* otherImage);
	// Destructor
	~MyImage();

	// operator overload
	MyImage& operator= (const MyImage& otherImage);

	// Reader & Writer functions
	void	setWidth(const int w) { Width = w; };
	void	setHeight(const int h) { Height = h; };
	void	setImageData(unsigned char* data) { Data = data; };
	void	setImagePath(const char* path) { strcpy(ImagePath, path); }
	int		getWidth()  const { return Width; };
	int		getHeight() const { return Height; };
	unsigned char*   getImageData()  const { return Data; };
	const char*   getImagePath() const { return ImagePath; };

	// Input Output operations
	bool	ReadImage();
	bool	WriteImage();

};

// Crop Image
MyImage* CropImage(const MyImage* image, int x, int y, int width, int height);

#endif //IMAGE_DISPLAY
