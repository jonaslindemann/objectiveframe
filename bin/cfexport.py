# -*- coding: iso-8859-15 -*-
'''
ObjectiveFrame generated CALFEM Python code.
'''

import numpy as np
import calfem.core as cfc


# ---- Materials

ep = np.array([
	[2.1e+09, 8.1e+07, 1, 1, 1, 1],
])

# ---- Elements

beam_topo = np.array([
])


# ----- Nodes

node_coords = np.array([
	[3, 0, -5],
	[-0.2, 0, -7.4],
	[-0.2, 0, -1.6],
	[1.4, 0, 2.6],
	[3, 1, -5],
	[-0.2, 1, -7.4],
	[-0.2, 1, -1.6],
	[1.4, 1, 2.6],
])

# ----- Local Z-axis

beam_orientation = np.array([
])

# ----- Boundary conditions

node_bc = np.array([
	[1, 1, 1, 1, 1, 1],
	[1, 1, 1, 0, 0, 0],
])

node_bc_val = np.array([
	[0, 0, 0, 0, 0, 0],
	[0, 0, 0, 0, 0, 0],
])

node_bc_idx = np.array([
])

node_load = np.array([
])

node_load_idx = np.array([
])

