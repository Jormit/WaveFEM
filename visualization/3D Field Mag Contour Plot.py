from mayavi import mlab
import numpy as np

import data_loader as dl

x, y, z, vec = dl.import_3d_field("../examples/outputs/Waveguide With Obstacle 3d.txt")
vec_real = np.real(vec)
vec_real_mag = np.linalg.norm(vec_real, axis=3)

mlab.contour3d(x, y, z, vec_real_mag, contours=10, transparent=True)
mlab.colorbar()
mlab.outline()
mlab.show()