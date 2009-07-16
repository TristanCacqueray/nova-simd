//  unary arithmetic simd function fallbacks, float version
//  Copyright (C) 2008, 2009 Tim Blechmann <tim@klingt.org>
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
//
//  implemented as part of nova

#ifndef SIMD_UNARY_ARITHMETIC_FALLBACKS_FLOAT_HPP
#define SIMD_UNARY_ARITHMETIC_FALLBACKS_FLOAT_HPP

#include "simd_unary_arithmetic.hpp"

namespace nova {

template <int n>
inline void abs_vec_simd(float * out, const float * src)
{
    abs_vec_simd<n>(out, src);
}

template <int n>
inline void sgn_vec_simd(float * out, const float * src)
{
    sgn_vec_simd<n>(out, src);
}

template <int n>
inline void square_vec_simd(float * out, const float * src)
{
    square_vec_simd<n>(out, src);
}

template <int n>
inline void cube_vec_simd(float * out, const float * src)
{
    cube_vec_simd<n>(out, src);
}

} /* namespace nova */

#endif /* SIMD_UNARY_ARITHMETIC_FALLBACKS_FLOAT_HPP */
