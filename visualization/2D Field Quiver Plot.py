import data_loader as dl
import matplotlib.pyplot as plt
import numpy as np

X, Y, vec = dl.import_2d_field("./Data/Ridged Waveguide Port 2d.txt")

plt.quiver(X, Y, np.real(vec[:,:,0]), np.real(vec[:,:,1]), scale=5)
plt.show()