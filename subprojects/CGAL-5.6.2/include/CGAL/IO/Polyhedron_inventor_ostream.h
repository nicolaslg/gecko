// Copyright (c) 1997  ETH Zurich (Switzerland).
// All rights reserved.
//
// This file is part of CGAL (www.cgal.org).
//
// $URL: https://github.com/CGAL/cgal/blob/v5.6.2/Polyhedron/include/CGAL/IO/Polyhedron_inventor_ostream.h $
// $Id: Polyhedron_inventor_ostream.h 580a1efd5c1 2020-05-22T15:31:50+02:00 Mael Rouxel-Labbé
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-Commercial
//
//
// Author(s)     : Lutz Kettner  <kettner@mpi-sb.mpg.de>

#ifndef CGAL_IO_POLYHEDRON_INVENTOR_OSTREAM_H
#define CGAL_IO_POLYHEDRON_INVENTOR_OSTREAM_H

#include <CGAL/license/Polyhedron.h>

#include <CGAL/Polyhedron_3.h>

#include <CGAL/IO/OI.h>
#include <CGAL/boost/graph/IO/Generic_facegraph_printer.h>

namespace CGAL {

template <class Traits,
          class Items,
          template <class T, class I, class A>
          class HDS, class Alloc>
Inventor_ostream_base& operator<<(Inventor_ostream_base& out,
                                  const Polyhedron_3<Traits, Items, HDS, Alloc>& P)
{
  IO::internal::Generic_facegraph_printer<Inventor_ostream_base,
                                          Polyhedron_3<Traits, Items, HDS, Alloc>,
                                          File_writer_inventor> printer(out);
  printer(P);

  return out;
}

} // namespace CGAL

#endif // CGAL_IO_POLYHEDRON_INVENTOR_OSTREAM_H
