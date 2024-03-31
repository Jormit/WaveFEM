import cadquery as cq
import numpy as np
import pyvista as pv

shape = cq.importers.importStep("horn.step")

points, faces = shape.val().tessellate(0.1)

points = list(map(lambda x: x.toTuple(),points))

faces = np.array(faces)
points = np.array(points)

print(faces)
print(points)

num_points = 3 * np.ones((faces.shape[0], 1), dtype="int")

mesh = pv.PolyData(points, np.hstack((num_points, faces)))
pl = pv.Plotter()
pl.add_mesh(mesh, opacity=0.85)

pl.show()