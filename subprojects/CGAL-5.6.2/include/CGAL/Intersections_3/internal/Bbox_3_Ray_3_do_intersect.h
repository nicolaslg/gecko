// Copyright (c) 2008 ETH Zurich (Switzerland)
// Copyright (c) 2008-2009 INRIA Sophia-Antipolis (France)
// All rights reserved.
//
// This file is part of CGAL (www.cgal.org)
//
// $URL: https://github.com/CGAL/cgal/blob/v5.6.2/Intersections_3/include/CGAL/Intersections_3/internal/Bbox_3_Ray_3_do_intersect.h $
// $Id: Bbox_3_Ray_3_do_intersect.h e8ec440859c 2022-12-05T14:51:31+01:00 Laurent Rineau
// SPDX-License-Identifier: LGPL-3.0-or-later OR LicenseRef-Commercial
//
//
// Author(s)     : Camille Wormser, Jane Tournois, Pierre Alliez, Stephane Tayeb

#ifndef CGAL_INTERNAL_INTERSECTIONS_3_BBOX_3_RAY_3_DO_INTERSECT_H
#define CGAL_INTERNAL_INTERSECTIONS_3_BBOX_3_RAY_3_DO_INTERSECT_H

// inspired from https://people.csail.mit.edu/amy/papers/box-jgt.pdf

#include <CGAL/Intersections_3/internal/Bbox_3_Segment_3_do_intersect.h>
// for CGAL::internal::do_intersect_bbox_segment_aux

#include <CGAL/Bbox_3.h>

namespace CGAL {
namespace Intersections {
namespace internal {

template <class K>
typename K::Boolean
do_intersect(const typename K::Ray_3& ray,
             const CGAL::Bbox_3& bbox,
             const K&)
{
  typedef typename K::FT FT;
  typedef typename K::Point_3 Point_3;

  const Point_3& source = ray.source();
  const Point_3& point_on_ray = ray.second_point();

  return do_intersect_bbox_segment_aux
    <FT,
     true,  // bounded at t=0
     false, // not bounded at t=1
     false> // do not use static filters
    (
     source.x(), source.y(), source.z(),
     point_on_ray.x(), point_on_ray.y(), point_on_ray.z(),
     bbox
     );
}

template <class K>
typename K::Boolean
do_intersect(const CGAL::Bbox_3& bbox,
             const typename K::Ray_3& ray,
             const K& k)
{
  return do_intersect(ray, bbox, k);
}

} // namespace internal
} // namespace Intersections
} // namespace CGAL

#endif // CGAL_INTERNAL_INTERSECTIONS_3_BBOX_3_RAY_3_DO_INTERSECT_H
