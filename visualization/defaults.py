title = "FEM3D"

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
        "port_bounding_boxes" : [],
        "bounding_box_padding" : 10.0,
        "pml_enable" : True,
        "pml_thickness" : 10.0,
        "frequency" : 5000000000.0,
        "target_elements_per_wavelength" : 3,
        "material_assignments" : [],
        "material_definitions" : {}
}

sim_location = "./out/build/x64-release/FEM3D.exe"

regular_color = [0.5, 0.5, 0.5]

highlight_color = [1, 0, 1]

edge_color = [0, 0, 0]