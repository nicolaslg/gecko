// Copyright (c) 2006,2007,2009,2010,2011 Tel-Aviv University (Israel).
// All rights reserved.
//
// This file is part of CGAL (www.cgal.org).
//
// $URL: https://github.com/CGAL/cgal/blob/v5.6.2/Arrangement_on_surface_2/include/CGAL/Arr_geometry_traits/Conic_point_2.h $
// $Id: Conic_point_2.h 7988148710b 2022-06-19T12:39:43+03:00 Efi Fogel
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-Commercial
//
//
// Author(s): Ron Wein <wein@post.tau.ac.il>
//            Efi Fogel <efifogel@gmail.com>

#ifndef CGAL_CONIC_POINT_2_H
#define CGAL_CONIC_POINT_2_H

#include <CGAL/license/Arrangement_on_surface_2.h>

/*! \file
 * Header file for the Conic_point_2<Alg_kernel> class.
 */

#include <list>

#include <CGAL/assertions.h>

namespace CGAL {

/*! \class
 * A class that stores additional information with the point's
 * coordinates, namely the conic IDs of the generating curves.
 */
template <typename AlgKernel>
class Conic_point_2 : public AlgKernel::Point_2 {
public:
  typedef AlgKernel                     Alg_kernel;
  typedef typename Alg_kernel::Point_2  Base;
  typedef Conic_point_2<Alg_kernel>     Self;

  typedef typename Alg_kernel::FT       Algebraic;

  /*! \class
   * Representation of an ID of a conic arc.
   */
  class Conic_id {
  private:
    size_t index;       // the index of the conic arc

  public:
    /*! Default constructor. */
    Conic_id() : index(0) {}

    /*! Constructor. */
    Conic_id(size_t ind) : index(ind) { CGAL_precondition(ind != 0); }

    /*! Check if the ID is valid. */
    bool is_valid() const { return (index != 0); }

    /*! Equality operator. */
    bool operator==(const Conic_id& id) const { return (index == id.index); }

    /*! Inequality operator. */
    bool operator!=(const Conic_id& id) const { return (index != id.index); }

    /*! Less-than operator. */
    bool operator<(const Conic_id& id) const { return (index < id.index); }

    /*! Greater-than operator. */
    bool operator>(const Conic_id& id) const { return (index > id.index); }
  };

private:
  typedef std::list<Conic_id>                          Ids_container;
  typedef typename std::list<Conic_id>::const_iterator Ids_iterator;

  Ids_container conic_ids;       // the IDs of the generating conics.

public:
  /// \name Constructors.
  //@{

  /*! Default constructors. */
  Conic_point_2() : Base() {}

  /*! Constrcutor from the base class. */
  Conic_point_2(const Base& p) : Base(p) {}

  /*! Constructor with homegeneous coordinates. */
  Conic_point_2(const Algebraic& hx, const Algebraic& hy, const Algebraic& hz) :
    Base(hx, hy, hz)
  {}

  /*! Constructor with Cartesian coordinates. */
  Conic_point_2(const Algebraic& x, const Algebraic& y) : Base(x, y) {}
  //@}

  /// \name Maintaining the generating conic IDs.
  //@{

  /*! Add a generating conic ID. */
  void set_generating_conic(const Conic_id& id)
  { if (id.is_valid()) conic_ids.push_back(id); }

  /*! Check if the given conic generates the point. */
  bool is_generating_conic(const Conic_id& id) const {
    if (! id.is_valid()) return false;
    for (auto it = conic_ids.begin(); it != conic_ids.end(); ++it)
      if (*it == id) return true;
    return false;
  }
  //@}

};

} //namespace CGAL

#endif
