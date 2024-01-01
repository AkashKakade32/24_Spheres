// Windows Header Files
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "OGL.h"

//OpenGL Header Files
#include<gl/GL.h>
#include<GL/glu.h>

// Macros
#define WIN_WIDTH 800
#define WIN_HEIGHT 600

// Link With OpenGl Library
#pragma comment(lib, "OpenGL32.lib")
#pragma comment (lib,"glu32.lib")

// Global function declarations
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// Global Variable Declaration
FILE* gpFile = NULL;

// Global Variations Declarations
HWND ghwnd = NULL;
BOOL gbActive = FALSE;
DWORD dwStyle = 0;
WINDOWPLACEMENT wpPrev = { sizeof(WINDOWPLACEMENT) };
BOOL gbFullscreen = FALSE;

BOOL bLight = FALSE;

GLfloat lightAmbient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
GLfloat lightDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat lightPosition[] = { 0.0f, 0.0f, 0.0f, 1.0f };

GLUquadric* quadric = NULL;

GLfloat angleForXRotation = 0.0f;
GLfloat angleForYRotation = 0.0f;
GLfloat angleForZRotation = 0.0f;

GLuint keyPressed = 0;

//OpenGL Related Global Variations Declarations
HDC ghdc = NULL;
HGLRC ghrc = NULL;  //Handle to OpenGL Rensering Context


// Entry Point Function
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{
	// Functions Declaratios
	int initialize(void);
	void uninitialize(void);
	void display(void);
	void update(void);

	// local variable declarations
	WNDCLASSEX wndclass;
	HWND hwnd;
	MSG msg;
	TCHAR szAppName[] = TEXT("AKWindow");
	int iResult = 0;



	// code
	gpFile = fopen("Log.txt", "w");
	if (gpFile == NULL)
	{
		MessageBox(NULL, TEXT("Log File Cannot Be Opened"), TEXT("Error"), MB_OK | MB_ICONERROR);
		exit(0);
	}
	fprintf(gpFile, "Program Started Successfully \n");

	// WNDCLASSEX initialization
	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wndclass.cbClsExtra = 0; 
	wndclass.cbWndExtra = 0;
	wndclass.lpfnWndProc = WndProc;
	wndclass.hInstance = hInstance;
	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(MYICON));
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.lpszClassName = szAppName;
	wndclass.lpszMenuName = NULL;
	wndclass.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(MYICON));

	fprintf(gpFile, "Successfully Added Icon To The File \n");

	// REgister WNDCLASSEX
	RegisterClassEx(&wndclass);
	
	// 
	int XPos = GetSystemMetrics(0);
	int YPos = GetSystemMetrics(1);

	fprintf(gpFile, "Xpos is %d Ypos is %d\n", XPos, YPos);

	// Create window
	hwnd = CreateWindowEx(WS_EX_APPWINDOW,
		szAppName,
		TEXT("Akash Avinash Kakade"),
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE,
		(XPos - WIN_WIDTH) / 2,
		(YPos - WIN_HEIGHT) / 2,
		WIN_WIDTH,
		WIN_HEIGHT,
		NULL,
		NULL,
		hInstance,
		NULL);

	ghwnd = hwnd;

	fprintf(gpFile, "Window Is Centered\n");

	// Initialization

	iResult = initialize();
	if (iResult != 0)
	{
		MessageBox(hwnd, TEXT("initialize() Failed"), TEXT("Error"), MB_OK | MB_ICONERROR);
		DestroyWindow(hwnd);
	}

	// Show the window
	ShowWindow(hwnd, iCmdShow);

	SetForegroundWindow(hwnd); // Top in z Order
	SetFocus(hwnd);

	BOOL bDone = FALSE;

	while (bDone == FALSE)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				bDone = TRUE;
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			if (gbActive == TRUE)
			{
				// Render
				display();

				// Update
				update();
			}
		}
	}

	uninitialize();

	return((int)msg.wParam);

}

// Callback Function
LRESULT CALLBACK WndProc(HWND hwnd,
	UINT iMsg,
	WPARAM wParam,
	LPARAM lParam)
{
	// Function Declarations 
	void ToggleFullscreen(void);
	void resize(int, int);

	// code
	switch (iMsg)
	{
	case WM_SETFOCUS:
		gbActive = TRUE;
		break;
	case WM_KILLFOCUS:
		gbActive = FALSE;
		break;
	case WM_SIZE:
		resize(LOWORD(lParam), HIWORD(lParam));
		break;
	case WM_ERASEBKGND:
		return(0);
	case WM_KEYDOWN:
		switch (LOWORD(wParam))
		{
		case VK_ESCAPE:
			fprintf(gpFile, "Program Ended Successfully by Pressing 'ESCAPE' Key \n");
			DestroyWindow(hwnd);
			break;
		}
		break;
	case WM_CHAR:
		switch (LOWORD(wParam))
		{
		case 'F':
		case 'f':
			if (gbFullscreen == FALSE)
			{
				fprintf(gpFile, "You have pressed 'F' / 'f' to Fullscreen The Window ... Now you are in Fullscreen Mode \n");
				ToggleFullscreen();
				gbFullscreen = TRUE;
			}
			else
			{
				fprintf(gpFile, "You have pressed 'F' / 'f' to EXIT Fullscreen window ... Now you are not in Fullscreen mode \n");
				ToggleFullscreen();
				gbFullscreen = FALSE;
			}
			break;

		case 'L':
		case 'l':
			if (bLight == FALSE)
			{
				glEnable(GL_LIGHTING);
				bLight = TRUE;
			}
			else
			{
				glDisable(GL_LIGHTING);
				bLight = FALSE;
			}
			break;

		case 'X':
		case 'x':
			keyPressed = 1;
			angleForXRotation = 0.0f; //Reset
			break;

		case 'Y':
		case 'y':
			keyPressed = 2;
			angleForYRotation = 0.0f; //Reset
			break;

		case 'Z':
		case 'z':
			keyPressed = 3;
			angleForZRotation = 0.0f; //Reset
			break;
		}
		break;
	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		break;
	}
	return(DefWindowProc(hwnd, iMsg, wParam, lParam));
}

void ToggleFullscreen(void)
{
	MONITORINFO mi = { sizeof(MONITORINFO) };

	//code

	if (gbFullscreen == FALSE)
	{
		dwStyle = GetWindowLong(ghwnd, GWL_STYLE);
		if (dwStyle & WS_OVERLAPPEDWINDOW)
		{
			if (GetWindowPlacement(ghwnd, &wpPrev) && GetMonitorInfo(MonitorFromWindow(ghwnd, MONITORINFOF_PRIMARY), &mi))
			{
				SetWindowLong(ghwnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
				SetWindowPos(ghwnd, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top, mi.rcMonitor.right - mi.rcMonitor.left, mi.rcMonitor.bottom - mi.rcMonitor.top, SWP_NOZORDER | SWP_FRAMECHANGED);
			}
		}

		ShowCursor(FALSE);
	}
	else
	{
		SetWindowPlacement(ghwnd, &wpPrev);
		SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED); // It has greater priority
		ShowCursor(TRUE);
	}
}

int initialize(void)
{
	// Function Declarations
	void resize(int, int);

	//code

	// Define Pixel Format Descriptor
	PIXELFORMATDESCRIPTOR pfd;
	int iPixelFormatIndex = 0;
	ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));

	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR); 
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cRedBits = 8;
	pfd.cGreenBits = 8;
	pfd.cBlueBits = 8;
	pfd.cAlphaBits = 8;
	pfd.cDepthBits = 32;


	// Get The DC
	ghdc = GetDC(ghwnd);
	if (ghdc == NULL)
	{
		fprintf(gpFile, "Get DC() Failed \n");
		return(-1);
	}

	// 
	iPixelFormatIndex = ChoosePixelFormat(ghdc, &pfd);
	if (iPixelFormatIndex == 0)
	{
		fprintf(gpFile, "ChoosePixelFormat() Failed\n");
		return(-2);
	}

	//Set Obtained Pixel Format
	if (SetPixelFormat(ghdc, iPixelFormatIndex, &pfd) == FALSE)
	{
		fprintf(gpFile, "SetPixelFormat() Failed\n");
		return(-3);
	}

	// Create OpenGl Context from Device Context
	ghrc = wglCreateContext(ghdc);
	if (ghrc == NULL)
	{
		fprintf(gpFile, "wglCreateContext() Failed\n");
		return(-4);
	}

	// Make Rendering Context Current
	if (wglMakeCurrent(ghdc, ghrc) == FALSE)
	{
		fprintf(gpFile, "wglMakeCurrent() Failed\n");
		return(-5);
	}

	//Enabling Depth
	glShadeModel(GL_SMOOTH); //Whenever there are colors and lights make it smooth
	glClearDepth(1.0f); //
	glEnable(GL_DEPTH_TEST); // Enabling Depth Test
	glDepthFunc(GL_LEQUAL); // Less than euqal to of ClearDepth value.
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	//Light Related Initialization 
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);

	glEnable(GL_LIGHT0);


	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	//Initialize Quadric
	quadric = gluNewQuadric();

	//Set The Clear Color Of Window to Blue. Here OpenGL Start
	glClearColor(0.25f, 0.25f, 0.25f, 1.0f);

	resize(WIN_WIDTH, WIN_HEIGHT);

	return(0);
}

void resize(int Width, int Height)
{
	// code
	if (Height <= 0)
		Height = 1;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	//gluPerspective(45.0f, (GLfloat)Width / (GLfloat)Height, 0.1f, 100.0f);

	glViewport(0, 0, (GLsizei)Width, (GLsizei)Height); //Binaculoros

	if (Width <= Height)
	{
		glOrtho(0.0f,
			15.5f,
			0.0f * ((GLfloat)Height / (GLfloat)Width),
			15.5f * ((GLfloat)Height / (GLfloat)Width),
			-10.0f,
			10.0f); 
	}
	else
	{
		glOrtho(0.0f * ((GLfloat)Width / (GLfloat)Height),
			15.5f * ((GLfloat)Width / (GLfloat)Height),
			0.0f,
			15.0f,
			-10.0f,
			10.0f);
	}

}

void display(void)
{
	// Variable Declarations
	GLfloat materialAmbient[4];
	GLfloat materialDiffuse[4];
	GLfloat materialSpecular[4];
	GLfloat Shininess;

	// code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//Animation
	if (keyPressed == 1)
	{
		glRotatef(angleForXRotation, 1.0f, 0.0f, 0.0f);

		lightPosition[2] = angleForXRotation;
	}

	if (keyPressed == 2)
	{
		glRotatef(angleForYRotation, 0.0f, 1.0f, 0.0f);

		lightPosition[0] = angleForYRotation;
	}

	if (keyPressed == 3)
	{
		glRotatef(angleForZRotation, 0.0f, 0.0f, 1.0f);

		lightPosition[1] = angleForZRotation;
	}

	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

	//First Column of Precious Stones
	//Emerald Material
	materialAmbient[0] = 0.0215;
	materialAmbient[1] = 0.1745;
	materialAmbient[2] = 0.0215;
	materialAmbient[3] = 1.0f;

	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

	materialDiffuse[0] = 0.07568; 
	materialDiffuse[1] = 0.61424; 
	materialDiffuse[2] = 0.07568; 
	materialDiffuse[3] = 1.0f;

	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

	materialSpecular[0] = 0.633;    
	materialSpecular[1] = 0.727811;
	materialSpecular[2] = 0.633;    
	materialSpecular[3] = 1.0f;

	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

	Shininess = 0.6 * 128;

	glMaterialf(GL_FRONT, GL_SHININESS, Shininess);

	glLoadIdentity();

	glTranslatef(1.5f, 14.0f, 0.0f);

	gluSphere(quadric, 1.0f, 30.0f, 30.0f); //gluSpehere creates all normals for you. (glu utility creates all normals)

	//Jade Material
	materialAmbient[0] = 0.135;
	materialAmbient[1] = 0.2225;
	materialAmbient[2] = 0.1575;
	materialAmbient[3] = 1.0f;

	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

	materialDiffuse[0] = 0.54;
	materialDiffuse[1] = 0.89;
	materialDiffuse[2] = 0.63;
	materialDiffuse[3] = 1.0f;

	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

	materialSpecular[0] = 0.316228;
	materialSpecular[1] = 0.316228;
	materialSpecular[2] = 0.316228;
	materialSpecular[3] = 1.0f;

	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

	Shininess = 0.1 * 128;

	glMaterialf(GL_FRONT, GL_SHININESS, Shininess);

	glLoadIdentity();

	glTranslatef(1.5f, 11.5f, 0.0f);

	gluSphere(quadric, 1.0f, 30.0f, 30.0f);

	//Obsidian 
	materialAmbient[0] = 0.05375;
	materialAmbient[1] = 0.05;
	materialAmbient[2] = 0.06625;
	materialAmbient[3] = 1.0f;

	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

	materialDiffuse[0] = 0.18275;
	materialDiffuse[1] = 0.17;
	materialDiffuse[2] = 0.22525;
	materialDiffuse[3] = 1.0f;

	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

	materialSpecular[0] = 0.332741;
	materialSpecular[1] = 0.328634;
	materialSpecular[2] = 0.346435;
	materialSpecular[3] = 1.0f;

	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

	Shininess = 0.3 * 128;

	glMaterialf(GL_FRONT, GL_SHININESS, Shininess);

	glLoadIdentity();

	glTranslatef(1.5f, 9.0f, 0.0f);

	gluSphere(quadric, 1.0f, 30.0f, 30.0f);

	//Pearl
	materialAmbient[0] = 0.25;
	materialAmbient[1] = 0.20725;
	materialAmbient[2] = 0.20725;
	materialAmbient[3] = 1.0f;

	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

	materialDiffuse[0] = 1.0;
	materialDiffuse[1] = 0.829;
	materialDiffuse[2] = 0.829;
	materialDiffuse[3] = 1.0f;

	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

	materialSpecular[0] = 0.296648;
	materialSpecular[1] = 0.296648;
	materialSpecular[2] = 0.296648;
	materialSpecular[3] = 1.0f;

	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

	Shininess = 0.088 * 128;

	glMaterialf(GL_FRONT, GL_SHININESS, Shininess);

	glLoadIdentity();

	glTranslatef(1.5f, 6.5f, 0.0f);

	gluSphere(quadric, 1.0f, 30.0f, 30.0f);

	//ruby material
	materialAmbient[0] = 0.1745f;
	materialAmbient[1] = 0.01175f;
	materialAmbient[2] = 0.01175f;
	materialAmbient[3] = 1.0f;

	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

	materialDiffuse[0] = 0.61425f;
	materialDiffuse[1] = 0.04136;
	materialDiffuse[2] = 0.04136;
	materialDiffuse[3] = 1.0f;

	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

	materialSpecular[0] = 0.727811;
	materialSpecular[1] = 0.626959;
	materialSpecular[2] = 0.626959;
	materialSpecular[3] = 1.0f;

	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

	Shininess = 0.6 * 128;

	glMaterialf(GL_FRONT, GL_SHININESS, Shininess);

	glLoadIdentity();

	glTranslatef(1.5f, 4.0f, 0.0f);

	gluSphere(quadric, 1.0f, 30.0f, 30.0f);

	//turquoise material
	materialAmbient[0] = 0.1f;
	materialAmbient[1] = 0.18725;
	materialAmbient[2] = 0.1745;
	materialAmbient[3] = 1.0f;

	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

	materialDiffuse[0] = 0.396;
	materialDiffuse[1] = 0.74151;
	materialDiffuse[2] = 0.69102;
	materialDiffuse[3] = 1.0f;

	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

	materialSpecular[0] = 0.297254;
	materialSpecular[1] = 0.30829;
	materialSpecular[2] = 0.306678;
	materialSpecular[3] = 1.0f;

	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

	Shininess = 0.1 * 128;

	glMaterialf(GL_FRONT, GL_SHININESS, Shininess);

	glLoadIdentity();

	glTranslatef(1.5f, 1.5f, 0.0f);

	gluSphere(quadric, 1.0f, 30.0f, 30.0f);

	//*****************************************************

	//2nd Column

	//brass
	materialAmbient[0] = 0.329412;
	materialAmbient[1] = 0.223529;
	materialAmbient[2] = 0.027451;
	materialAmbient[3] = 1.0f;

	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

	materialDiffuse[0] = 0.780392;
	materialDiffuse[1] = 0.568627;
	materialDiffuse[2] = 0.113725;
	materialDiffuse[3] = 1.0f;

	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

	materialSpecular[0] = 0.992157;
	materialSpecular[1] = 0.941176;
	materialSpecular[2] = 0.807843;
	materialSpecular[3] = 1.0f;

	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

	Shininess = 0.21794872 * 128;

	glMaterialf(GL_FRONT, GL_SHININESS, Shininess);

	glLoadIdentity();

	glTranslatef(7.5f, 14.0f, 0.0f);

	gluSphere(quadric, 1.0f, 30.0f, 30.0f);

	//bronze
	materialAmbient[0] = 0.2125;
	materialAmbient[1] = 0.1275;
	materialAmbient[2] = 0.054;
	materialAmbient[3] = 1.0f;

	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

	materialDiffuse[0] = 0.714;
	materialDiffuse[1] = 0.4284;
	materialDiffuse[2] = 0.18144;
	materialDiffuse[3] = 1.0f;

	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

	materialSpecular[0] = 0.393548;
	materialSpecular[1] = 0.271906;
	materialSpecular[2] = 0.166721;
	materialSpecular[3] = 1.0f;

	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

	Shininess = 0.2 * 128;

	glMaterialf(GL_FRONT, GL_SHININESS, Shininess);

	glLoadIdentity();

	glTranslatef(7.5f, 11.5f, 0.0f);

	gluSphere(quadric, 1.0f, 30.0f, 30.0f);

	//Chrome 
	materialAmbient[0] = 0.25;
	materialAmbient[1] = 0.25;
	materialAmbient[2] = 0.25;
	materialAmbient[3] = 1.0f;

	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

	materialDiffuse[0] = 0.4;
	materialDiffuse[1] = 0.4;
	materialDiffuse[2] = 0.4;
	materialDiffuse[3] = 1.0f;

	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

	materialSpecular[0] = 0.774597;
	materialSpecular[1] = 0.774597;
	materialSpecular[2] = 0.774597;
	materialSpecular[3] = 1.0f;

	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

	Shininess = 0.6 * 128;

	glMaterialf(GL_FRONT, GL_SHININESS, Shininess);

	glLoadIdentity();

	glTranslatef(7.5f, 9.0f, 0.0f);

	gluSphere(quadric, 1.0f, 30.0f, 30.0f);

	//Copper
	materialAmbient[0] = 0.19125;
	materialAmbient[1] = 0.0735;
	materialAmbient[2] = 0.0225;
	materialAmbient[3] = 1.0f;

	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

	materialDiffuse[0] = 0.7038;
	materialDiffuse[1] = 0.27048;
	materialDiffuse[2] = 0.0828;
	materialDiffuse[3] = 1.0f;

	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

	materialSpecular[0] = 0.256777;
	materialSpecular[1] = 0.137622;
	materialSpecular[2] = 0.086014;
	materialSpecular[3] = 1.0f;

	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

	Shininess = 0.1 * 128;

	glMaterialf(GL_FRONT, GL_SHININESS, Shininess);

	glLoadIdentity();

	glTranslatef(7.5f, 6.5f, 0.0f);

	gluSphere(quadric, 1.0f, 30.0f, 30.0f);


	//gold
	materialAmbient[0] = 0.24725;
	materialAmbient[1] = 0.1995;
	materialAmbient[2] = 0.0745;
	materialAmbient[3] = 1.0f;

	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

	materialDiffuse[0] = 0.75164;
	materialDiffuse[1] = 0.60648;
	materialDiffuse[2] = 0.22648;
	materialDiffuse[3] = 1.0f;

	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

	materialSpecular[0] = 0.628281;
	materialSpecular[1] = 0.555802;
	materialSpecular[2] = 0.366065;
	materialSpecular[3] = 1.0f;

	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

	Shininess = 0.4 * 128;

	glMaterialf(GL_FRONT, GL_SHININESS, Shininess);

	glLoadIdentity();

	glTranslatef(7.5f, 4.0f, 0.0f);

	gluSphere(quadric, 1.0f, 30.0f, 30.0f);

	//Silver
	materialAmbient[0] = 0.19225;
	materialAmbient[1] = 0.19225;
	materialAmbient[2] = 0.19225;
	materialAmbient[3] = 1.0f;

	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

	materialDiffuse[0] = 0.50754;
	materialDiffuse[1] = 0.50754;
	materialDiffuse[2] = 0.50754;
	materialDiffuse[3] = 1.0f;

	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

	materialSpecular[0] = 0.508273;
	materialSpecular[1] = 0.508273;
	materialSpecular[2] = 0.508273;
	materialSpecular[3] = 1.0f;

	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

	Shininess = 0.4 * 128;

	glMaterialf(GL_FRONT, GL_SHININESS, Shininess);

	glLoadIdentity();

	glTranslatef(7.5f, 1.5f, 0.0f);

	gluSphere(quadric, 1.0f, 30.0f, 30.0f);

	//****************************************************

	//3rd column

	//black
	materialAmbient[0] = 0.0;
	materialAmbient[1] = 0.0;
	materialAmbient[2] = 0.0;
	materialAmbient[3] = 1.0f;

	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

	materialDiffuse[0] = 0.01;
	materialDiffuse[1] = 0.01;
	materialDiffuse[2] = 0.01;
	materialDiffuse[3] = 1.0f;

	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

	materialSpecular[0] = 0.50;
	materialSpecular[1] = 0.50;
	materialSpecular[2] = 0.50;
	materialSpecular[3] = 1.0f;

	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

	Shininess = 0.25 * 128;

	glMaterialf(GL_FRONT, GL_SHININESS, Shininess);

	glLoadIdentity();

	glTranslatef(13.5f, 14.0f, 0.0f);

	gluSphere(quadric, 1.0f, 30.0f, 30.0f);

	//cyan
	materialAmbient[0] = 0.0;
	materialAmbient[1] = 0.1;
	materialAmbient[2] = 0.06;
	materialAmbient[3] = 1.0f;

	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

	materialDiffuse[0] = 0.0;
	materialDiffuse[1] = 0.50980392;
	materialDiffuse[2] = 0.50980392;
	materialDiffuse[3] = 1.0f;

	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

	materialSpecular[0] = 0.50196078;
	materialSpecular[1] = 0.50196078;
	materialSpecular[2] = 0.50196078;
	materialSpecular[3] = 1.0f;

	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

	Shininess = 0.25 * 128;

	glMaterialf(GL_FRONT, GL_SHININESS, Shininess);

	glLoadIdentity();

	glTranslatef(13.5f, 11.5f, 0.0f);

	gluSphere(quadric, 1.0f, 30.0f, 30.0f);

	
	//Green
	materialAmbient[0] = 0.0;
	materialAmbient[1] = 0.0;
	materialAmbient[2] = 0.0;
	materialAmbient[3] = 1.0f;

	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

	materialDiffuse[0] = 0.1;
	materialDiffuse[1] = 0.35;
	materialDiffuse[2] = 0.1;
	materialDiffuse[3] = 1.0f;

	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

	materialSpecular[0] = 0.45;
	materialSpecular[1] = 0.55;
	materialSpecular[2] = 0.45;
	materialSpecular[3] = 1.0f;

	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

	Shininess = 0.25 * 128;

	glMaterialf(GL_FRONT, GL_SHININESS, Shininess);

	glLoadIdentity();

	glTranslatef(13.5f, 9.0f, 0.0f);

	gluSphere(quadric, 1.0f, 30.0f, 30.0f);

	
	//red
	materialAmbient[0] = 0.0;
	materialAmbient[1] = 0.0;
	materialAmbient[2] = 0.0;
	materialAmbient[3] = 1.0f;

	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

	materialDiffuse[0] = 0.5;
	materialDiffuse[1] = 0.0;
	materialDiffuse[2] = 0.0;
	materialDiffuse[3] = 1.0f;

	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

	materialSpecular[0] = 0.7;
	materialSpecular[1] = 0.6;
	materialSpecular[2] = 0.6;
	materialSpecular[3] = 1.0f;

	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

	Shininess = 0.25 * 128;

	glMaterialf(GL_FRONT, GL_SHININESS, Shininess);

	glLoadIdentity();

	glTranslatef(13.5f, 6.5f, 0.0f);

	gluSphere(quadric, 1.0f, 30.0f, 30.0f);


	//white
	materialAmbient[0] = 0.0;
	materialAmbient[1] = 0.0;
	materialAmbient[2] = 0.0;
	materialAmbient[3] = 1.0f;

	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

	materialDiffuse[0] = 0.55;
	materialDiffuse[1] = 0.55;
	materialDiffuse[2] = 0.55;
	materialDiffuse[3] = 1.0f;

	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

	materialSpecular[0] = 0.70;
	materialSpecular[1] = 0.70;
	materialSpecular[2] = 0.70;
	materialSpecular[3] = 1.0f;

	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

	Shininess = 0.25 * 128;

	glMaterialf(GL_FRONT, GL_SHININESS, Shininess);

	glLoadIdentity();

	glTranslatef(13.5f, 4.0f, 0.0f);

	gluSphere(quadric, 1.0f, 30.0f, 30.0f);

	//Yellow Plastic
	materialAmbient[0] = 0.0;
	materialAmbient[1] = 0.0;
	materialAmbient[2] = 0.0;
	materialAmbient[3] = 1.0f;

	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

	materialDiffuse[0] = 0.5;
	materialDiffuse[1] = 0.5;
	materialDiffuse[2] = 0.5;
	materialDiffuse[3] = 1.0f;

	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

	materialSpecular[0] = 0.60;
	materialSpecular[1] = 0.60;
	materialSpecular[2] = 0.50;
	materialSpecular[3] = 1.0f;

	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

	Shininess = 0.25 * 128;

	glMaterialf(GL_FRONT, GL_SHININESS, Shininess);

	glLoadIdentity();

	glTranslatef(13.5f, 1.5f, 0.0f);

	gluSphere(quadric, 1.0f, 30.0f, 30.0f);

	//***************************************************

	//4th Column

	//black
	materialAmbient[0] = 0.02;
	materialAmbient[1] = 0.02;
	materialAmbient[2] = 0.02;
	materialAmbient[3] = 1.0f;

	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

	materialDiffuse[0] = 0.01;
	materialDiffuse[1] = 0.01;
	materialDiffuse[2] = 0.01;
	materialDiffuse[3] = 1.0f;

	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

	materialSpecular[0] = 0.4;
	materialSpecular[1] = 0.4;
	materialSpecular[2] = 0.4;
	materialSpecular[3] = 1.0f;

	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

	Shininess = 0.078125 * 128;

	glMaterialf(GL_FRONT, GL_SHININESS, Shininess);

	glLoadIdentity();

	glTranslatef(19.5f, 14.0f, 0.0f);

	gluSphere(quadric, 1.0f, 30.0f, 30.0f);

	//cyan
	materialAmbient[0] = 0.0;
	materialAmbient[1] = 0.05;
	materialAmbient[2] = 0.05;
	materialAmbient[3] = 1.0f;

	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

	materialDiffuse[0] = 0.4;
	materialDiffuse[1] = 0.5;
	materialDiffuse[2] = 0.5;
	materialDiffuse[3] = 1.0f;

	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

	materialSpecular[0] = 0.04;
	materialSpecular[1] = 0.7;
	materialSpecular[2] = 0.7;
	materialSpecular[3] = 1.0f;

	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

	Shininess = 0.078125 * 128;

	glMaterialf(GL_FRONT, GL_SHININESS, Shininess);

	glLoadIdentity();

	glTranslatef(19.5f, 11.5f, 0.0f);

	gluSphere(quadric, 1.0f, 30.0f, 30.0f);

	//Green
	materialAmbient[0] = 0.0;
	materialAmbient[1] = 0.05;
	materialAmbient[2] = 0.05;
	materialAmbient[3] = 1.0f;

	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

	materialDiffuse[0] = 0.4;
	materialDiffuse[1] = 0.5;
	materialDiffuse[2] = 0.4;
	materialDiffuse[3] = 1.0f;

	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

	materialSpecular[0] = 0.04;
	materialSpecular[1] = 0.7;
	materialSpecular[2] = 0.04;
	materialSpecular[3] = 1.0f;

	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

	Shininess = 0.078125 * 128;

	glMaterialf(GL_FRONT, GL_SHININESS, Shininess);

	glLoadIdentity();

	glTranslatef(19.5f, 9.0f, 0.0f);

	gluSphere(quadric, 1.0f, 30.0f, 30.0f);

	//red
	materialAmbient[0] = 0.05;
	materialAmbient[1] = 0.00;
	materialAmbient[2] = 0.00;
	materialAmbient[3] = 1.0f;

	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

	materialDiffuse[0] = 0.5;
	materialDiffuse[1] = 0.5;
	materialDiffuse[2] = 0.4;
	materialDiffuse[3] = 1.0f;

	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

	materialSpecular[0] = 0.7;
	materialSpecular[1] = 0.04;
	materialSpecular[2] = 0.04;
	materialSpecular[3] = 1.0f;

	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

	Shininess = 0.078125 * 128;

	glMaterialf(GL_FRONT, GL_SHININESS, Shininess);

	glLoadIdentity();

	glTranslatef(19.5f, 6.5f, 0.0f);

	gluSphere(quadric, 1.0f, 30.0f, 30.0f);

	//white
	materialAmbient[0] = 0.05;
	materialAmbient[1] = 0.05;
	materialAmbient[2] = 0.05;
	materialAmbient[3] = 1.0f;

	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

	materialDiffuse[0] = 0.5;
	materialDiffuse[1] = 0.5;
	materialDiffuse[2] = 0.5;
	materialDiffuse[3] = 1.0f;

	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

	materialSpecular[0] = 0.7;
	materialSpecular[1] = 0.7;
	materialSpecular[2] = 0.7;
	materialSpecular[3] = 1.0f;

	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

	Shininess = 0.078125 * 128;

	glMaterialf(GL_FRONT, GL_SHININESS, Shininess);

	glLoadIdentity();

	glTranslatef(19.5f, 4.0f, 0.0f);

	gluSphere(quadric, 1.0f, 30.0f, 30.0f);

	//Yellow Rubber
	materialAmbient[0] = 0.05;
	materialAmbient[1] = 0.05;
	materialAmbient[2] = 0.0;
	materialAmbient[3] = 1.0f;

	glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

	materialDiffuse[0] = 0.5;
	materialDiffuse[1] = 0.5;
	materialDiffuse[2] = 0.4;
	materialDiffuse[3] = 1.0f;

	glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

	materialSpecular[0] = 0.7;
	materialSpecular[1] = 0.7;
	materialSpecular[2] = 0.04;
	materialSpecular[3] = 1.0f;

	glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

	Shininess = 0.078125 * 128;

	glMaterialf(GL_FRONT, GL_SHININESS, Shininess);

	glLoadIdentity();

	glTranslatef(19.5f, 1.5f, 0.0f);

	gluSphere(quadric, 1.0f, 30.0f, 30.0f);

	SwapBuffers(ghdc);
}

void update(void)
{
	// code
	if (keyPressed == 1)
	{
		angleForXRotation = angleForXRotation + 1.0f;

		if (angleForXRotation >= 360.0f)
			angleForXRotation = angleForXRotation - 360.0f;

	}

	if (keyPressed == 2)
	{
		angleForYRotation = angleForYRotation + 1.0f;

		if (angleForYRotation >= 360.0f)
			angleForYRotation = angleForYRotation - 360.0f;
	}

	if (keyPressed == 3)
	{
		angleForZRotation = angleForZRotation + 1.0f;

		if (angleForZRotation >= 360.0f)
			angleForZRotation = angleForZRotation - 360.0f;
	}
}

void uninitialize(void)
{
	// Function Declarations
	void ToggleFullscreen(void);

	// code
	// If Application is exiting in Fullscreen
	if (gbFullscreen == TRUE)
	{
		ToggleFullscreen();
	}

	//Make the HDC as Current DC
	if (wglGetCurrentContext() == ghrc)
	{
		wglMakeCurrent(NULL, NULL);
	}

	//Delete Rendering Context
	if (ghrc)
	{
		wglDeleteContext(ghrc);
		ghrc = NULL;
	}

	if (quadric)
	{
		gluDeleteQuadric(quadric);
		quadric = NULL;
	}

	//Delete The HDC
	if (ghdc)
	{
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}

	// Destroy Window
	if (ghwnd)
	{
		DestroyWindow(ghwnd);
		ghwnd = NULL;
	}

	// Close the log file
	if (gpFile)
	{
		fprintf(gpFile, "Program Ended Successfully \n");
		fclose(gpFile);
		gpFile = NULL;
	}

}



