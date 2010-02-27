//  sse vector class
//
//  Copyright (C) 2010 Tim Blechmann
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

#ifndef VEC_SSE_HPP
#define VEC_SSE_HPP

#include <xmmintrin.h>

namespace nova
{

template <>
struct vec<float>
{


    vec(__m128 const & arg):
        data_(arg)
    {}

public:
    static const int size = 4;
    static const int objects_per_cacheline = 64/sizeof(float);

    /* @{ */
    /** constructors */
    vec(void)
    {}

    explicit vec(float f)
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
        data_ = _mm_loadu_ps(data);
    }

    void load_aligned(const float * data)
    {
        data_ = _mm_load_ps(data);
    }

    void store(float * dest) const
    {
        _mm_storeu_ps(dest, data_);
    }

    void store_aligned(float * dest) const
    {
        _mm_store_ps(dest, data_);
    }

    void store_aligned_stream(float * dest) const
    {
        _mm_stream_ps(dest, data_);
    }

    void clear(void)
    {
        data_ = _mm_setzero_ps();
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
        data_ = _mm_set_ps1(value);
    }

    float set_slope(float start, float slope)
    {
        float v1 = start + slope;
        float v2 = start + slope + slope;
        float v3 = start + slope + slope + slope;
        data_ = _mm_set_ps(v3, v2, v1, start);
        return slope + slope + slope + slope;
    }

    float set_exp(float start, float curve)
    {
        float v1 = start * curve;
        float v2 = v1 * curve;
        float v3 = v2 * curve;
        data_ = _mm_set_ps(v3, v2, v1, start);
        return v3 * curve;
    }

    float get (std::size_t index)
    {
        if (index == 0)
            return _mm_cvtss_f32(data_);

        __m128 ret = _mm_shuffle_ps(data_, data_, _MM_SHUFFLE(index, index, index, index));
        return _mm_cvtss_f32(ret);
    }
    /* @} */

    /* @{ */
    /** arithmetic operators */
#define OPERATOR_ASSIGNMENT(op, opcode) \
    vec & operator op(vec const & rhs) \
    { \
        data_ = opcode(data_, rhs.data_);\
        return *this;\
    }

    OPERATOR_ASSIGNMENT(+=, _mm_add_ps)
    OPERATOR_ASSIGNMENT(-=, _mm_sub_ps)
    OPERATOR_ASSIGNMENT(*=, _mm_mul_ps)
    OPERATOR_ASSIGNMENT(/=, _mm_div_ps)

#define ARITHMETIC_OPERATOR(op, opcode) \
    vec operator op(vec const & rhs) \
    { \
        return opcode(data_, rhs.data_); \
    }

    ARITHMETIC_OPERATOR(+, _mm_add_ps)
    ARITHMETIC_OPERATOR(-, _mm_sub_ps)
    ARITHMETIC_OPERATOR(*, _mm_mul_ps)
    ARITHMETIC_OPERATOR(/, _mm_div_ps)
    /* @} */

private:
    typedef union
    {
        float f[4];
        __m128 m;
    } cast_union;

    __m128 data_;
};

} /* namespace nova */

#undef OPERATOR_ASSIGNMENT
#undef ARITHMETIC_OPERATOR

#endif /* VEC_SSE_HPP */
