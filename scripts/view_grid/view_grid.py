import h5py
import napari
import sparse


file = h5py.File("out.h5")
group: h5py.Group = file["lvox computation values"]

dims = group.attrs["Dimensions"]
dset_hits = sparse.COO((group["hits"], (group["x"], group["y"], group["z"])), shape=dims).todense()
dset_counts = sparse.COO((group["counts"], (group["x"], group["y"], group["z"])), shape=dims).todense()
dset_lengths = sparse.COO((group["lengths"], (group["x"], group["y"], group["z"])), shape=dims).todense()
dset_pad = sparse.COO((group["pad"], (group["x"], group["y"], group["z"])), shape=dims).todense()
viewer = napari.Viewer(ndisplay=3)
viewer.add_image(dset_hits[:])
viewer.add_image(dset_counts[:])
viewer.add_image(dset_lengths[:])
viewer.add_image(dset_pad[:])
napari.run()
