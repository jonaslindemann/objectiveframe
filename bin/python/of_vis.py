# -*- coding: iso-8859-15 -*-

"""
This module provides a class for visualizing beams and bars using the Vedo library.
"""

import of_vis_beams as vb

class BeamVisualiser:
    """
    A class for visualizing beams and bars.

    Attributes:
    -----------
    solver : Solver
        An instance of the Solver class.

    Methods:
    --------
    draw_geometry():
        Draws the geometry of the beams and bars.

    draw_displacements():
        Draws the displacements of the beams and bars.

    draw():
        Draws both the geometry and displacements of the beams and bars.

    show_and_wait():
        Shows the visualization and waits for user input.
    """

    def __init__(self, solver):
        """
        Initializes a new instance of the BeamVisualizer class.

        Args:
            solver: The solver object to use for beam analysis.
        """
        self.solver = solver

    def draw_geometry(self):
        """
        Draws the geometry of the beam and bar elements in the solver object using the Vedo library.

        If the solver object contains any bar elements, they will be drawn in yellow color.
        If the solver object contains any beam elements, they will also be drawn in yellow color.

        Returns:
            None
        """
        if self.solver.n_bars > 0:
            vb.set_color_and_alpha("yellow", 1.0)
            vb.draw_bars(self.solver.edof_bars, self.solver.ex_br,
                          self.solver.ey_br, self.solver.ez_br)

        if self.solver.n_beams > 0:
            vb.set_color_and_alpha("yellow", 1.0)
            vb.draw_beams(self.solver.edof_beams, self.solver.ex_bm,
                           self.solver.ey_bm, self.solver.ez_bm)

    def draw_displacements(self):
        """
        Draws the displacements of bars and beams in the solver using the Vedo library.

        If there are bars in the solver, draws the bar displacements using the solver's a, coords_bars, edof_bars, dofs_bars, and magnfac attributes.
        If there are beams in the solver, draws the beam displacements using the solver's a, coords_beams, edof_beams, dofs_beams, and magnfac attributes.
        """

        if self.solver.n_bars > 0:
            vb.set_color_and_alpha("gray", 0.7)
            vb.draw_bar_displacements(self.solver.a, self.solver.coords_bars,
                                       self.solver.edof_bars, self.solver.dofs_bars, self.solver.magnfac)

        if self.solver.n_beams > 0:
            vb.set_color_and_alpha("gray", 0.7)
            vb.draw_beam_displacements(self.solver.a, self.solver.coords_beams,
                                        self.solver.edof_beams, self.solver.dofs_beams, self.solver.magnfac)

    def draw(self):
        """
        Draws the beam geometry and displacements using Vedo.

        Returns
        -------
        None
        """
        self.draw_geometry()
        self.draw_displacements()

    def show_and_wait(self):
        """
        Displays the beam visualization and waits for user input before closing the window.
        """
        vb.show_and_wait()
