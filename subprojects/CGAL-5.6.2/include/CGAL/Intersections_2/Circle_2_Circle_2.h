// Copyright (c) 2000
// Utrecht University (The Netherlands),
// ETH Zurich (Switzerland),
// INRIA Sophia-Antipolis (France),
// Max-Planck-Institute Saarbruecken (Germany),
// and Tel-Aviv University (Israel).  All rights reserved.
//
// This file is part of CGAL (www.cgal.org)
//
// $URL: https://github.com/CGAL/cgal/blob/v5.6.2/Intersections_2/include/CGAL/Intersections_2/Circle_2_Circle_2.h $
// $Id: Circle_2_Circle_2.h 3a4e230ac78 2022-11-22T12:22:42+01:00 Mael Rouxel-Labbé
// SPDX-License-Identifier: LGPL-3.0-or-later OR LicenseRef-Commercial
//
//
// Author(s)     : Geert-Jan Giezeman

#ifndef CGAL_INTERSECTIONS_2_CIRCLE_2_CIRCLE_2_H
#define CGAL_INTERSECTIONS_2_CIRCLE_2_CIRCLE_2_H

#include <CGAL/Distance_2/Point_2_Point_2.h>
#include <CGAL/Intersection_traits_2.h>

#include <CGAL/Circle_2.h>

namespace CGAL {
namespace Intersections {
namespace internal {

template <class K>
typename K::Boolean do_intersect(const typename K::Circle_2& circ1,
                                 const typename K::Circle_2& circ2,
                                 const K&)
{
  typedef typename K::FT FT;

  FT sr1 = circ1.squared_radius();
  FT sr2 = circ2.squared_radius();
  FT squared_dist = squared_distance(circ1.center(), circ2.center());
  FT temp = sr1+sr2-squared_dist;

  return !(FT(4)*sr1*sr2 < temp*temp);
}

} // namespace internal
} // namespace Intersections

CGAL_DO_INTERSECT_FUNCTION_SELF(Circle_2, 2)

} // namespace CGAL

#endif
