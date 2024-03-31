from mayavi import mlab
import numpy as np

import data_loader as dl

x, y, z, vec = dl.import_3d_field("./examples/outputs/Waveguide 3d.txt")
vec_real = np.real(vec)

mlab.quiver3d(x, y, z, vec_real[:,:,:,0], vec_real[:,:,:,1], vec_real[:,:,:,2])
mlab.colorbar()
mlab.outline()
mlab.show()