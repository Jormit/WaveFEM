import pyvista as pv
import numpy as np

pl = pv.Plotter()
cube = pv.Cube()

def callback(point):
    mouse_vector = np.subtract(point, pl.camera_position[0])
    mouse_vector = mouse_vector / np.linalg.norm(mouse_vector)

pl.add_mesh(cube, show_edges=True)
pl.enable_surface_point_picking(callback=callback, show_point=True, left_clicking=True)
pl.show()