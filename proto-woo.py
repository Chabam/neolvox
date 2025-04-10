import math

import numpy as np


def normal(vec):
    return vec / np.linalg.norm(vec)


# Used for the step variable
def get_sign(val):
    if val < 0:
        return -1
    elif val > 0:
        return 1
    else:
        return 0


def compute_step(dir):
    return np.array(list(map(get_sign, dir)))


def compute_idx(pos, cell_size):
    return np.array([math.floor(val / cell_size) for val in pos])


# NOTE: Python handles correctly infinite, C++ does not
def compute_tmax(pos, dir, cell_size):
    step = compute_step(dir)

    cur_voxel = compute_idx(pos, cell_size)
    negative_correction = [cell_size if cor == -1 else 0 for cor in step]
    next_bound = cur_voxel + step * cell_size + negative_correction

    return (next_bound - pos) / dir


def compute_delta(dir, cell_size):
    step = compute_step(dir)
    return cell_size / dir * step


# assume a cubic grid
cell_size = 1
pos = np.array([0.5, 0.5])
dir = normal(np.array([1, 2]))
step = compute_step(dir)
tmax = compute_tmax(pos, dir, cell_size)
delta = compute_delta(dir, cell_size)

tmax_x = tmax[0]
tmax_y = tmax[1]

delta_x = delta[0]
delta_y = delta[1]

step_x = step[0]
step_y = step[1]

current_idx = compute_idx(pos, cell_size)
visited_voxels = [current_idx.copy()]

for _ in range(10):
    if tmax_x < tmax_y:
        tmax_x += delta_x
        current_idx[0] += step_x
    else:
        tmax_y += delta_y
        current_idx[1] += step_y
    visited_voxels.append(current_idx.copy())

for vox in visited_voxels:
    print(f"({vox[0]},{vox[1]})")
