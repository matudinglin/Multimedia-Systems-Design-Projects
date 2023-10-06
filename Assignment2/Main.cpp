//*****************************************************************************
//
// Main.cpp : Defines the entry point for the application.
// Creates a white RGB image with a black line and displays it.
// Two images are displayed on the screen.
// Left Pane: Input Image, Right Pane: Modified Image
//
// Author - Parag Havaldar
// Code used by students as a starter code to display and modify images
//
//*****************************************************************************


// Include class files
#include "Image.h"
#include "Histogram.h"
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <unordered_map>

#define MAX_LOADSTRING 100

// Global Variables:
MyImage*			inImage;					// image objects
MyImage*			croppedImage;				// cropped image
std::vector<MyImage*> objectImages;				// vector of object images
int imageWidth = 640;							// image width
int imageHeight = 480;							// image height	
HINSTANCE		hInst;							// current instances
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// The title bar text

// Foward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

// Object detection parameters
int scanWindowWidth = 50;
int scanWindowHeight = 50;
int scanWindowStepWidth = 10;
int scanWindowStepHeight = 10;
double percentageThreshold = 0.8;
std::vector<std::pair<int, int>> candidateCoordinates;
POINT points[5];
COLORMODE colorMode = YUV;


// Main entry point for a windows application
int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	MSG msg;
	HACCEL hAccelTable;

	// Create a separate console window to display output to stdout
	AllocConsole();
	freopen("CONOUT$", "w", stdout);

	// Parse and get arguments
	std::string cmdLine(lpCmdLine);
	std::stringstream ss(cmdLine);
	std::string arg;
	std::vector<std::string> args;
	while (getline(ss, arg, ' '))
	{
		args.push_back(arg);
	}

	// Set up the input images
	inImage = new MyImage();
	inImage->setWidth(imageWidth);
	inImage->setHeight(imageHeight);
	inImage->setImagePath(args[0].c_str());
	inImage->ReadImage();

	// Add rest of arguments to vector of object images
	for (int i = 1; i < args.size(); i++)
	{
		MyImage* objectImage = new MyImage();
		objectImage->setWidth(imageWidth);
		objectImage->setHeight(imageHeight);
		objectImage->setImagePath(args[i].c_str());
		objectImage->ReadImage();
		objectImages.push_back(objectImage);
	}

	//------------------------------------------Object Detection------------------------------------------

	//// Get the histogram of the input image
	//Histogram inputHistogram(inImage);

	//// Get the histogram of each object image
	//std::vector<Histogram> objectHistograms;
	//for (int i = 0; i < objectImages.size(); i++)
	//{
	//	Histogram objectHistogram(objectImages[i]);
	//	objectHistograms.push_back(objectHistogram);
	//}

	//// For each histogram, get the similarity score between the input image and the object image
	//// Scan through input image and find the object with the highest similarity score
	//// For each scan window, get the histogram and similarity score

	//std::vector<double> scanWindowSimilarityScores;
	//std::vector<int> scanWindowXCoordinates;
	//std::vector<int> scanWindowYCoordinates;
	//for (int y = 0; y < imageHeight - scanWindowHeight; y += scanWindowStepHeight)
	//{
	//	for (int x = 0; x < imageWidth - scanWindowWidth; x += scanWindowStepWidth)
	//	{
	//		// Use the scan window to create a new image
	//		MyImage* scanWindowImage = CropImage(inImage, x, y, scanWindowWidth, scanWindowHeight);
	//		Histogram scanWindowHistogram(scanWindowImage);
	//		// Get the similarity score between the scan window and each object image
	//		std::vector<double> scanWindowScores;
	//		double scanWindowScore = getHistogramSimilarity(scanWindowHistogram, objectHistograms[0], colorMode);
	//		scanWindowScores.push_back(scanWindowScore);
	//		// Get the highest similarity score between the scan window and each object image
	//		double scanWindowSimilarityScore = *std::max_element(scanWindowScores.begin(), scanWindowScores.end());
	//		scanWindowSimilarityScores.push_back(scanWindowSimilarityScore);
	//		scanWindowXCoordinates.push_back(x);
	//		scanWindowYCoordinates.push_back(y);
	//	}
	//}
	//// Get the highest similarity score between the scan window and each object image
	//double highestScanWindowSimilarityScore = *std::max_element(scanWindowSimilarityScores.begin(), scanWindowSimilarityScores.end());
	//// Get the index of the highest similarity score
	//int highestScanWindowSimilarityScoreIndex = std::distance(scanWindowSimilarityScores.begin(), std::max_element(scanWindowSimilarityScores.begin(), scanWindowSimilarityScores.end()));
	//// Add the coordinates of the scan window within the threshold from the highest similarity score to the vector of candidate coordinates
	//
	//for (int i = 0; i < scanWindowSimilarityScores.size(); i++)
	//{
	//	if (scanWindowSimilarityScores[i] >= highestScanWindowSimilarityScore * percentageThreshold)
	//	{
	//		candidateCoordinates.push_back(std::make_pair(scanWindowXCoordinates[i], scanWindowYCoordinates[i]));
	//	}
	//}
	//// Combine the candidate region to a single region, assign to the points array
	//int minX = candidateCoordinates[0].first;
	//int maxX = candidateCoordinates[0].first + scanWindowWidth;
	//int minY = candidateCoordinates[0].second;
	//int maxY = candidateCoordinates[0].second + scanWindowHeight;
	//for (int i = 1; i < candidateCoordinates.size(); i++)
	//{
	//	if (candidateCoordinates[i].first < minX)
	//	{
	//		minX = candidateCoordinates[i].first;
	//	}
	//	if (candidateCoordinates[i].first + scanWindowWidth > maxX)
	//	{
	//		maxX = candidateCoordinates[i].first + scanWindowWidth;
	//	}
	//	if (candidateCoordinates[i].second < minY)
	//	{
	//		minY = candidateCoordinates[i].second;
	//	}
	//	if (candidateCoordinates[i].second + scanWindowHeight > maxY)
	//	{
	//		maxY = candidateCoordinates[i].second + scanWindowHeight;
	//	}
	//}
	//points[0].x = minX;
	//points[0].y = minY;
	//points[1].x = maxX;
	//points[1].y = minY;
	//points[2].x = maxX;
	//points[2].y = maxY;
	//points[3].x = minX;
	//points[3].y = maxY;
	//points[4].x = minX;
	//points[4].y = minY;


	// Scan through pixel in image
	// If pixel uv value is inside histogram of object image, make the pixel black color
	Histogram objectHistogram(objectImages[0]);
	const auto& uvHistogram = objectHistogram.getUVHistogram();
	for (int row = 0; row < imageHeight; row++)
	{
		for (int col = 0; col < imageWidth; col++)
		{
			// Get the pixel rgb value
			unsigned char r, g, b;
			unsigned char* Data = inImage->getImageData();
			r = Data[(row * imageWidth * 3) + (col * 3) + 2];
			g = Data[(row * imageWidth * 3) + (col * 3) + 1];
			b = Data[(row * imageWidth * 3) + (col * 3) + 0];
			// Convert rgb to uv
			unsigned char u, v;
			u = (unsigned char)(-0.14713 * r - 0.28886 * g + 0.436 * b);
			v = (unsigned char)(0.615 * r - 0.51499 * g - 0.10001 * b);
			
			// Check if the pixel uv value is inside the histogram of the object image
			if (uvHistogram[0][int(u)] > 0 && uvHistogram[1][int(v)] >0)
			{
				// Make the pixel black
				Data[(row * imageWidth * 3) + (col * 3) + 0] = 0;
				Data[(row * imageWidth * 3) + (col * 3) + 1] = 255;
				Data[(row * imageWidth * 3) + (col * 3) + 2] = 0;
			}
		}
	}
	

	//----------------------------------------------------------------------------------------------------



	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_IMAGE, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow)) 
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_IMAGE);

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return msg.wParam;
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage is only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_IMAGE);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= (LPCSTR)IDC_IMAGE;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

	return RegisterClassEx(&wcex);
}


//
//   FUNCTION: InitInstance(HANDLE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }
   
   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}


//
//  FUNCTION: WndProc(HWND, unsigned, WORD, LONG)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
// TO DO: part useful to render video frames, may place your own code here in this function
// You are free to change the following code in any way in order to display the video

	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	TCHAR szHello[MAX_LOADSTRING];
	LoadString(hInst, IDS_HELLO, szHello, MAX_LOADSTRING);
	RECT rt;
	GetClientRect(hWnd, &rt);

	switch (message) 
	{
		case WM_COMMAND:
			wmId    = LOWORD(wParam); 
			wmEvent = HIWORD(wParam); 
			// Parse the menu selections:
			switch (wmId)
			{
				case IDM_ABOUT:
				   DialogBox(hInst, (LPCTSTR)IDD_ABOUTBOX, hWnd, (DLGPROC)About);
				   break;
				case ID_MODIFY_IMAGE:
				   InvalidateRect(hWnd, &rt, false); 
				   break;
				case IDM_EXIT:
				   DestroyWindow(hWnd);
				   break;
				default:
				   return DefWindowProc(hWnd, message, wParam, lParam);
			}
			break;
		case WM_PAINT:
			{
				hdc = BeginPaint(hWnd, &ps);
				// TO DO: Add any drawing code here...
				//unsigned char text[1000];
				//strcpy(text, "The original image is shown as follows. \n");
				//DrawText(hdc, text, strlen(text), &rt, DT_LEFT);
				//strcpy(text, "\nUpdate program with your code to modify input image. \n");
				//DrawText(hdc, text, strlen(text), &rt, DT_LEFT);

				BITMAPINFO bmi;
				CBitmap bitmap;
				memset(&bmi,0,sizeof(bmi));
				bmi.bmiHeader.biSize = sizeof(bmi.bmiHeader);
				bmi.bmiHeader.biWidth = inImage->getWidth();
				bmi.bmiHeader.biHeight = -inImage->getHeight();  // Use negative height.  DIB is top-down.
				bmi.bmiHeader.biPlanes = 1;
				bmi.bmiHeader.biBitCount = 24;
				bmi.bmiHeader.biCompression = BI_RGB;
				bmi.bmiHeader.biSizeImage = inImage->getWidth()*inImage->getHeight();

				SetDIBitsToDevice(hdc,
								  0,0,inImage->getWidth(),inImage->getHeight(),
								  0,0,0,inImage->getHeight(),
								  inImage->getImageData(), &bmi, DIB_RGB_COLORS);

				//// Draw crop image
				//bmi.bmiHeader.biWidth = croppedImage->getWidth();
				//bmi.bmiHeader.biHeight = -croppedImage->getHeight();  // Use negative height.  DIB is top-down.
				//bmi.bmiHeader.biPlanes = 1;
				//bmi.bmiHeader.biBitCount = 24;
				//bmi.bmiHeader.biCompression = BI_RGB;
				//bmi.bmiHeader.biSizeImage = croppedImage->getWidth()*croppedImage->getHeight();
				//SetDIBitsToDevice(hdc,
				//				  0, 20, croppedImage->getWidth(), croppedImage->getHeight(),
				//				  0, 0, 0, croppedImage->getHeight(),
				//				  croppedImage->getImageData(), &bmi, DIB_RGB_COLORS);


				//// Set the color of the pen to Yellow, width = 5
				//HPEN hPen = CreatePen(PS_SOLID, 5, RGB(255, 255, 0));
				//SelectObject(hdc, hPen);
				//Polyline(hdc, points, 5);

				//// Draw regions in candidate coordinates
				//HPEN hPen = CreatePen(PS_SOLID, 5, RGB(255, 255, 0));
				//SelectObject(hdc, hPen);
				//for (int i = 0; i < candidateCoordinates.size(); i++)
				//{
				//	POINT points[5];
				//	points[0].x = candidateCoordinates[i].first;
				//	points[0].y = candidateCoordinates[i].second;
				//	points[1].x = candidateCoordinates[i].first + scanWindowWidth;
				//	points[1].y = candidateCoordinates[i].second;
				//	points[2].x = candidateCoordinates[i].first + scanWindowWidth;
				//	points[2].y = candidateCoordinates[i].second + scanWindowHeight;
				//	points[3].x = candidateCoordinates[i].first;
				//	points[3].y = candidateCoordinates[i].second + scanWindowHeight;
				//	points[4].x = candidateCoordinates[i].first;
				//	points[4].y = candidateCoordinates[i].second;
				//	Polyline(hdc, points, 5);
				//}

							   
				EndPaint(hWnd, &ps);
			}
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}




// Mesage handler for about box.
LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_INITDIALOG:
				return TRUE;

		case WM_COMMAND:
			if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) 
			{
				EndDialog(hDlg, LOWORD(wParam));
				return TRUE;
			}
			break;
	}
    return FALSE;
}


