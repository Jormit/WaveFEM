import pyvista as pv
import numpy as np

import data_loader as dl

x, y, z, vec = dl.import_3d_field("./examples/outputs/Horn 3d.txt")
vec_real = np.real(vec)
vec_real_mag = np.linalg.norm(vec_real, axis=3)
mesh = pv.StructuredGrid(x, y, z)
mesh.point_data['values'] = vec_real_mag.ravel(order='F')  # also the active scalars
isos = mesh.contour()
isos.plot(opacity=0.7)