import json
import os

class setup:
    def __init__(self, filename=None):
        self.filename = filename
        self.data = {}

        if (filename):
            f = open(filename)
            self.data = json.load(f)

        else:
            self.data["model_file"] = None
            self.data["port_centres"] = None
            self.data["bounding_box_padding"] = None
            self.data["pml_enable"] = None
            self.data["pml_thickness"] = None
            self.data["frequency"] = None
            self.data["target_elements_per_wavelength"] = None
            self.data["material_assignments"] = None
            self.data["material_definitions"] = {}

    def validate(self):
        valid = True
        if self.data["model_file"] is None:
            valid = False
            print("Model file not defined!")

        if self.data["port_centres"] is None:
            valid = False
            print("No ports defined!")

        if self.data["bounding_box_padding"] is None:
            valid = False
            print("Bounding box not defined!")

        if self.data["pml_enable"] is None:
            valid = False
            print("PML option not defined!")

        if self.data["pml_thickness"] is None:
            valid = False
            print("PML thickness not defined!")

        if self.data["frequency"] is None:
            valid = False
            print("Simulation frequency not defined!")

        if self.data["target_elements_per_wavelength"] is None:
            valid = False
            print("Elements per wavelength not defined!")

        if self.data["material_assignments"] is None:
            valid = False
            print("Material assignments not set!")

        if len(self.data["material_definitions"]) == 0:
            valid = False
            print("Material definitions not set!")            

        return valid
    
    def add_material(self, material):
        self.data["material_definitions"][material["name"]] = material

    def get_material(self, name):
        return self.data["material_definitions"][name]
    
    def get_materials(self):
        return self.data["material_definitions"]
    
    def contains_material(self, name):
        return name in self.data["material_definitions"]
    
    def assign_material(self, ids, material):
        for id in ids:
            self.data["material_assignments"][id] = material

    def update_from_model(self, model):
        self.data["model_file"] = os.path.basename(model.filename)
        self.data["material_assignments"] = [""] * model.get_num_parts()

    def print_setup(self):
        print(self.data)
        
    