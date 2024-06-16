import numpy as np

def import_structured_3d_field(filename):
    f=open(filename,"r+")
    lines = f.readlines()

    u = []
    v = []
    w = []

    line0=lines[0].split()
    line1=lines[1].split()
    line2=lines[2].split()

    start = (float(line0[0]), float(line0[1]), float(line0[2]))
    step = (float(line1[0]), float(line1[1]), float(line1[2]))
    steps = (int(line2[0]), int(line2[1]), int(line2[2]))

    for line in lines[3:]:
        line=line.split()
        if line: 
                u.append(complex(line[0]))
                v.append(complex(line[1]))
                w.append(complex(line[2]))

    u = np.array(u, dtype=complex)
    v = np.array(v, dtype=complex)
    w = np.array(w, dtype=complex)
    vec = np.stack((u,v,w), axis=1).reshape(steps + (3,))

    x,y,z = np.mgrid[start[0]:start[0] + step[0]*(steps[0]-1):steps[0]*1j,
                    start[1]:start[1] + step[1]*(steps[1]-1):steps[1]*1j,
                    start[2]:start[2] + step[2]*(steps[2]-1):steps[2]*1j]

    return x, y, z, vec

def import_unstructured_3d_field(filename):
    f=open(filename,"r+")
    lines = f.readlines()

    x = []
    y = []
    z = []

    u = []
    v = []
    w = []

    for line in lines:
        line=line.split()
        if line: 
                x.append(complex(line[0]))
                y.append(complex(line[1]))
                z.append(complex(line[2]))

                u.append(complex(line[3]))
                v.append(complex(line[4]))
                w.append(complex(line[5]))

    u = np.array(u, dtype=complex)
    v = np.array(v, dtype=complex)
    w = np.array(w, dtype=complex)

    vec = np.stack((u,v,w), axis=1)

    x = np.array(x)
    y = np.array(y)
    z = np.array(z)

    return x, y, z, vec

def import_2d_polar_field(filename):
    f=open(filename,"r+")
    lines = f.readlines()

    r = []
    theta = []
    phi = []

    line0=lines[0].split()
    line1=lines[1].split()

    start = float(line0[0])
    step = float(line0[1])
    steps = int(line0[2])

    for line in lines[2:]:
        line=line.split()
        if line: 
            r.append(complex(line[0]))
            theta.append(complex(line[1]))
            phi.append(complex(line[2]))

    angles = np.mgrid[start:start + step*(steps-1):steps*1j]

    return angles, r, theta, phi

    