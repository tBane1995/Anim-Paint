# Anim Paint v1.10

## Contents
- [Description](#description)
- [Screenshots](#screenshots)
- [Installation](#installation)
- [Technology](#technology)
- [License](#license)

## Description
**Anim Paint** is a simple pixel art painting tool inspired by MS Paint, designed for creating and editing pixel art with support for layers, frames, and animations.

## Screenshots
![Screenshot](screenshots/01.png)
![Screenshot](screenshots/02.png)
![Screenshot](screenshots/03.png)
![Screenshot](screenshots/04.png)
![Screenshot](screenshots/05.png)
![Screenshot](screenshots/06.png)
![Screenshot](screenshots/07.png)
![Screenshot](screenshots/08.png)
![Screenshot](screenshots/09.png)
![Screenshot](screenshots/10.png)
![Screenshot](screenshots/11.png)
![Screenshot](screenshots/12.png)
![Screenshot](screenshots/13.png)

## Installation
1. Download and install Visual Studio 2022
2. Download and install CMake
3. Download Library SFML-3.0.2 (https://www.sfml-dev.org/) i umieść ją w katalogu `C:\SFML-3.0.2`.
4. Open The **Command Prompt (cmd)**.
5. Go to project directory:
`
cd ..\..\Anim Paint
`
6. Create directory **build**:
`
mkdir build
`
7. Go to directory **build**:
`
cd build
`
8. Create project using **CMake**:
`
cmake ..
`
9. Build project:
`
cmake --build .
`

11. Set the Working Directory 
`Project -> Properties -> Debugging -> Working Directory -> "$(ProjectDir)/../Anim-Paint"`

12. Exe is in **build/Debug** lub **build/Release**

## Techonology
Program created in C++ with using SFML 3.0.2.  
  
## License
Open License – Attribution

This program may be:
- Downloaded
- Copied
- Modified
- Used in personal and commercial projects

Provided that:
- Attribution to the original author is retained
- The source (link to the repository) is provided
- In the event of modification, it must be clearly stated that the program was modified and by whom.