import os
import numpy as np
import pyvista as pv

import data_loader

PLOT_3D = 0
PLOT_2D = 1
PLOT_POLAR = 2

class dataset_base:
    def __init__(self, filename):
        return

    def plot(self, plotter):
        return None

    def parameters(self):
        return {}
    
    def set_parameters(self, params):
        return
    
    def plot_type(self):
        return PLOT_3D

class dataset_vtk (dataset_base):
    def __init__(self, filename):
        self.mesh = pv.read(filename)
    
    def plot(self, plotter):
        return self.mesh_plot(plotter)

    def mesh_plot(self, plotter):
        return plotter.add_mesh(self.mesh, style="wireframe", opacity=0.5, reset_camera=False)

class dataset_3d (dataset_base):
    def __init__(self, filename):
        self.x, self.y, self.z, self.vec = data_loader.import_structured_3d_field(filename)
        self.style = "Contour"
        self.quantity = "Real"

    def plot(self, plotter):
        if self.style == "Contour":
            return self.contour_plot(plotter)
        elif self.style == "Vector":
            return self.vector_plot(plotter)
        else:
            return self.clip_plane(plotter)
    
    def parameters(self):
        return {
            "Plot Style" : {
                "options" : ["Contour", "Vector", "Plane"],
                "selected" : self.style
            },
            "Quantity" : {
                "options" : ["Real", "Complex Magnitude"],
                "selected" : self.quantity
            }
        }
    
    def set_parameters(self, params):
        self.style = params["Plot Style"]["selected"]
        self.quantity = params["Quantity"]["selected"]

    def plot_quantity(self):
        if (self.quantity == "Complex Magnitude"):
            return np.abs(self.vec)
        else:
            return np.real(self.vec)

    def contour_plot(self, plotter):
        vec_real = self.plot_quantity()
        vec_real_mag = np.linalg.norm(vec_real, axis=3)
        mesh = pv.StructuredGrid(self.x, self.y, self.z)
        mesh.point_data['values'] = vec_real_mag.ravel(order='F')
        isos = mesh.contour()
        return plotter.add_mesh(isos, opacity=0.7, cmap='jet', reset_camera=False)
    
    def vector_plot(self, plotter):
        vec_real = self.plot_quantity()
        max_mag = np.max(np.linalg.norm(vec_real, axis=3))
        mesh = pv.StructuredGrid(self.x, self.y, self.z)
        mesh['vectors'] = np.column_stack((vec_real[:,:,:,0].ravel(order='F'), vec_real[:,:,:,1].ravel(order='F'), vec_real[:,:,:,2].ravel(order='F')))
        mesh.set_active_vectors("vectors")
        vector_mesh = mesh.glyph(orient="vectors", factor=1/max_mag * 10)
        return plotter.add_mesh(vector_mesh, reset_camera=False, cmap='jet')
    
    def clip_plane(self, plotter):
        vec_real = self.plot_quantity()
        vec_real_mag = np.linalg.norm(vec_real, axis=3)
        mesh = pv.StructuredGrid(self.x, self.y, self.z)
        mesh.point_data['values'] = vec_real_mag.ravel(order='F')
        return plotter.add_mesh_slice(mesh, reset_camera=False, cmap='jet')
    
class dataset_3d_unstructured (dataset_base):
    def __init__(self, filename):
        self.x, self.y, self.z, self.vec = data_loader.import_unstructured_3d_field(filename)

    def plot(self, plotter):
        return self.vector_plot(plotter)
    
    def vector_plot(self, plotter):
        vec_real = np.real(self.vec)
        max_mag = np.max(np.linalg.norm(vec_real, axis=1))
        points = np.vstack(np.c_[self.x, self.y, self.z])
        n_points = len(points)
        cell_type = np.full(n_points, pv.CellType.VERTEX)
        cells = np.c_[np.full(n_points, 1), range(n_points)]
        grid = pv.UnstructuredGrid(cells, cell_type, points)
        grid['vectors'] = vec_real

        return plotter.add_mesh(grid.glyph(orient='vectors', scale=True, factor=1/max_mag * 2), reset_camera=False, cmap='jet')
    

class dataset_2d_polar(dataset_base):
    def __init__(self, filename):
        self.angles, self.r, self.theta, self.phi = data_loader.import_2d_polar_field(filename)
        self.variable = "Theta"
        self.quantity = "Directivity (dB)"

    def plot_type(self):
        return PLOT_POLAR

    def plot(self, plotter):
        if (self.variable == "Phi"):
            variable = np.abs(self.phi)
        elif (self.variable == "Theta"):
            variable = np.abs(self.theta)
        else:
            variable = (np.abs(self.theta) + np.abs(self.phi))

        if (self.quantity == "Directivity (dB)"):
            variable = 20 * np.log10(variable/np.mean(variable))
        else:
            variable = variable / np.max(variable)
            
        plotter.axes.plot(self.angles, variable)
        plotter.axes.set_xlabel("Swept Angle")
        plotter.axes.set_ylabel(self.quantity)
        plotter.axes.set_theta_zero_location("N")

        plotter.draw()

        return plotter
    
    def parameters(self):
        return {
            "Variable" : {
                "options" : ["Phi", "Theta", "Total"],
                "selected" : self.variable
            },
            "Quantity" : {
                "options" : ["Directivity (dB)", "Normalized Magnitude"],
                "selected" : self.quantity
            }
        }
    
    def set_parameters(self, params):
        self.variable = params["Variable"]["selected"]
        self.quantity = params["Quantity"]["selected"]
    
class results:
    def __init__(self, directory):
        self.directory = directory
        self.datasets = {}
        self.active_dataset_actor = None
        self.active_dataset_name = None
        self.current_plotter = None

        if not os.path.exists(self.directory):
            return

        for file in os.listdir(self.directory):
            filename = os.fsdecode(file)
            if filename.endswith(".3d"):
                self.datasets[filename] = dataset_3d(self.directory + filename)

            if filename.endswith(".vtk"):
                self.datasets[filename] = dataset_vtk(self.directory + filename)

            if filename.endswith(".3du"):
                self.datasets[filename] = dataset_3d_unstructured(self.directory + filename)

            if filename.endswith(".ff"):
                self.datasets[filename] = dataset_2d_polar(self.directory + filename)

    def results_list(self):
        list = []
        for dataset in self.datasets:
            list.append(dataset)
        return list

    def activate_dataset(self, name, plotter):
        self.active_dataset_actor = self.datasets[name].plot(plotter)
        self.active_dataset_name = name
        self.current_plotter = plotter

    def plot_type(self, name):
        return self.datasets[name].plot_type()

    def deactivate_dataset(self):
        if (self.active_dataset_actor is not None):
            if (self.datasets[self.active_dataset_name].plot_type() == PLOT_3D):
                self.current_plotter.clear_plane_widgets()
                self.current_plotter.remove_actor(self.active_dataset_actor)
                self.active_dataset_actor = None
            else:
                self.current_plotter.axes.clear()
                self.active_dataset_actor = None

    def refresh_dataset(self):
        if (self.active_dataset_actor is not None):
            self.deactivate_dataset()
            self.active_dataset_actor = self.datasets[self.active_dataset_name].plot(self.current_plotter)

    def dataset_parameters(self):
        if (self.active_dataset_name is not None):
            return self.datasets[self.active_dataset_name].parameters()
        return []
    
    def set_dataset_parameters(self, params):
        if (self.active_dataset_name is not None):
            return self.datasets[self.active_dataset_name].set_parameters(params)


        
        
