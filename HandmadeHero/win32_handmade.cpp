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

struct win32_offscreen_buffer
{
	BITMAPINFO Info;
	//  Large chunk of memory used by Windows to point to something c doesn't
	//  understand.  It must be eventually changed to a pointer understood by
	//  the c library.
	void *Memory;
	int Width;
	int Height;
	int Pitch;
	int BytesPerPixel;
};

//  TODO: Global variable...for now...
global_variable bool Running;
global_variable win32_offscreen_buffer GlobalBackbuffer;

struct win32_window_dimension
{
	int Width;
	int Height;
};

win32_window_dimension Win32GetWindowDimension(HWND Window)
{
	win32_window_dimension Result;
	RECT ClientRect;
	GetClientRect(Window, &ClientRect);
	
	Result.Width = ClientRect.right - ClientRect.left;
	Result.Height = ClientRect.bottom - ClientRect.top;
	
	return Result;
}

internal void
RenderWeirdGradient(win32_offscreen_buffer Buffer, int XOffset, int YOffset)
{
	// TODO: See what the optimizer does
	
	uint8 *Row = (uint8 *) Buffer.Memory;
	//  This will go over each pixel in the row, for each row.
	for (int Y = 0; Y < Buffer.Height; Y++)
	{
		uint8 *Pixel = (uint8 *) Row;
		for (int X = 0; X < Buffer.Width; X++)
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
		
		Row += Buffer.Pitch;
	}
}

internal void
Win32ResizeDIBSection(win32_offscreen_buffer *Buffer, int Width, int Height)
{
	
	//  TODO: Maybe free first, free after, then fre first if that fails
	
	//  Check if there is already memory in the buffer, and get rid of it:
	if (Buffer->Memory)
	{
		VirtualFree(Buffer->Memory, 0, MEM_RELEASE);
	}
	
	//  Assign pertinent values to the buffer:
	Buffer->Width = Width;
	Buffer->Height = Height;
	Buffer->BytesPerPixel = 4;
	
	Buffer->Info.bmiHeader.biSize = sizeof(Buffer->Info.bmiHeader);
	Buffer->Info.bmiHeader.biWidth = Buffer->Width;
	Buffer->Info.bmiHeader.biHeight = -Buffer->Height;
	Buffer->Info.bmiHeader.biPlanes = 1;
	Buffer->Info.bmiHeader.biBitCount = 32;
	Buffer->Info.bmiHeader.biCompression = BI_RGB;
	
	//  NOTE: Help from Chris Hecker
	//  TODO: BytesPerPixel was removed from here
	int BitmapMemorySize = (Buffer->Width * Buffer->Height) * Buffer->BytesPerPixel;
	Buffer->Memory = VirtualAlloc(0, BitmapMemorySize, MEM_COMMIT, PAGE_READWRITE);
	
	//RenderWeirdGradient(0, 0);
	
	// TODO: Probably will clear screen to black
	Buffer->Pitch = Width * Buffer->BytesPerPixel;
}

internal void
Win32DisplayBufferInWindow(HDC DeviceContext,
						   int WindowWidth,
						   int WindowHeight,
						   win32_offscreen_buffer Buffer,
						   int X, int Y,
						   int Width, int Height)
{
	// TODO: Aspect Ratio Correction
	StretchDIBits(DeviceContext,
	              /*
	              X, Y, Width, Height,
				  X, Y, Width, Height,
				  */
				  0, 0, WindowWidth, WindowHeight,
				  0, 0, Buffer.Width, Buffer.Height,
				  Buffer.Memory, &Buffer.Info,
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
			
			win32_window_dimension Dimension = Win32GetWindowDimension(Window);
			Win32DisplayBufferInWindow(DeviceContext, Dimension.Width, Dimension.Height, GlobalBackbuffer, X, Y, Width, Height);
			
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
	
	Win32ResizeDIBSection(&GlobalBackbuffer, 1288, 728);
	
	WindowClass.style = CS_HREDRAW|CS_VREDRAW;
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
				
				RenderWeirdGradient(GlobalBackbuffer, XOffset, YOffset);
				
				HDC DeviceContext = GetDC(Window);
				
				win32_window_dimension Dimension = Win32GetWindowDimension(Window);
				Win32DisplayBufferInWindow(DeviceContext, Dimension.Width, Dimension.Height, GlobalBackbuffer, 0, 0, Dimension.Width, Dimension.Height);
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