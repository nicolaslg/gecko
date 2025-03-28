// Copyright (c) 2005  INRIA Sophia-Antipolis (France).
// All rights reserved.
//
// This file is part of CGAL (www.cgal.org).
//
// $URL: https://github.com/CGAL/cgal/blob/v5.6.2/Surface_mesher/include/CGAL/Surface_mesher/Null_oracle_visitor.h $
// $Id: Null_oracle_visitor.h 254d60f642a 2019-10-19T15:23:19+02:00 Sébastien Loriot
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-Commercial
//
//
// Author(s)     : Laurent Rineau


#ifndef CGAL_SURFACE_MESHER_NULL_ORACLE_VISITOR_H
#define CGAL_SURFACE_MESHER_NULL_ORACLE_VISITOR_H

#include <CGAL/license/Surface_mesher.h>


namespace CGAL {

  namespace Surface_mesher {

  /** \interface OracleVisitor
      \brief Concept of a visitor of oracles.

      \fn void new_point(Point& p)
      Called before a new Point is returned by the oracle.
      \param p The point that has just been computed and will be returned.
  */

  /** Trivial model of the OracleVisitor concept. */
  struct Null_oracle_visitor
  {
    template <class P>
    void new_point(P&) const
    {
    }
  };

  }  // namespace Surface_mesher

} // namespace CGAL


#endif  // CGAL_SURFACE_MESHER_NULL_ORACLE_VISITOR_H
