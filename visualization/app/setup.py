import json

class setup:
    def __init__(self):

        self.model_file = None
        self.port_centres = None
        self.bounding_box_padding = None
        self.pml_enable = None
        self.pml_thickness = None
        self.frequency = None
        self.target_elements_per_wavelength = None
        self.material_assignments = None
        self.material_definitions = None

        return

    def __init__(self, filename):
        self.filename = filename

        f = open(filename)
        data = json.load(f)

        self.model_file = data['model_file']
        self.port_centres = data['port_centres']
        self.bounding_box_padding = data['bounding_box_padding']
        self.pml_enable = data['pml_enable']
        self.pml_thickness = data['pml_thickness']
        self.frequency = data['frequency']
        self.target_elements_per_wavelength = data['target_elements_per_wavelength']
        self.material_assignments = data['conductor']
        self.material_definitions = data['material_definitions']

        print(data)

    def validate(self):
        valid = True
        if self.model_file is None:
            valid = False
            print("Model file not defined!")

        if self.port_centres is None:
            valid = False
            print("No ports defined!")

        if self.bounding_box_padding is None:
            valid = False
            print("Bounding box not defined!")

        if self.pml_enable is None:
            valid = False
            print("PML option not defined!")

        if self.pml_thickness is None:
            valid = False
            print("PML thickness not defined!")

        if self.frequency is None:
            valid = False
            print("Simulation frequency not defined!")

        if self.target_elements_per_wavelength is None:
            valid = False
            print("Elements per wavelength not defined!")

        if self.material_assignments is None:
            valid = False
            print("Material assignments not set!")

        if self.material_definitions is None:
            valid = False
            print("Material definitions not set!")            

        return valid
        
    