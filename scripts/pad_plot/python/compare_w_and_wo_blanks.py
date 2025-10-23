import h5py
import matplotlib.pyplot as plt
import numpy
import argparse
import sparse


file_w = h5py.File("with_blanks.h5", "r")
file_wo = h5py.File("without_blanks.h5", "r")

def load_pad(file):
    dset = file["pad"]
    dims = dset.attrs["Dimensions"]
    return sparse.COO((dset["values"], (dset["x"], dset["y"], dset["z"])), shape=dims).todense()

pad_w = load_pad(file_w)
pad_wo = load_pad(file_wo)

diff = pad_wo - pad_w

profile = numpy.average(pad_wo, axis=(0, 1)) - numpy.average(pad_w, axis=(0, 1))
figure, axis = plt.subplots()
axis.plot(profile, range(pad_w.shape[2]))
axis.set(xlabel="pad", ylabel="z level", title="diff pad values")
axis.grid()

plt.show()


