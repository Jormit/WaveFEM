import cadquery as cq
import numpy as np
import pyvista as pv
import geo

regular_color = [0.5, 0.5, 0.5]
highlight_color = [1, 0, 1]

class model:
    def __init__(self, filename):
        self.shape = cq.importers.importStep(filename)
        self.plot_handles = []
        self.highlighted_tags = []
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
            self.plot_handles.append(plotter.add_mesh(mesh, opacity=0.85, color=regular_color))

        plotter.add_axes()
        plotter.show()

    def get_part_ids(self):
        return list(range(self.get_num_parts()))
    
    def get_num_parts(self):
        return self.shape.solids().size()
    
    def highlight_part(self, id):
        if (id >= self.get_num_parts()):
            return
        s = self.shape.solids().all()[id]
        for f in s.faces().vals():
            tag = self.face_2_tag[f]
            self.highlighted_tags.append(tag)
            self.plot_handles[tag].GetProperty().SetColor(highlight_color)

    def remove_highlights(self):
        for id in self.highlighted_tags:
            self.plot_handles[id].GetProperty().SetColor(regular_color)
        self.highlighted_tags = []

    def select_faces(self, point, vector):
        self.selected_face_index = 0
        self.selected_faces = self.shape.val().facesIntersectedByLine(tuple(point), tuple(vector))        
    
    def cycle_highlighted_face(self, plotter):
        num_selected_faces = len(self.selected_faces)
        if (num_selected_faces > 0):
            # Remove Previous Selection
            previous_index = (self.selected_face_index - 1 + num_selected_faces) % num_selected_faces
            f = self.selected_faces[previous_index]
            tag = self.face_2_tag[f]
            self.plot_handles[tag].GetProperty().SetColor(regular_color)

            # Add New Selection
            f = self.selected_faces[self.selected_face_index]
            tag = self.face_2_tag[f]
            self.plot_handles[tag].GetProperty().SetColor(highlight_color)

            bbox = self.shape.faces(tag=tag).val().BoundingBox(0.01)
            print(bbox.xmin, bbox.xmax, bbox.ymin, bbox.ymax, bbox.zmin, bbox.zmax)

            self.highlighted_tags.append(tag)

            self.selected_face_index = (self.selected_face_index + 1) % num_selected_faces
    