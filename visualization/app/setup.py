import json
import os
import defaults

class setup:
    def __init__(self, filename=None):
        self.filename = filename
        self.path = None
        self.data = {}

        if (filename):
            f = open(filename)
            self.data = json.load(f)
            self.path = os.path.dirname(filename)

        else:
            self.data = defaults.setup

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

    def remove_material(self, name):
        del self.data["material_definitions"][name]

    def get_material(self, name):
        return self.data["material_definitions"][name]
    
    def get_materials(self):
        return self.data["material_definitions"]
    
    def contains_material(self, name):
        return name in self.data["material_definitions"]
    
    def assign_material(self, ids, material):
        for id in ids:
            self.data["material_assignments"][id] = material

    def update_material_assignment(self, name, new_name):
        if self.data["material_assignments"] is None:
            return
        for i in range(len(self.data["material_assignments"])):
            if self.data["material_assignments"][i] == name:
                self.data["material_assignments"][i] = new_name

    def add_port(self, bbox):
        self.data["port_centres"].append([
            bbox.xmin-defaults.bbox_padding,
            bbox.xmax+defaults.bbox_padding,
            bbox.ymin-defaults.bbox_padding,
            bbox.ymax+defaults.bbox_padding,
            bbox.zmin-defaults.bbox_padding,
            bbox.zmax+defaults.bbox_padding])

    def get_port(self, id):        
        return self.data["port_centres"][id]

    def num_ports(self):
        return len(self.data["port_centres"])

    def update_from_model(self, model):
        self.data["port_centres"] = []
        self.data["model_file"] = os.path.basename(model.filename)
        self.data["material_assignments"] = [""] * model.get_num_parts()

    def print_setup(self):
        print(self.data)

    def get_step_filename(self):
        return os.path.join(self.path, self.data["model_file"]).replace("/","\\")
        
    def has_filename(self):
        return self.filename is not None
    
    def set_filename(self, filename):
        self.filename = filename
        self.path = os.path.dirname(filename)

    def save_setup(self):
        with open(self.filename, 'w') as f:
            json.dump(self.data, f)

    def get_misc_params(self):
        return {
            "bounding_box_padding": self.data["bounding_box_padding"],
            "pml_enable": self.data["pml_enable"],
            "pml_thickness": self.data["pml_thickness"],
            "frequency": self.data["frequency"],
            "target_elements_per_wavelength": self.data["target_elements_per_wavelength"]
        }
    
    def set_misc_params(self, data):
        self.data["bounding_box_padding"] = data["bounding_box_padding"]
        self.data["pml_enable"] = data["pml_enable"]
        self.data["pml_thickness"] = data["pml_thickness"]
        self.data["frequency"] = data["frequency"]
        self.data["target_elements_per_wavelength"] = data["target_elements_per_wavelength"]

    def get_part_ids(self):
        ids = []
        for i in range(len(self.data["material_assignments"])):
            material = self.data["material_assignments"][i]
            if material == "":
                ids.append("Body_{}".format(i))
            else:
                ids.append("Body_{} [{}]".format(i, material))
        return ids
