#include "material.h"
#include "complex"

const int PML_X = 0;
const int PML_Y = 1;
const int PML_Z = 2;

material generate_pml(int type, double alpha, double beta);