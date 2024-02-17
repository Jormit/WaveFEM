import data_loader as dl
import matplotlib.pyplot as plt
import numpy as np

x,y,vec = dl.import_2d_field("./Data/Ridged Waveguide Port.txt", (30, 30))


plt.quiver(x,y,np.real(vec[:,:,0]), np.real(vec[:,:,1]), scale=4)
plt.show()