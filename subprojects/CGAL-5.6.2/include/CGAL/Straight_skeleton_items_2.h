// Copyright (c) 2005-2008 Fernando Luis Cacciola Carballal. All rights reserved.
//
// This file is part of CGAL (www.cgal.org).
//
// $URL: https://github.com/CGAL/cgal/blob/v5.6.2/Straight_skeleton_2/include/CGAL/Straight_skeleton_items_2.h $
// $Id: Straight_skeleton_items_2.h 38312c1e637 2023-02-14T10:04:03+01:00 Mael Rouxel-Labbé
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-Commercial
//
// Author(s)     : Fernando Cacciola <fernando_cacciola@ciudad.com.ar>
//
#ifndef CGAL_STRAIGHT_SKELETON_ITEMS_2_H
#define CGAL_STRAIGHT_SKELETON_ITEMS_2_H 1

#include <CGAL/license/Straight_skeleton_2.h>


#include <CGAL/Straight_skeleton_vertex_base_2.h>
#include <CGAL/Straight_skeleton_halfedge_base_2.h>
#include <CGAL/Straight_skeleton_face_base_2.h>

namespace CGAL {

class Straight_skeleton_items_2
{
public:

  template<class Refs, class Traits>
  struct Vertex_wrapper
  {
    typedef typename Traits::RT RT ;
    typedef typename Traits::Point_2 Point ;
    typedef Straight_skeleton_vertex_base_2 < Refs, Point, RT > Vertex;
  };

  template<class Refs, class Traits>
  struct Halfedge_wrapper
  {
    typedef typename Traits::FT FT ;
    typedef Straight_skeleton_halfedge_base_2 < Refs, FT > Halfedge;
  };

  template<class Refs, class Traits>
  struct Face_wrapper
  {
    typedef Straight_skeleton_face_base_2 < Refs > Face;
  } ;
};

} // end namespace CGAL

#endif // CGAL_STRAIGHT_SKELETON_ITEMS_2_H //
// EOF //

