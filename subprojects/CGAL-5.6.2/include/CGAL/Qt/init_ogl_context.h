// Copyright (c) 2021  GeometryFactory Sarl (France).
// All rights reserved.
//
// This file is part of CGAL (www.cgal.org).
//
// $URL: https://github.com/CGAL/cgal/blob/v5.6.2/GraphicsView/include/CGAL/Qt/init_ogl_context.h $
// $Id: init_ogl_context.h 9a202e2bbc8 2021-01-12T13:27:31+01:00 Maxime Gimeno
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-Commercial
//
//
// Author(s)     : Maxime Gimeno

#ifndef CGAL_QT_CONTEXT_INITIALIZATION_H
#define CGAL_QT_CONTEXT_INITIALIZATION_H

#include <CGAL/license/GraphicsView.h>


#include <QSurfaceFormat>
#include <QCoreApplication>
namespace CGAL
{
namespace Qt
{
inline void init_ogl_context(int major, int minor) {
  QSurfaceFormat fmt;
#ifdef Q_OS_MAC
  if(major == 4)
  {
    fmt.setVersion(4, 1);
  }
  else
  {
     fmt.setVersion(major, minor);
  }
#else
  fmt.setVersion(major, minor);
#endif
  fmt.setRenderableType(QSurfaceFormat::OpenGL);
  fmt.setProfile(QSurfaceFormat::CoreProfile);
  fmt.setOption(QSurfaceFormat::DebugContext);
  QSurfaceFormat::setDefaultFormat(fmt);

  //for windows
  QCoreApplication::setAttribute(::Qt::AA_UseDesktopOpenGL);

  //We set the locale to avoid any trouble with VTK
  setlocale(LC_ALL, "C");
}

} //end Qt
} //end CGAL
#endif // CGAL_QT_CONTEXT_INITIALIZATION_H
