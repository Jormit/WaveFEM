import numpy as np
from mayavi import mlab

import data_loader as dl

x,y,z,vec = dl.import_3d_field("Data/Waveguide 3d.txt")
vec_real = np.real(vec)
vec_real_mag = np.linalg.norm(vec_real, axis=3)

mlab.volume_slice(x, y, z, vec_real_mag, plane_orientation='x_axes')
mlab.volume_slice(x, y, z, vec_real_mag, plane_orientation='y_axes')
mlab.volume_slice(x, y, z, vec_real_mag, plane_orientation='z_axes')
mlab.colorbar()
mlab.outline()
mlab.show()



            