//  altivec vector class
//
//  Copyright (C) 2011 Tim Blechmann
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; see the file COPYING.  If not, write to
//  the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
//  Boston, MA 02111-1307, USA.

#ifndef VEC_ALTIVEC_HPP
#define VEC_ALTIVEC_HPP

#include <altivec.h>

#include "detail/vec_math.hpp"
#include "vec_int_altivec.hpp"
#include "detail/math.hpp"

#if defined(__GNUC__) && defined(NDEBUG)
#define always_inline inline  __attribute__((always_inline))
#else
#define always_inline inline
#endif

namespace nova
{

template <>
struct vec<float>
{

    typedef vector float internal_vector_type;
    typedef float float_type;

private:
    static internal_vector_type set_vector(float f0, float f1, float f2, float f3)
    {
#ifdef __GNUC__
        return (internal_vector_type){f0, f1, f2, f3};
#else
#error compiler not supported
#endif

    }

    static internal_vector_type set_vector(float f)
    {
        return set_vector(f, f, f, f);
    }

public:

    static inline internal_vector_type gen_sign_mask(void)
    {
        return set_bitmask(0x80000000);
    }

    static inline internal_vector_type gen_abs_mask(void)
    {
        return set_bitmask(0x7fffffff);
    }

    static inline internal_vector_type gen_one(void)
    {
        return set_vector(1.f);
    }

    static inline internal_vector_type gen_05(void)
    {
        return set_vector(0.5f);
    }

    static inline internal_vector_type set_bitmask(unsigned int mask)
    {
        union {
            unsigned int i;
            float f;
        } u;
        u.i = mask;
        return set_vector(u.f);
    }

    static inline internal_vector_type gen_exp_mask(void)
    {
        return set_bitmask(0x7F800000);
    }

    static inline internal_vector_type gen_exp_mask_1(void)
    {
        return set_bitmask(0x3F000000);
    }

    static inline internal_vector_type gen_ones(void)
    {
        return set_bitmask(0xFFFFFFFF);
    }

    static inline internal_vector_type gen_zero(void)
    {
        return set_bitmask(0);
    }

    vec(internal_vector_type const & arg):
        data_(arg)
    {}

public:
    static const int size = 4;
    static const int objects_per_cacheline = 64/sizeof(float);

    /* @{ */
    /** constructors */
    vec(void)
    {}

    vec(float f)
    {
        set_vec(f);
    }

    vec(vec const & rhs)
    {
        data_ = rhs.data_;
    }
    /* @} */

    /* @{ */
    /** io */
    void load(const float * data)
    {
        data_ = set_vector(data[0], data[1], data[2], data[3]);
    }

    void load_aligned(const float * data)
    {
        data_ = vec_ld(0, data);
    }

    void load_first(const float * data)
    {
        data_ = vec_lde(0, data);
    }

    void store(float * dest) const
    {
        vec_st(data_, 0, dest);
    }

    void store_aligned(float * dest) const
    {
        vec_st(data_, 0, dest);
    }

    void store_aligned_stream(float * dest) const
    {
        vec_st(data_, 0, dest);
    }

    void clear(void)
    {
        data_ = gen_zero();
    }

    operator internal_vector_type (void) const
    {
        return data_;
    }

    /* @} */

    /* @{ */
    /** element access */
    void set (std::size_t index, float value)
    {
        float * data = (float*)&data_;
        data[index] = value;
    }

    void set_vec (float value)
    {
        data_ = set_vector(value, value, value, value);
    }

    float set_slope(float start, float slope)
    {
        float v1 = start + slope;
        float v2 = start + slope + slope;
        float v3 = start + slope + slope + slope;
        data_ = set_vector(start, v1, v2, v3);
        return slope + slope + slope + slope;
    }

    float set_exp(float start, float curve)
    {
        float v1 = start * curve;
        float v2 = v1 * curve;
        float v3 = v2 * curve;
        data_ = set_vector(start, v1, v2, v3);
        return v3 * curve;
    }

    float get (std::size_t index) const
    {
        float * data = (float*)&data_;
        return data[index];
    }
    /* @} */

    /* @{ */

private:
    static internal_vector_type vec_mul(internal_vector_type const & lhs, internal_vector_type const & rhs)
    {
        return vec_madd(lhs, rhs, gen_zero());
    }

    static internal_vector_type vec_reciprocal(internal_vector_type const & arg)
    {
        // adapted from http://developer.apple.com/hardwaredrivers/ve/algorithms.html

        // Get the reciprocal estimate
        vector float estimate = vec_re(arg);

        // One round of Newton-Raphson refinement
        return vec_madd(vec_nmsub(estimate, arg, gen_one()), estimate, estimate);
    }

    static internal_vector_type vec_div(internal_vector_type const & lhs, internal_vector_type const & rhs)
    {
        return vec_mul(lhs, vec_reciprocal(rhs));
    }

public:
    /** arithmetic operators */
#define OPERATOR_ASSIGNMENT(op, opcode) \
    vec & operator op(vec const & rhs) \
    { \
        data_ = opcode(data_, rhs.data_);\
        return *this;\
    }

    OPERATOR_ASSIGNMENT(+=, vec_add)
    OPERATOR_ASSIGNMENT(-=, vec_sub)
    OPERATOR_ASSIGNMENT(*=, vec_mul)
    OPERATOR_ASSIGNMENT(/=, vec_div)

#define ARITHMETIC_OPERATOR(op, opcode) \
    vec operator op(vec const & rhs) const \
    { \
        return opcode(data_, rhs.data_); \
    } \
 \
    friend vec operator op(vec const & lhs, float f)  \
    { \
        return opcode(lhs.data_, vec(f).data_); \
    } \
    \
    friend vec operator op(float f, vec const & rhs)  \
    { \
        return opcode(vec(f).data_, rhs.data_); \
    }

    ARITHMETIC_OPERATOR(+, vec_add)
    ARITHMETIC_OPERATOR(-, vec_sub)
    ARITHMETIC_OPERATOR(*, vec_mul)
    ARITHMETIC_OPERATOR(/, vec_div)

    friend vec operator -(const vec & arg)
    {
        return vec_xor(arg.data_, gen_sign_mask());
    }

    friend vec reciprocal(const vec & arg)
    {
        return vec_reciprocal(arg.data_);
    }

private:
    static internal_vector_type vec_not(internal_vector_type const & arg)
    {
        return vec_nor(arg, arg);
    }

    static internal_vector_type vec_cmpneq(internal_vector_type const & lhs, internal_vector_type const & rhs)
    {
        internal_vector_type equal = (internal_vector_type)vec_cmpeq(lhs, rhs);
        return vec_not(equal);
    }

public:

#define RELATIONAL_OPERATOR(op, opcode) \
    vec operator op(vec const & rhs) const \
    { \
        const internal_vector_type one = gen_one(); \
        return vec_and(opcode(data_, rhs.data_), one); \
    }

    RELATIONAL_OPERATOR(<, vec_cmplt)
    RELATIONAL_OPERATOR(<=, vec_cmple)
    RELATIONAL_OPERATOR(>, vec_cmpgt)
    RELATIONAL_OPERATOR(>=, vec_cmpge)
    RELATIONAL_OPERATOR(==, vec_cmpeq)
    RELATIONAL_OPERATOR(!=, vec_cmpneq)

#undef RELATIONAL_OPERATOR

    /* @{ */
#define BITWISE_OPERATOR(op, opcode) \
    vec operator op(vec const & rhs) const \
    { \
        return opcode(data_, rhs.data_); \
    }

    BITWISE_OPERATOR(&, vec_and)
    BITWISE_OPERATOR(|, vec_or)
    BITWISE_OPERATOR(^, vec_xor)

    friend inline vec andnot(vec const & lhs, vec const & rhs)
    {
        return vec_andc(lhs.data_, rhs.data_);
    }

#undef BITWISE_OPERATOR

#define RELATIONAL_MASK_OPERATOR(op, opcode) \
    friend vec mask_##op(vec const & lhs, vec const & rhs) \
    { \
        return internal_vector_type(opcode(lhs.data_, rhs.data_)); \
    }

    RELATIONAL_MASK_OPERATOR(lt, vec_cmplt)
    RELATIONAL_MASK_OPERATOR(le, vec_cmple)
    RELATIONAL_MASK_OPERATOR(gt, vec_cmpgt)
    RELATIONAL_MASK_OPERATOR(ge, vec_cmpge)
    RELATIONAL_MASK_OPERATOR(eq, vec_cmpeq)
    RELATIONAL_MASK_OPERATOR(neq, vec_cmpneq)

#undef RELATIONAL_MASK_OPERATOR

    friend inline vec select(vec lhs, vec rhs, vec bitmask)
    {
        return vec_sel(lhs.data_, rhs.data_, (vector unsigned int)bitmask.data_);
    }

    /* @} */

    /* @{ */
    /** unary functions */
    friend inline vec abs(vec const & arg)
    {
        return vec_abs(arg.data_);
    }

    friend always_inline vec sign(vec const & arg)
    {
/*        vec ret;
        for (int i = 0; i != size; ++i)
            ret.set(i, detail::sign(arg.get(i)));
        return ret;*/

        return detail::vec_sign(arg);
    }

    friend inline vec square(vec const & arg)
    {
        return vec_mul(arg.data_, arg.data_);
    }

private:
    static internal_vector_type vec_rsqrt(internal_vector_type const & arg)
    {
        // adapted from http://developer.apple.com/hardwaredrivers/ve/algorithms.html

        //Get the square root reciprocal estimate
        vector float zero =    gen_zero();
        vector float oneHalf = gen_05();
        vector float one =     gen_one();
        vector float estimate = vec_rsqrte(arg);

        //One round of Newton-Raphson refinement
        vector float estimateSquared = vec_madd(estimate, estimate, zero);
        vector float halfEstimate = vec_madd(estimate, oneHalf, zero);
        return vec_madd(vec_nmsub(arg, estimateSquared, one), halfEstimate, estimate);
    }

    static internal_vector_type vec_sqrt(internal_vector_type const & arg)
    {
        // adapted from http://developer.apple.com/hardwaredrivers/ve/algorithms.html
        return vec_mul(arg, vec_rsqrt(arg));
    }

public:
    friend inline vec sqrt(vec const & arg)
    {
        return vec_sqrt(arg.data_);
    }

    friend inline vec cube(vec const & arg)
    {
        return vec_mul(arg.data_, vec_mul(arg.data_, arg.data_));
    }
    /* @} */

    /* @{ */
    /** binary functions */
    friend inline vec max_(vec const & lhs, vec const & rhs)
    {
        return vec_max(lhs.data_, rhs.data_);
    }

    friend inline vec min_(vec const & lhs, vec const & rhs)
    {
        return vec_min(lhs.data_, rhs.data_);
    }
    /* @} */

    /* @{ */
    /** rounding functions */

    friend inline vec round(vec const & arg)
    {
        return detail::vec_round_float(arg);
        // return vec_round(arg.data_); testsuite fails: seems to round differently than we do?
    }

    friend inline vec frac(vec const & arg)
    {
        return arg - floor(arg);
    }

    friend inline vec floor(vec const & arg)
    {
        return vec_floor(arg.data_);
    }

    friend inline vec ceil(vec const & arg)
    {
        return vec_ceil(arg.data_);
    }
    /* @} */


    /* @{ */
    /** mathematical functions */

#if 0
    typedef nova::detail::int_vec_altivec int_vec;

    friend inline vec exp(vec const & arg)
    {
        return detail::vec_exp_float(arg);
    }

    friend inline vec log(vec const & arg)
    {
        return detail::vec_log_float(arg);
    }

    friend inline vec pow(vec const & arg1, vec const & arg2)
    {
        return detail::vec_pow(arg1, arg2);
    }

    friend inline vec sin(vec const & arg)
    {
        return detail::vec_sin_float(arg);
    }

    friend inline vec cos(vec const & arg)
    {
        return detail::vec_cos_float(arg);
    }

    friend inline vec tan(vec const & arg)
    {
        return detail::vec_tan_float(arg);
    }

    friend inline vec asin(vec const & arg)
    {
        return detail::vec_asin_float(arg);
    }

    friend inline vec acos(vec const & arg)
    {
        return detail::vec_acos_float(arg);
    }

    friend inline vec atan(vec const & arg)
    {
        return detail::vec_atan_float(arg);
    }

    friend inline vec tanh(vec const & arg)
    {
        return detail::vec_tanh_float(arg);
    }
#else

#define APPLY_UNARY_FALLBACK(NAME, FUNCTION)        \
    friend inline vec NAME(vec const & arg)         \
    {                                               \
        vec ret;                                    \
        for (int i = 0; i != 4; ++i)                \
            ret.set(i, FUNCTION(arg.get(i)));       \
        return ret;                                 \
    }

    APPLY_UNARY_FALLBACK(exp, detail::exp)
    APPLY_UNARY_FALLBACK(log, detail::log)

    friend inline vec pow(vec const & arg1, vec const & arg2)
    {
        vec ret;
        for (int i = 0; i != 4; ++i)
            ret.set(i, pow(arg1.get(i), arg2.get(i)));
        return ret;
    }

    APPLY_UNARY_FALLBACK(sin, detail::sin)
    APPLY_UNARY_FALLBACK(cos, detail::cos)
    APPLY_UNARY_FALLBACK(tan, detail::tan)
    APPLY_UNARY_FALLBACK(asin, detail::asin)
    APPLY_UNARY_FALLBACK(acos, detail::acos)
    APPLY_UNARY_FALLBACK(atan, detail::atan)

    APPLY_UNARY_FALLBACK(tanh, detail::tanh)

#undef APPLY_UNARY_FALLBACK
#endif

    friend inline vec signed_pow(vec const & lhs, vec const & rhs)
    {
        return detail::vec_signed_pow(lhs, rhs);
    }

    friend inline vec signed_sqrt(vec const & arg)
    {
        vec ret;
        for (int i = 0; i != size; ++i)
            ret.set(i, detail::signed_sqrt(arg.get(i)));
        return ret;

        // FIXME: this seems to be broken
        // return detail::vec_signed_sqrt(arg);
    }

    friend inline vec log2(vec const & arg)
    {
        return detail::vec_log2(arg);
    }

    friend inline vec log10(vec const & arg)
    {
        return detail::vec_log10(arg);
    }
    /* @} */

    /* @{ */

    inline float horizontal_min(void) const
    {
        float * data = (float*)&data_;
        return *std::min_element(data, data + size);
    }

    inline float horizontal_max(void) const
    {
        float * data = (float*)&data_;
        return *std::max_element(data, data + size);
    }

    inline float horizontal_sum(void) const
    {
        float * data = (float*)&data_;
        float ret = 0;
        for (int i = 0; i != size; ++i)
            ret += data[i];
        return ret;
    }
    /* @} */

    /* @{ */
#if 0
    vec (int_vec const & rhs):
        data_((internal_vector_type)rhs.data_)
    {}

    int_vec truncate_to_int(void) const
    {
        return int_vec(vec_ctu(data_, 0));
    }
#endif
    /* @} */

private:
    typedef union
    {
        float f[4];
        internal_vector_type m;
    } cast_union;

    internal_vector_type data_;
};

} /* namespace nova */

#undef always_inline

#endif /* VEC_ALTIVEC_HPP */
