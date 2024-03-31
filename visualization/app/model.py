import cadquery as cq
import numpy as np
import pyvista as pv

class model:
    def __init__(self, filename):
        self.shape = cq.importers.importStep(filename)
        self.points, self.faces = self.shape.val().tessellate(0.1)

        self.points = list(map(lambda x: x.toTuple(), self.points))

        self.faces = np.array(self.faces)
        self.points = np.array(self.points)

    def plot(self, plotter):
        num_points = 3 * np.ones((self.faces.shape[0], 1), dtype="int")
        mesh = pv.PolyData(self.points, np.hstack((num_points, self.faces)))
        plotter.add_mesh(mesh, opacity=0.85)
        plotter.show()

    def get_part_ids(self):
        return range(self.shape.solids().vals())