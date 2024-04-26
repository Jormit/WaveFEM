bbox_padding = 1e-5

material = {
        "name": "Material",
        "ep": 1.0,
        "mu": 1.0,
        "tand": 0.00,
        "PEC": False
}

setup = {
        "model_file" : None,
        "port_centres" : [],
        "bounding_box_padding" : 10.0,
        "pml_enable" : True,
        "pml_thickness" : 10.0,
        "frequency" : 5000000.0,
        "target_elements_per_wavelength" : 3,
        "material_assignments" : [],
        "material_definitions" : {}
}