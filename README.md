# ObjectiveFrame - README

ObjectiveFrame is a finite element beam analysis application developed at Structural Mechanics at Lund university by Jonas Lindemann in his PhD work. The application focused on how to make finite element analysis real-time explorable with direct feedback methods.

 * Objective Frame - An educational tool for understanding the behavior of structures - https://portal.research.lu.se/en/publications/objective-frame-an-educational-tool-for-understanding-the-behavio
 * Techniques for distributed access and visualisation computational mechanics - 
https://www.lth.se/fileadmin/byggnadsmekanik/publications/tvsm1000/web1016.pdf

ObjectiveFrame has also been used to develop distributed techniques for finite element applications using the CORBA framework.

* CORBA in distributed Finite element applications - https://portal.research.lu.se/en/publications/corba-in-distributed-finite-element-applications

The application has also been used as an experimental application for user interface techniques within the field of finite element analysis. Daniel Åkesson implemented 3d gesture controls using a leap-controller for interacting with finite element models.

* Using 3D gesture controls for interacting with mechanical models - https://portal.research.lu.se/en/publications/using-3d-gesture-controls-for-interacting-with-mechanical-models-2

* Using 3D gesture controls for interacting with mechanical models - https://portal.research.lu.se/en/publications/using-3d-gesture-controls-for-interacting-with-mechanical-models

Pierre Olsson developed routines and user interfaces for computing section properties.

## Implementation details.

The application is implemented as a C++ application using OpenGL for hardware accelerated rendering. ObjectiveFrame uses a scene graph model implemented using the C++ library Interactive Visualisation Framework - Ivf++ (https://github.com/jonaslindemann/ivfplusplus). The solver is implemented using the matrix-library Newmat11. 

This version of the application has been given a large overhaul both with regards to C++ and the user interface. To increase the interactive aspect of the application the user interface has been reimplemented using the ImGui - library (https://github.com/ocornut/imgui). This is an immediate mode user interface library that provides a state-less user interface rendered directly in the OpenGL window. 

## Future development

 * The ImGui implementation still builds on the FLTK library as it has been a large effort to unbind the close ties to this library. The next iterations will base the implementation directly on the GLFW library. (COMPLETED)
 * User interface improvements. There is a still a lot of non-standard ways of interacting with the application, but these are also some things that will be worked out in upcoming versions. (ONGOING)
 * Interactions with the CALFEM for Python library. I would like to see the application as a pre-processor for generating CALFEM code that can be used by students in Structural Mechanics. (ONGOING)
 * An improved solver - Currently the newmat-based solver works OK, but lacks scalability as it is not very actively maintained. The eigen-library would be a candidate to replace newmat. (COMPLETED)
 * More ways of extending the application with additional solvers, geometry generation and more. 
 * A richer scripting API.

 ## References

 The latest version of ObjectiveFrame uses TetGen (included as an executable - tetgen.exe) for meshing beams. See:
 
 * ang Si. 2015. "TetGen, a Delaunay-Based Quality Tetrahedral Mesh Generator". ACM Trans. on Mathematical Software. 41 (2), Article 11 (February 2015), 36 pages. DOI=10.1145/2629697 http://doi.acm.org/10.1145/2629697

 Solver is implemented using the Eigen matrix library:

 * https://eigen.tuxfamily.org/

Jonas Lindemann

