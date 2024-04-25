import cadquery as cq
import numpy as np
import pyvista as pv
import geo

regular_color = [0.5, 0.5, 0.5]
highlight_color = [1, 0, 1]
edge_color = [0, 0, 0]

class model:
    def __init__(self, filename):
        self.filename = filename
        self.shape = cq.importers.importStep(filename)
        self.plot_handles = []
        self.highlighted_surface = None
        self.highlighted_parts = []
        self.face_2_tag = {}
        self.selected_faces = []
        self.selected_face_index = 0
        
        i = 0
        for s in self.shape.solids().all():
           s.val().mesh(0.1, 0.1)
           for f in s.faces().all():
               f.tag(i)
               self.face_2_tag[f.val()] = i
               i+=1

        self.face_count = i

    def plot(self, plotter):
        for i in range(self.face_count):
            face = self.shape.faces(tag=i).val()
            vertices, triangles = geo.tesselate_face(face)
            num_vertices = 3 * np.ones((triangles.shape[0], 1), dtype="int")
            mesh = pv.PolyData(vertices, np.hstack((num_vertices, triangles)))
            feat_edges = mesh.extract_feature_edges()
            feat_edges.clear_data()
            self.plot_handles.append(plotter.add_mesh(mesh, color=regular_color))
            plotter.add_mesh(feat_edges, color=edge_color, render_lines_as_tubes=True, line_width=2)
        plotter.add_axes()
        plotter.show()
    
    def get_num_parts(self):
        return self.shape.solids().size()
    
    def reset_shading(self, opacity=1):
        for handle in self.plot_handles:
            handle.GetProperty().SetColor(regular_color)
            handle.GetProperty().SetOpacity(opacity)
    
    def highlight_part(self, id):
        if (id >= self.get_num_parts()):
            return
        s = self.shape.solids().all()[id]
        for f in s.faces().vals():
            tag = self.face_2_tag[f]
            self.plot_handles[tag].GetProperty().SetColor(highlight_color)
            self.plot_handles[tag].GetProperty().SetOpacity(1)

        self.highlighted_parts.append(id)

    def get_highlighted_parts(self):
        return self.highlighted_parts

    def remove_highlights(self):
        self.reset_shading()
        self.highlighted_surface = None
        self.highlighted_parts = []

    def select_faces(self, point, vector):
        self.selected_face_index = 0
        self.selected_faces = self.shape.val().facesIntersectedByLine(tuple(point), tuple(vector))        
    
    def cycle_highlighted_face(self, plotter):
        num_selected_faces = len(self.selected_faces)
        if (num_selected_faces > 0):
            # Remove Previous Selection
            self.reset_shading(0.3)

            # Add New Selection
            f = self.selected_faces[self.selected_face_index]
            tag = self.face_2_tag[f]
            self.highlighted_surface = tag

            self.plot_handles[tag].GetProperty().SetColor(highlight_color)
            self.plot_handles[tag].GetProperty().SetOpacity(1)            

            self.selected_face_index = (self.selected_face_index + 1) % num_selected_faces

    def get_selected_face_bbox(self):
        if (self.highlighted_surface is None):
            return None
        return self.shape.faces(tag=self.highlighted_surface).val().BoundingBox(0.01)
    
    def highlight_faces_in_bounding_box(self, bbox):
        faces = self.shape.faces(cq.selectors.BoxSelector((bbox[0], bbox[2], bbox[4]), (bbox[1], bbox[3], bbox[5]), True)).vals()

        self.reset_shading(0.3)
        for f in faces:
            tag = self.face_2_tag[f]
            self.plot_handles[tag].GetProperty().SetColor(highlight_color)
            self.plot_handles[tag].GetProperty().SetOpacity(1)
        
