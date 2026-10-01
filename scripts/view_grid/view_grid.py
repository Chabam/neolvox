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

viewer = napari.Viewer(ndisplay=3)

dsets = ["pad", "hits", "counts", "lengths", "potential lengths", "potential counts"]

for dset_name in dsets:
    if dset_name in group:
        dset = sparse.COO(coords, group[dset_name], shape=dims).todense()
        viewer.add_image(dset[:], name=dset_name)

viewer.scene.camera.orientation2d = ('up', 'right')
napari.run()
