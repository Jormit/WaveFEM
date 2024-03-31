import numpy as np
import matplotlib.pyplot as plt

coords = np.array([[0,0],
                   [1,0],
                   [0,1]])

def get_simplex_coefficients(coordinates):
    mat = coordinates.T
    mat = np.vstack([mat, [1,1,1]])
    return np.linalg.inv(mat)

def basis(i, j, simplex_coeff, simplex_coords, type):
    if (type == 1):
        return simplex_coords[i-1]*simplex_coeff[j-1][:3] - simplex_coords[j-1]*simplex_coeff[i-1][:3]
    elif (type == 2):
        return simplex_coords[i-1]*simplex_coeff[j-1][:3] + simplex_coords[j-1]*simplex_coeff[i-1][:3]
    return 0

simplex_coeff = get_simplex_coefficients(coords)

steps = 10 

X = np.zeros((steps * steps * steps))
Y = np.zeros((steps * steps * steps))

U = np.zeros((steps * steps * steps))
V = np.zeros((steps * steps * steps))

# Plot whitney elem
i = 0
for x in np.linspace(0,1,steps):
    for y in np.linspace(0,1,steps):
            simplex_coords = simplex_coeff @ np.array([x,y,1])
            if not sum(n < 0 for n in simplex_coords):
                X[i] = x
                Y[i] = y
                vec = basis(1, 2, simplex_coeff, simplex_coords, 2)
                U[i] = vec[0]
                V[i] = vec[1]
                i+=1
            
plt.quiver(X,Y,U,V, scale=10)

plt.show()