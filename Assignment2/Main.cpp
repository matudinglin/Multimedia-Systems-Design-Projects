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

#define MAX_LOADSTRING 100

// Global Variables:
MyImage*			inImage;					// image objects
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
int scanWindowStepWidth = 50;
int scanWindowStepHeight = 50;
std::vector<std::pair<int, int>> candidateCoordinates;
double threshold = 0.05;


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

	// Get the histogram of the input image
	Histogram inputHistogram(inImage);
	std::cout << "Input image histogram: " << std::endl;
	std::cout << inputHistogram << std::endl;

	// Get the histogram of each object image
	std::vector<Histogram> objectHistograms;
	for (int i = 0; i < objectImages.size(); i++)
	{
		Histogram objectHistogram(objectImages[i]);
		objectHistograms.push_back(objectHistogram);
		std::cout << "Object image " << i << " histogram: " << std::endl;
		std::cout << objectHistogram << std::endl;
	}

	// Get similarity score between input image and each object image
	std::vector<double> similarityScores;
	for (int i = 0; i < objectHistograms.size(); i++)
	{
		double similarityScore = getHistogramSimilarity(inputHistogram, objectHistograms[i]);
		similarityScores.push_back(similarityScore);
		std::cout << "Similarity score between input image and object image " << i << ": " << similarityScore << std::endl;
	}

	// Scan through input image and find the object with the highest similarity score
	// For each scan window, get the histogram and similarity score
	std::vector<double> scanWindowSimilarityScores;
	std::vector<int> scanWindowXCoordinates;
	std::vector<int> scanWindowYCoordinates;
	for (int y = 0; y < imageHeight - scanWindowHeight; y += scanWindowStepHeight)
	{
		for (int x = 0; x < imageWidth - scanWindowWidth; x += scanWindowStepWidth)
		{
			// Get the histogram of the scan window
			MyImage* scanWindowImage = new MyImage();
			scanWindowImage->setWidth(scanWindowWidth);
			scanWindowImage->setHeight(scanWindowHeight);
			scanWindowImage->setImagePath(inImage->getImagePath());
			scanWindowImage->setImageData(inImage->getImageData() + (y * imageWidth + x) * 3);
			Histogram scanWindowHistogram(scanWindowImage);
			// Get the similarity score between the scan window and each object image
			std::vector<double> scanWindowScores;
			for (int i = 0; i < objectHistograms.size(); i++)
			{
				double scanWindowScore = getHistogramSimilarity(scanWindowHistogram, objectHistograms[i]);
				scanWindowScores.push_back(scanWindowScore);
			}
			// Get the highest similarity score between the scan window and each object image
			double scanWindowSimilarityScore = *std::max_element(scanWindowScores.begin(), scanWindowScores.end());
			scanWindowSimilarityScores.push_back(scanWindowSimilarityScore);
			scanWindowXCoordinates.push_back(x);
			scanWindowYCoordinates.push_back(y);
		}
	}
	// Get the highest similarity score between the scan window and each object image
	double highestScanWindowSimilarityScore = *std::max_element(scanWindowSimilarityScores.begin(), scanWindowSimilarityScores.end());
	// Get the index of the highest similarity score
	int highestScanWindowSimilarityScoreIndex = std::distance(scanWindowSimilarityScores.begin(), std::max_element(scanWindowSimilarityScores.begin(), scanWindowSimilarityScores.end()));
	// Add the coordinates of the scan window within the threshold from the highest similarity score to the vector of candidate coordinates
	for (int i = 0; i < scanWindowSimilarityScores.size(); i++)
	{
		if (scanWindowSimilarityScores[i] >= highestScanWindowSimilarityScore - threshold)
		{
			candidateCoordinates.push_back(std::make_pair(scanWindowXCoordinates[i], scanWindowYCoordinates[i]));
			// Print the coordinates and score of the candidate
			std::cout << "Candidate coordinates: (" << scanWindowXCoordinates[i] << ", " << scanWindowYCoordinates[i] << ")" << std::endl;
			std::cout << "Candidate score: " << scanWindowSimilarityScores[i] << std::endl;
			
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
				//char text[1000];
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

				// Draw a rectangle around the candidate coordinates
				for (int i = 0; i < candidateCoordinates.size(); i++)
				{
					Rectangle(hdc, candidateCoordinates[i].first, candidateCoordinates[i].second, candidateCoordinates[i].first + scanWindowWidth, candidateCoordinates[i].second + scanWindowHeight);
				}

							   
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


