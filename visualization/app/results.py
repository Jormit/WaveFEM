import os
import numpy as np
import pyvista as pv

import defaults
import data_loader

class dataset_3d:
    def __init__(self, filename):
        self.x, self.y, self.z, self.vec = data_loader.import_3d_field(filename)    

    def countour_plot(self, plotter):
        vec_real = np.real(self.full_field_solution.vec)
        vec_real_mag = np.linalg.norm(vec_real, axis=3)
        mesh = pv.StructuredGrid(self.x, self.y, self.z)
        mesh.point_data['values'] = vec_real_mag.ravel(order='F')
        isos = mesh.contour()
        return plotter.add_mesh(isos)        

class results:
    def __init__(self, directory):
        self.directory = directory
        self.datasets = {}
        self.active_dataset = None

        if not os.path.exists(self.directory):
            return

        for file in os.listdir(self.directory):
            filename = os.fsdecode(file)
            if filename.endswith(".3d"):
                self.datasets[filename] = dataset_3d(self.directory + filename)
                print(filename)

    def results_list(self):
        list = []
        for dataset in self.datasets:
            list.append(dataset)
        return list

    def activate_dataset(self, name, plotter):
        self.active_dataset = self.datasets[name].countour_plot(plotter)

        
        
