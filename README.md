# Anthragon
Anthragon is a modern and cross platform C++ GUI library

## Building
The lib is automatically build with custom python scripts. We use the following technologies:
- Python
- Conan 
- Premake5
- VisualStudio / MSBuild (On Windows)
- Make and GCC/Clang (On Linux) 

Building happens in the following order:
1) Premake5 will be downloaded
2) Conan will be called and it will download (and compile when required) all external dependencies
3) Premake5 will be invoked and generates a solution / makefile based on OS and configuration
4) The final solution / makefile can now be executed
