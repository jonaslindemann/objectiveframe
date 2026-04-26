# Eigenmode solver

ObjectiveFrame can compute and visualize eigenmodes for a beam model. This is especially useful when a structure is unstable, under-constrained, or close to singular, because the displayed mode shape often reveals how the model can move.

## When eigenmodes appear

Eigenmode analysis can be triggered automatically when a normal calculation fails because the structure is unstable or the solver cannot solve the system. In that case ObjectiveFrame computes a small set of modes, opens the eigenmode window, and shows the first mode shape.

You can also compute modes manually from the eigenmode window.

## Opening the eigenmode window

Use the **Eigenmode Analysis** window to compute and inspect modes. The window contains:

- **Number of modes**: choose how many modes to compute, from 1 to 20.
- **Compute Eigenmodes**: run the eigenmode solver for the current model.
- **Clear**: remove the current eigenmode visualization.
- **Mode**: switch between computed modes.
- **Animate**: oscillate the current mode shape.
- **Show animation in secondary view**: keep the editable model in the main view and show the animated eigenmode separately.
- **Speed**: control animation speed.
- **Scale Factor**: increase or decrease the displayed deformation.
- **Mode stability**: list computed modes and show whether a mode is unstable.

## Interpreting results

The eigenmode shape is a diagnostic visualization. It shows a deformation pattern associated with the model stiffness matrix.

- Negative eigenvalues are shown as **UNSTABLE** and indicate an instability mode.
- Small or near-zero eigenvalues can indicate a mechanism, missing support, or poor constraint.
- Positive eigenvalues are displayed as approximate frequencies in Hz in the eigenmode window.

!!! note

    Eigenmode shapes are scaled for display. The drawn deformation is not a physical displacement result from a load case.

## Typical workflow

1. Build or open a model.
2. Run the normal calculation with **Calc/Execute** or `[Ctrl+R]`.
3. If the calculation reports instability, inspect the eigenmode that appears.
4. Use the mode list and animation to identify the unconstrained motion.
5. Add or adjust boundary conditions, element connectivity, or material/section properties.
6. Clear the eigenmode visualization and run the normal calculation again.

For example, if a tower sways sideways as a rigid body in the first unstable mode, the base constraints probably do not restrain the required degrees of freedom. If one bay twists independently, check whether beams are connected to the intended nodes.

## Technical notes

The solver extracts the free stiffness matrix after constrained degrees of freedom have been removed and solves it using Eigen's symmetric eigenvalue solver. Eigenvectors are mapped back to nodal degrees of freedom so ObjectiveFrame can display the corresponding mode shape.

The animation uses a sine phase and the selected scale factor. It is meant to make mechanisms easier to see, not to simulate dynamic time response.
