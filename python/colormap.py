import numpy as np

r = np.linspace(0.1, 1.0, 128)
g = np.linspace(0.1, 0.0, 128)
b = np.linspace(0.1, 0.0, 128)

blue_colormap = np.zeros((128, 3), dtype=float)

blue_colormap[:,0] = b
blue_colormap[:,1] = g
blue_colormap[:,2] = r

red_colormap = np.zeros((128, 3), dtype=float)

red_colormap[:,0] = r
red_colormap[:,1] = g
red_colormap[:,2] = b

np.savetxt("blue_black.map", blue_colormap, fmt="%0.4f", header="128", comments="")
np.savetxt("red_black.map", red_colormap, fmt="%0.4f", header="128", comments="")

