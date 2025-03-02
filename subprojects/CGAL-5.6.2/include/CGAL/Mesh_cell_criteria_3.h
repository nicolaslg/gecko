// Copyright (c) 2004-2009  INRIA Sophia-Antipolis (France).
// All rights reserved.
//
// This file is part of CGAL (www.cgal.org).
//
// $URL: https://github.com/CGAL/cgal/blob/v5.6.2/Mesh_3/include/CGAL/Mesh_cell_criteria_3.h $
// $Id: Mesh_cell_criteria_3.h cc051878fbd 2022-12-15T15:57:10+01:00 Jane Tournois
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-Commercial
//
//
// Author(s)     : Laurent RINEAU, Stephane Tayeb


#ifndef CGAL_MESH_CELL_CRITERIA_3_H
#define CGAL_MESH_CELL_CRITERIA_3_H

#include <CGAL/license/Mesh_3.h>


#include <CGAL/Mesh_3/mesh_standard_cell_criteria.h>
#include <CGAL/Mesh_3/Is_mesh_domain_field_3.h>

#include <boost/config.hpp>
#  include <boost/callable_traits/is_invocable.hpp>

#include <type_traits>

namespace CGAL {

template <typename Tr,
          typename Visitor_ = Mesh_3::Cell_criterion_visitor_with_radius_lower_bound<Tr> >
class Mesh_cell_criteria_3
{
public:
  typedef Visitor_ Visitor;
  typedef typename Visitor::Cell_quality Cell_quality;
  typedef typename Visitor::Is_cell_bad  Is_cell_bad;

  typedef Mesh_3::Abstract_criterion<Tr,Visitor> Abstract_criterion;
private:
  typedef Mesh_3::Criteria<Tr,Visitor> Criteria;

  typedef typename Tr::Cell_handle Cell_handle;
  typedef typename Tr::Geom_traits::FT FT;

  typedef Mesh_cell_criteria_3<Tr> Self;

public:

  /**
   * @brief Constructor
   * @param radius_edge_bound the radius-edge bound
   * @param radius_bound the radius bound (tet sizing)
   */
  Mesh_cell_criteria_3(const FT& radius_edge_bound,
                       const FT& radius_bound,
                       const FT& min_radius_bound = 0.)
  {
    if (FT(0) != min_radius_bound)
      init_min_radius(min_radius_bound);

    if ( FT(0) != radius_bound )
      init_radius(radius_bound);

    if ( FT(0) != radius_edge_bound )
      init_radius_edge(radius_edge_bound);
  }

  // Nb: SFINAE to avoid wrong matches with built-in numerical types
  // as int.
  template <typename Sizing_field>
  Mesh_cell_criteria_3(const FT& radius_edge_bound,
                       const Sizing_field& radius_bound,
                       const FT& min_radius_bound = 0.,
                       std::enable_if_t<
                         Mesh_3::Is_mesh_domain_field_3<Tr,Sizing_field>::value
                       >* = 0
                       )
  {
    if (FT(0) != min_radius_bound)
      init_min_radius(min_radius_bound);

    init_radius(radius_bound);

    if ( FT(0) != radius_edge_bound )
      init_radius_edge(radius_edge_bound);
  }

  /// Destructor
  ~Mesh_cell_criteria_3() { }

  /**
   * @brief returns whether the cell `cell` is bad or not.
   * @param tr the triangulation within which `cell` lives
   * @param cell the cell
   */
  Is_cell_bad operator()(const Tr& tr, const Cell_handle& cell) const
  {
    return criteria_(tr, cell);
  }

  void add(Abstract_criterion* criterion)
  {
    criteria_.add(criterion);
  }

private:
  void init_radius_edge(const FT& radius_edge_bound)
  {
    typedef Mesh_3::Cell_radius_edge_criterion<Tr,Visitor> Radius_edge_criterion;
    criteria_.add(new Radius_edge_criterion(radius_edge_bound));
  }

  void init_radius(const FT& radius_bound)
  {
    typedef Mesh_3::Cell_uniform_size_criterion<Tr,Visitor> Radius_criterion;
    criteria_.add(new Radius_criterion(radius_bound));
  }

  template < typename Sizing_field>
  void init_radius(const Sizing_field& radius_bound)
  {
    typedef Mesh_3::Cell_variable_size_criterion<Tr,Visitor,Sizing_field>
      Radius_criterion;

    criteria_.add(new Radius_criterion(radius_bound));
  }

  void init_min_radius(const FT& min_radius_bound)
  {
    typedef Mesh_3::Cell_uniform_size_criterion<Tr, Visitor> Radius_criterion;
    criteria_.add(new Radius_criterion(min_radius_bound, true/*lower bound*/));
  }


private:
  Criteria criteria_;

};  // end class Mesh_cell_criteria_3

}  // end namespace CGAL


#endif // CGAL_MESH_CELL_CRITERIA_3_H
