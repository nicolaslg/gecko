// Copyright (c) 2000
// Utrecht University (The Netherlands),
// ETH Zurich (Switzerland),
// INRIA Sophia-Antipolis (France),
// Max-Planck-Institute Saarbruecken (Germany),
// and Tel-Aviv University (Israel).  All rights reserved.
//
// This file is part of CGAL (www.cgal.org)
//
// $URL: https://github.com/CGAL/cgal/blob/v5.6.2/Intersections_2/include/CGAL/Intersections_2/Iso_rectangle_2_Point_2.h $
// $Id: Iso_rectangle_2_Point_2.h 8ba0b41f510 2022-11-22T12:35:10+01:00 Mael Rouxel-Labbé
// SPDX-License-Identifier: LGPL-3.0-or-later OR LicenseRef-Commercial
//
//
// Author(s)     : Geert-Jan Giezeman


#ifndef CGAL_INTERSECTIONS_2_POINT_2_ISO_RECTANGLE_2_H
#define CGAL_INTERSECTIONS_2_POINT_2_ISO_RECTANGLE_2_H

#include <CGAL/Iso_rectangle_2.h>
#include <CGAL/Point_2.h>
#include <CGAL/Intersection_traits_2.h>

namespace CGAL {

namespace Intersections {

namespace internal {

template <class K>
inline
typename K::Boolean
do_intersect(const typename K::Point_2& pt,
             const typename K::Iso_rectangle_2& iso,
             const K&)
{
  return !iso.has_on_unbounded_side(pt);
}

template <class K>
inline
typename K::Boolean
do_intersect(const typename K::Iso_rectangle_2& iso,
             const typename K::Point_2& pt,
             const K&)
{
  return !iso.has_on_unbounded_side(pt);
}

template <class K>
typename CGAL::Intersection_traits
<K, typename K::Point_2, typename K::Iso_rectangle_2>::result_type
intersection(const typename K::Point_2 &pt,
             const typename K::Iso_rectangle_2 &iso,
             const K& k)
{
  if (internal::do_intersect(pt,iso,k)) {
    return intersection_return<typename K::Intersect_2, typename K::Point_2, typename K::Iso_rectangle_2>(pt);
  }
  return intersection_return<typename K::Intersect_2, typename K::Point_2, typename K::Iso_rectangle_2>();
}

template <class K>
typename CGAL::Intersection_traits
<K, typename K::Point_2, typename K::Iso_rectangle_2>::result_type
intersection(const typename K::Iso_rectangle_2 &iso,
             const typename K::Point_2 &pt,
             const K& k)
{
  return internal::intersection(pt, iso, k);
}

} // namespace internal
} // namespace Intersections

CGAL_INTERSECTION_FUNCTION(Point_2, Iso_rectangle_2, 2)
CGAL_DO_INTERSECT_FUNCTION(Point_2, Iso_rectangle_2, 2)

} // namespace CGAL

#endif // CGAL_INTERSECTIONS_2_POINT_2_ISO_RECTANGLE_2_H
