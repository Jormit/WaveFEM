#include "fem.h"
#include <iostream>

#include <Spectra/SymGEigsShiftSolver.h>
#include <Spectra/MatOp/SymShiftInvert.h>
#include <Spectra/MatOp/SparseSymMatProd.h>

std::pair<Eigen::VectorXd, Eigen::MatrixXd> fem::solve_eigenproblem(const Eigen::SparseMatrix<double>& S, const Eigen::SparseMatrix<double>& T)
{
    using OpType = Spectra::SymShiftInvert<double, Eigen::Sparse, Eigen::Sparse>;
    using BOpType = Spectra::SparseSymMatProd<double>;
    OpType op(S, T);
    BOpType Bop(T);

    Spectra::SymGEigsShiftSolver<OpType, BOpType, Spectra::GEigsMode::ShiftInvert> geigs(op, Bop, 3, 6, 3.0);

    geigs.init();
    int nconv = geigs.compute(Spectra::SortRule::LargestMagn);

    Eigen::VectorXd evalues;
    Eigen::MatrixXd evecs;
    if (geigs.info() == Spectra::CompInfo::Successful)
    {
        evalues = geigs.eigenvalues();
        evecs = geigs.eigenvectors();
    }

    return { evalues, evecs };
}

