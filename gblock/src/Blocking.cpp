/*----------------------------------------------------------------------------*/
#include "gecko/gblock/Blocking.h"
/*----------------------------------------------------------------------------*/
using namespace gecko;
using namespace gecko::gblock;
/*----------------------------------------------------------------------------*/
// int CellInfo::m_counter_global_id = 0;
/*----------------------------------------------------------------------------*/
Blocking::Blocking(cad::GeomManager *AGeomModel, bool AInitAsBoundingBox) : m_geom_model(AGeomModel), m_counter(0)
{
	if (AInitAsBoundingBox) {
		init_from_bounding_box();
	}
}
/*----------------------------------------------------------------------------*/
Blocking::Blocking(const Blocking &ABl) : m_geom_model(ABl.m_geom_model), m_gmap(ABl.m_gmap), m_counter(ABl.m_counter)
{
	auto listBlocks = get_all_blocks();
	for (auto b : listBlocks) {
		b->info().counter = &m_counter;
	}
	auto listFaces = get_all_faces();
	for (auto b : listFaces) {
		b->info().counter = &m_counter;
	}
	auto listEdges = get_all_edges();
	for (auto b : listEdges) {
		b->info().counter = &m_counter;
	}
	auto listNodes = get_all_nodes();
	for (auto b : listNodes) {
		b->info().counter = &m_counter;
	}
}

/*----------------------------------------------------------------------------*/
void Blocking::reset_classification()
{
	auto listBlocks = get_all_blocks();
	for (auto b : listBlocks) {
		b->info().geom_dim = 4;
		b->info().geom_id = NullID;
	}
	auto listFaces = get_all_faces();
	for (auto b : listFaces) {
		b->info().geom_dim = 4;
		b->info().geom_id = NullID;	}
	auto listEdges = get_all_edges();
	for (auto b : listEdges) {
		b->info().geom_dim = 4;
		b->info().geom_id = NullID;
	}
	auto listNodes = get_all_nodes();
	for (auto b : listNodes) {
		b->info().geom_dim = 4;
		b->info().geom_id = NullID;
	}
}
/*----------------------------------------------------------------------------*/
Blocking::~Blocking() {}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
bool Blocking::operator==(Blocking &ABlocking)
{
	bool sameBlocking=true;
	if(get_all_blocks().size() != ABlocking.get_all_blocks().size()){
		sameBlocking = false;
	}
	else if(get_all_faces().size() != ABlocking.get_all_faces().size()){
		sameBlocking = false;
	}
	else if(get_all_edges().size() != ABlocking.get_all_edges().size()){
		sameBlocking = false;
	}
	else if(get_all_nodes().size() != ABlocking.get_all_nodes().size()){
		sameBlocking = false;
	}

	auto listNodes= get_all_nodes();
	auto listNodesComparedBlocking = ABlocking.get_all_nodes();

	for(auto n1 : listNodes){
		bool found = false;
		for(auto n2 : listNodesComparedBlocking){
			if(n1->info().point.X() == n2->info().point.X() &&
				n1->info().point.Y() == n2->info().point.Y() &&
				n1->info().point.Z() == n2->info().point.Z()){
				found = true;
				break;
				}
		}
		if(!found){
			sameBlocking=false;
			break;
		}
	}
	return sameBlocking;
}
/*----------------------------------------------------------------------------*/
GMap3 *
Blocking::gmap()
{
	return &m_gmap;
}

/*----------------------------------------------------------------------------*/
cad::GeomManager *
Blocking::geom_model() const
{
	return m_geom_model;
}

/*----------------------------------------------------------------------------*/
Blocking::Node
Blocking::create_node(const int AGeomDim, const int AGeomId, const math::Point &APoint)
{
	return m_gmap.create_attribute<0>(NodeInfo(this->getCounter(), m_geom_model, AGeomDim, AGeomId, APoint));
}

/*----------------------------------------------------------------------------*/
Blocking::Edge
Blocking::create_edge(const int AGeomDim, const int AGeomId)
{
	return m_gmap.create_attribute<1>(CellInfo(this->getCounter(), m_geom_model, 1, AGeomDim, AGeomId));
}

/*----------------------------------------------------------------------------*/
Blocking::Face
Blocking::create_face(const int AGeomDim, const int AGeomId)
{
	return m_gmap.create_attribute<2>(CellInfo(this->getCounter(), m_geom_model, 2, AGeomDim, AGeomId));
}

/*----------------------------------------------------------------------------*/
Blocking::Block
Blocking::create_block(const int AGeomDim, const int AGeomId)
{
	return m_gmap.create_attribute<3>(CellInfo(this->getCounter(), m_geom_model, 3, AGeomDim, AGeomId));
}
/*----------------------------------------------------------------------------*/
void
Blocking::extract_boundary(std::set<TCellID> &ANodeIds, std::set<TCellID> &AEdgeIds, std::set<TCellID> &AFaceIds)
{
	ANodeIds.clear();
	AEdgeIds.clear();
	AFaceIds.clear();
	for (auto it = m_gmap.attributes<2>().begin(), itend = m_gmap.attributes<2>().end(); it != itend; ++it) {
		auto adj_blocks = get_blocks_of_face(it);
		if(adj_blocks.size()==1){
			//means we have a boundary face
			AFaceIds.insert(it->info().topo_id);
			auto adj_edges = get_edges_of_face(it);
			for (auto e:adj_edges)
				AEdgeIds.insert(e->info().topo_id);
			auto adj_nodes = get_nodes_of_face(it);
			for (auto n:adj_nodes)
				ANodeIds.insert(n->info().topo_id);
		}
	}
}
/*----------------------------------------------------------------------------*/
std::vector<Blocking::Face>
Blocking::get_faces_of_block(const Blocking::Block AB)
{
	Dart3 d1 = AB->dart();
	std::vector<Blocking::Face> faces;
	faces.resize(6);

	faces[0] = m_gmap.attribute<2>(d1);
	faces[1] = m_gmap.attribute<2>(m_gmap.alpha<2, 1, 0, 1, 2>(d1));
	faces[2] = m_gmap.attribute<2>(m_gmap.alpha<2>(d1));
	faces[3] = m_gmap.attribute<2>(m_gmap.alpha<1, 0, 1, 2>(d1));
	faces[4] = m_gmap.attribute<2>(m_gmap.alpha<1, 2>(d1));
	faces[5] = m_gmap.attribute<2>(m_gmap.alpha<0, 1, 2>(d1));
	return faces;
}

/*----------------------------------------------------------------------------*/
std::vector<Blocking::Block>
Blocking::get_all_blocks()
{
	std::vector<Blocking::Block> blocks;
	for (auto it = m_gmap.attributes<3>().begin(), itend = m_gmap.attributes<3>().end(); it != itend; ++it) {
		blocks.push_back(it);
	}
	return blocks;
}

/*----------------------------------------------------------------------------*/
std::vector<Blocking::Face>
Blocking::get_all_faces()
{
	std::vector<Blocking::Face> faces;
	for (auto it = m_gmap.attributes<2>().begin(), itend = m_gmap.attributes<2>().end(); it != itend; ++it) {
		faces.push_back(it);
	}
	return faces;
}

/*----------------------------------------------------------------------------*/
std::vector<TCellID>
Blocking::get_all_id_nodes()
{
	auto nodes = get_all_nodes();
	std::vector<TCellID> ids;
	ids.reserve(nodes.size());
	for (auto n : nodes)
		ids.push_back(n->info().topo_id);
	return ids;
}

/*----------------------------------------------------------------------------*/
std::vector<TCellID>
Blocking::get_all_id_edges()
{
	auto edges = get_all_edges();
	std::vector<TCellID> ids;
	ids.reserve(edges.size());
	for (auto e : edges)
		ids.push_back(e->info().topo_id);
	return ids;
}

/*----------------------------------------------------------------------------*/
std::vector<TCellID>
Blocking::get_all_id_faces()
{
	auto faces = get_all_faces();
	std::vector<TCellID> ids;
	ids.reserve(faces.size());
	for (auto f : faces)
		ids.push_back(f->info().topo_id);
	return ids;
}

/*----------------------------------------------------------------------------*/
std::vector<TCellID>
Blocking::get_all_id_blocks()
{
	auto blocks = get_all_blocks();
	std::vector<TCellID> ids;
	ids.reserve(blocks.size());
	for (auto b : blocks)
		ids.push_back(b->info().topo_id);
	return ids;
}

/*----------------------------------------------------------------------------*/
std::vector<Blocking::Edge>
Blocking::get_edges_of_node(const Node AN)
{
	std::vector<Edge> edges;
	Dart3 d = AN->dart();
	for (auto it = m_gmap.one_dart_per_incident_cell<1, 0>(d).begin(), itend = m_gmap.one_dart_per_incident_cell<1, 0>(d).end(); it != itend; ++it) {
		edges.push_back(m_gmap.attribute<1>(it));
	}
	return edges;
}

/*----------------------------------------------------------------------------*/
std::vector<Blocking::Face>
Blocking::get_faces_of_node(const Node AN)
{
	std::vector<Face> faces;
	Dart3 d = AN->dart();
	for (auto it = m_gmap.one_dart_per_incident_cell<2, 0>(d).begin(), itend = m_gmap.one_dart_per_incident_cell<2, 0>(d).end(); it != itend; ++it) {
		faces.push_back(m_gmap.attribute<2>(it));
	}
	return faces;
}

/*----------------------------------------------------------------------------*/
std::vector<Blocking::Edge>
Blocking::get_edges_of_face(const Face AF)
{
	std::vector<Edge> edges;
	Dart3 d = AF->dart();
	for (auto it = m_gmap.one_dart_per_incident_cell<1, 2>(d).begin(), itend = m_gmap.one_dart_per_incident_cell<1, 2>(d).end(); it != itend; ++it) {
		edges.push_back(m_gmap.attribute<1>(it));
	}
	return edges;
}

/*----------------------------------------------------------------------------*/
std::vector<Blocking::Block>
Blocking::get_blocks_of_node(const Node AN)
{
	std::vector<Block> blocks;
	Dart3 d = AN->dart();
	for (auto it = m_gmap.one_dart_per_incident_cell<3, 0>(d).begin(), itend = m_gmap.one_dart_per_incident_cell<3, 0>(d).end(); it != itend; ++it) {
		blocks.push_back(m_gmap.attribute<3>(it));
	}
	return blocks;
}

/*----------------------------------------------------------------------------*/
std::vector<Blocking::Face>
Blocking::get_faces_of_edge(const Edge AE)
{
	std::vector<Face> faces;
	Dart3 d = AE->dart();
	for (auto it = m_gmap.one_dart_per_incident_cell<2, 1>(d).begin(), itend = m_gmap.one_dart_per_incident_cell<2, 1>(d).end(); it != itend; ++it) {
		faces.push_back(m_gmap.attribute<2>(it));
	}
	return faces;
}

/*----------------------------------------------------------------------------*/
std::vector<Blocking::Block>
Blocking::get_blocks_of_edge(const Edge AE)
{
	std::vector<Block> blocks;
	Dart3 d = AE->dart();
	for (auto it = m_gmap.one_dart_per_incident_cell<3, 1>(d).begin(), itend = m_gmap.one_dart_per_incident_cell<3, 1>(d).end(); it != itend; ++it) {
		blocks.push_back(m_gmap.attribute<3>(it));
	}
	return blocks;
}

/*----------------------------------------------------------------------------*/
std::vector<Blocking::Block>
Blocking::get_blocks_of_face(const Face AF)
{
	std::vector<Block> blocks;
	Dart3 d = AF->dart();
	for (auto it = m_gmap.one_dart_per_incident_cell<3, 2>(d).begin(), itend = m_gmap.one_dart_per_incident_cell<3, 2>(d).end(); it != itend; ++it) {
		blocks.push_back(m_gmap.attribute<3>(it));
	}
	return blocks;
}
/*----------------------------------------------------------------------------*/
Blocking::Edge
Blocking::get_edge(const Blocking::Node AN1, const Blocking::Node AN2)
{
	return get_edge(AN1->info().topo_id, AN2->info().topo_id);
}
/*----------------------------------------------------------------------------*/
Blocking::Edge
Blocking::get_edge(const int AN1, const int AN2)
{
	auto edges = get_all_edges();
	for (auto e : edges) {
		auto nodes_of_e = get_nodes_of_edge(e);
		if (nodes_of_e[0]->info().topo_id == AN1 && nodes_of_e[1]->info().topo_id == AN2)
			return e;
		else if (nodes_of_e[0]->info().topo_id == AN2 && nodes_of_e[1]->info().topo_id == AN1)
			return e;
	}
	std::cout<<"AN1="<<AN1<<std::endl;
	std::cout<<"AN2="<<AN2<<std::endl;
	throw GMDSException("No edge with given end points");
}
/*----------------------------------------------------------------------------*/
std::vector<Blocking::Edge>
Blocking::get_all_edges()
{
	std::vector<Blocking::Edge> edges;
	for (auto it = m_gmap.attributes<1>().begin(), itend = m_gmap.attributes<1>().end(); it != itend; ++it) {
		edges.push_back(it);
	}
	return edges;
}

/*----------------------------------------------------------------------------*/
std::vector<Blocking::Node>
Blocking::get_all_nodes()
{
	std::vector<Blocking::Node> nodes;
	for (auto it = m_gmap.attributes<0>().begin(), itend = m_gmap.attributes<0>().end(); it != itend; ++it) {
		nodes.push_back(it);
	}
	return nodes;
}

/*----------------------------------------------------------------------------*/
std::vector<Blocking::Edge>
Blocking::get_edges_of_block(const Blocking::Block AB)
{
	Dart3 d = AB->dart();
	std::vector<Blocking::Edge> edges;
	edges.resize(12);

	edges[0] = m_gmap.attribute<1>(d);
	edges[1] = m_gmap.attribute<1>(m_gmap.alpha<1, 0, 1>(d));
	edges[2] = m_gmap.attribute<1>(m_gmap.alpha<2, 1, 0, 1>(d));
	edges[3] = m_gmap.attribute<1>(m_gmap.alpha<1, 0, 1, 2, 1, 0, 1>(d));
	d = m_gmap.alpha<1>(d);
	edges[4] = m_gmap.attribute<1>(d);
	edges[5] = m_gmap.attribute<1>(m_gmap.alpha<1, 0, 1>(d));
	edges[6] = m_gmap.attribute<1>(m_gmap.alpha<2, 1, 0, 1>(d));
	edges[7] = m_gmap.attribute<1>(m_gmap.alpha<1, 0, 1, 2, 1, 0, 1>(d));
	d = m_gmap.alpha<1>(d);
	d = m_gmap.alpha<2, 1>(d);
	edges[8] = m_gmap.attribute<1>(d);
	edges[9] = m_gmap.attribute<1>(m_gmap.alpha<1, 0, 1>(d));
	edges[10] = m_gmap.attribute<1>(m_gmap.alpha<2, 1, 0, 1>(d));
	edges[11] = m_gmap.attribute<1>(m_gmap.alpha<1, 0, 1, 2, 1, 0, 1>(d));

	return edges;
}

/*----------------------------------------------------------------------------*/
std::vector<Blocking::Node>
Blocking::get_nodes_of_block(const Blocking::Block AB)
{
	Dart3 d1 = AB->dart();
	std::vector<Blocking::Node> nodes;
	nodes.resize(8);
	nodes[0] = m_gmap.attribute<0>(d1);
	nodes[1] = m_gmap.attribute<0>(m_gmap.alpha<0, 1>(d1));
	nodes[2] = m_gmap.attribute<0>(m_gmap.alpha<0, 1, 0, 1>(d1));
	nodes[3] = m_gmap.attribute<0>(m_gmap.alpha<1, 0>(d1));

	Dart3 d2 = m_gmap.alpha<2, 1, 0, 1, 2>(d1);
	nodes[4] = m_gmap.attribute<0>(d2);
	nodes[5] = m_gmap.attribute<0>(m_gmap.alpha<0, 1>(d2));
	nodes[6] = m_gmap.attribute<0>(m_gmap.alpha<0, 1, 0, 1>(d2));
	nodes[7] = m_gmap.attribute<0>(m_gmap.alpha<1, 0>(d2));

	return nodes;
}

/*----------------------------------------------------------------------------*/
std::vector<Blocking::Node>
Blocking::get_nodes_of_face(const Blocking::Face AF)
{
	Dart3 d1 = AF->dart();
	std::vector<Blocking::Node> nodes;
	Dart3 d = d1;
	do {
		nodes.push_back(m_gmap.attribute<0>(d));
		d = m_gmap.alpha<0, 1>(d);
	} while (d != d1);
	return nodes;
}

/*----------------------------------------------------------------------------*/
std::vector<Blocking::Node>
Blocking::get_nodes_of_edge(const Blocking::Edge AE)
{
	std::vector<Blocking::Node> nodes;
	nodes.resize(2);
	Dart3 d = AE->dart();
	auto d_node = m_gmap.attribute<0>(d);
	auto d0_node = m_gmap.attribute<0>(m_gmap.alpha<0>(d));
	if(d_node->info().topo_id< d0_node->info().topo_id) {
		nodes[0] = d_node;
		nodes[1] = d0_node;
	}
	else{
		nodes[0] = d0_node;
		nodes[1] = d_node;
	}
	return nodes;
}

/*----------------------------------------------------------------------------*/
math::Point
Blocking::get_center_of_edge(const Edge AE)
{
	std::vector<Node> n = get_nodes_of_edge(AE);
	return 0.5 * (n[0]->info().point + n[1]->info().point);
}

/*----------------------------------------------------------------------------*/
math::Point
Blocking::get_center_of_face(const Face AF)
{
	std::vector<Node> nodes = get_nodes_of_face(AF);
	math::Point center(0, 0, 0);
	for (auto n : nodes) {
		center = center + n->info().point;
	}
	return (1.0 / nodes.size()) * center;
}

/*----------------------------------------------------------------------------*/
math::Vector3d
Blocking::get_normal_of_face(const Face AF)
{
	std::vector<Node> nodes = get_nodes_of_face(AF);
	math::Point center = get_center_of_face(AF);
	math::Vector3d n({0, 0, 0});
	for (auto i = 0; i < nodes.size(); i++) {
		Node prev_node = (i == 0) ? nodes[nodes.size() - 1] : nodes[i - 1];
		Node curr_node = nodes[i];
		math::Vector3d vp = prev_node->info().point - center;
		math::Vector3d vc = curr_node->info().point - center;
		n = n + vp.cross(vc);
	}
	n.normalize();
	return n;
}

/*----------------------------------------------------------------------------*/
math::Point
Blocking::get_center_of_block(const Block AB)
{
	std::vector<Node> nodes = get_nodes_of_block(AB);
	math::Point center(0, 0, 0);
	for (auto n : nodes) {
		center = center + n->info().point;
	}
	return (1.0 / nodes.size()) * center;
}

/*----------------------------------------------------------------------------*/
void
Blocking::move_node(Node AN, math::Point &ALoc)
{
	AN->info().point = ALoc;
	if (AN->info().geom_dim == 0) {
		// classified onto a point
		cad::GeomPoint *geom_pnt = m_geom_model->getPoint(AN->info().geom_id);
		AN->info().point = geom_pnt->point();
	}
	else if (AN->info().geom_dim == 1) {
		// classified onto a curve
		cad::GeomCurve *geom_curve = m_geom_model->getCurve(AN->info().geom_id);
		geom_curve->project(AN->info().point);
	}
	else if (AN->info().geom_dim == 2) {
		// classified onto a surface
		cad::GeomSurface *geom_surf = m_geom_model->getSurface(AN->info().geom_id);
		geom_surf->project(AN->info().point);
	}
	// otherwise nothing else to do
}

/*----------------------------------------------------------------------------*/
Blocking::Block
Blocking::create_block(const math::Point &AP1,
                       const math::Point &AP2,
                       const math::Point &AP3,
                       const math::Point &AP4,
                       const math::Point &AP5,
                       const math::Point &AP6,
                       const math::Point &AP7,
                       const math::Point &AP8)
{
	// Initialize attribute for the created hexahedron
	Dart3 d1 = m_gmap.make_combinatorial_hexahedron();

	// 0D attribute
	m_gmap.set_attribute<0>(d1, create_node(4, NullID, AP1));
	m_gmap.set_attribute<0>(m_gmap.alpha<0>(d1), create_node(4, NullID, AP2));
	m_gmap.set_attribute<0>(m_gmap.alpha<0, 1, 0>(d1), create_node(4, NullID, AP3));
	m_gmap.set_attribute<0>(m_gmap.alpha<1, 0>(d1), create_node(4, NullID, AP4));
	m_gmap.set_attribute<0>(m_gmap.alpha<2, 1, 0, 1, 2>(d1), create_node(4, NullID, AP5));
	m_gmap.set_attribute<0>(m_gmap.alpha<2, 1, 0, 1, 2, 0, 1>(d1), create_node(4, NullID, AP6));
	m_gmap.set_attribute<0>(m_gmap.alpha<2, 1, 0, 1, 2, 0, 1, 0, 1>(d1), create_node(4, NullID, AP7));
	m_gmap.set_attribute<0>(m_gmap.alpha<2, 1, 0, 1, 2, 1, 0>(d1), create_node(4, NullID, AP8));

	// go through all the edges
	for (auto it = m_gmap.one_dart_per_incident_cell<1, 3>(d1).begin(), itend = m_gmap.one_dart_per_incident_cell<1, 3>(d1).end(); it != itend; ++it) {
		m_gmap.set_attribute<1>(it, create_edge(4, NullID));
	}

	// go through all the faces
	for (auto it = m_gmap.one_dart_per_incident_cell<2, 3>(d1).begin(), itend = m_gmap.one_dart_per_incident_cell<2, 3>(d1).end(); it != itend; ++it) {
		m_gmap.set_attribute<2>(it, create_face(4, NullID));
	}
	Block b = create_block(4, NullID);
	m_gmap.set_attribute<3>(d1, b);
	return b;
}
/*----------------------------------------------------------------------------*/
void
Blocking::remove_block(const TCellID ABlockId)
{
	Block AB = get_block(ABlockId);
	Dart3 d = AB->dart();
	m_gmap.remove_cell<3>(d);
}

/*----------------------------------------------------------------------------*/
void
Blocking::remove_block(Blocking::Block AB)
{
	Dart3 d = AB->dart();
	m_gmap.remove_cell<3>(d);
}

/*----------------------------------------------------------------------------*/
std::tuple<int, int, math::Point>
Blocking::get_node_info(const int ANodeId)
{
	for (auto it = m_gmap.attributes<0>().begin(), itend = m_gmap.attributes<0>().end(); it != itend; ++it) {
		if (it->info().topo_id == ANodeId) {
			return std::make_tuple(it->info().geom_dim, it->info().geom_id, it->info().point);
		}
	}
	return std::make_tuple(-1, -1, math::Point(0, 0, 0));
}

/*----------------------------------------------------------------------------*/
std::tuple<int, int>
Blocking::get_edge_info(const int AEdgeId)
{
	for (auto it = m_gmap.attributes<1>().begin(), itend = m_gmap.attributes<1>().end(); it != itend; ++it) {
		if (it->info().topo_id == AEdgeId) {
			return std::make_tuple(it->info().geom_dim, it->info().geom_id);
		}
	}
	return std::make_tuple(-1, -1);
}

/*----------------------------------------------------------------------------*/
std::tuple<int, int>
Blocking::get_face_info(const int AFaceId)
{
	for (auto it = m_gmap.attributes<2>().begin(), itend = m_gmap.attributes<2>().end(); it != itend; ++it) {
		if (it->info().topo_id == AFaceId) {
			return std::make_tuple(it->info().geom_dim, it->info().geom_id);
		}
	}
	return std::make_tuple(-1, -1);
}

/*----------------------------------------------------------------------------*/
std::tuple<int, int>
Blocking::get_block_info(const int ABlockId)
{
	for (auto it = m_gmap.attributes<3>().begin(), itend = m_gmap.attributes<3>().end(); it != itend; ++it) {
		if (it->info().topo_id == ABlockId) {
			return std::make_tuple(it->info().geom_dim, it->info().geom_id);
		}
	}
	return std::make_tuple(-1, -1);
}
/*----------------------------------------------------------------------------*/
Blocking::Node
Blocking::get_node(const int AId)
{
	auto list_nodes = get_all_nodes();
	bool found = false;
	for (auto n : list_nodes) {
		if (n->info().topo_id == AId) {
			return n;
		}
	}
	assert(found);
}
/*----------------------------------------------------------------------------*/
Blocking::Edge
Blocking::get_edge(const int AId)
{
	auto list_edges = get_all_edges();
	bool found = false;
	for (auto e : list_edges) {
		if (e->info().topo_id == AId) {
			return e;
		}
	}
	assert(found);
}
/*----------------------------------------------------------------------------*/
Blocking::Face
Blocking::get_face(const int AId)
{
	auto list_faces = get_all_faces();
	bool found = false;
	for (auto f : list_faces) {
		if (f->info().topo_id == AId) {
			return f;
		}
	}
	assert(found);
}
/*----------------------------------------------------------------------------*/
Blocking::Block
Blocking::get_block(const int AId)
{
	auto list_blocks = get_all_blocks();
	bool found = false;
	for (auto b : list_blocks) {
		if (b->info().topo_id == AId) {
			return b;
		}
	}
	assert(found);
}
/*----------------------------------------------------------------------------*/
int
Blocking::get_node_id(Blocking::Node &ANode)
{
	return ANode->info().topo_id;
}
/*----------------------------------------------------------------------------*/
int
Blocking::get_edge_id(Blocking::Edge &AEdge)
{
	return AEdge->info().topo_id;
}
/*----------------------------------------------------------------------------*/
int
Blocking::get_face_id(Blocking::Face &AFace)
{
	return AFace->info().topo_id;
}
/*----------------------------------------------------------------------------*/
int
Blocking::get_block_id(Blocking::Block &ABlock)
{
	return ABlock->info().topo_id;
}
/*----------------------------------------------------------------------------*/
bool
Blocking::is_valid_topology() const
{
	return m_gmap.is_valid();
}

/*----------------------------------------------------------------------------*/
bool
Blocking::is_valid_connected()
{
	std::vector<Block> blocks;
	blocks.push_back(get_all_blocks()[0]);
	std::vector<Block> checked_blocks;
	while(!blocks.empty()){//current block pas dans list checked_blocks
		auto current_block = blocks.back();
		if(std::find(checked_blocks.begin(), checked_blocks.end(), current_block) == checked_blocks.end()){
			checked_blocks.push_back(current_block);
		}
		blocks.pop_back();
		auto faces = get_faces_of_block(current_block);
		for (auto current_face : faces){
			auto blocks_current_face = get_blocks_of_face(current_face);

			//si block current face pas dans check blocs : add dans blocks / sinon ignorer
			for(auto b_curFace : blocks_current_face ){
				bool in = false;
				if(std::find(checked_blocks.begin(), checked_blocks.end(), b_curFace) == checked_blocks.end()){
					blocks.push_back(b_curFace);
				}
				/*for(auto b : checked_blocks){

					if(b->info().topo_id == b_curFace->info().topo_id){
						in = true;
					}
				}
				if(!in){
					blocks.push_back(b_curFace);
				}*/
			}
		}
	}

	if(get_all_blocks().size() == checked_blocks.size()) return true;
	else return false;
}
/*----------------------------------------------------------------------------*/
std::string
Blocking::info() const
{
	std::ostringstream mess;
	mess << "Blocking Info: " << std::endl;
	m_gmap.display_characteristics(mess);
	mess << ", validity=" << (is_valid_topology() ? "true" : "false");
	mess << "\nOd-attributes: ";
	for (auto at : m_gmap.attributes<0>()) {
		mess << "[" << at.info().point << ",(dim:" << at.info().geom_dim << ",id:" << at.info().geom_id << ")]; ";
	}
	mess << "\n1d-attributes: ";
	for (auto at : m_gmap.attributes<1>()) {
		mess << "(dim:" << at.info().geom_dim << ",id:" << at.info().geom_id << "); ";
	}
	mess << "\n2d-attributes: ";
	for (auto at : m_gmap.attributes<2>()) {
		mess << "(dim:" << at.info().geom_dim << ",id:" << at.info().geom_id << "); ";
	}
	mess << "\n3d-attributes: ";
	for (auto at : m_gmap.attributes<3>()) {
		mess << "(dim:" << at.info().geom_dim << ",id:" << at.info().geom_id << "); ";
	}
	return mess.str();
}

/*----------------------------------------------------------------------------*/
void
Blocking::convert_to_mesh(Mesh &AMesh)
{
	MeshModel model = AMesh.getModel();
	if (!model.has(N) || !model.has(E) || !model.has(F) || !model.has(R) || !model.has(E2N) || !model.has(F2N) || !model.has(R2N))
		throw GMDSException("Wrong mesh model for block->mesh conversion");

	AMesh.clear();

	Variable<int> *var_node_topo_id = AMesh.newVariable<int, GMDS_NODE>("blocking_topo_id");
	Variable<int> *var_node_topo_dim = AMesh.newVariable<int, GMDS_NODE>("blocking_topo_dim");
	Variable<int> *var_node_geom_id = AMesh.newVariable<int, GMDS_NODE>("blocking_geom_id");
	Variable<int> *var_node_geom_dim = AMesh.newVariable<int, GMDS_NODE>("blocking_geom_dim");

	Variable<int> *var_edge_topo_id = AMesh.newVariable<int, GMDS_EDGE>("blocking_topo_id");
	Variable<int> *var_edge_topo_dim = AMesh.newVariable<int, GMDS_EDGE>("blocking_topo_dim");
	Variable<int> *var_edge_geom_id = AMesh.newVariable<int, GMDS_EDGE>("blocking_geom_id");
	Variable<int> *var_edge_geom_dim = AMesh.newVariable<int, GMDS_EDGE>("blocking_geom_dim");

	Variable<int> *var_face_topo_id = AMesh.newVariable<int, GMDS_FACE>("blocking_topo_id");
	Variable<int> *var_face_topo_dim = AMesh.newVariable<int, GMDS_FACE>("blocking_topo_dim");
	Variable<int> *var_face_geom_id = AMesh.newVariable<int, GMDS_FACE>("blocking_geom_id");
	Variable<int> *var_face_geom_dim = AMesh.newVariable<int, GMDS_FACE>("blocking_geom_dim");

	Variable<int> *var_region_topo_id = AMesh.newVariable<int, GMDS_REGION>("blocking_topo_id");
	Variable<int> *var_region_topo_dim = AMesh.newVariable<int, GMDS_REGION>("blocking_topo_dim");
	Variable<int> *var_region_geom_id = AMesh.newVariable<int, GMDS_REGION>("blocking_geom_id");
	Variable<int> *var_region_geom_dim = AMesh.newVariable<int, GMDS_REGION>("blocking_geom_dim");

	// mapping from blocking node ids to mesh node ids
	std::map<int, TCellID> n2n;

	// nodes
	for (auto at : m_gmap.attributes<0>()) {
		gmds::Node n = AMesh.newNode(at.info().point);
		var_node_topo_id->set(n.id(), at.info().topo_id);
		var_node_topo_dim->set(n.id(), at.info().topo_dim);
		var_node_geom_id->set(n.id(), at.info().geom_id);
		var_node_geom_dim->set(n.id(), at.info().geom_dim);
		n2n[at.info().topo_id] = n.id();
	}
	// edges
	for (auto it = m_gmap.attributes<1>().begin(), itend = m_gmap.attributes<1>().end(); it != itend; ++it) {
		auto att = m_gmap.info_of_attribute<1>(it);
		std::vector<Node> cell_nodes = get_nodes_of_edge(it);
		gmds::Edge e = AMesh.newEdge(n2n[cell_nodes[0]->info().topo_id], n2n[cell_nodes[1]->info().topo_id]);
		var_edge_topo_id->set(e.id(), att.topo_id);
		var_edge_topo_dim->set(e.id(), att.topo_dim);
		var_edge_geom_id->set(e.id(), att.geom_id);
		var_edge_geom_dim->set(e.id(), att.geom_dim);
	}
	// faces
	for (auto it = m_gmap.attributes<2>().begin(), itend = m_gmap.attributes<2>().end(); it != itend; ++it) {
		auto att = m_gmap.info_of_attribute<2>(it);
		std::vector<Node> cell_nodes = get_nodes_of_face(it);
		if (cell_nodes.size() != 4) {
			continue;
			//throw GMDSException("Only quad blocking faces can be converted into mesh");
		}
		gmds::Face f = AMesh.newQuad(n2n[cell_nodes[0]->info().topo_id], n2n[cell_nodes[1]->info().topo_id], n2n[cell_nodes[2]->info().topo_id],
		                             n2n[cell_nodes[3]->info().topo_id]);

		var_face_topo_id->set(f.id(), att.topo_id);
		var_face_topo_dim->set(f.id(), att.topo_dim);
		var_face_geom_id->set(f.id(), att.geom_id);
		var_face_geom_dim->set(f.id(), att.geom_dim);
	}
	// blocks
	for (auto it = m_gmap.attributes<3>().begin(), itend = m_gmap.attributes<3>().end(); it != itend; ++it) {
		auto att = m_gmap.info_of_attribute<3>(it);
		std::vector<Node> cell_nodes = get_nodes_of_block(it);
		if (cell_nodes.size() != 8) {
			continue;
			throw GMDSException("Only hex blocks can be converted into mesh");
		}

		gmds::Region r = AMesh.newHex(n2n[cell_nodes[0]->info().topo_id], n2n[cell_nodes[1]->info().topo_id], n2n[cell_nodes[2]->info().topo_id],
		                              n2n[cell_nodes[3]->info().topo_id], n2n[cell_nodes[4]->info().topo_id], n2n[cell_nodes[5]->info().topo_id],
		                              n2n[cell_nodes[6]->info().topo_id], n2n[cell_nodes[7]->info().topo_id]);

		var_region_topo_id->set(r.id(), att.topo_id);
		var_region_topo_dim->set(r.id(), att.topo_dim);
		var_region_geom_id->set(r.id(), att.geom_id);
		var_region_geom_dim->set(r.id(), att.geom_dim);
	}
}

/*----------------------------------------------------------------------------*/
void Blocking::init_from_bounding_box()
{
	TCoord min[3] = {MAXFLOAT, MAXFLOAT, MAXFLOAT};
	TCoord max[3] = {-MAXFLOAT, -MAXFLOAT, -MAXFLOAT};
	std::vector<cad::GeomVolume *> vols;
	m_geom_model->getVolumes(vols);
	for (auto v : vols) {
		TCoord v_min[3], v_max[3];
		v->computeBoundingBox(v_min, v_max);
		for (auto i = 0; i < 3; i++)
			if (v_min[i] < min[i]) min[i] = v_min[i];
		for (auto i = 0; i < 3; i++)
			if (v_max[i] > max[i]) max[i] = v_max[i];
	}
	math::Point p1(min[0], min[1], min[2]);
	math::Point p2(min[0], max[1], min[2]);
	math::Point p3(max[0], max[1], min[2]);
	math::Point p4(max[0], min[1], min[2]);
	math::Point p5(min[0], min[1], max[2]);
	math::Point p6(min[0], max[1], max[2]);
	math::Point p7(max[0], max[1], max[2]);
	math::Point p8(max[0], min[1], max[2]);
	create_block(p1, p2, p3, p4, p5, p6, p7, p8);
}
/*----------------------------------------------------------------------------*/
void
Blocking::init_from_mesh(Mesh &AMesh)
{

	MeshModel model = AMesh.getModel();
	if (!model.has(N) || !model.has(R) || !model.has(R2N)) throw GMDSException("Wrong mesh model for mesh->block->mesh conversion");

	// map from gmap node ids to init mesh id
	std::map<TCellID, TCellID> n2n;
	for (auto r_id : AMesh.regions()) {
		Region r = AMesh.get<Region>(r_id);
		// only hex cells are converted
		if (r.type() == GMDS_HEX) {
			std::vector<gmds::Node> ns = r.get<gmds::Node>();
			Block b = create_block(ns[0].point(), ns[1].point(), ns[2].point(), ns[3].point(), ns[4].point(), ns[5].point(), ns[6].point(), ns[7].point());
			std::vector<Node> b_nodes = get_nodes_of_block(b);
			for (auto i = 0; i < 8; i++) {
				math::Point pi = ns[i].point();
				auto min_dist = pi.distance2(b_nodes[0]->info().point);
				auto min_id = b_nodes[0]->info().topo_id;
				for (auto j = 1; j < 8; j++) {
					auto dist_j = pi.distance2(b_nodes[j]->info().point);
					if (dist_j < min_dist) {
						min_dist = dist_j;
						min_id = b_nodes[j]->info().topo_id;
					}
				}
				n2n[min_id] = ns[i].id();
			}
		}
	}

	// now we glue blocks;
	for (auto it_r = m_gmap.attributes<3>().begin(), it_rend = m_gmap.attributes<3>().end(); it_r != it_rend; ++it_r) {
		std::vector<Face> cell_faces = get_faces_of_block(it_r);
		for (auto f : cell_faces) {
			Dart3 d = f->dart();
			if (m_gmap.is_free<3>(d)) {
				// means the face is not connected
				std::vector<Node> f_nodes = get_nodes_of_face(f);
				// We go through all the 3-free faces and try to connect to f
				bool found_and_glue = false;
				for (auto it_f = m_gmap.attributes<2>().begin(), it_fend = m_gmap.attributes<2>().end(); it_f != it_fend; ++it_f) {
					auto att = m_gmap.info_of_attribute<2>(it_f);
					Dart3 d2 = it_f->dart();
					if (d2 != d && m_gmap.is_free<3>(d2)) {
						// free dart and different face, we check if this face can be connected
						std::vector<Node> f2_nodes = get_nodes_of_face(it_f);
						// same nodes?
						bool one_missing = false;
						for (auto i = 0; i < f_nodes.size() && !one_missing; i++) {
							bool found_same = false;
							for (auto j = 0; j < f2_nodes.size() && !found_same; j++) {
								if (n2n[f_nodes[i]->info().topo_id] == n2n[f2_nodes[j]->info().topo_id]) {
									found_same = true;
								}
							}
							if (!found_same) one_missing = true;
						}

						if (!one_missing) {
							// we have the same ids, we need to glue in the right order
							found_and_glue = true;
							// Dart of the initial face d, and the face to glue d2
							TCellID ref_id = n2n[m_gmap.attribute<0>(d)->info().topo_id];
							TCellID ref_id_0 = n2n[m_gmap.attribute<0>(m_gmap.alpha<0>(d))->info().topo_id];

							// starting from d2, we move to find the one that correspond to ref_id
							Dart3 d_glue = d2;
							while (n2n[m_gmap.attribute<0>(d_glue)->info().topo_id] != ref_id) {
								d_glue = m_gmap.alpha<1, 0>(d_glue);
							}
							if (n2n[m_gmap.attribute<0>(m_gmap.alpha<0>(d_glue))->info().topo_id] == ref_id_0) {
								m_gmap.sew<3>(d_glue, d);
							}
							else {
								m_gmap.sew<3>(m_gmap.alpha<1>(d_glue), d);
							}
						}
					}
				}
			}
		}
	}
}

/*----------------------------------------------------------------------------*/
void
Blocking::save_vtk_blocking(const std::string &AFileName)
{
	Mesh m(gmds::MeshModel(gmds::DIM3 | gmds::N | gmds::E | gmds::F | gmds::R | gmds::E2N | gmds::F2N | gmds::R2N));
	convert_to_mesh(m);
	IGMeshIOService ios(&m);

	std::string block_filename = "block_"+AFileName + ".vtk";
	std::string edge_filename = "edge_"+AFileName + ".vtk";
	std::string face_filename = "face_"+AFileName + ".vtk";

	VTKWriter writer_block(&ios);
	writer_block.setCellOptions(N|R);
	writer_block.setDataOptions(N|R);
	writer_block.write(block_filename);

	VTKWriter writer_face(&ios);
	writer_face.setCellOptions(N|F);
	writer_face.setDataOptions(N|F);
	writer_face.write(face_filename);

	VTKWriter writer_edge(&ios);
	writer_edge.setCellOptions(N|E);
	writer_edge.setDataOptions(N|E);
	writer_edge.write(edge_filename);

}
/*----------------------------------------------------------------------------*/
void
Blocking::save_vtk_darts(const std::string &AFilename, int AToMark)
{
    const double ratio0_ = 0.9;
    const double ratio1_ = 0.9;
    const double ratio2_ = 0.9;
    const double ratio3_ = 0.85;
    const double display_alpha1_ = 0.1;
    const double display_alpha2_ = 0.4;
    const double display_alpha3_ = 0.5;

    GMap3::size_type mark = m_gmap.get_new_mark();

    // In some (all?) cases when in 2d a unique 3d cell exists and all darts self reference themselves by alpha3
    // check whether we have "real" 3d cells that we need to effectively write
    bool is2d = true;
    if(m_gmap.one_dart_per_cell<3>().size() == 1) {
        int index = 0;
        for(auto it = m_gmap.darts().begin(); it != m_gmap.darts().end(); ++it) {
            if(index != m_gmap.darts().index(m_gmap.get_alpha<3>(it))) {
                is2d = false;
                break;
            }
            index++;
        }
    } else {
        is2d = false;
    }

    // edges
    std::map<Dart3, std::pair<gmds::math::Point, gmds::math::Point> > dart_pos;
    std::map<Dart3, int > dart_node_ids;
    std::map<Dart3, int > dart_edge_ids;
    std::map<Dart3, int > dart_face_ids;
    std::map<Dart3, int > dart_region_ids;

    int index_node=0;
    for (auto it = m_gmap.one_dart_per_cell<0>().begin(); it != m_gmap.one_dart_per_cell<0>().end(); it++, index_node++) {

        Dart3 d = it;
        for(GMap3::Dart_of_orbit_range<1,2,3>::iterator
                    itbis(m_gmap.darts_of_orbit<1,2,3>(d).begin()),
                    itend(m_gmap.darts_of_orbit<1,2,3>(d).end()); itbis!=itend; ++itbis) {

            GMap3::Dart_handle dbis = itbis;
            dart_node_ids[dbis] = index_node;
        }
    }

    int index_edge=0;
    for (auto it = m_gmap.one_dart_per_cell<1>().begin(); it != m_gmap.one_dart_per_cell<1>().end(); it++, index_edge++) {

        Dart3 d = it;

        Blocking::Edge e = this->get_edge(m_gmap.attribute<1>(d)->info().topo_id);
        std::vector<Blocking::Node> cell_nodes = this->get_nodes_of_edge(e);
        gmds::math::Point barycenter;
        barycenter = 0.5 * (cell_nodes[0]->info().point + cell_nodes[1]->info().point);

        //		std::vector<Node> cell_nodes = get_nodes_of_edge(it);
        //		GMap3::Point barycenter = m_gmap.barycenter<1>(d);
        gmds::math::Point barycenter_gmds(barycenter.X(), barycenter.Y(), barycenter.Z());

        for(GMap3::Dart_of_orbit_range<0,2,3>::iterator
                    itbis(m_gmap.darts_of_orbit<0,2,3>(d).begin()),
                    itend(m_gmap.darts_of_orbit<0,2,3>(d).end()); itbis!=itend; ++itbis) {

            GMap3::Dart_handle d0 = itbis;

            if(!m_gmap.is_marked(d0, mark)) {
                GMap3::Dart_handle d1 = m_gmap.alpha(d0, 0);
                m_gmap.mark(d0, mark);
                m_gmap.mark(d1, mark);

                gmds::math::Point pt0_gmds = m_gmap.attribute<0>(d0)->info().point;
                gmds::math::Point pt1_gmds = m_gmap.attribute<0>(d1)->info().point;

                //				GMap3::Point pt0 = this->lcc()->vertex_attribute(d0)->point();
                //				GMap3::Point pt1 = this->lcc()->vertex_attribute(d1)->point();
                //				gmds::math::Point pt0_gmds(pt0.x(), pt0.y(), pt0.z());
                //				gmds::math::Point pt1_gmds(pt1.x(), pt1.y(), pt1.z());

                dart_pos.emplace(d0, std::pair<gmds::math::Point, gmds::math::Point> (pt0_gmds, pt0_gmds + ratio0_ * (barycenter_gmds - pt0_gmds)));
                dart_pos.emplace(d1, std::pair<gmds::math::Point, gmds::math::Point> (pt1_gmds, pt1_gmds + ratio0_ * (barycenter_gmds - pt1_gmds)));

                gmds::math::Point e0_0 = (1.- ratio1_) * (barycenter_gmds - dart_pos[d0].first) + dart_pos[d0].first;
                gmds::math::Point e0_1 = (1.- ratio1_) * (barycenter_gmds - dart_pos[d0].second) + dart_pos[d0].second;
                gmds::math::Point e1_0 = (1.- ratio1_) * (barycenter_gmds - dart_pos[d1].first) + dart_pos[d1].first;
                gmds::math::Point e1_1 = (1.- ratio1_) * (barycenter_gmds - dart_pos[d1].second) + dart_pos[d1].second;
                dart_pos[d0].first = e0_0;
                dart_pos[d0].second = e0_1;
                dart_pos[d1].first = e1_0;
                dart_pos[d1].second = e1_1;

                dart_edge_ids[d0] = index_edge;
                dart_edge_ids[d1] = index_edge;
            }
        }
    }

    int index_face=0;
    for (auto it = m_gmap.one_dart_per_cell<2>().begin(); it != m_gmap.one_dart_per_cell<2>().end(); it++, index_face++) {
        Dart3 d = it;

        if(dart_pos.find(d) == dart_pos.end()) {
            std::cout<<"a dart from a 2-cell was not previously treated with the 1-cells"<<std::endl;
        }


        Blocking::Face f = this->get_face(m_gmap.attribute<2>(d)->info().topo_id);
        std::vector<Blocking::Node> cell_nodes = this->get_nodes_of_face(f);
        gmds::math::Point barycenter;
        for(auto n: cell_nodes) {
            barycenter = barycenter + n->info().point;
        }
        barycenter = ((double) 1. / (double) cell_nodes.size()) * barycenter;
        //		GMap3::Point barycenter = lcc()->barycenter<2>(d);
        gmds::math::Point barycenter_gmds(barycenter.X(), barycenter.Y(), barycenter.Z());

        for(GMap3::Dart_of_orbit_range<0,1,3>::iterator
                    itbis(m_gmap.darts_of_orbit<0,1,3>(d).begin()),
                    itend(m_gmap.darts_of_orbit<0,1,3>(d).end()); itbis!=itend; ++itbis) {

            GMap3::Dart_handle dbis = itbis;

            if(dart_pos.find(dbis) == dart_pos.end()) {
                std::cout<<"a dart from a 2-cell was not previously treated with the 1-cells"<<std::endl;
            }

            gmds::math::Point e0_0 = (1.- ratio2_) * (barycenter_gmds - dart_pos[dbis].first) + dart_pos[dbis].first;
            gmds::math::Point e0_1 = (1.- ratio2_) * (barycenter_gmds - dart_pos[dbis].second) + dart_pos[dbis].second;
            dart_pos[dbis].first = e0_0;
            dart_pos[dbis].second = e0_1;
            dart_face_ids[dbis] = index_face;
        }
    }

    if(!is2d) {

        int index_region=0;
        for (auto it = m_gmap.one_dart_per_cell<3>().begin(); it != m_gmap.one_dart_per_cell<3>().end(); it++, index_region++) {

            Dart3 d = it;

            if(dart_pos.find(d) == dart_pos.end()) {
                std::cout<<"a dart from a 3-cell was not previously treated with the 1-cells"<<std::endl;
            }

            Blocking::Block b = this->get_block(m_gmap.attribute<3>(d)->info().topo_id);
            std::vector<Blocking::Node> cell_nodes = this->get_nodes_of_block(b);
            gmds::math::Point barycenter;
            for(auto n: cell_nodes) {
                barycenter = barycenter + n->info().point;
            }
            barycenter = ((double) 1. / (double) cell_nodes.size()) * barycenter;
            //			GMap3::Point barycenter = lcc()->barycenter<3>(d);
            gmds::math::Point barycenter_gmds(barycenter.X(), barycenter.Y(), barycenter.Z());

            for(GMap3::Dart_of_orbit_range<0,1,2>::iterator
                        itbis(m_gmap.darts_of_orbit<0,1,2>(d).begin()),
                        itend(m_gmap.darts_of_orbit<0,1,2>(d).end()); itbis!=itend; ++itbis) {

                GMap3::Dart_handle dbis = itbis;

                if(dart_pos.find(dbis) == dart_pos.end()) {
                    std::cout<<"a dart from a 3-cell was not previously treated with the 1-cells"<<std::endl;
                }

                gmds::math::Point e0_0 = (1.- ratio3_) * (barycenter_gmds - dart_pos[dbis].first) + dart_pos[dbis].first;
                gmds::math::Point e0_1 = (1.- ratio3_) * (barycenter_gmds - dart_pos[dbis].second) + dart_pos[dbis].second;
                dart_pos[dbis].first = e0_0;
                dart_pos[dbis].second = e0_1;

                dart_region_ids[dbis] = index_region;
            }
        }
    }

    m_gmap.free_mark(mark);

    // fill the gmds mesh
    // We create triangles because paraview seems to not be able to display edge cell types
    gmds::MeshModel model(gmds::MeshModel(gmds::DIM3|gmds::N|gmds::F|gmds::F2N));
    gmds::Mesh m(model);

    gmds::Variable<int>* var_node_type = m.newVariable<int, gmds::GMDS_NODE>("node_type");
    gmds::Variable<int>* var_edge_type = m.newVariable<int, gmds::GMDS_FACE>("edge_type");

    gmds::Variable<int>* var_edge_marked = m.newVariable<int, gmds::GMDS_FACE>("edge_marked");

    // trying to give an index to the i-cells
    gmds::Variable<int>* var_index_node_ids = m.newVariable<int, gmds::GMDS_FACE>("index_node_ids");
    gmds::Variable<int>* var_index_edge_ids = m.newVariable<int, gmds::GMDS_FACE>("index_edge_ids");
    gmds::Variable<int>* var_index_face_ids = m.newVariable<int, gmds::GMDS_FACE>("index_face_ids");
    gmds::Variable<int>* var_index_region_ids = m.newVariable<int, gmds::GMDS_FACE>("index_regione_ids");

    for(auto d: dart_pos) {
        gmds::Node n0 = m.newNode(d.second.first);
        gmds::Node n1 = m.newNode(d.second.second);
        gmds::Face f = m.newTriangle(n0, n1, n1);
        if(AToMark != -1) {
            if(m_gmap.is_marked(d.first, AToMark)) {
                (*var_edge_marked)[f.id()] = 1;
            }
        }
        (*var_node_type)[n0.id()] = 1;
        (*var_node_type)[n1.id()] = 0;
        (*var_edge_type)[f.id()] = 4;

        (*var_index_node_ids)[f.id()] = dart_node_ids[d.first];
        (*var_index_edge_ids)[f.id()] = dart_edge_ids[d.first];
        (*var_index_face_ids)[f.id()] = dart_face_ids[d.first];
        (*var_index_region_ids)[f.id()] = dart_region_ids[d.first];
    }

    // now for the alpha links
    // avoid creating the kinks twice by marking the darts
    std::set<Dart3> set_alpha0;
    std::set<Dart3> set_alpha1;
    std::set<Dart3> set_alpha2;
    std::set<Dart3> set_alpha3;

    for(auto d: dart_pos) {
        Dart3 dh = d.first;
        Dart3 d0 = m_gmap.alpha(dh, 0);
        Dart3 d1 = m_gmap.alpha(dh, 1);
        Dart3 d2 = m_gmap.alpha(dh, 2);
        Dart3 d3 = m_gmap.alpha(dh, 3);

        if(d0 != dh) {
            if(dart_pos.find(d0) != dart_pos.end()) {
                if(set_alpha0.find(dh) == set_alpha0.end()) {
                    gmds::Node n0 = m.newNode(d.second.second);
                    gmds::Node n1 = m.newNode(dart_pos[d0].second);
                    gmds::Face f = m.newTriangle(n0, n1, n1);
                    (*var_node_type)[n0.id()] = 0;
                    (*var_node_type)[n1.id()] = 0;
                    (*var_edge_type)[f.id()] = 0;

                    set_alpha0.insert(dh);
                    set_alpha0.insert(d0);

                    (*var_index_node_ids)[f.id()] = -1;
                    (*var_index_edge_ids)[f.id()] = -1;
                    (*var_index_face_ids)[f.id()] = -1;
                    (*var_index_region_ids)[f.id()] = -1;
                }
            }
        }
        if(d1 != dh) {
            if(dart_pos.find(d1) != dart_pos.end()) {
                if(set_alpha1.find(dh) == set_alpha1.end()) {
                    gmds::Node n0 = m.newNode(d.second.first + display_alpha1_ * (d.second.second - d.second.first));
                    gmds::Node n1 = m.newNode(dart_pos[d1].first + display_alpha1_ * (dart_pos[d1].second - dart_pos[d1].first));
                    gmds::Face f = m.newTriangle(n0, n1, n1);
                    (*var_node_type)[n0.id()] = 0;
                    (*var_node_type)[n1.id()] = 0;
                    (*var_edge_type)[f.id()] = 1;

                    set_alpha1.insert(dh);
                    set_alpha1.insert(d1);

                    (*var_index_node_ids)[f.id()] = -1;
                    (*var_index_edge_ids)[f.id()] = -1;
                    (*var_index_face_ids)[f.id()] = -1;
                    (*var_index_region_ids)[f.id()] = -1;
                }

            }
        }
        if(d2 != dh) {
            if(dart_pos.find(d2) != dart_pos.end()) {
                if(set_alpha2.find(dh) == set_alpha2.end()) {
                    gmds::Node n0 = m.newNode(d.second.first + display_alpha2_ * (d.second.second - d.second.first));
                    gmds::Node n1 = m.newNode(dart_pos[d2].first + display_alpha2_ * (dart_pos[d2].second - dart_pos[d2].first));
                    gmds::Face f = m.newTriangle(n0, n1, n1);
                    (*var_node_type)[n0.id()] = 0;
                    (*var_node_type)[n1.id()] = 0;
                    (*var_edge_type)[f.id()] = 2;

                    set_alpha2.insert(dh);
                    set_alpha2.insert(d2);

                    (*var_index_node_ids)[f.id()] = -1;
                    (*var_index_edge_ids)[f.id()] = -1;
                    (*var_index_face_ids)[f.id()] = -1;
                    (*var_index_region_ids)[f.id()] = -1;
                }

            }
        }
        if(d3 != dh) {
            if(dart_pos.find(d3) != dart_pos.end()) {
                if(set_alpha3.find(dh) == set_alpha3.end()) {
                    gmds::Node n0 = m.newNode(d.second.first + display_alpha3_ * (d.second.second - d.second.first));
                    gmds::Node n1 = m.newNode(dart_pos[d3].first + display_alpha3_ * (dart_pos[d3].second - dart_pos[d3].first));
                    gmds::Face f = m.newTriangle(n0, n1, n1);
                    (*var_node_type)[n0.id()] = 0;
                    (*var_node_type)[n1.id()] = 0;
                    (*var_edge_type)[f.id()] = 3;

                    set_alpha3.insert(dh);
                    set_alpha3.insert(d3);

                    (*var_index_node_ids)[f.id()] = -1;
                    (*var_index_edge_ids)[f.id()] = -1;
                    (*var_index_face_ids)[f.id()] = -1;
                    (*var_index_region_ids)[f.id()] = -1;
                }

            }
        }
    }

    gmds::IGMeshIOService ioService(&m);
    gmds::VTKWriter vtkWriter(&ioService);
    vtkWriter.setCellOptions(gmds::N | gmds::F);
    vtkWriter.setDataOptions(gmds::N | gmds::F);
    vtkWriter.write(AFilename);
}
/*----------------------------------------------------------------------------*/
std::vector<std::vector<Blocking::Edge>>
Blocking::get_all_sheet_edge_sets()
{
	std::vector<std::vector<Edge>> edges;
	std::vector<Edge> all_edges = get_all_edges();
	std::map<TCellID, bool> edge_done;
	for (auto e : all_edges) {
		edge_done[e->info().topo_id] = false;
	}

	bool remain_edge_to_do = true;
	while (remain_edge_to_do) {
		remain_edge_to_do = false;
		// we try and find the first that is not already put into a sheet set
		bool found_edge = false;
		auto edge_index = 0;
		for (auto i = 0; i < all_edges.size() && !found_edge; i++) {
			// we found an edge to treat
			if (edge_done[all_edges[i]->info().topo_id] == false) {
				found_edge = true;
				edge_index = i;
			}
		}
		if (found_edge) {
			// work to do, we will do another iteration
			remain_edge_to_do = true;
			std::vector<Edge> sh_edges;
			get_all_sheet_edges(all_edges[edge_index], sh_edges);
			// we store the sheet edges
			edges.push_back(sh_edges);
			// now we mark them as treated
			for (auto e : sh_edges) {
				edge_done[e->info().topo_id] = true;
			}
		}
	}
	return edges;
}

/*----------------------------------------------------------------------------*/
void
Blocking::get_all_sheet_edges(const Edge AE, std::vector<Edge> &AEdges)
{
	AEdges.clear();
	std::vector<Dart3> sheet_darts;
	get_all_sheet_darts(AE, sheet_darts);
	AEdges.resize(sheet_darts.size());
	for (auto i = 0; i < sheet_darts.size(); i++) {
		AEdges[i] = m_gmap.attribute<1>(sheet_darts[i]);
	}
}

/*----------------------------------------------------------------------------*/
void
Blocking::get_all_sheet_darts(const Edge AE, std::vector<Dart3> &ADarts)
{
	ADarts.clear();
	// we allocate a mark to know all the edges we go through
	auto edge_mark = m_gmap.get_new_mark();

	std::vector<Dart3> front;
	// For all the sheet operations, we orient the edges using its end point ids (from the lowest to the highest).
	// We order that here!!
	auto first_dart = AE->dart();
	if (m_gmap.attribute<0>(first_dart)->info().topo_id > m_gmap.attribute<0>(m_gmap.alpha<0>(first_dart))->info().topo_id)
		first_dart = m_gmap.alpha<0>(first_dart);

	front.push_back(first_dart);
	// the current dart belongs to the final set of darts
	ADarts.push_back(first_dart);
	// we mark all the dart of the inital edge to avoid to traverse it twice
	m_gmap.mark_cell<1>(AE->dart(), edge_mark);

	// Now we propagate along topological parallel edges in each adjacent hexahedral cell
	while (!front.empty()) {
		// we pick the last dart of the front
		Dart3 d = front.back();
		front.pop_back();

		// we traverse all the darts of the orbit<2,3> starting from d

		for (GMap3::Dart_of_orbit_range<2, 3>::iterator it(m_gmap.darts_of_orbit<2, 3>(d).begin()), itend(m_gmap.darts_of_orbit<2, 3>(d).end()); it != itend;
		     ++it) {
			auto d_next_edge = m_gmap.alpha<1, 0, 1>(it);
			if (!m_gmap.is_marked(d_next_edge, edge_mark)) {
				// it means that the edge containing the dart d_next_edge has not been traversed already.
				// We mark the dart of the corresponding edge, and we add it to the front
				front.push_back(d_next_edge);
				m_gmap.mark_cell<1>(d_next_edge, edge_mark);
				// We also add it to the set of darts to return
				ADarts.push_back(d_next_edge);
			}
		}
	}
	// We must unmark all the marked edges. As we stored one dart per edge, it is straightforward
	for (auto d : ADarts) {
		m_gmap.unmark_cell<1>(d, edge_mark);
	}
	m_gmap.free_mark(edge_mark);
}

/*----------------------------------------------------------------------------*/
std::vector<Blocking::Block>
Blocking::get_all_chord_blocks(const Face AF)
{
	std::vector<Blocking::Block> bls;
	auto block_mark = m_gmap.get_new_mark();

	std::vector<Dart3> face_darts;
	get_all_chord_darts(AF, face_darts);
	for (auto d : face_darts) {
		if (!m_gmap.is_marked(d, block_mark)) {
			bls.push_back(m_gmap.attribute<3>(d));
			m_gmap.mark_cell<3>(d, block_mark);
		}
		if (!m_gmap.is_marked(m_gmap.alpha<3>(d), block_mark)) {
			bls.push_back(m_gmap.attribute<3>(m_gmap.alpha<3>(d)));
			m_gmap.mark_cell<3>(m_gmap.alpha<3>(d), block_mark);
		}
	}

	// We must unmark all the marked edges. As we stored one dart per edge, it is straightforward
	for (auto b : bls) {
		m_gmap.unmark_cell<3>(b->dart(), block_mark);
	}
	m_gmap.free_mark(block_mark);
	return bls;
}

/*----------------------------------------------------------------------------*/
void
Blocking::get_all_chord_darts(const Face AF, std::vector<Dart3> &ADarts)
{
	ADarts.clear();
	// we allocate a mark to know all the faces we go through
	auto face_mark = m_gmap.get_new_mark();

	std::vector<Dart3> front;
	front.push_back(AF->dart());
	if (!m_gmap.is_free<3>(AF->dart())) front.push_back(m_gmap.alpha<3>(AF->dart()));

	// the current dart belongs to the final set of darts
	ADarts.push_back(AF->dart());
	// we mark all the dart of the initial face to avoid to traverse it twice
	m_gmap.mark_cell<2>(AF->dart(), face_mark);

	// Now we propagate along topological parallel faces in each adjacent block
	while (!front.empty()) {
		// we pick the last dart of the front
		Dart3 d = front.back();
		front.pop_back();

		auto d_next_face = m_gmap.alpha<2, 1, 0, 1, 2, 3>(d);
		if (!m_gmap.is_marked(d_next_face, face_mark)) {
			// it means that the edge containing the dart d_next_edge has not been traversed already.
			// We mark the dart of the corresponding edge, and we add it to the front
			front.push_back(d_next_face);
			m_gmap.mark_cell<2>(d_next_face, face_mark);
			// We also add it to the set of darts to return
			ADarts.push_back(d_next_face);
			if (!m_gmap.is_free<3>(d_next_face)) front.push_back(m_gmap.alpha<3>(d_next_face));
		}
	}
	// We must unmark all the marked edges. As we stored one dart per edge, it is straightforward
	for (auto d : ADarts) {
		m_gmap.unmark_cell<2>(d, face_mark);
	}
	m_gmap.free_mark(face_mark);
}

/*----------------------------------------------------------------------------*/
std::tuple<Blocking::Edge, double, double>
Blocking::get_cut_info(const int APointId,
                       const std::vector<Blocking::Edge> AEdges)
{
	auto point_to_capture = m_geom_model->getPoint(APointId);
	gmds::math::Point p(point_to_capture->X(), point_to_capture->Y(), point_to_capture->Z());
	return get_cut_info(p, AEdges);
}
/*----------------------------------------------------------------------------*/
std::tuple<Blocking::Edge, double, double>
Blocking::get_cut_info(const gmds::math::Point& APoint,
                       const std::vector<Blocking::Edge> AEdges)
{
	std::tuple<Blocking::Edge, double,double> cut_param;
	double min_dist = 1000;

	for (auto e : AEdges) {
		auto proj_info = get_projection_info(APoint, e);
		if (proj_info.second < 1 && proj_info.second > 0 && proj_info.first < min_dist) {
			cut_param = std::make_tuple(e, proj_info.second, proj_info.first);
			min_dist = proj_info.first;
		}
	}
	return cut_param;
}
/*----------------------------------------------------------------------------*/
void
Blocking::cut_sheet(const Edge AE)
{
	cut_sheet(AE, 0.5);
}

/*----------------------------------------------------------------------------*/
void
Blocking::cut_sheet(const Edge AE, const math::Point &AP)
{
	std::vector<Node> n = get_nodes_of_edge(AE);
	math::Point p0 = n[0]->info().point;
	math::Point p1 = n[1]->info().point;
	math::Segment s01(p0, p1);
	math::Point p = s01.project(AP);
	double param = math::Segment(p0, p).computeLength() / s01.computeLength();
	cut_sheet(AE, param);
}

/*----------------------------------------------------------------------------*/
void
Blocking::cut_sheet(const Edge AE, const double AParam)
{
	assert(AParam > 0 && AParam < 1);
	// Note: the parameterization starts from the first node of AE, which is the one having the node
	// of lowest topo id
	// We get a dart per sheet edge
	std::vector<Dart3> sheet_darts;
	get_all_sheet_darts(AE, sheet_darts);

	// then we insert a node in the middle of each edge, and we keep one of its darts
	std::vector<Dart3> in_edge_darts;
	auto mark_edge_darts = m_gmap.get_new_mark();

	for (auto d : sheet_darts) {
		Dart3 d0 = m_gmap.alpha<0>(d);
		math::Point pa = m_gmap.attribute<0>(d)->info().point;
		math::Point pb = m_gmap.attribute<0>(d0)->info().point;
		auto edge_att = m_gmap.attribute<1>(d)->info();

		Dart3 d_edge = m_gmap.insert_cell_0_in_cell_1(d);     // d_edge = alpha<0,1>(d)
		auto darts_23 = m_gmap.darts_of_orbit<2, 3>(d_edge);
		for (auto d_edge_23 = darts_23.begin(); d_edge_23 != darts_23.end(); d_edge_23++) {
			in_edge_darts.push_back(d_edge_23);
			m_gmap.mark(d_edge_23, mark_edge_darts);
		}

		math::Point pc = (1 - AParam) * pa + AParam * pb;
		if (edge_att.geom_dim == 1) {
			auto curve = m_geom_model->getCurve(edge_att.geom_id);
			curve->project(pc);
		}
		else if (edge_att.geom_dim == 2) {
			auto surf = m_geom_model->getSurface(edge_att.geom_id);
			surf->project(pc);
		}
		m_gmap.set_attribute<0>(d_edge, create_node(edge_att.geom_dim, edge_att.geom_id, pc));
	}

	// then we cut faces by inserting an edge
	auto mark_done = m_gmap.get_new_mark();
	std::vector<Dart3> in_face_darts;
	for (auto d : in_edge_darts) {
		if (!m_gmap.is_marked(d, mark_done)) {
			// as the node is not marked, it means we didn't insert an edge in this face
			// WARNING wrong pattern for self-intersecting sheet
			Dart3 d2 = m_gmap.alpha<0, 1, 0, 1, 0>(d);
			if (!m_gmap.is_marked(d2, mark_edge_darts)) {
				d2 = m_gmap.alpha<1, 0>(d2);
			}
			auto face_att = m_gmap.attribute<2>(d)->info();

			Dart3 d_face = m_gmap.insert_cell_1_in_cell_2(d, d2);

			m_gmap.set_attribute<1>(d_face, create_edge(face_att.geom_dim, face_att.geom_id));

			in_face_darts.push_back(d_face);
			m_gmap.mark(d, mark_done);
			m_gmap.mark(d2, mark_done);
			// the face can be shared by two regions and so, we must mark the right darts
			if (!m_gmap.is_free<3>(d)) {
				m_gmap.mark(m_gmap.alpha<3>(d), mark_done);
				m_gmap.mark(m_gmap.alpha<3>(d2), mark_done);
			}
		}
	}
	// We unmark all the darts, and we reuse the mark mark_done for faces now
	m_gmap.unmark_all(mark_done);

	// and finally, we cut blocks
	for (auto d : in_face_darts) {
		if (!m_gmap.is_marked(d, mark_done)) {
			// we get the loop of darts we are going to use to cut the block
			// one dart per edge must be given (see the implementation of insert_cell_2_in_cell_3 in CGAL)
			std::vector<Dart3> loop_darts;
			Dart3 current_dart = d;
			do {
				loop_darts.push_back(current_dart);
				m_gmap.mark(current_dart, mark_done);
				m_gmap.mark(m_gmap.alpha<0>(current_dart), mark_done);
				current_dart = m_gmap.alpha<0, 1, 2, 1>(current_dart);
			} while (current_dart != d);
			auto face_att = m_gmap.attribute<2>(d)->info();
			Dart3 d_block = m_gmap.insert_cell_2_in_cell_3(loop_darts.begin(), loop_darts.end());
			// a face is created by splitting a block, it is classified on a 3-dimensional geom cell
			m_gmap.set_attribute<2>(d_block, create_face(3, NullID));
		}
	}
	// we free the mark after unmarking all the darts. This is not optimal.
	m_gmap.unmark_all(mark_edge_darts);
	m_gmap.free_mark(mark_edge_darts);
	m_gmap.unmark_all(mark_done);
	m_gmap.free_mark(mark_done);
}
/*----------------------------------------------------------------------------*/
void Blocking::cut_sheet(const TCellID AnEdgeId, const double AParam)
{
	auto allEdges = get_all_edges();
	for (auto e : allEdges) {
		if (e->info().topo_id == AnEdgeId) {
			cut_sheet(e, AParam);
		}
	}
}
/*----------------------------------------------------------------------------*/
std::pair<double, double>
Blocking::get_projection_info(const math::Point &AP, Blocking::Edge &AEdge)
{
	std::pair<double, double> dist_coord;

		std::vector<Node> end_points = get_nodes_of_edge(AEdge);
		math::Point end0 = end_points[0]->info().point;
		math::Point end1 = end_points[1]->info().point;
		math::Vector3d v1 = end1 - end0;
		math::Vector3d v2 = AP - end0;
		double coord = 0.0;
		double distance = 0.0;
		auto a = v1.dot(v2);
		if (a <= 0.0) {
			coord = 0.0;
			distance = AP.distance(end0);
		}
		else {
			auto b = v1.dot(v1);
			if (a >= b) {
				coord = 1.0;
				distance = AP.distance(end1);
			}
			else {
				coord = a / b;
				distance = AP.distance(end0 + coord * v1);
			}
		}

	return std::make_pair(distance, coord);

}

/*----------------------------------------------------------------------------*/
std::vector<std::pair<double, double>>
Blocking::get_projection_info(const math::Point &AP, std::vector<Blocking::Edge> &AEdges)
{
	std::vector<std::pair<double, double>> dist_coord;
	for (auto e : AEdges) {
		dist_coord.push_back(get_projection_info(AP,e));
	}
	return dist_coord;
}
/*----------------------------------------------------------------------------*/
bool
Blocking::validate_pillowing_surface(std::vector<Face> &AFaces)
{
	auto mark_face = m_gmap.get_new_mark();
	for (auto f : AFaces) {
		m_gmap.mark_cell<2>(f->dart(), mark_face);
	}

	// Now, we have a manifold surface if for each face f of AFace, every edge adjacent to f:
	//  1. has exactly two adjacent faces in AF, OR
	//  2. has only one adjacent faces, but is classified on a curve or a surface
	bool found_error = false;
	for (int i_f = 0; i_f < AFaces.size() && !found_error; i_f++) {
		Face fi = AFaces[i_f];
		std::vector<Blocking::Edge> edges_of_fi = get_edges_of_face(fi);
		for (auto e : edges_of_fi) {
			std::vector<Blocking::Face> faces_of_e = get_faces_of_edge(e);
			int nb_faces_in_pillow_surface = 0;
			int nb_faces_in_pillow_surface_on_boundary = 0;
			for (auto f_of_e : faces_of_e) {
				if (m_gmap.is_marked(f_of_e->dart(), mark_face)) {
					nb_faces_in_pillow_surface++;
					if (f_of_e->info().geom_dim == 2) nb_faces_in_pillow_surface_on_boundary++;
				}
			}
			if (nb_faces_in_pillow_surface < 1 || nb_faces_in_pillow_surface > 2)
				found_error = true;
			else if (nb_faces_in_pillow_surface == 1) {
				// We have two cases
				// The face is on the boundary, so the edge must be on a curve
				if (nb_faces_in_pillow_surface_on_boundary == 1 && e->info().geom_dim != 1) found_error = true;
				// if the face is inside the volume, the edge must be on curve or a surface
				if (nb_faces_in_pillow_surface_on_boundary == 0 && !(e->info().geom_dim != 1 || e->info().geom_dim != 2)) found_error = true;
			}
			// else we have two faces, it is okay.
		}
	}

	for (auto f : AFaces) {
		m_gmap.unmark_cell<2>(f->dart(), mark_face);
	}
	m_gmap.free_mark(mark_face);

	return !found_error;
}
/*----------------------------------------------------------------------------*/
void
Blocking::mark_half_face(gecko::gblock::Dart3 ADart, int AMark)
{
	Dart3 d = ADart;
	do {
		m_gmap.mark(d, AMark);
		m_gmap.mark(m_gmap.alpha<0>(d), AMark);
		d = m_gmap.alpha<0, 1>(d);
	} while (d != ADart);
}
/*----------------------------------------------------------------------------*/
std::tuple<int, int, int, int>
Blocking::compute_pillow_twin_node(const Node &ANode, const int AMark)
{
	Dart3 d = ANode->dart();
	// We traverse the node set of darts to get a blue one
	bool found_blue_dart = false;
	Dart3 blue_dart;
	for (auto it(m_gmap.darts_of_orbit<1, 2, 3>(d).begin()), itend(m_gmap.darts_of_orbit<1, 2, 3>(d).end()); it != itend && !found_blue_dart; ++it) {
		if (m_gmap.is_marked(it, AMark)) {
			found_blue_dart = true;
			blue_dart = it;
		}
	}
	// we turn in the vertex of blue dart to count the number of blue faces adjacent to the node
	// we get one dart for each face (2) incident to the vertex (0) containing d
	auto nb_incident_blue_faces = 0;
	for (auto it = m_gmap.one_dart_per_incident_cell<2, 0>(blue_dart).begin(), itend = m_gmap.one_dart_per_incident_cell<2, 0>(blue_dart).end(); it != itend;
	     ++it) {

		auto fi = m_gmap.attribute<2>(it);
		if (m_gmap.is_marked(it, AMark) || m_gmap.is_marked(m_gmap.alpha<3>(it), AMark)) {
			nb_incident_blue_faces++;
		}
	}
	auto nb_incident_blue_blocks = 0;
	auto mark_block = m_gmap.get_new_mark();
	for (auto it = m_gmap.one_dart_per_incident_cell<3, 0>(blue_dart).begin(), itend = m_gmap.one_dart_per_incident_cell<3, 0>(blue_dart).end(); it != itend;
	     ++it) {
		// We have one dart of the block (the dart belongs to the vertex too
		auto found_blue_dart_in_block = false;
		m_gmap.mark_cell<3>(it, mark_block);
		for (auto it2 = m_gmap.one_dart_per_incident_cell<2, 0>(it).begin(), it2end = m_gmap.one_dart_per_incident_cell<2, 0>(it).end(); it2 != it2end; ++it2) {
			// the provided dart does not necessary belong to the block
			if (m_gmap.is_marked(it2, AMark) && m_gmap.is_marked(it2, mark_block)) {
				found_blue_dart_in_block = true;
			}
			else if (m_gmap.is_marked(m_gmap.alpha<3>(it2), AMark) && m_gmap.is_marked(m_gmap.alpha<3>(it2), mark_block)) {
				found_blue_dart_in_block = true;
			}
		}
		if (found_blue_dart_in_block) {
			nb_incident_blue_blocks++;
		}
		m_gmap.unmark_cell<3>(it, mark_block);
	}
	m_gmap.free_mark(mark_block);
	int cell_topo_dim = 0;
	int cell_topo_id = 0;
	int cell_geom_dim = 0;
	int cell_geom_id = 0;
	// We know the number of incident blue faces and blue blocks
	if ((nb_incident_blue_faces == 1 && nb_incident_blue_blocks == 1) || (nb_incident_blue_faces == 2 && nb_incident_blue_blocks == 2)
	    || (nb_incident_blue_faces == 3 && nb_incident_blue_blocks == 3) || (nb_incident_blue_faces > 3 && nb_incident_blue_blocks == nb_incident_blue_faces)) {
		// slide along a boundary edge
		// the blue dart belongs to the node, the face and block
		Dart3 edge_dart = m_gmap.alpha<2, 1>(blue_dart);
		auto edge_info = m_gmap.attribute<1>(edge_dart);
		cell_topo_dim = edge_info->info().topo_dim;
		cell_topo_id = edge_info->info().topo_id;
		cell_geom_dim = edge_info->info().geom_dim;
		cell_geom_id = edge_info->info().geom_id;
	}
	else if ((nb_incident_blue_faces == 2 && nb_incident_blue_blocks == 1) || (nb_incident_blue_faces == 4 && nb_incident_blue_blocks == 2)) {
		// slide along a  face
		// the blue dart belongs to the node, the face and block
		Dart3 face_dart = m_gmap.alpha<2>(blue_dart);
		if (m_gmap.is_marked(face_dart, AMark)) {
			// we pick the third face adjacent to the node and that belongs to the block
			face_dart = m_gmap.alpha<1, 2>(blue_dart);
		}
		auto face_info = m_gmap.attribute<2>(face_dart);
		cell_topo_dim = face_info->info().topo_dim;
		cell_topo_id = face_info->info().topo_id;
		cell_geom_dim = face_info->info().geom_dim;
		cell_geom_id = face_info->info().geom_id;
	}
	else if (nb_incident_blue_faces == 3 && nb_incident_blue_blocks == 1) {
		// get inside a block
		auto block_info = m_gmap.attribute<3>(blue_dart);
		cell_topo_dim = block_info->info().topo_dim;
		cell_topo_id = block_info->info().topo_id;
		cell_geom_dim = block_info->info().geom_dim;
		cell_geom_id = block_info->info().geom_id;
	}
	else {
		throw GMDSException("Unmanaged node configuration during the pillowing preparation");
	}

	return std::make_tuple(cell_topo_dim, cell_topo_id, cell_geom_dim, cell_geom_id);
}

/*----------------------------------------------------------------------------*/
bool
Blocking::pillow(std::vector<Face> &AFaces)
{
	bool is_input_ok = validate_pillowing_surface(AFaces);
	if (!is_input_ok) return false;

	auto mark_face = m_gmap.get_new_mark();
	for (auto f : AFaces) {
		m_gmap.mark_cell<2>(f->dart(), mark_face);
	}

	// Now, we have a manifold surface, we color each side of the surface with a different color
	auto mark_blue_side = m_gmap.get_new_mark();
	auto mark_red_side = m_gmap.get_new_mark();

	// we first color a seed face. If we have a volumetric face, we color it, otherwise, it will be a boundary face
	// and we will only use the blue color
	bool found_seed = false;
	bool only_boundary_faces = false;
	for (auto i_f = 0; i_f < AFaces.size() && !found_seed; i_f++) {
		Dart3 di = AFaces[i_f]->dart();
		if (!m_gmap.is_free<3>(di)) {
			// in-volume face
			found_seed = true;
			// We mark all darts of di side in blue
			mark_half_face(di, mark_blue_side);
			// and the opposite in red
			mark_half_face(m_gmap.alpha<3>(di), mark_red_side);
		}
	}
	if (!found_seed) {
		// means we only have boundary faces. We color all of them in blue
		only_boundary_faces = true;
		for (auto f : AFaces) {
			m_gmap.mark_cell<2>(f->dart(), mark_blue_side);
		}
	}
	else {
		bool all_sides_colored = false;
		// only one face has its two sides colored in red and blue
		while (!all_sides_colored) {
			all_sides_colored = true;
			for (auto f : AFaces) {
				auto d = f->dart();
				if (!m_gmap.is_marked(d, mark_blue_side) && !m_gmap.is_marked(d, mark_red_side)) {
					// this face is not colored
					all_sides_colored = false;
					// now we try to color it
					// For that we turn around all its edges to find
					Dart3 current_dart = d;

					auto color_current_face = false;
					do {
						// does a dart of the edge containing current dart is colored?
						Dart3 edge_dart = m_gmap.alpha<2>(current_dart);
						auto found_blue = false;
						auto found_red = false;
						do {
							if (m_gmap.is_marked(edge_dart, mark_blue_side)) {
								found_blue = true;
							}
							else if (m_gmap.is_marked(edge_dart, mark_red_side)) {
								found_red = true;
							}
							else {
								// we go to the next dart
								edge_dart = m_gmap.alpha<3, 2>(edge_dart);
							}
						} while (edge_dart != current_dart && edge_dart != m_gmap.alpha<2>(current_dart) && !found_blue && !found_red);
						// color the face?
						if (found_blue) {
							color_current_face = true;
							// We mark all darts of di side in blue
							mark_half_face(current_dart, mark_blue_side);
							// and the opposite in red (if it exists)
							if (!m_gmap.is_free<3>(current_dart)) {
								mark_half_face(m_gmap.alpha<3>(current_dart), mark_red_side);
							}
						}
						else if (found_red) {
							color_current_face = true;
							// We mark all darts of di side in red
							mark_half_face(current_dart, mark_red_side);
							// and the opposite in blue (if it exists)
							if (!m_gmap.is_free<3>(current_dart)) {
								mark_half_face(m_gmap.alpha<3>(current_dart), mark_blue_side);
							}
						}
						// we go to the next dart
						current_dart = m_gmap.alpha<0, 1>(current_dart);
					} while (current_dart != d && !color_current_face);
				}
			}
		}

		// We check the number of blue and red half-faces. We work with the blue half-faces only and their number
		// must be the greatest
		int nb_only_blue = 0, nb_only_red = 0, nb_blue_and_red = 0;
		for (auto f : AFaces) {
			Dart3 d = f->dart();
			if (m_gmap.is_free<3>(d)) {
				if (m_gmap.is_marked(d, mark_blue_side))
					nb_only_blue++;
				else if (m_gmap.is_marked(d, mark_red_side))
					nb_only_red++;
				else
					throw GMDSException("Error during blue-red coloring of face");
			}
			else if ((m_gmap.is_marked(d, mark_blue_side) && m_gmap.is_marked(m_gmap.alpha<3>(d), mark_red_side))
			         || (m_gmap.is_marked(d, mark_red_side) && m_gmap.is_marked(m_gmap.alpha<3>(d), mark_blue_side))) {
				nb_blue_and_red++;
			}
			else
				throw GMDSException("Error during blue-red coloring of face");
		}
		if (nb_only_blue < nb_only_red) {
			// we switch the blue and red mark, we work on blue only
			auto temp_mark = mark_blue_side;
			mark_blue_side = mark_red_side;
			mark_red_side = temp_mark;
		}
		else if (nb_only_red == nb_only_blue && nb_only_red != 0) {
			// we don't handle this situation right now, we have to quit the function
			// we clean boolean marks before leaving
			m_gmap.unmark_all(mark_face);
			m_gmap.unmark_all(mark_blue_side);
			m_gmap.unmark_all(mark_red_side);
			m_gmap.free_mark(mark_face);
			m_gmap.free_mark(mark_blue_side);
			m_gmap.free_mark(mark_red_side);
			return false;
		}
	}

	// Now we pillow towards blue side
	bool non_symetric_blue_red = false;
	// we get all the nodes and edges of the faces to pillow
	std::set<Node> surf_nodes;
	std::set<Edge> surf_edges;
	for (auto f : AFaces) {
		std::vector<Node> nodes_of_f = get_nodes_of_face(f);
		for (auto n : nodes_of_f)
			surf_nodes.insert(n);
		std::vector<Edge> edges_of_f = get_edges_of_face(f);
		for (auto e : edges_of_f)
			surf_edges.insert(e);
	}
	// now for each surface node, we construct the data that will help us to move it afterward
	std::map<Dart3, std::tuple<int, int, int, int>> pillow_node_data;
	for (auto n : surf_nodes) {
		auto pillow_data = compute_pillow_twin_node(n, mark_blue_side);
		for (GMap3::Dart_of_orbit_range<1, 2, 3>::iterator it(m_gmap.darts_of_orbit<1, 2, 3>(n->dart()).begin()),
		     itend(m_gmap.darts_of_orbit<1, 2, 3>(n->dart()).end());
		     it != itend; ++it) {
			pillow_node_data[it] = pillow_data;
		}
	}
	// We deduce from this information each new edge classification (nothing done right now for
	// faces that become in-volume)
	// For each old edge id, we keep in mind the geometric data for the new edge
	std::map<Dart3, std::pair<int, int>> pillow_edge_data;

	for (auto e : surf_edges) {
		Dart3 d_edge = e->dart();
		auto [n0_tdim, n0_tid, n0_gdim, n0_gid] = pillow_node_data[d_edge];
		auto [n1_tdim, n1_tid, n1_gdim, n1_gid] = pillow_node_data[m_gmap.alpha<0>(d_edge)];
		//     std::cout << "(" << n0_gdim << ", " << n0_gid << ") - (" << n1_gdim << ", " << n1_gid << ")" << std::endl;
		auto edge_dim = 4;
		auto edge_id = NullID;
		if (n0_gdim == n1_gdim && n1_gid == n0_gid) {
			// the 2 nodes are moved on the same entity, and so the edge
			edge_dim = n0_gdim;
			edge_id = n0_gid;
		}
		else if (n0_gdim == 2 && n1_gdim == 2 && n1_gid != n0_gid) {
			// the 2 nodes are moved onto different surfaces, the edge is in a volume
			edge_dim = 3;
			edge_id = NullID;
		}
		else if (n0_gdim == 1 && n1_gdim == 1 && n1_gid != n0_gid) {
			// the 2 nodes are moved along different curves, we need the common surface
			// We need to find this surface!
			cad::GeomCurve *c0 = m_geom_model->getCurve(n0_gid);
			cad::GeomCurve *c1 = m_geom_model->getCurve(n1_gid);
			auto s01_id = m_geom_model->getCommonSurface(c0, c1);
			edge_dim = 2;
			edge_id = s01_id;
		}
		else if (n0_gdim < n1_gdim) {
			// the edge is on the n1 entity
			edge_dim = n1_gdim;
			edge_id = n1_gid;
		}
		else if (n0_gdim > n1_gdim) {
			// the edge is on the n0 entity
			edge_dim = n0_gdim;
			edge_id = n0_gid;
		}
		for (GMap3::Dart_of_orbit_range<0, 2, 3>::iterator it(m_gmap.darts_of_orbit<0, 2, 3>(d_edge).begin()),
		     itend(m_gmap.darts_of_orbit<0, 2, 3>(d_edge).end());
		     it != itend; ++it) {
			pillow_edge_data[it] = std::make_pair(edge_dim, edge_id);
			//         std::cout << "Edge data (" << edge_dim << ", " << edge_id << ")" << std::endl;
		}
	}

	// We add a mark in order to avoid to reassign node attributes to many times. Moreover, the key of the
	//  pillow_node_data map is the topo node id that will likely change afet a 3-sew (since node attributes will be
	//  merged)

	// We are going first by unsewing some darts and keeping the info
	std::vector<Dart3> blue_darts;
	for (auto f : AFaces) {
		Dart3 df = f->dart();
		// We are going to duplicate this face
		if (m_gmap.is_marked(df, mark_blue_side))
			blue_darts.push_back(df);
		else
			blue_darts.push_back(m_gmap.alpha<3>(df));
	}
	// we go through blue darts only
	// for each blue dart, we keep in mind if it was sewed and we unsew if necessary
	std::map<Dart3, bool> was_sewed;
	std::map<Dart3, Dart3> prev_3_dart;

	for (auto d : blue_darts) {
		if (m_gmap.is_free<3>(d)) {
			was_sewed[d] = false;
		}
		else {
			was_sewed[d] = true;
			prev_3_dart[d] = m_gmap.alpha<3>(d);
			m_gmap.unsew<3>(d);
		}
	}

	auto mark_already_merged_node = m_gmap.get_new_mark();
	auto mark_already_merged_edge = m_gmap.get_new_mark();
	for (auto df : blue_darts) {
		// We are going to duplicate this face
		Face f = m_gmap.attribute<2>(df);
		// df is blue
		std::vector<Node> nodes_of_f = get_nodes_of_face(f);
		Block b = create_block(nodes_of_f[0]->info().point, nodes_of_f[1]->info().point, nodes_of_f[2]->info().point, nodes_of_f[3]->info().point,
		                       nodes_of_f[0]->info().point, nodes_of_f[1]->info().point, nodes_of_f[2]->info().point, nodes_of_f[3]->info().point);
		Dart3 db = b->dart();
		// we must find the dart to 3-sew with df
		math::Point df_point = m_gmap.attribute<0>(df)->info().point;
		double dist_min = df_point.distance2(m_gmap.attribute<0>(db)->info().point);
		Dart3 db_min = db;
		Dart3 db_cur = m_gmap.alpha<0, 1>(db);
		while (db_cur != db) {
			double dist_cur = df_point.distance2(m_gmap.attribute<0>(db_cur)->info().point);
			if (dist_cur < dist_min) {
				dist_min = dist_cur;
				db_min = db_cur;
			}
			db_cur = m_gmap.alpha<0, 1>(db_cur);
		}
		// We must now to check if df must be sewed to db_min or a<1>(db_min)
		math::Point pnt_df0 = m_gmap.attribute<0>(m_gmap.alpha<0>(df))->info().point;
		math::Point pnt_db_min0 = m_gmap.attribute<0>(m_gmap.alpha<0>(db_min))->info().point;
		math::Point pnt_db_min10 = m_gmap.attribute<0>(m_gmap.alpha<1, 0>(db_min))->info().point;
		if (pnt_db_min10.distance2(pnt_df0) < pnt_db_min0.distance2(pnt_df0)) {
			// switch the edge s
			db_min = m_gmap.alpha<1>(db_min);
		}

		// we copy the face attribute
		Dart3 d_opp_face = m_gmap.alpha<2, 1, 0, 1, 2>(db_min);
		if (was_sewed[df]) {
			m_gmap.sew<3>(d_opp_face, prev_3_dart[df]);
		}
		else {
			m_gmap.attribute<2>(d_opp_face)->info().geom_dim = f->info().geom_dim;
			m_gmap.attribute<2>(d_opp_face)->info().geom_id = f->info().geom_id;
		}
		m_gmap.sew<3>(df, db_min);
		m_gmap.attribute<2>(df)->info().geom_dim = 3;
		m_gmap.attribute<2>(df)->info().geom_id = NullID;

		// after sewing, we change classification info
		Dart3 d = df;
		do {
			auto att_node_d = m_gmap.attribute<0>(d);
			auto att_edge_d = m_gmap.attribute<1>(d);

			if (!m_gmap.is_marked(d, mark_already_merged_node)) {
				m_gmap.mark_cell<0>(d, mark_already_merged_node);

				// the opposite node keep the previous embedding
				Dart3 d_opp = m_gmap.alpha<3, 2, 1, 0, 1, 2>(d);
				m_gmap.attribute<0>(d_opp)->info().geom_id = att_node_d->info().geom_id;
				m_gmap.attribute<0>(d_opp)->info().geom_dim = att_node_d->info().geom_dim;

				// the current node is embedded onto the detected geom feature
				auto [tdim, tid, gdim, gid] = pillow_node_data[d];
				m_gmap.attribute<0>(d)->info().geom_dim = gdim;
				m_gmap.attribute<0>(d)->info().geom_id = gid;

				// the edge connected the two nodes is classified onto the same feature
				Dart3 d_edge = m_gmap.alpha<3, 2, 1>(d);
				m_gmap.attribute<1>(d_edge)->info().geom_dim = gdim;
				m_gmap.attribute<1>(d_edge)->info().geom_id = gid;
			}

			if (m_gmap.is_marked(d, mark_already_merged_edge)) {
				// means the corresponding block must be 3-sewed to another block in the inserted sheet
				// From d, we lock for a blue dart in the edge orbit
				Dart3 d3 = m_gmap.alpha<2>(d);
				bool found = false;
				do {
					if (m_gmap.is_marked(d3, mark_blue_side)) {
						found = true;
					}
					else {
						d3 = m_gmap.alpha<3, 2>(d3);
					}
				} while (!found);
				m_gmap.sew<3>(m_gmap.alpha<3, 2>(d), m_gmap.alpha<3, 2>(d3));
			}
			else {
				m_gmap.mark_cell<1>(d, mark_already_merged_edge);
				auto [e_geom_dim, e_geom_id] = pillow_edge_data[d];

				// the opposite edge keep the previous embedding
				Dart3 d_opp = m_gmap.alpha<3, 2, 1, 0, 1, 2>(d);
				m_gmap.attribute<1>(d_opp)->info().geom_id = att_edge_d->info().geom_id;
				m_gmap.attribute<1>(d_opp)->info().geom_dim = att_edge_d->info().geom_dim;

				// the current edge is embedded onto the detected geom feature
				//  std::cout<<"Edge "<<m_gmap.attribute<1>(d)->info().topo_id<<" on: ("<<e_geom_dim<<", "<<e_geom_id<<")"<<std::endl;
				m_gmap.attribute<1>(d)->info().geom_dim = e_geom_dim;
				m_gmap.attribute<1>(d)->info().geom_id = e_geom_id;

				// the face connected the two edge is classified onto the same feature
				Dart3 d_face = m_gmap.alpha<3, 2>(d);
				m_gmap.attribute<2>(d_face)->info().geom_dim = e_geom_dim;
				m_gmap.attribute<2>(d_face)->info().geom_id = e_geom_id;
			}
			d = m_gmap.alpha<0, 1>(d);
		} while (d != df);
	}

	//-------------------------------------------------------
	// clean marks

	m_gmap.unmark_all(mark_face);
	m_gmap.unmark_all(mark_blue_side);
	m_gmap.unmark_all(mark_red_side);
	m_gmap.unmark_all(mark_already_merged_node);
	m_gmap.unmark_all(mark_already_merged_edge);

	m_gmap.free_mark(mark_face);
	m_gmap.free_mark(mark_blue_side);
	m_gmap.free_mark(mark_red_side);
	m_gmap.free_mark(mark_already_merged_node);
	m_gmap.free_mark(mark_already_merged_edge);
	/*   std::cout << "============== Check final  =====================" << std::endl;
	   for (auto it = m_gmap.attributes<0>().begin(), itend = m_gmap.attributes<0>().end(); it != itend; ++it) {
	       std::cout << "Node " << it->info().topo_id << " geom info (" << it->info().geom_dim << ", "
	                 << it->info().geom_id << ")" << std::endl;
	   }
	   for (auto it = m_gmap.attributes<1>().begin(), itend = m_gmap.attributes<1>().end(); it != itend; ++it) {
	       std::cout << "Edge " << it->info().topo_id << " geom info (" << it->info().geom_dim << ", "
	                 << it->info().geom_id << ")" << std::endl;
	   }
	   std::cout << "=================================================" << std::endl;
	  */
	return true;
}

/*----------------------------------------------------------------------------*/
void
Blocking::smooth(const int ANbIterations)
{
	for (auto i = 0; i < ANbIterations; i++) {
		smooth_curves(1);
		smooth_surfaces(1);
		smooth_volumes(1);
	}
}

/*----------------------------------------------------------------------------*/
void
Blocking::smooth_curves(const int ANbIterations)
{
	// We traverse all the nodes of the block structure and we keep in mind, those
	// that are on curves
	std::vector<Node> to_smooth;
	for (auto it = m_gmap.attributes<0>().begin(), itend = m_gmap.attributes<0>().end(); it != itend; ++it) {
		if (it->info().geom_dim == 1) {
			// node on a curve
			to_smooth.push_back(it);
		}
	}
	for (auto n : to_smooth) {
		// We get the location of each adjacent node connected through a 1-classified edge
		Dart3 d = n->dart();
		math::Point p = n->info().point;
		math::Point deviation(0, 0, 0);
		auto ball_size = 0;
		// We get one dart per 1-cell adjacent to the 0-cell that contains d
		for (auto it = m_gmap.one_dart_per_incident_cell<1, 0>(d).begin(), itend = m_gmap.one_dart_per_incident_cell<1, 0>(d).end(); it != itend; ++it) {
			Edge e = m_gmap.attribute<1>(it);
			if (e->info().geom_dim == 1) {
				// we keep the opposite node
				Dart3 d_opp = m_gmap.alpha<0>(it);
				deviation = deviation + m_gmap.attribute<0>(d_opp)->info().point;
				ball_size++;
			}
		}
		if (ball_size != 0) {
			// we mode n
			math::Point p_new = 1.0 / (double) ball_size * deviation;
			p = 0.5 * p + 0.5 * p_new;
			move_node(n, p);
		}
	}
}

/*----------------------------------------------------------------------------*/
void
Blocking::smooth_surfaces(const int ANbIterations)
{
	// We traverse all the nodes of the block structure and we keep in mind, those
	// that are on surfaces
	std::vector<Node> to_smooth;
	for (auto it = m_gmap.attributes<0>().begin(), itend = m_gmap.attributes<0>().end(); it != itend; ++it) {
		if (it->info().geom_dim == 2) {
			// node on a surface
			to_smooth.push_back(it);
		}
	}
	for (auto n : to_smooth) {
		// We get the location of each adjacent node connected through a 2-classified edge
		Dart3 d = n->dart();
		math::Point p = n->info().point;
		math::Point deviation(0, 0, 0);
		auto ball_size = 0;
		// We get one dart per 1-cell adjacent to the 0-cell that contains d
		for (auto it = m_gmap.one_dart_per_incident_cell<1, 0>(d).begin(), itend = m_gmap.one_dart_per_incident_cell<1, 0>(d).end(); it != itend; ++it) {
			Edge e = m_gmap.attribute<1>(it);
			if (e->info().geom_dim == 2) {
				// we keep the opposite node
				Dart3 d_opp = m_gmap.alpha<0>(it);
				deviation = deviation + m_gmap.attribute<0>(d_opp)->info().point;
				ball_size++;
			}
		}
		if (ball_size != 0) {
			// we mode n
			math::Point p_new = 1.0 / (double) ball_size * deviation;
			p = 0.5 * p + 0.5 * p_new;
			move_node(n, p);
		}
	}
}

/*----------------------------------------------------------------------------*/
void
Blocking::smooth_volumes(const int ANbIterations)
{
	// We traverse all the nodes of the block structure and we keep in mind, those
	// that are in volume
	std::vector<Node> to_smooth;
	for (auto it = m_gmap.attributes<0>().begin(), itend = m_gmap.attributes<0>().end(); it != itend; ++it) {
		if (it->info().geom_dim == 3) {
			// node on a surface
			to_smooth.push_back(it);
		}
	}
	for (auto n : to_smooth) {
		// We get the location of each adjacent node connected through a 2-classified edge
		Dart3 d = n->dart();
		math::Point p = n->info().point;
		math::Point deviation(0, 0, 0);
		auto ball_size = 0;
		// We get one dart per 1-cell adjacent to the 0-cell that contains d
		for (auto it = m_gmap.one_dart_per_incident_cell<1, 0>(d).begin(), itend = m_gmap.one_dart_per_incident_cell<1, 0>(d).end(); it != itend; ++it) {
			Edge e = m_gmap.attribute<1>(it);
			if (e->info().geom_dim == 3) {
				// we keep the opposite node
				Dart3 d_opp = m_gmap.alpha<0>(it);
				deviation = deviation + m_gmap.attribute<0>(d_opp)->info().point;
				ball_size++;
			}
		}
		if (ball_size != 0) {
			// we mode n
			math::Point p_new = 1.0 / (double) ball_size * deviation;
			p = 0.5 * p + 0.5 * p_new;
			move_node(n, p);
		}
	}
}

/*----------------------------------------------------------------------------*/
bool
Blocking::collapse_chord(const Face AF, const Node AN1, const Node AN2)
{
	// first we check that AN1 and AN2 belongs to AF and are opposite in AF
	Dart3 dart_f = AF->dart();
	Dart3 dart_n1 = AN1->dart();
	Dart3 dart_n2 = AN2->dart();
	// Are n1 and n2 in the face?
	bool found_n1 = false, found_n2 = false;
	std::vector<Dart3> all_darts_n1, all_darts_n2;
	for (GMap3::Dart_of_orbit_range<1, 2, 3>::iterator it(m_gmap.darts_of_orbit<1, 2, 3>(dart_n1).begin()), itend(m_gmap.darts_of_orbit<1, 2, 3>(dart_n1).end());
	     it != itend; ++it) {
		all_darts_n1.push_back(it);
	}
	for (GMap3::Dart_of_orbit_range<1, 2, 3>::iterator it(m_gmap.darts_of_orbit<1, 2, 3>(dart_n2).begin()), itend(m_gmap.darts_of_orbit<1, 2, 3>(dart_n2).end());
	     it != itend; ++it) {
		all_darts_n2.push_back(it);
	}
	// We traverse all the darts of the face
	for (GMap3::Dart_of_orbit_range<0, 1, 3>::iterator it(m_gmap.darts_of_orbit<0, 1, 3>(dart_f).begin()), itend(m_gmap.darts_of_orbit<0, 1, 3>(dart_f).end());
	     it != itend; ++it) {
		// Now we go through the dart of the current node
		for (auto d : all_darts_n1) {
			if (it == d) found_n1 = true;
		}
		for (auto d : all_darts_n2) {
			if (it == d) found_n2 = true;
		}
	}
	if (!found_n1 || !found_n2) return false;

	// Are n1 and n2 opposite in the face ?
	bool n1_is_n2_opposite = false;
	for (auto d2 : all_darts_n2) {
		for (auto d1 : all_darts_n1) {
			if (m_gmap.alpha<0, 1, 0>(d1) == d2) n1_is_n2_opposite = true;
		}
	}
	if (!n1_is_n2_opposite) return false;

	// We need the pair of nodes to collapse in each face. To do that, we iteratively work in each
	// face traversed by the sheet
	std::vector<Dart3> chord_darts, chord_darts_opp;
	get_all_chord_darts(AF, chord_darts);
	for (auto d : chord_darts) {
		chord_darts_opp.push_back(m_gmap.alpha<0, 1, 0>(d));
	}

	// Before going further, we check that all the pair of opposite nodes can be merged (geometric conditions)
	for (auto i = 0; i < chord_darts.size(); i++) {
		Node ni = m_gmap.attribute<0>(chord_darts[i]);
		Node nj = m_gmap.attribute<0>(chord_darts_opp[i]);
		if (ni->info().geom_dim == 0 && nj->info().geom_dim == 0) {
			// two nodes can not be merged together
			return false;
		}
		else if (ni->info().geom_dim == 1 && nj->info().geom_dim == 1 && ni->info().geom_id != nj->info().geom_id) {
			// two nodes on different curves cannot be merged
			return false;
		}
		else if (ni->info().geom_dim == 2 && nj->info().geom_dim == 2 && ni->info().geom_id != nj->info().geom_id) {
			// two nodes on different surfaces cannot be merged
			return false;
		}
	}
	// Now we have pairs of darts that belongs to opposite nodes that must be collapsed. We get nodes of adjacent faces
	// to 3-sew
	std::vector<std::pair<Dart3, Dart3>> to_sew;
	for (auto i = 0; i < chord_darts.size(); i++) {
		Dart3 di = chord_darts[i];
		Dart3 dj = chord_darts_opp[i];
		to_sew.push_back(std::make_pair(m_gmap.alpha<2, 3>(di), m_gmap.alpha<2, 3>(dj)));
		to_sew.push_back(std::make_pair(m_gmap.alpha<1, 2, 3>(di), m_gmap.alpha<1, 2, 3>(dj)));
		if (!m_gmap.is_free<3>(di)) {
			to_sew.push_back(std::make_pair(m_gmap.alpha<3, 2, 3>(di), m_gmap.alpha<3, 2, 3>(dj)));
			to_sew.push_back(std::make_pair(m_gmap.alpha<3, 1, 2, 3>(di), m_gmap.alpha<3, 1, 2, 3>(dj)));
		}
	}
	// We remove chord blocks
	std::vector<Block> chord_blocks = get_all_chord_blocks(AF);
	for (auto b : chord_blocks) {
		remove_block(b);
	}
	// and now, we fill the void by sewing darts
	for (auto dd : to_sew) {
		if (m_gmap.is_free<3>(dd.first)) {
			m_gmap.sew<3>(dd.first, dd.second);
		}
	}
	return true;
}
