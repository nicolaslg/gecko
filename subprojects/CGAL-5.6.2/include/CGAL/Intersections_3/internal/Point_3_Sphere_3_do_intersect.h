// Copyright (c) 2003  INRIA Sophia-Antipolis (France).
// All rights reserved.
//
// This file is part of CGAL (www.cgal.org)
//
// $URL: https://github.com/CGAL/cgal/blob/v5.6.2/Intersections_3/include/CGAL/Intersections_3/internal/Point_3_Sphere_3_do_intersect.h $
// $Id: Point_3_Sphere_3_do_intersect.h 3a4e230ac78 2022-11-22T12:22:42+01:00 Mael Rouxel-Labbé
// SPDX-License-Identifier: LGPL-3.0-or-later OR LicenseRef-Commercial
//
//
// Author(s)     : Andreas Fabri

#ifndef CGAL_INTERNAL_INTERSECTIONS_3_POINT_3_SPHERE_3_INTERSECTION_H
#define CGAL_INTERNAL_INTERSECTIONS_3_POINT_3_SPHERE_3_INTERSECTION_H

namespace CGAL {
namespace Intersections {
namespace internal {

template <class K>
inline
typename K::Boolean
do_intersect(const typename K::Point_3& pt,
             const typename K::Sphere_3& sphere,
             const K& k)
{
  return k.has_on_boundary_3_object()(sphere, pt);
}

template <class K>
inline
typename K::Boolean
do_intersect(const typename K::Sphere_3& sphere,
             const typename K::Point_3& pt,
             const K& k)
{
  return k.has_on_boundary_3_object()(sphere, pt);
}

} // namespace internal
} // namespace Intersections
} // namespace CGAL

#endif // CGAL_INTERNAL_INTERSECTIONS_3_POINT_3_SPHERE_3_INTERSECTION_H
