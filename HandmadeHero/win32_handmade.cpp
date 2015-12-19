/*
 * Add Header...
*/

#include <Windows.h>

// Functions that are only accessible from this file:
#define internal static

// Value persists throughout runtime, but only accessible in that scope:
#define local_persist static

// Basic global variable
#define global_variable static

//  TODO: Global variable...for now...
global_variable bool Running;

global_variable BITMAPINFO BitmapInfo;
global_variable void *BitmapMemory;
global_variable HBITMAP BitmapHandle;
global_variable HDC BitmapDeviceContext;

internal void
Win32ResizeDIBSection(int Width, int Height)
{
	
	//  TODO: Maybe free first, free after, then fre first if that fails
	
	//  Check to see if there is already a BitmapHandle
	//  If there is, delete it.
	if(BitmapHandle)
	{
		DeleteObject((HGDIOBJ)BitmapHandle);
	}
	
	if (!BitmapDeviceContext)
	{
		//  TODO:  Should this be recreated?
		BitmapDeviceContext = CreateCompatibleDC(0);
	}
	
	BitmapInfo.bmiHeader.biSize = sizeof(BitmapInfo.bmiHeader);
	BitmapInfo.bmiHeader.biWidth = Width;
	BitmapInfo.bmiHeader.biHeight = Height;
	BitmapInfo.bmiHeader.biPlanes = 1;
	BitmapInfo.bmiHeader.biBitCount = 32;
	BitmapInfo.bmiHeader.biCompression = BI_RGB;
	
	//  Ask Windows for device context that's compatible with the
	//  argument:
	BitmapHandle = CreateDIBSection(
		BitmapDeviceContext, &BitmapInfo,
		DIB_RGB_COLORS, &BitmapMemory,
		0, 0);
		
}

internal void
Win32UpdateWindow(HDC DeviceContext, int X, int Y, int Width, int Height)
{
	StretchDIBits(DeviceContext,
	              X, Y, Width, Height,
				  X, Y, Width, Height,
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
			Win32UpdateWindow(DeviceContext, X, Y, Width, Height);
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
		HWND WindowHandle = 
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
		if (WindowHandle)
		{
			Running = true;
			while(Running)
			{
				MSG Message;
				//  Check for window message, store it in Message
				//  and set results to MessageResult:
				BOOL MessageResult = GetMessageA(&Message, 0, 0, 0);
				if(MessageResult > 0)
				{
					TranslateMessage(&Message);
					DispatchMessageA(&Message);
				}
				else
				{
					break;  // exit the for-loop
				}
				
			}
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