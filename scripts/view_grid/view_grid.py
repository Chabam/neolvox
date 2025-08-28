import h5py
import napari
import sparse


file = h5py.File("out.h5")
group: h5py.Group = file["lvox computation values"]

dims = group.attrs["Dimensions"]
dset_m = sparse.COO((group["counts"], (group["x"], group["y"], group["z"])), shape=dims).todense()
viewer = napari.Viewer(ndisplay=3)
viewer.add_image(dset_m[:])
napari.run()
