#pragma once

#include <Kokkos_Core.hpp>

namespace kokkos_builtin_derivative {

// Parallel sum:

template <class Viewtype, class Layout, int Rank = Viewtype::rank(), typename iType = int>
struct ViewSum;

template <class Viewtype, class Layout, typename iType>
struct ViewSum<Viewtype, Layout, 1, iType> {

  template<class ExecSpace, class ResultT>
  static void execute(ResultT& result, const Viewtype& v, const ExecSpace space = ExecSpace()) {

    using policy_type = Kokkos::RangePolicy<ExecSpace, Kokkos::IndexType<iType>>;
    using value_type  = typename Viewtype::value_type;

    value_type sum;

    Kokkos::parallel_reduce(
        "ViewSum-1D",
        policy_type(space, 0, v.extent(0)),
        KOKKOS_LAMBDA (
            const iType& i0, 
            value_type& update) {
                update += v(i0);
        },
        sum );
    
    result += sum;
  }
};

template <class Viewtype, class Layout, typename iType>
struct ViewSum<Viewtype, Layout, 2, iType> {

  template<class ExecSpace, class ResultT>
  static void execute(ResultT& result, const Viewtype& v, const ExecSpace space = ExecSpace()) {

    static const Kokkos::Iterate outer_iteration_pattern =
        Kokkos::layout_iterate_type_selector<Layout>::outer_iteration_pattern;
    static const Kokkos::Iterate inner_iteration_pattern =
        Kokkos::layout_iterate_type_selector<Layout>::inner_iteration_pattern;
    using iterate_type =
        Kokkos::Rank<2, outer_iteration_pattern, inner_iteration_pattern>;
    using policy_type =
        Kokkos::MDRangePolicy<ExecSpace, iterate_type, Kokkos::IndexType<iType>>;
    using value_type  = typename Viewtype::value_type;

    value_type sum;

    Kokkos::parallel_reduce(
        "ViewSum-2D",
        policy_type(space, {0, 0}, {v.extent(0), v.extent(1)}),
        KOKKOS_LAMBDA (
            const iType& i0, 
            const iType& i1, 
            value_type& update) {
                update += v(i0, i1);
        },
        sum );
    
    result += sum;
  }
};

template <class Viewtype, class Layout, typename iType>
struct ViewSum<Viewtype, Layout, 3, iType> {

  template<class ExecSpace, class ResultT>
  static void execute(ResultT& result, const Viewtype& v, const ExecSpace space = ExecSpace()) {

    static const Kokkos::Iterate outer_iteration_pattern =
        Kokkos::layout_iterate_type_selector<Layout>::outer_iteration_pattern;
    static const Kokkos::Iterate inner_iteration_pattern =
        Kokkos::layout_iterate_type_selector<Layout>::inner_iteration_pattern;
    using iterate_type =
        Kokkos::Rank<3, outer_iteration_pattern, inner_iteration_pattern>;
    using policy_type =
        Kokkos::MDRangePolicy<ExecSpace, iterate_type, Kokkos::IndexType<iType>>;
    using value_type  = typename Viewtype::value_type;

    value_type sum;

    Kokkos::parallel_reduce(
        "ViewSum-3D",
        policy_type(space, {0, 0}, {v.extent(0), v.extent(1), v.extent(2)}),
        KOKKOS_LAMBDA (
            const iType& i0, 
            const iType& i1,
            const iType& i2, 
            value_type& update) {
                update += v(i0, i1, i2);
        },
        sum );
    
    result += sum;
  }
};

// Parallel add

template <class Viewtype, class Layout, int Rank = Viewtype::rank(), typename iType = int>
struct ViewAdd;

template <class Viewtype, class Layout, typename iType>
struct ViewAdd<Viewtype, Layout, 1, iType> {

  template<class ExecSpace, class ResultT>
  static void execute(const Viewtype& v, ResultT& update, const ExecSpace space = ExecSpace()) {

    using policy_type = Kokkos::RangePolicy<ExecSpace, Kokkos::IndexType<iType>>;

    Kokkos::parallel_for(
        "ViewAdd-1D",
        policy_type(space, 0, v.extent(0)),
        KOKKOS_LAMBDA (
            const iType& i0) {
                v(i0) += update;
        });
  }
};

template <class Viewtype, class Layout, typename iType>
struct ViewAdd<Viewtype, Layout, 2, iType> {

  template<class ExecSpace, class ResultT>
  static void execute(const Viewtype& v, ResultT& update, const ExecSpace space = ExecSpace()) {

    static const Kokkos::Iterate outer_iteration_pattern =
        Kokkos::layout_iterate_type_selector<Layout>::outer_iteration_pattern;
    static const Kokkos::Iterate inner_iteration_pattern =
        Kokkos::layout_iterate_type_selector<Layout>::inner_iteration_pattern;
    using iterate_type =
        Kokkos::Rank<2, outer_iteration_pattern, inner_iteration_pattern>;
    using policy_type =
        Kokkos::MDRangePolicy<ExecSpace, iterate_type, Kokkos::IndexType<iType>>;

    Kokkos::parallel_for(
        "ViewAdd-2D",
        policy_type(space, {0, 0}, {v.extent(0), v.extent(1)}),
        KOKKOS_LAMBDA (
            const iType& i0, 
            const iType& i1) {
                v(i0, i1) += update;
        });
  }
};

template <class Viewtype, class Layout, typename iType>
struct ViewAdd<Viewtype, Layout, 3, iType> {

  template<class ExecSpace, class ResultT>
  static void execute(const Viewtype& v, ResultT& update, const ExecSpace space = ExecSpace()) {

    static const Kokkos::Iterate outer_iteration_pattern =
        Kokkos::layout_iterate_type_selector<Layout>::outer_iteration_pattern;
    static const Kokkos::Iterate inner_iteration_pattern =
        Kokkos::layout_iterate_type_selector<Layout>::inner_iteration_pattern;
    using iterate_type =
        Kokkos::Rank<3, outer_iteration_pattern, inner_iteration_pattern>;
    using policy_type =
        Kokkos::MDRangePolicy<ExecSpace, iterate_type, Kokkos::IndexType<iType>>;

    Kokkos::parallel_for(
        "ViewAdd-3D",
        policy_type(space, {0, 0}, {v.extent(0), v.extent(1), v.extent(2)}),
        KOKKOS_LAMBDA (
            const iType& i0, 
            const iType& i1,
            const iType& i2) {
                v(i0, i1, i2) += update;
        });
  }
};


template <typename ViewtypeA>
void parallel_sum(typename ViewtypeA::value_type &sum, const ViewtypeA A) {
  Kokkos::fence("parallel_sum: pre sum fence");
  if (A.span_is_contiguous()) {

    using ViewTypeFlat = Kokkos::View<
        typename ViewtypeA::value_type*, Kokkos::LayoutRight,
        Kokkos::Device<typename ViewtypeA::execution_space,
                      std::conditional_t<ViewtypeA::rank == 0,
                                          typename ViewtypeA::memory_space,
                                          Kokkos::AnonymousSpace>>,
        Kokkos::MemoryTraits<0>>;

    ViewTypeFlat A_flat(A.data(), A.size());
    ViewSum<ViewTypeFlat, Kokkos::LayoutRight, 1, int>::template execute<typename ViewTypeFlat::execution_space>(sum, A_flat);
  }
  else {
    ViewSum<ViewtypeA, typename ViewtypeA::array_layout, ViewtypeA::rank, int>::template execute<typename ViewtypeA::execution_space>(sum, A);
  }
  Kokkos::fence("parallel_sum: post sum fence");
}

template <class ExecSpace, typename ViewtypeA>
void parallel_sum(const ExecSpace& space, typename ViewtypeA::value_type &sum, const ViewtypeA A) {
  space.fence("parallel_sum: pre sum fence");
  if (A.span_is_contiguous()) {

    using ViewTypeFlat = Kokkos::View<
        typename ViewtypeA::value_type*, Kokkos::LayoutRight,
        Kokkos::Device<typename ViewtypeA::execution_space,
                      std::conditional_t<ViewtypeA::rank == 0,
                                          typename ViewtypeA::memory_space,
                                          Kokkos::AnonymousSpace>>,
        Kokkos::MemoryTraits<0>>;

    ViewTypeFlat A_flat(A.data(), A.size());
    ViewSum<ViewTypeFlat, Kokkos::LayoutRight, 1, int>::template execute<typename ViewTypeFlat::execution_space>(sum, A_flat, space);
  }
  else {
    ViewSum<ViewtypeA, typename ViewtypeA::array_layout, ViewtypeA::rank, int>::template execute<ExecSpace>(sum, A, space);
  }
  space.fence("parallel_sum: post sum fence");
}

template <typename ViewtypeA>
void parallel_sum(ViewtypeA A, const typename ViewtypeA::value_type b) {
  Kokkos::fence("parallel_sum: pre add fence");
  if (A.span_is_contiguous()) {

    using ViewTypeFlat = Kokkos::View<
        typename ViewtypeA::value_type*, Kokkos::LayoutRight,
        Kokkos::Device<typename ViewtypeA::execution_space,
                      std::conditional_t<ViewtypeA::rank == 0,
                                          typename ViewtypeA::memory_space,
                                          Kokkos::AnonymousSpace>>,
        Kokkos::MemoryTraits<0>>;

    ViewTypeFlat A_flat(A.data(), A.size());
    ViewAdd<ViewTypeFlat, Kokkos::LayoutRight, 1, int>::template execute<typename ViewTypeFlat::execution_space>(A_flat, b);
  }
  else {
    ViewAdd<ViewtypeA, typename ViewtypeA::array_layout, ViewtypeA::rank, int>::template execute<typename ViewtypeA::execution_space>(A, b);
  }
  Kokkos::fence("parallel_sum: post add fence");
}

template <class ExecSpace, typename ViewtypeA>
void parallel_sum(const ExecSpace& space, ViewtypeA A, const typename ViewtypeA::value_type b) {
  space.fence("parallel_sum: pre add fence");
  if (A.span_is_contiguous()) {

    using ViewTypeFlat = Kokkos::View<
        typename ViewtypeA::value_type*, Kokkos::LayoutRight,
        Kokkos::Device<typename ViewtypeA::execution_space,
                      std::conditional_t<ViewtypeA::rank == 0,
                                          typename ViewtypeA::memory_space,
                                          Kokkos::AnonymousSpace>>,
        Kokkos::MemoryTraits<0>>;

    ViewTypeFlat A_flat(A.data(), A.size());
    ViewAdd<ViewTypeFlat, Kokkos::LayoutRight, 1, int>::template execute<typename ViewTypeFlat::execution_space>(A_flat, b, space);
  }
  else {
    ViewAdd<ViewtypeA, typename ViewtypeA::array_layout, ViewtypeA::rank, int>::template execute<ExecSpace>(A, b, space);
  }
  space.fence("parallel_sum: post add fence");
}

}