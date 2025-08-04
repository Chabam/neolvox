import h5py
import napari
import sparse


file = h5py.File("test.h5")
dset = file["pad"]
dims = dset.attrs["Dimensions"]
dset_m = sparse.COO((dset["values"], (dset["x"], dset["y"], dset["z"])), shape=dims).todense()
viewer = napari.Viewer(ndisplay=3)
viewer.add_image(dset_m[:])
napari.run()
