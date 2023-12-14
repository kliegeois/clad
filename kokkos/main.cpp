#include "clad/Differentiator/Differentiator.h"
#include <Kokkos_Core.hpp>
#include "functor_for.hpp"
#include "lambda_reduction.hpp"
#include "lambda_reduction_subview.hpp"

int main(int argc, char* argv[]) {
  Kokkos::initialize(argc, argv);
  {
    Kokkos::View<double **> A("A", 10, 10);
    Kokkos::View<double *> x("x", 10);
    Kokkos::View<double *> y("y", 10);

    Kokkos::deep_copy(A, 3);
    Kokkos::deep_copy(x, 2);
    Kokkos::deep_copy(y, 4);

    std::cout << f(3.,4.) << std::endl;
    std::cout << weightedDotProduct_1(A, x, y) << std::endl;
    std::cout << weightedDotProduct_2(A, x, y) << std::endl;

    auto f_dx = clad::differentiate(f, "x");
    // Any of the two below will generate an "error: Attempted differentiation w.r.t. member 'x' which is not of real type."
    //auto weightedDotProduct_1_dx = clad::differentiate(weightedDotProduct_1<typeof(A),typeof(x),typeof(y)>, "x");
    //auto weightedDotProduct_2_dx = clad::differentiate(weightedDotProduct_2<typeof(A),typeof(x),typeof(y)>, "x");

  }
  Kokkos::finalize();

}