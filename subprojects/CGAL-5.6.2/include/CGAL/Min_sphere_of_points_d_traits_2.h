// Copyright (c) 2010  ETH Zurich (Switzerland).
// All rights reserved.
//
// This file is part of CGAL (www.cgal.org).
//
// $URL: https://github.com/CGAL/cgal/blob/v5.6.2/Bounding_volumes/include/CGAL/Min_sphere_of_points_d_traits_2.h $
// $Id: Min_sphere_of_points_d_traits_2.h 07793738355 2020-03-26T13:31:46+01:00 Sébastien Loriot
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-Commercial
//
//
// Author(s)     : Kaspar Fischer, Andreas Fabri

#ifndef CGAL_MIN_SPHERE_OF_POINTS_D_TRAITS_2_H
#define CGAL_MIN_SPHERE_OF_POINTS_D_TRAITS_2_H

#include <CGAL/license/Bounding_volumes.h>


#include <CGAL/tags.h>

namespace CGAL {

 struct Farthest_first_heuristic;

  template<typename K_,                      // kernel
    typename FT_,                            // number type
    typename UseSqrt_ = Tag_false,           // whether to use square-roots
    typename Algorithm_ = Farthest_first_heuristic> // algorithm to use
  class Min_sphere_of_points_d_traits_2 {
  public: // types:
    typedef FT_ FT;
    typedef FT_ Radius;
    typedef typename K_::Point_2 Point;
    typedef Point Sphere;
    typedef typename K_::Cartesian_const_iterator_2 Cartesian_const_iterator;
    typedef UseSqrt_ Use_square_roots;
    typedef Algorithm_ Algorithm;

  public: // constants:
    static const int D = 2;                  // dimension

  public: // accessors:
    static inline const FT radius(const Sphere&) {
      return 0;
    }

    static inline Cartesian_const_iterator
      center_cartesian_begin(const Sphere& s) {
      return s.cartesian_begin();
    }
  };

} // namespace CGAL

#endif // CGAL_MIN_SPHERE_OF_POINTS_D_TRAITS_2_H
