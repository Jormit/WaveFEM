import cadquery as cq
import numpy as np
import pyvista as pv

regular_color = [0.4, 0.6, 0.6]
highlight_color = [0, 0, 1]

class model:
    def __init__(self, filename):
        self.faces = []
        self.points = []
        self.shape = cq.importers.importStep(filename)
        self.plot_handles = []
        self.highlighted_parts = []

        for s in self.shape.solids().vals():
            s_points, s_faces = s.tessellate(0.1)
            s_points = list(map(lambda x: x.toTuple(), s_points))
            self.faces.append(np.array(s_faces))
            self.points.append(np.array(s_points))

    def plot(self, plotter):
        for (f, p) in zip(self.faces, self.points):
            num_points = 3 * np.ones((f.shape[0], 1), dtype="int")
            mesh = pv.PolyData(p, np.hstack((num_points, f)))
            self.plot_handles.append(plotter.add_mesh(mesh, opacity=0.85, color=regular_color))
        plotter.show()

    def get_part_ids(self):
        return list(range(self.get_num_parts()))
    
    def get_num_parts(self):
        return self.shape.solids().size()
    
    def highlight_part(self, id):
        if (id >= self.get_num_parts()):
            return
        self.highlighted_parts.append(id)  
        self.plot_handles[id].GetProperty().SetColor(highlight_color)

    def remove_highlights(self):
        for id in self.highlighted_parts:
            self.plot_handles[id].GetProperty().SetColor(regular_color)
        self.highlight_parts = []