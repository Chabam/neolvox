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
    dset: h5py.Dataset = file["pad"]

    dims = dset.attrs["Dimensions"]

    dset_m = sparse.COO((dset["values"], (dset["x"], dset["y"], dset["z"])), shape=dims).todense()

    profile = numpy.average(dset_m, axis=(0, 1))
    figure, axis = plt.subplots()
    axis.plot(profile, range(dims[2]))
    axis.set(xlabel="pad", ylabel="z level", title=args.file_name + " pad values")
    axis.grid()

    plt.show()


if __name__ == "__main__":
    main()
