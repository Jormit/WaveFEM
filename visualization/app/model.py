import cadquery as cq
import numpy as np
import pyvista as pv
import geo

regular_color = [0.4, 0.6, 0.6]
highlight_color = [1, 0, 1]

class model:
    def __init__(self, filename):
        self.faces = []
        self.points = []

        self.selected_faces = None
        self.selected_faces_faces = None
        self.selected_faces_points = None
        self.selected_face_handle = None

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

    def select_faces(self, point, vector):
        self.selected_faces = self.shape.val().facesIntersectedByLine(tuple(point), tuple(vector))
        self.selected_faces_points, self.selected_faces_faces = geo.tesselate_face_list(self.selected_faces)
        self.selected_faces_faces = np.array(self.selected_faces_faces)
        self.selected_faces_points = np.array(self.selected_faces_points)
    
    def cycle_highlighted_face(self, plotter):
        if (self.selected_face_handle is not None):
            plotter.remove_actor(self.selected_face_handle)
        
        if (len(self.selected_faces_faces)):
            f = self.selected_faces_faces[0]
            p = self.selected_faces_points[0]
            num_points = 3 * np.ones((f.shape[0], 1), dtype="int")
            mesh = pv.PolyData(p, np.hstack((num_points, f)))
            self.selected_face_handle = plotter.add_mesh(mesh, color=highlight_color)
    