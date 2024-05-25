<img src='design/Logo.png' width="300px">

## About
A 3d electromagnetic field solver in C++ with a Python frontend.

## Current Functionality
- Loading and meshing .step files with different materials assigned to each part. 
- 2D Port solver for TEM and TE modes supporting inhomogenous cross-sections.
- Multiport 3D Field solver.
- S-parameter computation.
- Domain truncation using PML.

## Todo
- Frequency sweeping
- Radiation patterns
- Automatic mesh refinement

## Examples
Simulations of some basic geometries compared against HFSS.

### Horn Antenna
<img src='examples/photos/Horn Antenna.png' height="250px"> <img src='examples/photos/Horn HFSS.png' height="250px">

### Waveguide with Dialectric Cross Section
<img src='examples/photos/Dialectric Loaded Waveguide.png' height="250px"> <img src='examples/photos/Dialectric Loaded Waveguide HFSS.png' height="250px">

### Waveguide with Dialectric Obstacle
<img src='examples/photos/Dialectric Obstacle.png' height="250px"> <img src='examples/photos/Dialectric Obstacle HFSS.png' height="250px">

## Acknowledgements 

### Libraries
GMSH - https://gmsh.info/

Pyvista - https://pyvista.org/

CadQuery - https://github.com/CadQuery/cadquery

Eigen - https://eigen.tuxfamily.org/index.php?title=Main_Page

PyQT - https://doc.qt.io/qtforpython-6/

### Icons
https://p.yusukekamiyamane.com/
