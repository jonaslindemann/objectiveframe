# -*- coding: iso-8859-15 -*-

import of_solver as ofs
import of_vis as ofv

# --- Import model

import test as model

if __name__ == "__main__":
    
    solver = ofs.BeamBarSolver(model)
    solver.execute()

    vis = ofv.BeamVisualiser(solver)
    vis.draw()
    vis.show_and_wait()






