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


# This is already done by the grid
def compute_idx(pos, cell_size, min_bounds):
    return np.array(
        list(map(lambda v, min: math.floor(v / cell_size) - min, pos, min_bounds))
    )


# NOTE: Python handles correctly infinite, C++ does not
def compute_tmax(pos, dir, cell_size, min_bounds):
    step = compute_step(dir)

    cur_voxel = compute_idx(pos, cell_size, min_bounds)
    negative_correction = [cell_size if cor == -1 else 0 for cor in step]
    next_bound = cur_voxel + step * cell_size + negative_correction

    return (next_bound - pos) / dir


def compute_delta(dir, cell_size):
    step = compute_step(dir)
    return cell_size / dir * step


# TODO: implement "0" dir
cell_size = 1
min_bounds = np.array([1, 0])
max_bounds = np.array([10, 5])
dim_x = max_bounds[0] - min_bounds[0]
dim_y = max_bounds[1] - min_bounds[1]
pos = np.array([1.5, 4.5])
dir = normal(np.array([-2, -0.1]))
step = compute_step(dir)
tmax = compute_tmax(pos, dir, cell_size, min_bounds)
delta = compute_delta(dir, cell_size)

tmax_x = tmax[0]
tmax_y = tmax[1]

delta_x = delta[0]
delta_y = delta[1]

step_x = step[0]
step_y = step[1]

current_idx = compute_idx(pos, cell_size, min_bounds)
visited_voxels = [current_idx.copy()]


def idx_to_coord(idx, cell_size):
    return 1 + idx * cell_size


while True:
    if tmax_x < tmax_y:
        tmax_x += delta_x
        current_idx[0] += step_x
    else:
        tmax_y += delta_y
        current_idx[1] += step_y

    if not (
        current_idx[0] >= 0
        and current_idx[1] >= 0
        and current_idx[0] <= dim_x
        and current_idx[1] <= dim_y
    ):
        break

    visited_voxels.append(current_idx.copy())

for vox in visited_voxels:
    print(f"({vox[0]},{vox[1]})")
