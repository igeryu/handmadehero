/*
    Creator: Alan Johnson
	Notice: (C) Copyright 2015 by Molly Rocket, Inc.
*/

#include <Windows.h>
#include <stdint.h>

// Functions that are only accessible from this file:
#define internal static

// Value persists throughout runtime, but only accessible in that scope:
#define local_persist static

// Basic global variable
#define global_variable static

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

//  TODO: Global variable...for now...
global_variable bool Running;

global_variable BITMAPINFO BitmapInfo;

//  Large chunk of memory used by Windows to point to something c doesn't
//  understand.  It must be eventually changed to a pointer understood by
//  the c library.
global_variable void *BitmapMemory;
global_variable int BitmapWidth;
global_variable int BitmapHeight;
global_variable int BytesPerPixel = 4;

internal void
RenderWeirdGradient(int XOffset, int YOffset)
{
	int Width = BitmapWidth;
	int Height = BitmapHeight;
	
	int Pitch = Width * BytesPerPixel;
	uint8 *Row = (uint8 *) BitmapMemory;
	//  This will go over each pixel in the row, for each row.
	for (int Y = 0; Y < BitmapHeight; Y++)
	{
		uint8 *Pixel = (uint8 *) Row;
		for (int X = 0; X < BitmapWidth; X++)
		{
			*Pixel = (uint8)(X + XOffset);
			Pixel++;
			
			*Pixel = (uint8)(Y + YOffset);
			Pixel++;
			
			*Pixel = 0;
			Pixel++;
			
			*Pixel = 0;
			Pixel++;
		}
		
		Row += Pitch;
	}
}

internal void
Win32ResizeDIBSection(int Width, int Height)
{
	
	//  TODO: Maybe free first, free after, then fre first if that fails
	
	//  Start Checking
	
	if (BitmapMemory)
	{
		VirtualFree(BitmapMemory, 0, MEM_RELEASE);
	}
	
	BitmapWidth = Width;
	BitmapHeight = Height;
	
	BitmapInfo.bmiHeader.biSize = sizeof(BitmapInfo.bmiHeader);
	BitmapInfo.bmiHeader.biWidth = BitmapWidth;
	BitmapInfo.bmiHeader.biHeight = -BitmapHeight;
	BitmapInfo.bmiHeader.biPlanes = 1;
	BitmapInfo.bmiHeader.biBitCount = 32;
	BitmapInfo.bmiHeader.biCompression = BI_RGB;
	
	//  NOTE: Help from Chris Hecker
	//  TODO: BytesPerPixel was removed from here
	int BitmapMemorySize = (BitmapWidth * BitmapHeight) * BytesPerPixel;
	BitmapMemory = VirtualAlloc(0, BitmapMemorySize, MEM_COMMIT, PAGE_READWRITE);
	
	//RenderWeirdGradient(0, 0);
	
	// TODO: Probably will clear screen to black
}

internal void
Win32UpdateWindow(HDC DeviceContext, RECT *WindowRect, int X, int Y, int Width, int Height)
{
	int WindowWidth = WindowRect->right - WindowRect->left;
	int WindowHeight = WindowRect->bottom - WindowRect->top;
	StretchDIBits(DeviceContext,
	              /*
	              X, Y, Width, Height,
				  X, Y, Width, Height,
				  */
				  0, 0, BitmapWidth, BitmapHeight,
				  0, 0, WindowWidth, WindowHeight,
				  BitmapMemory, &BitmapInfo,
				  DIB_RGB_COLORS,
				  SRCCOPY);
}

LRESULT CALLBACK
Win32MainWindowCallback(HWND Window,
				        UINT Message,
						WPARAM WParam,
						LPARAM LParam)
{
	LRESULT Result = 0;
	
	switch(Message)
	{
		case WM_SIZE:
		{
			RECT ClientRect;
			GetClientRect(Window, &ClientRect);
			int Width = ClientRect.right - ClientRect.left;
			int Height = ClientRect.bottom - ClientRect.top;
			Win32ResizeDIBSection(Width, Height);
		} break;
		
		case WM_DESTROY:
		{
			//  TODO: Add more later (recreate window?)
			Running = false;
		} break;
		
		case WM_CLOSE:
		{
			//  TODO: Add more later (message to user?)
			Running = false;
		} break;
		
		case WM_ACTIVATEAPP:
		{
			OutputDebugStringA("WM_ACTIVATEAPP\n");
		} break;
		
		case WM_PAINT:
		{
			PAINTSTRUCT Paint;
			HDC DeviceContext = BeginPaint(Window, &Paint);
			int X = Paint.rcPaint.left;
			int Y = Paint.rcPaint.top;
			int Height = Paint.rcPaint.bottom - Paint.rcPaint.top;
			int Width = Paint.rcPaint.right - Paint.rcPaint.left;
			
			//  TODO: temporary..
			RECT ClientRect;
			GetClientRect(Window, &ClientRect);
			
			Win32UpdateWindow(DeviceContext, &ClientRect, X, Y, Width, Height);
			EndPaint(Window, &Paint);
		} break;
		
		default:
		{
			Result = DefWindowProc(Window, Message, WParam, LParam);
		} break;
		
	}
		
		return (Result);
		
};

int CALLBACK
WinMain(HINSTANCE Instance,
		HINSTANCE PrevInstance,
		LPSTR CommandLine,
		int ShowCode)
{
	WNDCLASS WindowClass =  {};
	
	// TODO: Check about CS_OWNDC|CS_HREDRAW|CS_VREDRAW
	WindowClass.hInstance = Instance;
	WindowClass.lpfnWndProc = Win32MainWindowCallback;
	//WindowClass.hIcon;
	WindowClass.lpszClassName = "HandmadeHeroWindowClass";
	
	if (RegisterClass(&WindowClass))
	{
		//  Ask Windows to create a window:
		HWND Window = 
			CreateWindowExA(
			0,
			WindowClass.lpszClassName,
			"Handmade Hero",
			WS_OVERLAPPEDWINDOW|WS_VISIBLE,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			0,
			0,
			Instance,
			0);
		
		//  Check if Windows created a window and gave it to me:
		if (Window)
		{
			int XOffset = 0;
			int YOffset = 0;
			
			Running = true;
			while(Running)
			{
				
				MSG Message;
				
				//  Check for window message, store it in Message
				//  and set results to MessageResult:
				while (BOOL MessageResult = PeekMessageA(&Message, 0, 0, 0, PM_REMOVE))
				{
					if (Message.message == WM_QUIT)
					{
						Running = false;
					}
					TranslateMessage(&Message);
					DispatchMessageA(&Message);
				}
				
				RenderWeirdGradient(XOffset, YOffset);
				
				HDC DeviceContext = GetDC(Window);
				RECT ClientRect;
				GetClientRect(Window, &ClientRect);
				int WindowWidth = ClientRect.right - ClientRect.left;
				int WindowHeight = ClientRect.bottom - ClientRect.top;
				Win32UpdateWindow(DeviceContext, &ClientRect, 0, 0, WindowWidth, WindowHeight);
				ReleaseDC(Window, DeviceContext);
				
				XOffset++;
				
			}  //  While Running...
		}
		else
		{
			//  TODO: log something...
		}
	}
	else
	{
		//  TODO: log something...
	}

	return(0);
};