// Copyright (c) 2007 Fernando Luis Cacciola Carballal. All rights reserved.
//
// This file is part of CGAL (www.cgal.org).
//
// $URL: https://github.com/CGAL/cgal/blob/v5.6.2/Straight_skeleton_2/include/CGAL/Straight_skeleton_2/Polygon_iterators.h $
// $Id: Polygon_iterators.h 15c674d5c91 2023-02-14T14:59:10+01:00 Mael Rouxel-Labbé
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-Commercial
//
// Author(s)     : Fernando Cacciola <fernando_cacciola@ciudad.com.ar>

#ifndef CGAL_STRAIGHT_SKELETON_POLYGON_ITERATOR_H
#define CGAL_STRAIGHT_SKELETON_POLYGON_ITERATOR_H 1

#include <CGAL/license/Straight_skeleton_2.h>

#include <CGAL/Straight_skeleton_2/Straight_skeleton_aux.h>

#include <boost/shared_ptr.hpp>

#include <type_traits>

namespace CGAL {
namespace CGAL_SS_i {

// Polygon_2-type container
template <typename Poly>
inline typename Poly::const_iterator
vertices_begin(const Poly& aPoly,
               std::enable_if_t<!has_Hole_const_iterator<Poly>::value>* = nullptr)
{ return aPoly.begin(); }

template <typename Poly>
inline typename Poly::const_iterator
vertices_end(const Poly& aPoly,
             std::enable_if_t<!has_Hole_const_iterator<Poly>::value>* = nullptr)
{ return aPoly.end(); }

template <typename Poly>
inline typename Poly::const_iterator
vertices_begin(const boost::shared_ptr<Poly>& aPoly,
               std::enable_if_t<!has_Hole_const_iterator<Poly>::value>* = nullptr)
{ return aPoly->begin(); }

template <typename Poly>
inline typename Poly::const_iterator
vertices_end(const boost::shared_ptr<Poly> & aPoly,
             std::enable_if_t<!has_Hole_const_iterator<Poly>::value>* = nullptr)
{ return aPoly->end(); }

// Polygon_with_holes_2-type container
template <typename PolyWithHoles>
inline typename PolyWithHoles::Polygon_2::const_iterator
vertices_begin(const PolyWithHoles& aPoly,
               std::enable_if_t<has_Hole_const_iterator<PolyWithHoles>::value>* = nullptr)
{ return aPoly.outer_boundary().begin(); }

template <typename PolyWithHoles>
inline typename PolyWithHoles::Polygon_2::const_iterator
vertices_end(const PolyWithHoles& aPoly,
             std::enable_if_t<has_Hole_const_iterator<PolyWithHoles>::value>* = nullptr)
{ return aPoly.outer_boundary().end(); }

template <typename PolyWithHoles>
inline typename PolyWithHoles::Polygon_2::const_iterator
vertices_begin(const boost::shared_ptr<PolyWithHoles>& aPoly,
               std::enable_if_t<has_Hole_const_iterator<PolyWithHoles>::value>* = nullptr)
{ return aPoly->outer_boundary().begin(); }

template <typename PolyWithHoles>
inline typename PolyWithHoles::Polygon_2::const_iterator
vertices_end(const boost::shared_ptr<PolyWithHoles>& aPoly,
             std::enable_if_t<has_Hole_const_iterator<PolyWithHoles>::value>* = nullptr)
{ return aPoly->outer_boundary().end(); }

} // namespace CGAL_SS_i
} // namespace CGAL

#endif // CGAL_STRAIGHT_SKELETON_POLYGON_ITERATOR_H //
