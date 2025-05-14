import h5py
import matplotlib.pyplot as plt
import numpy
import argparse


def main():

    argparser = argparse.ArgumentParser("plot_pad")
    argparser.add_argument("file_name")

    args = argparser.parse_args()


    file = h5py.File(args.file_name, "r")
    pad_dset: h5py.Dataset = file["pad"]

    (_, _, dim_z) = pad_dset.shape

    profile = numpy.sum(pad_dset, axis=(0, 1))
    figure, axis = plt.subplots()
    axis.plot(profile, range(dim_z))
    axis.set(xlabel="pad", ylabel="z level", title="pad values")
    axis.grid()

    plt.show()


if __name__ == "__main__":
    main()
