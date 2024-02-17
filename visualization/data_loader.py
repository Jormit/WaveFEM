import numpy as np

def import_3d_field(filename, dim):
    f=open(filename,"r+")
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

    x = x.reshape(dim)
    y = y.reshape(dim)
    z = z.reshape(dim)
    vec = np.stack((u,v,w), axis=1).reshape(dim + (3,))

    return x,y,z,vec