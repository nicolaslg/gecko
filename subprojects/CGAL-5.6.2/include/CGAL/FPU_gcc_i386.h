// Copyright (c) 2010-2011  GeometryFactory Sarl (France)
//
// This file is part of CGAL (www.cgal.org)
//
// $URL: https://github.com/CGAL/cgal/blob/v5.6.2/Number_types/include/CGAL/FPU_gcc_i386.h $
// $Id: FPU_gcc_i386.h 07793738355 2020-03-26T13:31:46+01:00 Sébastien Loriot
// SPDX-License-Identifier: LGPL-3.0-or-later OR LicenseRef-Commercial
//
//
// Author(s)     : Laurent Rineau

extern "C" {
#include <fenv.h>
}

namespace CGAL {

// brute-force replacement for C99 (which does not require an inline-function)
inline int
feclearexcept(int exceptions) {
    // TODO: clear only given exceptions
    asm volatile("fnclex");
    return 0;
}

inline int
fetestexcept(int exceptions) {
    int status;
    asm volatile("fnstsw %0" : "=m" (status));
    return status & exceptions;
}

} // end namespace CGAL
