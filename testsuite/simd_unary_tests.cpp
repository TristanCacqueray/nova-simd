#include <iostream>
#define BOOST_TEST_MAIN
#include <boost/test/included/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>

#include <cmath>

#include "../benchmarks/cache_aligned_array.hpp"
#include "../simd_unary_arithmetic.hpp"

using namespace nova;
using namespace std;

static const int size = 64;

template <typename float_type>
void run_sign_test(void)
{
    aligned_array<float_type, size> in;
    aligned_array<float_type, size> out;
    in[0] = -2;
    in[1] = in[2] = 0;
    in[3] = 2;

    nova::sgn_vec_simd(out.begin(), in.begin(), size);

    BOOST_REQUIRE_EQUAL( out[0], -1 );
    BOOST_REQUIRE_EQUAL( out[1], 0 );
    BOOST_REQUIRE_EQUAL( out[2], 0 );
    BOOST_REQUIRE_EQUAL( out[3], 1 );

}

BOOST_AUTO_TEST_CASE( sign_test )
{
    run_sign_test<float>();
    run_sign_test<double>();
}

template <typename float_type>
void run_abs_test(void)
{
    aligned_array<float_type, size> in;
    aligned_array<float_type, size> out;
    in[0] = -2;
    in[1] = in[2] = 0;
    in[3] = 2;

    nova::abs_vec_simd(out.begin(), in.begin(), size);

    BOOST_REQUIRE_EQUAL( out[0], 2 );
    BOOST_REQUIRE_EQUAL( out[1], 0 );
    BOOST_REQUIRE_EQUAL( out[2], 0 );
    BOOST_REQUIRE_EQUAL( out[3], 2 );

}

BOOST_AUTO_TEST_CASE( abs_test )
{
    run_abs_test<float>();
    run_abs_test<double>();
}

template <typename float_type>
void run_square_test(void)
{
    aligned_array<float_type, size> in;
    aligned_array<float_type, size> out;
    in[0] = -2;
    in[1] = in[2] = 0;
    in[3] = 2;

    nova::square_vec_simd(out.begin(), in.begin(), size);

    BOOST_REQUIRE_EQUAL( out[0], 4 );
    BOOST_REQUIRE_EQUAL( out[1], 0 );
    BOOST_REQUIRE_EQUAL( out[2], 0 );
    BOOST_REQUIRE_EQUAL( out[3], 4 );

}

BOOST_AUTO_TEST_CASE( square_test )
{
    run_square_test<float>();
    run_square_test<double>();
}

template <typename float_type>
void run_cube_test(void)
{
    aligned_array<float_type, size> in;
    aligned_array<float_type, size> out;
    in[0] = -2;
    in[1] = in[2] = 0;
    in[3] = 2;

    nova::cube_vec_simd(out.begin(), in.begin(), size);

    BOOST_REQUIRE_EQUAL( out[0], -8 );
    BOOST_REQUIRE_EQUAL( out[1], 0 );
    BOOST_REQUIRE_EQUAL( out[2], 0 );
    BOOST_REQUIRE_EQUAL( out[3], 8 );

}

BOOST_AUTO_TEST_CASE( cube_test )
{
    run_cube_test<float>();
    run_square_test<double>();
}
