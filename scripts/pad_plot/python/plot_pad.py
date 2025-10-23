import h5py
import matplotlib.pyplot as plt
import numpy
import argparse
import sparse


def main():

    argparser = argparse.ArgumentParser("plot_pad")
    argparser.add_argument("file_name")

    args = argparser.parse_args()


    file = h5py.File(args.file_name, "r")
    group: h5py.Group = file["lvox"]

    dims = group.attrs["Dimensions"]
    min_vals = group.attrs["Minimal coordinates values"]
    voxel_size = group.attrs["Voxel size"]

    dset_m = sparse.COO((group["pad"], (group["x"], group["y"], group["z"])), shape=dims).todense()
    dset_m = numpy.nan_to_num(dset_m)

    profile = numpy.average(dset_m, axis=(0, 1))
    figure, axis = plt.subplots()
    axis.plot(profile, [min_vals[2] + voxel_size * i for i in range(dims[2])])
    axis.set(xlabel="pad", ylabel="z level", title=args.file_name + " pad values")
    axis.grid()

    plt.show()


if __name__ == "__main__":
    main()
