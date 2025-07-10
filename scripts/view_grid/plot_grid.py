import h5py
import napari
import sparse


file = h5py.File("MLS-PRF133_lvox_out.h5")
dset = file["lengths"]
dset_m = sparse.COO((dset["values"], (dset["x"], dset["y"], dset["z"])), shape=(409,401,80)).todense()
viewer = napari.Viewer(ndisplay=3)
viewer.add_image(dset_m[:])
napari.run()
