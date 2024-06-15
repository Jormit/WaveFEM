#include <iostream>

#include "functions.h"
#include "constants.h"

const double grad_h = 1e-5;

std::complex<double> func::free_space_greens_function(Eigen::Vector3d r, Eigen::Vector3d r_, double wavenumber)
{
    auto norm = (r - r_).norm();
    auto j = std::complex<double>{ 0, 1 };
    return std::exp(j * wavenumber * norm) / norm;
}

Eigen::Vector3cd func::free_space_greens_function_grad(Eigen::Vector3d r, Eigen::Vector3d r_, double wavenumber)
{
    Eigen::Vector3cd grad_g;
    auto norm = (r - r_).norm();
    auto j = std::complex<double>{ 0, 1 };
    grad_g = (1.0 / norm - j * wavenumber) * std::exp(j * wavenumber * norm) / norm * (r_ - r) / norm;
    return grad_g;
}

/*
Eigen::Vector3cd func::free_space_greens_function_grad(Eigen::Vector3d r, Eigen::Vector3d r_, double wavenumber)
{
    Eigen::Vector3cd grad_g;

    Eigen::Vector3d dr_plus_x;
    Eigen::Vector3d dr_minus_x;
    dr_plus_x << r(0) + grad_h, r(1), r(2);
    dr_minus_x << r(0) - grad_h, r(1), r(2);
    grad_g(0) = (free_space_greens_function(dr_plus_x, r_, wavenumber) - free_space_greens_function(dr_minus_x, r_, wavenumber)) / (2* grad_h);

    Eigen::Vector3d dr_plus_y;
    Eigen::Vector3d dr_minus_y;
    dr_plus_y << r(0), r(1) + grad_h, r(2);
    dr_minus_y << r(0), r(1) - grad_h, r(2);
    grad_g(1) = (free_space_greens_function(dr_plus_y, r_, wavenumber) - free_space_greens_function(dr_minus_y, r_, wavenumber)) / (2 * grad_h);

    Eigen::Vector3d dr_plus_z;
    Eigen::Vector3d dr_minus_z;
    dr_plus_z << r(0), r(1), r(2) + grad_h;
    dr_minus_z << r(0), r(1), r(2) - grad_h;
    grad_g(2) = (free_space_greens_function(dr_plus_z, r_, wavenumber) - free_space_greens_function(dr_minus_z, r_, wavenumber)) / (2 * grad_h);

    return grad_g;
}
*/




