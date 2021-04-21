# Ray-Tracer

- The main function is in "EntryPoint.cpp".
- To change render mode (DrawPoints / GenerateImage), change the enum in the main function.
- The application has an interface with all the options explained in the console. 
Note that for the input to work, your active window should be the application window, and not the
console window, because glut captures the input.

Notes:
- The draw version will draw the image 10% vertically over time to quickly see results.
- The code has a new structure, but all the relevant functions should be accessible
from the "SceneRenderer.cpp" file.