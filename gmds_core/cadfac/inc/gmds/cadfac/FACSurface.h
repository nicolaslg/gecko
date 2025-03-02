/*----------------------------------------------------------------------------*/
/*
 * FACSurface.h
 *
 *  Created on: 27 juin 2011
 *      Author: ledouxf
 */
/*----------------------------------------------------------------------------*/
#ifndef GMDS_GEOM_FACETEDSURFACE_H_
#define GMDS_GEOM_FACETEDSURFACE_H_
/*----------------------------------------------------------------------------*/
// GMDS File Headers
/*----------------------------------------------------------------------------*/
#include <gmds/cad/GeomSurface.h>
#include <gmds/ig/Face.h>
#include <gmds/math/Triangle.h>
/*----------------------------------------------------------------------------*/
#include <tuple>
/*----------------------------------------------------------------------------*/
#include <CGAL/AABB_tree.h>
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Triangle_3.h>
#include <CGAL/AABB_triangle_primitive.h>
#include <CGAL/AABB_traits.h>
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
namespace gmds{
/*----------------------------------------------------------------------------*/
    namespace cad{

    	typedef CGAL::Simple_cartesian<double> Kernel;
    	typedef Kernel::Point_3 Point;
    	typedef Kernel::Triangle_3 Triangle;
    	typedef CGAL::AABB_triangle_primitive<Kernel, std::vector<Triangle>::iterator> Primitive;
    	typedef CGAL::AABB_traits<Kernel, Primitive> Traits;
    	typedef CGAL::AABB_tree<Traits> Tree;
/*----------------------------------------------------------------------------*/
        class FACCurve;
        class FACPoint;
        class FACVolume;
/*----------------------------------------------------------------------------*/
/** \class FacetedSurface
 *  \brief This class implements the surface services that are required by the
 *  	   mesh to the geometrical model.
 *  \param TCoord the data type used to store geometrical data
 */
/*----------------------------------------------------------------------------*/
        class  FACSurface : public GeomSurface {

        public:


            /*------------------------------------------------------------------------*/
            /** \brief  Constructor.
             *
             *  @param AMesh mesh support
             *  \param AP 		the points adjacent to the surface
             *  \param AP2 		the curves adjacent to the surface
             *  \param ADiscret the triangles discretizing the surface
             *  \param AName	the surface name
             */
            FACSurface(Mesh* AMesh,
                       std::vector<TCellID>& ADiscret,
                       const std::string& AName="Unknown surface");

            /*------------------------------------------------------------------------*/
            /** \brief  Destructor.
             */
            virtual ~FACSurface();


            /*------------------------------------------------------------------------*/
            /** \brief  computes the area of the entity.
             */
            virtual TCoord computeArea() const;

            /*------------------------------------------------------------------------*/
            /** \brief  computes the bounding box
             *
             *	\param minXYZ The minimum coordinate of the bounding box.
             *	\param maxXYZ The maximum coordinate of the bounding box.
             */
            virtual void computeBoundingBox(TCoord minXYZ[3], TCoord maxXYZ[3]) const;
	         virtual std::tuple<TCoord,TCoord,TCoord,TCoord,TCoord,TCoord>  BBox() const;


            /*------------------------------------------------------------------------*/
            /** \brief Move a point AP near the surface to the closest point on the
             * 		   surface.
             *  \param AP
             */
//	virtual void project(math::Point<3,TCoord>& AP) const;

            /*------------------------------------------------------------------------*/
            /** \brief  computes normal at the closest point to AP in 3D. In this case,
             * 			it is the normal to one of the triangles that compose the
             * 			surface.
             *
             *  \param AP the point
             *
             *  \param AV 	normal vector at the closest point of AP on this
             */
            virtual void computeNormal(	const math::Point& AP, math::Vector3d& AV) const;

            /*------------------------------------------------------------------------*/
            /** \brief Get the closest point from AP on the surface
             *  \param AP a 3D point
             *
             *  \return the closest point of APoint on the surface
             */

            virtual math::Point	closestPoint(const math::Point& AP) const;

            /*------------------------------------------------------------------------*/
            /** \brief Get center of the surface.
             *
             *  \return the center of the surface
             */
            virtual math::Point	center() const;

            /*------------------------------------------------------------------------*/
            int id() const {return m_id;}

            /*------------------------------------------------------------------------*/
            /** \brief  Sets the internal mesh representation
             *
             *  \param AFaces a vector of mesh faces
             */
            void setMeshFaces(const std::vector<gmds::Face >& AFaces);

            /*------------------------------------------------------------------------*/
            /** \brief  Returns a copy of the internal mesh representation
             *
             *  \param AFaces a vector of mesh faces
             */
            void getMeshFaces(std::vector<Face>& AFaces) const;

            /*------------------------------------------------------------------------*/
            /** \brief  Returns a triangulation of the surface
             *
             *  \param ATri a triangulation
             */
            virtual void getTriangulation(std::vector<gmds::math::Triangle >& ATri) const;

            /**@brief Accessor to the adjacent points. Warning, there is no
             *  assumption about the ordering
             * @return points that are adjacent to this point
             */
            virtual std::vector<GeomPoint*>& points();

            /**@brief Accessor to the adjacent curves. Warning, there is no
             *  assumption about the ordering
             * @return curves that are adjacent to this point
             */
            virtual std::vector<GeomCurve*>& curves();

	         /**@brief Accessor to the bounding loop. A loop is
	          * a set of curves that bounds the surface. A surface with a
	          * hole has 2 loops: the outside one and the one that bounds the
	          * hole.
	          * Warning, there is no assumption about the ordering
	          * @return loops
	          */
	         std::vector<std::vector<GeomCurve *> > loops();
            /**@brief Accessor to the adjacent volumes. Warning, there is no
             *  assumption about the ordering
             * @return volumes that are adjacent to this point
             */
            virtual std::vector<GeomVolume*>& volumes();

            void propagateOrient(Face AFace, int AMarkTreatedFaces, int AMarkFacesInverted, Mesh* AMesh);
            bool checkSameOrientFace(Face AFaceRef, Face AFaceCheck);
            void invertAllFaces();
            void invertFace(Face AFace);

            /**@brief Reset the global id counter to 1.
             */
            static void resetIdCounter();
        private:


        	/**@brief Build a AABB tree data structure to accelerate retrieval information
			 *        in the surface.
			 */
        	void buildResearchTree();

        private:

            /** the global mesh the surface is built on*/
            Mesh* m_support;
            /** Surface id*/
            int m_id;
            /** global surface index to generate next surface index*/
            static int m_next_id;

            /**id of the faces that discretizes the current surface*/
            std::vector<TCellID > m_mesh_faces;
            /** adjacent geometric points that bound this surface*/
            std::vector<GeomPoint*> m_adjacent_points;
            /** adjacent geometric curves that bound this surface*/
            std::vector<GeomCurve*> m_adjacent_curves;
            /** volumes adjacent to this surface*/
            std::vector<GeomVolume*> m_adjacent_volumes;

        	/** Research tree to accelerate distance queries*/
        	Tree m_AABBTree;
        	std::vector<Triangle> m_CGAL_triangles;
        };
/*----------------------------------------------------------------------------*/
    } // namespace cad
/*----------------------------------------------------------------------------*/
} // namespace gmds
/*----------------------------------------------------------------------------*/
#endif /* GMDS_GEOM_FACETEDSURFACE_H_ */
/*----------------------------------------------------------------------------*/
