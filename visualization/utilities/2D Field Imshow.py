import data_loader as dl
import matplotlib.pyplot as plt
import numpy as np

X, Y, vec = dl.import_2d_field("./examples/outputs/Horn Port 2d.txt")
vec_real = np.real(vec)
vec_real_mag = np.linalg.norm(vec_real, axis=2)

plt.imshow(vec_real_mag)
plt.colorbar()
plt.title("Field Magnitude")
plt.show()