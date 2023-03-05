---
hide:
  - navigation
---
# About ObjectiveFrame

The application was orginially developed at Structural Mechanics by Jonas Lindemann for his PhD thesis work. See:

 * [Objective Frame - An educational tool for understanding the behavior of structures](https://portal.research.lu.se/en/publications/objective-frame-an-educational-tool-for-understanding-the-behavio)
 * [Designing a new user interface for ObjectiveFrame](
https://www.byggmek.lth.se/fileadmin/byggnadsmekanik/publications/tvsm7000/web7137.pdf)
 * [Techniques for distributed access and visualisation computational mechanics](https://www.lth.se/fileadmin/byggnadsmekanik/publications/tvsm1000/web1016.pdf)

ObjectiveFrame has also been used to develop distributed techniques for finite element applications using the CORBA framework.

* [CORBA in distributed Finite element applications](https://portal.research.lu.se/en/publications/corba-in-distributed-finite-element-applications)

The application has also been used as an experimental application for user interface techniques within the field of finite element analysis. Daniel Åkesson implemented 3d gesture controls using a leap-controller for interacting with finite element models.

* [Using 3D gesture controls for interacting with mechanical models](https://portal.research.lu.se/en/publications/using-3d-gesture-controls-for-interacting-with-mechanical-models-2)

Pierre Olsson developed routines and user interfaces for computing section properties.

## Implementation details.

The application is implemented as a C++ application using OpenGL for hardware accelerated rendering. ObjectiveFrame uses a scene graph model implemented using the C++ library [Interactive Visualisation Framework - Ivf++](https://github.com/jonaslindemann/ivfplusplus). The solver is implemented using the matrix-library [Eigen](https://eigen.tuxfamily.org/). 

This version of the application has been given a large overhaul both with regards to C++ and the user interface. To increase the interactive aspect of the application the user interface has been reimplemented using the [ImGui - library](https://github.com/ocornut/imgui). This is an immediate mode user interface library that provides a state-less user interface rendered directly in the OpenGL window. The underlying framework for rendering OpenGL windows is the [GLFW library](https://www.glfw.org/)

In the newest release ObjectiveFrame can also take advantage of the [TetGen](https://www.wias-berlin.de/software/index.jsp?id=TetGen&lang=1) mesh generator for quickly creating beam/truss-structures from points. 

## Future development

 * User interface improvements. There is a still a lot of non-standard ways of interacting with the application, but these are also some things that will be worked out in upcoming versions.
 * Interactions with the CALFEM for Python library. I would like to see the application as a pre-processor for generating CALFEM code that can be used by students in Structural Mechanics.
 * More developed scripting interface for creating more interesting plugins for generating beam/truss-structures.

Jonas Lindemann

