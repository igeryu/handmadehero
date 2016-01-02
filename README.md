# handmadehero
Me following handmadehero.org

Steps:

Day 1.
- Create a ``shell`` batch file to get the command-prompt configured correctly.
  - Runs "``vcvarsall.bat x64``" in the VS12 folder.
  - Adds both the game working-directory and "``../Visual Studio 12.0/Common7/IDE``" to the front of ``PATH``.
  - Sets the working-directory to the game directory.
- Set up the command-prompt for working in your directory by using the "`` /k [shell.bat file path]``" arguments.
- Create a ``build`` batch file to compile the ``.cpp`` files using "``cl -Zi [cpp file path]``".
- Create a ``.cpp`` file with the ``windows.h`` header file and a call to ``WinMain()``.

Day 2.
- Replace the ``WinMain()`` function call from Day 1 with the ``WndClass()`` struct.
- Added a ``WindowProc()`` function named ``Win32MainWindowCallback()`` to handle messages from Windows.
  - Set the window to paint itself white
- Configured the ``WNDCLASS`` ``WindowClass``.
- Created a spinning for-loop to catch messages and route them to ``Win32MainWindowCallback()``

Day 3.
- Added boolean ``Running`` variable
  - Changed the ``WM_CLOSE`` and ``WM_DESTROY`` cases in ``Win32MainWindowCallback()`` to change ``Running`` to ``false``.
  - ``Running`` is set to ``true`` right after the Window handle is tested
 - ``define``'d ``static`` to ``local_persist``, ``global_variable`` and ``internal`` for clarity.
  - Changed ``Operation`` in ``Win32MainWindowCallback()`` to ``local_persist``
 - Created ``Win32ResizeDIBSection()`` to be called anytime the window is resized.
  - This check if there is already a ``BitmapHandle`` and deletes it if there is.
  - It then checks if the ``BitmapDeviceContext`` is ``NULL`` and creates a new one if it is.
  - Then ``Win32ResizeDIBSection()`` configures ``BitmapInfo`` based on the function parameters.
  - Then it creates a new ``BitmapHandle``.
  
Day 4.
- Refactoring:
 - Removed ``BitmapHandle`` and ``BitmapDeviceContext``.
 - Removed code to check if ``BitmapHandle`` exists and if ``BitmapDeviceContext`` doesn't exist.
 - Removed call to ``CreateDIBSection()`` for a new ``BitmapHandle``.
- Changed ``BitmapInfo.bmiHeader.biWidth`` to a negative value, to put the screen coordinate origin at the top-left
- Created a temporary ``RenderWeirdGradient()`` function to map the the last 8 bits of the pixel X and Y coordinates to their blue and green values, respectively.
- Moved the call of ``RenderWeirdGradient()`` to the ``while(Running)`` loop inside ``WinMain()`` and changed the ``XOffset`` incrementally to achieve animation.

Day 5.
- Moved the global variables (``BitmapInfo``, ``BitmapMemory``, ``BitmapWidth``, ``BitmapHeight``, and ``BytesPerPixel``) to a ``win32_offscreen_buffer`` ``struct``.
- Created a ``win32_window_dimension`` struct to hold ``Width`` and ``Height`` integers, and allow the new ``Win32GetWindowDimension()`` function to return both variables together.
- Moved the code that was in the ``WM_SIZE`` case of ``Win32MainWindowCallback()`` to ``WinMain()``.
 - This made the window resize the animated pattern with it, rather than the pattern size being independent of the window size.
