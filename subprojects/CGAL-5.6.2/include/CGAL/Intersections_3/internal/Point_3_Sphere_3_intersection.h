// Copyright (c) 2003  INRIA Sophia-Antipolis (France).
// All rights reserved.
//
// This file is part of CGAL (www.cgal.org)
//
// $URL: https://github.com/CGAL/cgal/blob/v5.6.2/Intersections_3/include/CGAL/Intersections_3/internal/Point_3_Sphere_3_intersection.h $
// $Id: Point_3_Sphere_3_intersection.h c2d1adfb69b 2021-06-23T17:34:48+02:00 Mael Rouxel-Labbé
// SPDX-License-Identifier: LGPL-3.0-or-later OR LicenseRef-Commercial
//
//
// Author(s)     : Andreas Fabri

#ifndef CGAL_POINT_3_SPHERE_3_INTERSECTION_H
#define CGAL_POINT_3_SPHERE_3_INTERSECTION_H

#include <CGAL/Intersection_traits_3.h>
#include <CGAL/Intersections_3/internal/Point_3_Sphere_3_do_intersect.h>

namespace CGAL {
namespace Intersections {
namespace internal {

template <class K>
typename CGAL::Intersection_traits<K, typename K::Point_3, typename K::Sphere_3>::result_type
intersection(const typename K::Point_3& pt,
             const typename K::Sphere_3& sphere,
             const K& k)
{
  if(do_intersect(pt, sphere, k))
    return intersection_return<typename K::Intersect_3, typename K::Point_3, typename K::Sphere_3>(pt);

  return intersection_return<typename K::Intersect_3, typename K::Point_3, typename K::Sphere_3>();
}

template <class K>
typename CGAL::Intersection_traits<K, typename K::Sphere_3, typename K::Point_3>::result_type
intersection(const typename K::Sphere_3& sphere,
             const typename K::Point_3& pt,
             const K& k)
{
  return internal::intersection(pt, sphere, k);
}

} // namespace internal
} // namespace Intersections
} // namespace CGAL

#endif // CGAL_POINT_3_SPHERE_3_INTERSECTION_H
