import h5py
import napari
import sparse
import numpy


file = h5py.File("out.h5")
group: h5py.Group = file["lvox"]

dims = group.attrs["Dimensions"]
minIdx = group.attrs["Minimal index values"]

x = group["x"][()]
y = group["y"][()]
z = group["z"][()]

xs = x - minIdx[0]
ys = y - minIdx[1]
zs = z - minIdx[2]

coords = numpy.vstack([xs, ys, zs])

dset_hits = sparse.COO(coords, group["hits"], shape=dims).todense()
dset_counts = sparse.COO(coords, group["counts"], shape=dims).todense()
dset_lengths = sparse.COO(coords, group["lengths"], shape=dims).todense()
dset_pad = sparse.COO(coords, group["pad"], shape=dims).todense()
viewer = napari.Viewer(ndisplay=3)
viewer.add_image(dset_pad[:], name="pad")
viewer.add_image(dset_hits[:], name="hits")
viewer.add_image(dset_counts[:], name="counts")
viewer.add_image(dset_lengths[:], name="lengths")
viewer.camera.orientation2d = ('up', 'right')
napari.run()
