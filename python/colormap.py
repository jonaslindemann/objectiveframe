import numpy as np

n = 128
t = np.linspace(0.0, 1.0, n)

# blue_black.map / red_black.map: for blending mode (starts from near-black)
r = np.linspace(0.1, 1.0, n)
g = np.linspace(0.1, 0.0, n)
b = np.linspace(0.1, 0.0, n)

blue_black = np.column_stack([b, g, r])   # (0.1,0.1,0.1) -> (0,0,1)
red_black  = np.column_stack([r, g, b])   # (0.1,0.1,0.1) -> (1,0,0)

np.savetxt("blue_black.map", blue_black, fmt="%0.4f", header="128", comments="")
np.savetxt("red_black.map",  red_black,  fmt="%0.4f", header="128", comments="")

# blue.map: gray (0.7,0.7,0.7) at zero force -> vivid blue (0,0,1) at max compression
blue = np.column_stack([0.7*(1-t), 0.7*(1-t), 0.7 + 0.3*t])

# red.map: gray (0.7,0.7,0.7) at zero force -> vivid red (1,0,0) at max tension
red  = np.column_stack([0.7 + 0.3*t, 0.7*(1-t), 0.7*(1-t)])

def save_map(path, data):
    with open(path, 'w') as f:
        f.write(f"{len(data)}\n")
        for row in data:
            f.write(f"    {row[0]:.4f}    {row[1]:.4f}    {row[2]:.4f}\n")

save_map("blue.map", blue)
save_map("red.map",  red)

# colormap11.map: vivid rainbow blue->cyan->green->yellow->red (HSV, S=V=1)
import colorsys
rainbow = [colorsys.hsv_to_rgb((2.0/3.0) * (1.0 - i/(n-1)), 1.0, 1.0) for i in range(n)]
save_map("colormap11.map", rainbow)
