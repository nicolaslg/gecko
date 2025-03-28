// Copyright (c) 2006  GeometryFactory (France). All rights reserved.
//
// This file is part of CGAL (www.cgal.org).
//
// $URL: https://github.com/CGAL/cgal/blob/v5.6.2/Surface_mesh_simplification/include/CGAL/Surface_mesh_simplification/Policies/Edge_collapse/Edge_count_ratio_stop_predicate.h $
// $Id: Edge_count_ratio_stop_predicate.h 837573119d1 2022-11-02T13:55:32+01:00 Mael Rouxel-Labbé
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-Commercial
//
// Author(s)     : Fernando Cacciola <fernando.cacciola@geometryfactory.com>
//
#ifndef CGAL_SURFACE_MESH_SIMPLIFICATION_POLICIES_EDGE_COLLAPSE_EDGE_COUNT_RATIO_STOP_PREDICATE_H
#define CGAL_SURFACE_MESH_SIMPLIFICATION_POLICIES_EDGE_COLLAPSE_EDGE_COUNT_RATIO_STOP_PREDICATE_H

#include <CGAL/license/Surface_mesh_simplification.h>

#include <CGAL/Surface_mesh_simplification/internal/Common.h>
#include <CGAL/Surface_mesh_simplification/Policies/Edge_collapse/Edge_profile.h>

namespace CGAL {
namespace Surface_mesh_simplification {

// Stops when the ratio of initial to current number of edges is below some value.
template<class TM_>
class Edge_count_ratio_stop_predicate
{
public:
  typedef TM_                                                 TM;
  typedef typename boost::graph_traits<TM>::edges_size_type   size_type;

  Edge_count_ratio_stop_predicate(const double ratio)
    : m_ratio(ratio)
  {
    CGAL_warning(0. < ratio && ratio <= 1.);
  }

  template <typename F, typename Profile>
  bool operator()(const F& /*current_cost*/,
                  const Profile& /*profile*/,
                  size_type initial_edge_count,
                  size_type current_edge_count) const
  {
    return (static_cast<double>(current_edge_count) / static_cast<double>(initial_edge_count)) < m_ratio;
  }

private:
  double m_ratio;
};

} // namespace Surface_mesh_simplification
} // namespace CGAL

#endif // CGAL_SURFACE_MESH_SIMPLIFICATION_POLICIES_EDGE_COLLAPSE_EDGE_COUNT_RATIO_STOP_PREDICATE_H
