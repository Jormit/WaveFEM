import numpy as np
from mayavi import mlab

f=open("full_port_0_solution.txt","r+")
x = []
y = []
z = []
u = []
v = []
w = []

for line in f:
     line=line.split()
     if line: 
            x.append(float(line[0]))
            y.append(float(line[1]))
            z.append(float(line[2]))
            u.append(complex(line[3]))
            v.append(complex(line[4]))
            w.append(complex(line[5]))

x = np.array(x, dtype=float)
y = np.array(y, dtype=float)
z = np.array(z, dtype=float)
u = np.array(u, dtype=complex)
v = np.array(v, dtype=complex)
w = np.array(w, dtype=complex)

vec = np.stack((u,v,w), axis=1)
vec_real = np.real(vec)
vec_real_mag = np.linalg.norm(vec_real, axis=1)

mlab.volume_slice(x.reshape((30,30,30)), y.reshape((30,30,30)), z.reshape((30,30,30)), vec_real_mag.reshape((30,30,30)), plane_orientation='x_axes')
mlab.volume_slice(x.reshape((30,30,30)), y.reshape((30,30,30)), z.reshape((30,30,30)), vec_real_mag.reshape((30,30,30)), plane_orientation='y_axes')
mlab.volume_slice(x.reshape((30,30,30)), y.reshape((30,30,30)), z.reshape((30,30,30)), vec_real_mag.reshape((30,30,30)), plane_orientation='z_axes')
mlab.show()



            