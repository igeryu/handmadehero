# handmadehero
Me following handmadehero.org

Steps:

Day 1.
- Create a 'shell' batch file to get the command-prompt configured correctly.
  ~ Runs "vcvarsall.bat x64" in the VS12 folder.
  ~ Adds both the game working-directory and "../Visual Studio 12.0/Common7/IDE" to the front of PATH.
  ~ Sets the working-directory to the game directory.
- Set up the command-prompt for working in your directory by using the " /k [shell.bat file path]" arguments.
- Create a 'build' batch file to compile the .cpp files using "cl -Zi [cpp file path]".
- Create a cpp file with the 'windows.h' header file and a call to WinMain().

Day 2.
- Replace the WinMain() function call from Day 1 with the WndClass() struct.
- Added a WindowProc function named MainWindowCallback() to handle messages from Windows.
  - Set the window to paint itself white
- Configured the WNDCLASS WindowClass.
- Created a spinning for-loop to catch messages and route them to MainWindowCallback()