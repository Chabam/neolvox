import h5py
import napari


file = h5py.File("cubes_lvox_out.h5", "r")
hits_dset = file["lengths"]
viewer = napari.Viewer(ndisplay=3)
viewer.add_image(hits_dset[:])
napari.run()
