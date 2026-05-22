# About ObjectiveFrame

ObjectiveFrame is an interactive finite element analysis application for beam and frame structures. It was originally developed at Structural Mechanics at Lund University by Jonas Lindemann as part of PhD work on real-time explorable finite element analysis and direct feedback methods.

The project combines research lineage with a practical teaching goal: make structural mechanics easier to explore visually.

## Research Background

- [Objective Frame - An educational tool for understanding the behavior of structures](https://portal.research.lu.se/en/publications/objective-frame-an-educational-tool-for-understanding-the-behavio)
- [Designing a new user interface for ObjectiveFrame](https://www.byggmek.lth.se/fileadmin/byggnadsmekanik/publications/tvsm7000/web7137.pdf)
- [Techniques for distributed access and visualisation computational mechanics](https://www.lth.se/fileadmin/byggnadsmekanik/publications/tvsm1000/web1016.pdf)
- [CORBA in distributed finite element applications](https://portal.research.lu.se/en/publications/corba-in-distributed-finite-element-applications)
- [Using 3D gesture controls for interacting with mechanical models](https://portal.research.lu.se/en/publications/using-3d-gesture-controls-for-interacting-with-mechanical-models-2)

Daniel Akesson implemented 3D gesture controls using a Leap Motion controller for interacting with finite element models. Pierre Olsson developed routines and user interfaces for computing section properties.

## Implementation

ObjectiveFrame is implemented in C++ using OpenGL for hardware-accelerated rendering. It uses:

- [Dear ImGui](https://github.com/ocornut/imgui) for the immediate mode user interface.
- [GLFW](https://www.glfw.org/) for OpenGL windows and input.
- [Eigen](https://eigen.tuxfamily.org/) for matrix and solver routines.
- [TetGen](https://www.wias-berlin.de/software/index.jsp?id=TetGen&lang=1) for generating beam and truss structures from points.
- [Interactive Visualisation Framework - Ivf++](https://github.com/jonaslindemann/ivfplusplus) for scene graph and visualization infrastructure.

## Current Focus

- Interactive structural feedback.
- Educational finite element workflows.
- Eigenmode visualization.
- Scripted model generation through ChaiScript.
- CALFEM-oriented export workflows for structural mechanics teaching.
- AI-assisted modelling experiments.

## Future Development

- More standard and discoverable UI workflows.
- Improved CALFEM export and teaching examples.
- A richer scripting and plugin API.
- Better onboarding material, short videos, and example projects.
- Broader platform support.

Jonas Lindemann, 2026
