# Learning FEM with ObjectiveFrame

ObjectiveFrame is designed to help students build structural intuition while learning finite element modelling. The goal is not only to compute answers, but to see how supports, loads, stiffness, geometry, and modelling choices affect structural behavior.

Start students in the **Simple** interface from the start page. It hides element types, the load, support and material property dialogs, and the selection filters, which leaves the quick support and quick force tools as the way to support and load a structure. Switch to **Advanced** when the course reaches the concepts those dialogs express — a model keeps all of its data either way.

## Lesson Path

| Lesson | Concept | Suggested model |
| --- | --- | --- |
| 1. First beam model | Nodes, elements, supports, point loads | `bin/examples_misc/beam2.df3` |
| 2. Truss bridge | Axial force and load paths | `bin/examples/bar_bridge.df3` |
| 3. Beam bridge | Bending behavior and moments | `bin/examples/bridge_with_beams.df3` |
| 4. Boundary conditions | Stable and unstable structures | `bin/examples_misc/simple_structure.df3` |
| 5. Building frame | Lateral loads and frame action | `bin/examples/building_with_load.df3` |
| 6. Dome structure | Three-dimensional structural behavior | `bin/examples/dome_frame.df3` |
| 7. Eigenmodes | Instability and mode shapes | `bin/examples_misc/simple_structure.df3`, with a support removed |
| 8. Feedback mode | Real-time structural intuition | `bin/examples/bar_bridge.df3` |
| 9. Self-weight | Permanent load and load factors | `bin/examples/dome_frame.df3` |

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

### Permanent Load

Turn on **View / Self-weight...** and run the analysis again. Self-weight is the load a structure carries whether or not anyone applies one, and the three modes — material density, mass per length, and a total load spread by length — are three different levels of modelling assumption about the same thing. The load factor makes the difference between a characteristic and a design load case visible in one field.

## Classroom Exercises

- **Exercise 1:** Build a simply supported beam and compare deflection for three load positions.
- **Exercise 2:** Compare a truss bridge and a beam bridge with the same support span.
- **Exercise 3:** Make a frame unstable, inspect the eigenmode, then fix the missing constraint.
- **Exercise 4:** Use feedback mode to find the most sensitive load position in a bridge.
- **Exercise 5:** Export geometry for CALFEM and compare results in Python.
- **Exercise 6:** Build one bay of a truss, then repeat it with **Modify / Array** and compare the response for three, six and twelve bays.
- **Exercise 7:** Load a frame with self-weight only, then with a point load of the same total magnitude, and compare the moment distributions.

## For Educators

A few things make ObjectiveFrame easier to teach with:

- Start the class in one of the **Simple** interfaces, and reveal the property dialogs when the course needs them. `objframe.exe --ui-mode=simple-bar` makes a shortcut that always comes up as a truss interface showing normal force only; `--ui-mode=simple-beam` is the same interface over beams, with the full set of sectional results.
- The bundled examples appear as thumbnails on the start page, so an exercise can start from "open the third one" rather than from a file path.
- The **Create** menu's structure generators build parametric trusses, frames, domes and towers, which is the quickest way to produce a variant of an exercise per student.
- Models, and the scripts that generate them, are plain files that can be handed out and collected. See [ChaiScript scripting](chaiscript.md).
