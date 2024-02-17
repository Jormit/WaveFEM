from mayavi import mlab
import numpy as np

import data_loader as dl

x,y,z,vec = dl.import_3d_field("Data/Ridged Waveguide 3d.txt", (30, 30, 30))
vec_real = np.real(vec)
vec_real_mag = np.linalg.norm(vec_real, axis=3)

print(z.shape)
mlab.contour3d(x, y, z, vec_real_mag, contours=10, transparent=True)
mlab.colorbar()
mlab.outline()
mlab.show()



            