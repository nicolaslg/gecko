// Copyright (c) 2009-2017 GeometryFactory (France).
// All rights reserved.
//
// This file is part of CGAL (www.cgal.org).
//
// $URL: https://github.com/CGAL/cgal/blob/v5.6.2/SMDS_3/include/CGAL/facets_in_complex_3_to_triangle_mesh.h $
// $Id: facets_in_complex_3_to_triangle_mesh.h 2db4d016d76 2022-06-17T14:17:37+02:00 Jane Tournois
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-Commercial
//
//
// Author(s)     : Maxime Gimeno,
//                 Mael Rouxel-Labbé

#ifndef CGAL_FACETS_IN_COMPLEX_3_TO_TRIANGLE_MESH_H
#define CGAL_FACETS_IN_COMPLEX_3_TO_TRIANGLE_MESH_H

#include <CGAL/license/SMDS_3.h>

#include <CGAL/array.h>
#include <CGAL/boost/graph/Euler_operations.h>
#include <CGAL/Cartesian_converter.h>
#include <CGAL/Kernel_traits.h>
#include <CGAL/Polygon_mesh_processing/orient_polygon_soup.h>
#include <CGAL/Polygon_mesh_processing/polygon_soup_to_polygon_mesh.h>
#include <CGAL/Time_stamper.h>

#include <boost/tuple/tuple.hpp>

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <unordered_map>
#include <vector>

namespace CGAL {

namespace SMDS_3 {

namespace internal {

template <class Polygon>
void resize(Polygon& p, std::size_t size)
{
  p.resize(size);
}

template <std::size_t N, class INT>
void resize(std::array<INT, N>&, std::size_t CGAL_assertion_code(size))
{
  CGAL_assertion(size == N);
}

template<class C3T3, class PointContainer, class FaceContainer, class PatchIndexContainer>
void facets_in_complex_3_to_triangle_soup(const C3T3& c3t3,
                                          const typename C3T3::Subdomain_index sd_index,
                                          PointContainer& points,
                                          FaceContainer& faces,
                                          PatchIndexContainer& patches,
                                          const bool normals_point_outside_of_the_subdomain = true,
                                          const bool export_all_facets = false)
{
  typedef typename PointContainer::value_type                            Point_3;
  typedef typename FaceContainer::value_type                             Face;

  typedef typename C3T3::Triangulation                                   Tr;
  typedef typename C3T3::Surface_patch_index                             Surface_patch_index;

  typedef typename Tr::Cell_handle                                       Cell_handle;
  typedef typename Tr::Weighted_point                                    Weighted_point;
  typedef typename Tr::Facet                                             Facet;

  typedef std::unordered_map<Point_3, std::size_t>                       PIM;

  typedef typename C3T3::size_type                                       size_type;

  // triangulation point to range point
  CGAL::Cartesian_converter<typename CGAL::Kernel_traits<typename Tr::Geom_traits::Point_3>::type,
                            typename CGAL::Kernel_traits<Point_3>::type> t2r;

  size_type nf = c3t3.number_of_facets_in_complex();
  faces.reserve(faces.size() + nf);
  patches.reserve(faces.size() + nf);
  points.reserve(points.size() + nf/2); // approximating Euler

  PIM p_to_ids;
  std::size_t inum = 0;

  for(Facet fit : c3t3.facets_in_complex())
  {
    Cell_handle c = fit.first;
    int s = fit.second;
    const Surface_patch_index spi = c->surface_patch_index(s);
    Face f;
    resize(f, 3);

    typename C3T3::Subdomain_index cell_sdi = c3t3.subdomain_index(c);
    typename C3T3::Subdomain_index opp_sdi = c3t3.subdomain_index(c->neighbor(s));

    if(!export_all_facets && cell_sdi != sd_index && opp_sdi != sd_index)
      continue;

    for(std::size_t i=1; i<4; ++i)
    {
      CGAL_assertion_code(typedef typename Tr::Vertex_handle Vertex_handle;)
      CGAL_assertion_code(Vertex_handle v = c->vertex((s+i)&3);)
      CGAL_assertion(v != Vertex_handle() && !c3t3.triangulation().is_infinite(v));

      const Weighted_point& wp = c3t3.triangulation().point(c, (s+i)&3);
      const Point_3& bp = t2r(c3t3.triangulation().geom_traits().construct_point_3_object()(wp));

      auto insertion_res = p_to_ids.emplace(bp, inum);
      if(insertion_res.second) // new point
      {
        points.push_back(bp);
        ++inum;
      }

      f[i-1] = insertion_res.first->second;
    }

    if(export_all_facets)
    {
      if((cell_sdi > opp_sdi) == (s%2 == 1))
        std::swap(f[0], f[1]);
    }
    else
    {
      if(((cell_sdi == sd_index) == (s%2 == 1)) == normals_point_outside_of_the_subdomain)
        std::swap(f[0], f[1]);
    }

    faces.push_back(f);
    patches.push_back(spi);
  }
}

template<class C3T3, class PointContainer, class FaceContainer, class SurfacePatchContainer>
void facets_in_complex_3_to_triangle_soup(const C3T3& c3t3,
                                          PointContainer& points,
                                          FaceContainer& faces,
                                          SurfacePatchContainer& patches)
{
  typedef typename C3T3::Subdomain_index              Subdomain_index;
  Subdomain_index useless = Subdomain_index();
  facets_in_complex_3_to_triangle_soup(c3t3, useless, points, faces, patches,
                                       true/*point outward*/, true /*extract all facets*/);
}

template <typename Index2FaceMap, typename SurfacePatchRange>
void set_face_patches(const Index2FaceMap&,
                      const SurfacePatchRange&,
                      const internal_np::Param_not_found&)
{
  return;
}

template <typename Index2FaceMap,
          typename SurfacePatchRange,
          typename FacePatchMap>
void set_face_patches(const Index2FaceMap& i2f,
                      const SurfacePatchRange& patches,
                      const FacePatchMap& fpmap)
{
  for (auto index_and_face : i2f)
  {
    put(fpmap, index_and_face.second, patches[index_and_face.first]);
  }
}

} // end namespace internal

} // end namespace SMDS_3

  /**
   * @ingroup PkgSMDS3Functions
   *
   * @brief builds a `TriangleMesh` from the surface facets, with a consistent orientation
   *   at the interface of two subdomains.
   *
   * This function exports the surface as a `TriangleMesh` and appends it to `tmesh`, using
   *   `orient_polygon_soup()`.
   *
   * @tparam C3T3 a model of `MeshComplexWithFeatures_3InTriangulation_3`.
   * @tparam TriangleMesh a model of `MutableFaceGraph` with an internal point property map.
   *    The point type must be compatible with the one used in `C3T3`.
   * @tparam NamedParameters a sequence of \ref bgl_namedparameters "Named Parameters"
   *
   * @param c3t3 an instance of `C3T3`
   * @param tmesh an instance of `TriangleMesh`
   * @param np an optional sequence of \ref bgl_namedparameters "Named Parameters" among the ones listed below
   *
   * \cgalNamedParamsBegin
   *   \cgalParamNBegin{face_patch_map}
  *     \cgalParamDescription{a property map with the patch id's associated to the faces of `faces(tmesh)`}
  *     \cgalParamType{a class model of `ReadWritePropertyMap` with `boost::graph_traits<TriangleMesh>::%face_descriptor`
  *                    as key type and the desired property, model of `CopyConstructible`
  *                    and `LessThanComparable`,
  *                    and compatible with the `Surface_patch_index` type of `C3T3` as value type.}
  *     \cgalParamDefault{If not provided, faces patch ids are ignored.}
  *   \cgalParamNEnd
  * \cgalNamedParamsEnd
  */
  template<class C3T3, class TriangleMesh, typename NamedParameters = parameters::Default_named_parameters>
  void facets_in_complex_3_to_triangle_mesh(const C3T3& c3t3,
    TriangleMesh& tmesh,
    const NamedParameters& np = parameters::default_values())
  {
    namespace PMP = CGAL::Polygon_mesh_processing;

    typedef typename boost::property_map<TriangleMesh, boost::vertex_point_t>::type  VertexPointMap;
    typedef typename boost::property_traits<VertexPointMap>::value_type              Point_3;
    typedef typename boost::graph_traits<TriangleMesh>::face_descriptor              face_descriptor;
    typedef typename C3T3::Surface_patch_index                                       Surface_patch_index;

    typedef std::array<std::size_t, 3>                                       Face;

    std::vector<Face> faces;
    std::vector<Surface_patch_index> patches;
    std::vector<Point_3> points;

    SMDS_3::internal::facets_in_complex_3_to_triangle_soup(c3t3, points, faces, patches);

    if (!PMP::is_polygon_soup_a_polygon_mesh(faces))
      PMP::orient_polygon_soup(points, faces);
    CGAL_postcondition(PMP::is_polygon_soup_a_polygon_mesh(faces));

    std::unordered_map<std::size_t, face_descriptor> i2f;
    PMP::polygon_soup_to_polygon_mesh(points, faces, tmesh,
      CGAL::parameters::polygon_to_face_output_iterator(std::inserter(i2f, i2f.end())));

    using parameters::get_parameter;
    SMDS_3::internal::set_face_patches(i2f,
                                      patches,
                                      get_parameter(np, internal_np::face_patch));
  }

} // namespace CGAL

#endif // CGAL_FACETS_IN_COMPLEX_3_TO_TRIANGLE_MESH_H
