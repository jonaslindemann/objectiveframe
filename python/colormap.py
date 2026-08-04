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

# blue.map: gray (0.7,0.7,0.7) at zero force -> blue (0.3,0.3,1.0) at max compression.
# Stops short of pure blue (0,0,1): that's perceptually the darkest primary
# (luminance ~0.07), which made the most heavily loaded members the darkest
# thing on screen. Floor keeps luminance >= ~0.35 across the whole map.
blue = np.column_stack([0.7 - 0.4*t, 0.7 - 0.4*t, 0.7 + 0.3*t])

# red.map: gray (0.7,0.7,0.7) at zero force -> red (1.0,0.25,0.25) at max tension.
# Same reasoning as blue.map: pure red (1,0,0) is dimmer (luminance ~0.21)
# than it looks in isolation; stopping short keeps it clearly visible.
red  = np.column_stack([0.7 + 0.3*t, 0.7 - 0.45*t, 0.7 - 0.45*t])

def save_map(path, data):
    with open(path, 'w') as f:
        f.write(f"{len(data)}\n")
        for row in data:
            f.write(f"    {row[0]:.4f}    {row[1]:.4f}    {row[2]:.4f}\n")

save_map("blue.map", blue)
save_map("red.map",  red)

# colormap11.map: neutral gray at low scalar values, then muted teal/green/yellow/red.
# This map is used for magnitudes (T/V/M/Navier), where zero has no sign and
# should not look like a strong "blue result". The first 30 percent stays close
# to the ordinary beam color, then color ramps in as the normalized magnitude
# becomes structurally meaningful.
stops = [
    (0.00, np.array([0.70, 0.70, 0.70])),
    (0.30, np.array([0.66, 0.66, 0.66])),
    (0.52, np.array([0.30, 0.68, 0.72])),
    (0.72, np.array([0.42, 0.72, 0.38])),
    (0.88, np.array([0.96, 0.78, 0.24])),
    (1.00, np.array([1.00, 0.10, 0.10])),
]

scalar = []
for i in range(n):
    u = i / (n - 1)
    for j in range(len(stops) - 1):
        t0, c0 = stops[j]
        t1, c1 = stops[j + 1]
        if u <= t1:
            a = (u - t0) / (t1 - t0)
            scalar.append(c0 * (1.0 - a) + c1 * a)
            break
    else:
        scalar.append(stops[-1][1])

save_map("colormap11.map", scalar)
