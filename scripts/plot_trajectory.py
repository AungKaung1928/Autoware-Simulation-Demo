#!/usr/bin/env python3
"""plot_trajectory.py <trace.csv> <out.svg>: draw the odometry trace written by
test/test_launch_pipeline.py (TRAJ_CSV) as an SVG: path, start, goal and tolerance circle.
Standard library only."""
import sys

src, out = sys.argv[1], sys.argv[2]
lines = open(src).read().splitlines()
_, gx, gy, _, tol = lines[0].lstrip('# ').split()
gx, gy, tol = float(gx), float(gy), float(tol)
pts = [tuple(map(float, l.split(','))) for l in lines[2:] if l]
xs, ys = [p[1] for p in pts] + [gx], [p[2] for p in pts] + [gy]
pad = 1.0
x0, x1, y0, y1 = min(xs) - pad, max(xs) + pad, min(ys) - pad, max(ys) + pad
W = 640
s = W / (x1 - x0)
H = int((y1 - y0) * s) + 70


def X(x):
    return (x - x0) * s


def Y(y):
    return (y1 - y) * s + 10


grid = []
for gxv in [v for v in range(int(x0) + (x0 > 0), int(x1) + 1) if 12 < X(v) < W - 12]:
    grid.append(f'<line x1="{X(gxv):.1f}" y1="{Y(y1):.1f}" x2="{X(gxv):.1f}" y2="{Y(y0):.1f}" stroke="#e6eaed"/>'
                f'<text x="{X(gxv):.1f}" y="{Y(y0) + 16:.1f}" text-anchor="middle" fill="#6b7785">{gxv}</text>')
for gyv in [v for v in range(int(y0) + (y0 > 0), int(y1) + 1) if Y(v) < Y(y0) - 12]:
    grid.append(f'<line x1="{X(x0):.1f}" y1="{Y(gyv):.1f}" x2="{X(x1):.1f}" y2="{Y(gyv):.1f}" stroke="#e6eaed"/>'
                f'<text x="{X(x0) + 4:.1f}" y="{Y(gyv) + 14:.1f}" fill="#6b7785">{gyv}</text>')
path = ' '.join(f'{X(x):.1f},{Y(y):.1f}' for _, x, y in pts)
t_end, xe, ye = pts[-1]
d_end = ((xe - gx) ** 2 + (ye - gy) ** 2) ** 0.5
svg = f'''<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 {W} {H}" font-family="Helvetica, Arial, sans-serif" font-size="13">
<rect width="{W}" height="{H}" fill="#ffffff"/>
{''.join(grid)}
<circle cx="{X(gx):.1f}" cy="{Y(gy):.1f}" r="{tol * s:.1f}" fill="#e8f3f5" stroke="#005f73" stroke-dasharray="4 3"/>
<polyline points="{path}" fill="none" stroke="#10161e" stroke-width="2.5"/>
<circle cx="{X(pts[0][1]):.1f}" cy="{Y(pts[0][2]):.1f}" r="5" fill="#10161e"/>
<text x="{X(pts[0][1]) + 8:.1f}" y="{Y(pts[0][2]) + 18:.1f}" fill="#3f4d58">start ({pts[0][1]:.2f}, {pts[0][2]:.2f})</text>
<circle cx="{X(gx):.1f}" cy="{Y(gy):.1f}" r="4" fill="#005f73"/>
<text x="{X(gx):.1f}" y="{Y(gy) - tol * s - 8:.1f}" text-anchor="middle" fill="#005f73">goal ({gx:.1f}, {gy:.1f}), tolerance {tol} m</text>
<text x="10" y="{H - 14}" fill="#3f4d58">/odom, CI pipeline test: {len(pts)} samples, {t_end:.1f} s, final distance {d_end:.2f} m (metres, odom frame)</text>
</svg>
'''
open(out, 'w').write(svg)
print(f'{out}: {len(pts)} points, final distance {d_end:.2f} m')
