#include "DWTCompression.h"

// Constructors
DWTCompression::DWTCompression(MyImage* originalImage, int compressionLevel) : originalImage(originalImage), compressionLevel(compressionLevel)
{
	compressedImage = new MyImage(*originalImage);
}

// Compression
void DWTCompression::compress()
{
	
}
