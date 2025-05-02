Project name: Visualization of submolecular bioparticle design

Project dependencies
To successfully build and run the project, you will need the following dependencies:

OpenGL is a library for working with graphics. It is already built into most operating systems, 
but you need to make sure that you have header files and development tools installed.

On Linux: install the freeglut3-dev, libglew-dev packages.

GLM is a library for working with mathematics (vectors, matrices, etc.).

Install via a package manager (for example, sudo apt install libglm-dev on Linux)
or download from the official website.

ImGui is a library for creating an interface (buttons, sliders, etc.).

Download from the official repository and add to the project.

CMake is a system for building the project.

Install via package manager sudo apt install cmake on Linux

C++ compiler (e.g. g++ or clang).

Program logic
1. What does the program do?
The program visualizes DNA, RNA, and protein molecules in 3D space. 
You can rotate the camera, change the parameters of the molecules (e.g. DNA twisting or RNA waves), 
and observe the interactions between molecules 
(e.g. DNA and RNA hybridization or protein binding to DNA).

2. How does it work?
OpenGL is responsible for rendering 3D graphics.

GLM helps with the math: it calculates the positions of molecules, the distances between them, etc.

ImGui creates an interface where you can control the parameters (e.g. simulation speed or interaction modes).

3. Main components:
Molecules:

DNA: a helix of nucleotides (adenine, thymine, cytosine, guanine).

RNA: single-stranded molecule with wave-like structure.

Protein: three-dimensional structure consisting of layers.

Interactions:

DNA and RNA hybridization: DNA and RNA nucleotides bind if they are close to each other.

Protein binding: protein is "attracted" to DNA.

Transcription: RNA "copies" information from DNA.

4. How to control the program?
Interface:

There is a control panel in the upper right corner where you can turn on/off automatic rotation, 
change the simulation speed, select a molecule for the camera focus, etc.

There is an "Advanced Settings" section where you can adjust the parameters of the molecules
(for example, DNA twisting or RNA wave amplitude).

Camera:

LMB + mouse movement - camera rotation.

Automatic rotation can be turned on/off in the interface.

Interaction modes:

The interface allows you to choose how the molecules interact (for example, 
DNA and RNA hybridization or protein binding).

5. What does it look like?
DNA is a double helix, colored in different colors (each color corresponds to a specific nucleotide).

RNA is a wavy chain.

Protein is a three-dimensional structure similar to a layer cake.

When molecules interact, bonds (white lines) appear between them.

6. Why is it useful?
This program helps visualize complex biological processes that are difficult to imagine in your mind.
For example, how DNA and RNA interact during transcription or how proteins bind to DNA.
It is useful for teaching and research.

©made by Alim who did it for 6 hours. Hope you like it
