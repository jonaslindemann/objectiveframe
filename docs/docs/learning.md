# Learning FEM with ObjectiveFrame

ObjectiveFrame is designed to help students build structural intuition while learning finite element modelling. The goal is not only to compute answers, but to see how supports, loads, stiffness, geometry, and modelling choices affect structural behavior.

!!! note "Student material placeholder"

    Add downloadable worksheets, lecture slides, and short narrated clips here. Suggested formats: PDF handouts, `.df3` exercise files, and 2-minute videos.

## Lesson Path

| Lesson | Concept | Suggested model |
| --- | --- | --- |
| 1. First beam model | Nodes, elements, supports, point loads | `bin/examples_misc/beam2.df3` |
| 2. Truss bridge | Axial force and load paths | `bin/examples/bar_bridge.df3` |
| 3. Beam bridge | Bending behavior and moments | `bin/examples/bridge_with_beams.df3` |
| 4. Boundary conditions | Stable and unstable structures | `bin/examples_misc/simple_structure.df3` |
| 5. Building frame | Lateral loads and frame action | `bin/examples/building_with_load.df3` |
| 6. Dome structure | Three-dimensional structural behavior | `bin/examples/dome_frame.df3` |
| 7. Eigenmodes | Instability and mode shapes | Placeholder model |
| 8. Feedback mode | Real-time structural intuition | Placeholder guided exercise |

## Concepts to Explore

### Beam and Frame Deformation

Open a simple beam or bridge example, run the analysis, and switch between deformation, normal force, and moment visualization. Ask how the response changes when stiffness, span length, or support placement changes.

### Boundary Conditions

Remove or modify supports and run the analysis again. Use eigenmode visualization to understand how an under-constrained structure can move.

### Load Paths

Move a load through a truss or frame and observe which members carry force. Feedback mode is especially useful here because the structural response updates interactively.

### Section Properties

Change section dimensions or section type and compare deformation and internal forces. This helps connect cross-section geometry with stiffness.

### Modal Behavior

Use the eigenmode solver to visualize mode shapes. This is useful for explaining instability, mechanisms, and why constraints matter.

## Classroom Exercise Placeholders

- **Exercise 1:** Build a simply supported beam and compare deflection for three load positions.
- **Exercise 2:** Compare a truss bridge and a beam bridge with the same support span.
- **Exercise 3:** Make a frame unstable, inspect the eigenmode, then fix the missing constraint.
- **Exercise 4:** Use feedback mode to find the most sensitive load position in a bridge.
- **Exercise 5:** Export geometry for CALFEM and compare results in Python.

## For Educators

Suggested additions for a complete course package:

- Starter `.df3` files.
- Completed solution `.df3` files.
- Short result explanations.
- Discussion questions.
- Slides with screenshots.
- A 2-minute intro video for each lesson.
