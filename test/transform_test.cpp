// Copyright 2015, Tobias Hermann and the FunctionalPlus contributors.
// https://github.com/Dobiasd/FunctionalPlus
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <doctest/doctest.h>
#include <fplus/fplus.hpp>

namespace {
    typedef std::vector<int> IntVector;
    typedef std::vector<IntVector> IntVectors;
    IntVector xs = {1,2,2,3,2};
    auto squareLambda = [](auto x) { return x*x; };
    typedef std::array<int, 5> IntArray5;
    IntArray5 xs_array = {{1,2,2,3,2}};
    typedef std::list<int> IntList;
    IntList intList = { 1,2,2,3,2 };
    bool is_even(int value)
    {
        return ( value % 2 == 0 );
    }
}

TEST_CASE("transform_test - replicate_elems")
{
    using namespace fplus;
    REQUIRE_EQ(replicate_elems(2, xs), IntVector({1,1,2,2,2,2,3,3,2,2}));
}

TEST_CASE("transform_test - interleave")
{
    using namespace fplus;
    REQUIRE_EQ(interleave(IntVectors()), IntVector());
    REQUIRE_EQ(interleave(IntVectors({})), IntVector());
    REQUIRE_EQ(interleave(IntVectors({IntVector({})})), IntVector());
    REQUIRE_EQ(interleave(IntVectors({{},{}})), IntVector());
    REQUIRE_EQ(interleave(IntVectors({{1},{}})), IntVector({1}));
    REQUIRE_EQ(interleave(IntVectors({{1,2,3},{4,5},{6,7,8}})), IntVector({1,4,6,2,5,7,3,8}));
    REQUIRE_EQ(interleave(IntVectors({{1,2,3},{4,5},{6,7,8}})), IntVector({1,4,6,2,5,7,3,8}));
}

TEST_CASE("transform_test - transpose")
{
    using namespace fplus;
    REQUIRE_EQ(transpose(IntVectors()), IntVectors());
    REQUIRE_EQ(transpose(IntVectors({})), IntVectors());
    REQUIRE_EQ(transpose(IntVectors({},{})), IntVectors());
    REQUIRE_EQ(transpose(IntVectors({ { 1 }, { 2 } })), IntVectors({ { 1, 2 } }));
    REQUIRE_EQ(transpose(IntVectors({ { 1, 2 } })), IntVectors({ { 1 }, { 2 } }));
    REQUIRE_EQ(transpose(IntVectors({ { 1, 2 }, { 3, 4 } })), IntVectors({ { 1, 3 }, { 2, 4 } }));
    REQUIRE_EQ(transpose(IntVectors({ { 1, 2, 3 }, { 4, 5, 6 } })), IntVectors({ { 1, 4 }, { 2, 5 }, { 3, 6 } }));
    REQUIRE_EQ(transpose(IntVectors({{1,2,3},{4,5,6},{7,8,9}})), IntVectors({{1,4,7},{2,5,8},{3,6,9}}));
    REQUIRE_EQ(transpose(IntVectors({{1,2,3},{4,5},{7,8,9}})), IntVectors({{1,4,7},{2,5,8},{3,9}}));
}

TEST_CASE("transform_test - shuffle")
{
    using namespace fplus;

    const auto shuffled1 = shuffle(std::mt19937::default_seed, xs);
    REQUIRE(is_permutation_of(shuffled1, xs));

    const auto shuffled1_rvalue = shuffle(std::mt19937::default_seed, IntVector({1,2,2,3,2}));
    REQUIRE(is_permutation_of(shuffled1_rvalue, xs));

    const auto shuffled2 = shuffle(std::random_device()(), xs);
    REQUIRE(is_permutation_of(shuffled2, xs));
}

TEST_CASE("transform_test - random_element")
{
    using namespace fplus;

    const auto elem1 = random_element(std::mt19937::default_seed, xs);
    REQUIRE(is_elem_of(elem1, xs));

    const auto elem2 = random_element(std::random_device()(), xs);
    REQUIRE(is_elem_of(elem2, xs));
}

TEST_CASE("transform_test - random_elements")
{
    using namespace fplus;

    const auto check_is_elem_of_xs = bind_2nd_of_2(is_elem_of<IntVector>, xs);

    const auto sampled1 = random_elements(std::mt19937::default_seed, 23, xs);
    REQUIRE_EQ(sampled1.size(), 23);
    REQUIRE(all_by(check_is_elem_of_xs, sampled1));

    const auto sampled2 = random_elements(std::random_device()(), 23, xs);
    REQUIRE_EQ(sampled2.size(), 23);
    REQUIRE(all_by(check_is_elem_of_xs, sampled2));
}

TEST_CASE("transform_test - sample")
{
    using namespace fplus;

    const auto check_is_elem_of_xs = bind_2nd_of_2(is_elem_of<IntVector>, xs);

    const auto sampled1 = sample(std::mt19937::default_seed, 3, xs);
    REQUIRE_EQ(sampled1.size(), 3);
    REQUIRE(all_by(check_is_elem_of_xs, sampled1));

    const auto sampled2 = sample(std::random_device()(), 3, xs);
    REQUIRE_EQ(sampled2.size(), 3);
    REQUIRE(all_by(check_is_elem_of_xs, sampled2));
}

TEST_CASE("transform_test - transform")
{
    using namespace fplus;

    auto intTimes2 = [](int x) -> int { return x*2; };
    auto intTimes3 = [](int x) -> int { return x*3; };

    REQUIRE_EQ(transform([](auto x) { return x*x; }, xs), IntVector({1,4,4,9,4}));
    std::initializer_list<int> initListInts = { 1,2,2,3,2 };
    REQUIRE_EQ(transform(squareLambda, std::vector<int>(initListInts)), IntVector({1,4,4,9,4}));
    REQUIRE_EQ(transform_convert<std::vector<int>>(squareLambda, initListInts), IntVector({1,4,4,9,4}));
    REQUIRE_EQ(transform_convert<std::vector<int>>(squareLambda, xs_array), IntVector({1,4,4,9,4}));
    REQUIRE_EQ(transform(squareLambda, xs), IntVector({1,4,4,9,4}));
    REQUIRE_EQ(transform(squareLambda, intList), IntList({ 1,4,4,9,4 }));
    REQUIRE_EQ(transform_parallelly(squareLambda, intList), IntList({ 1,4,4,9,4 }));
    REQUIRE_EQ(transform_parallelly_n_threads(3, squareLambda, intList), IntList({ 1,4,4,9,4 }));
    REQUIRE_EQ(transform_convert<IntList>(squareLambda, xs), IntList({ 1,4,4,9,4 }));

    REQUIRE_EQ(transform(squareLambda, std::set<int>({1,2,3,-3})), std::set<int>({1,4,9}));
    REQUIRE_EQ(transform_inner(intTimes2, IntVectors({{1,3,4},{1,2}})), IntVectors({{2,6,8},{2,4}}));

    typedef std::array<int, 2> IntArray2;
    typedef std::array<IntArray2, 3> IntArrays2_3;
    IntArrays2_3 int_arrays = {{IntArray2({{1,2}}),IntArray2({{2,3}}),IntArray2({{3,4}})}};
    IntArrays2_3 int_arrays_times_2 = {{IntArray2({{2,4}}),IntArray2({{4,6}}),IntArray2({{6,8}})}};
    REQUIRE_EQ(transform_inner(intTimes2, int_arrays), int_arrays_times_2);

    auto add_size_t_and_int = [](std::size_t a, int b) -> int
    {
        return static_cast<int>(a) + b;
    };
    REQUIRE_EQ(transform_with_idx(add_size_t_and_int, xs), IntVector({1+0,2+1,2+2,3+3,2+4}));

    std::vector<std::function<int(int)>> multiplyFunctions = {intTimes2, intTimes3};
    REQUIRE_EQ(apply_functions(multiplyFunctions, 4), IntVector({8, 12}));

    auto showInt = [](auto x) { return fplus::show(x); };
    std::vector<decltype(showInt)> showIntFuncs = {showInt, showInt};
    REQUIRE_EQ(apply_functions(showIntFuncs, 4), std::vector<std::string>({"4", "4"}));

    REQUIRE_EQ(apply_function_n_times(squareLambda, 3, 2), 256);

    REQUIRE_EQ(transform(squareLambda, xs_array), IntArray5({{1,4,4,9,4}}));
}

TEST_CASE("transform_test - reduce")
{
    using namespace fplus;
    REQUIRE_EQ(reduce_parallelly(std::plus<int>(), 100, xs), 110);
    REQUIRE_EQ(reduce_1_parallelly(std::plus<int>(), xs), 10);
}

TEST_CASE("transform_test - keep_if_parallelly")
{
    const std::vector<int> v = {1, 2, 3, 2, 4, 5};
    auto result = fplus::keep_if_parallelly(is_even, v);
    REQUIRE_EQ(result, std::vector<int>({2, 2, 4}));
}

TEST_CASE("transform_test - transform_reduce")
{
    const std::vector<int> v = {1, 2, 3, 4, 5};
    auto result = fplus::transform_reduce(
        fplus::square<int>, std::plus<int>(), 0, v);
    REQUIRE_EQ(result, 55);
}

TEST_CASE("transform_test - transform_reduce_1")
{
    const std::vector<int> v = {1, 2, 3, 4, 5};
    auto result = fplus::transform_reduce_1(
        fplus::square<int>, std::plus<int>(), v);
    REQUIRE_EQ(result, 55);
}

TEST_CASE("transform_test - transform_reduce_parallelly")
{
    const std::vector<int> v = {1, 2, 3, 4, 5};
    auto result = fplus::transform_reduce_parallelly(
        fplus::square<int>, std::plus<int>(), 0, v);
    REQUIRE_EQ(result, 55);
}

TEST_CASE("transform_test - transform_reduce_1_parallelly")
{
    const std::vector<int> v = {1, 2, 3, 4, 5};
    auto result = fplus::transform_reduce_1_parallelly(
        fplus::square<int>, std::plus<int>(), v);
    REQUIRE_EQ(result, 55);
}

// https://stackoverflow.com/a/21083096/1866775
template <typename T>
struct mallocator {
  using value_type = T;

  mallocator() = default;
  template <class U>
  mallocator(const mallocator<U>&) {}

  T* allocate(std::size_t n) {
    if (n <= std::numeric_limits<std::size_t>::max() / sizeof(T)) {
      if (auto ptr = std::malloc(n * sizeof(T))) {
        return static_cast<T*>(ptr);
      }
    }
    throw std::bad_alloc();
  }
  void deallocate(T* ptr, std::size_t) {
    std::free(ptr);
  }
};

template <typename T, typename U>
inline bool operator == (const mallocator<T>&, const mallocator<U>&) {
  return true;
}

template <typename T, typename U>
inline bool operator != (const mallocator<T>& a, const mallocator<U>& b) {
  return !(a == b);
}

template <typename T>
using special_vector = std::vector<T, mallocator<T>>;

TEST_CASE("transform_test - custom_allocator")
{
    using namespace fplus;
    special_vector<int> xs = {1,2,2,3,2};
    special_vector<std::string> ys = fplus::transform([](auto x) { return std::to_string(x); }, xs);
}
