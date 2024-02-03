#include <string>
#include <algorithm>
#include <unordered_map>
#include <iostream>
#include <cmath>

#include "mesher_interface.h"
#include "helpers.h"

void mesher_interface::initialize() {
	gmsh::initialize();
}

int mesher_interface::import_model(const std::string& filename)
{
	std::vector<std::pair<int, int> > v;
	gmsh::model::occ::importShapes(filename, v);
	gmsh::model::occ::synchronize();
	return v[0].second;
}

void mesher_interface::mesh_model(int mesh_size_min, int mesh_size_max)
{
	gmsh::option::setNumber("Mesh.MeshSizeMin", mesh_size_min);
	gmsh::option::setNumber("Mesh.MeshSizeMax", mesh_size_max);
	gmsh::model::mesh::generate(3);
	gmsh::model::mesh::removeDuplicateNodes();
	gmsh::model::mesh::removeDuplicateElements();
	gmsh::model::mesh::createEdges();
	gmsh::model::mesh::createFaces();
}

void mesher_interface::view_model()
{
	gmsh::fltk::run();
}

box mesher_interface::get_bounding_box(int dim, int tag)
{
	double xmin = 0, ymin = 0, zmin = 0, xmax = 0, ymax = 0, zmax = 0;
	try
	{
		gmsh::model::getBoundingBox(dim, tag, xmin, ymin, zmin, xmax, ymax, zmax);
	}
	catch (...)
	{
	}
	return box(xmin, ymin, zmin, xmax, ymax, zmax);
}

std::vector<box> mesher_interface::get_bounding_box(int dim, std::vector<int> tags)
{
	std::vector<box> boxes;
	for (auto t : tags)
	{
		boxes.push_back(get_bounding_box(dim, t));
	}
	return boxes;
}

box mesher_interface::get_bounding_box()
{
	return get_bounding_box(-1, -1);
}

int mesher_interface::add_box(box b)
{
	int id = gmsh::model::occ::addBox(b.xmin, b.ymin, b.zmin, b.xmax - b.xmin, b.ymax - b.ymin, b.zmax - b.zmin);
	gmsh::model::occ::synchronize();
	return id;
}

int mesher_interface::subtract(int id1, int id2, bool remove_tool)
{
	std::vector<std::pair<int, int> > ov;
	std::vector<std::vector<std::pair<int, int> > > ovv;
	gmsh::model::occ::cut({ {3, id1} }, { {3, id2} }, ov, ovv, -1, true, remove_tool);
	gmsh::model::occ::synchronize();
	return ov[0].second;
}

std::vector<node> mesher_interface::get_all_nodes()
{
	std::vector<size_t> nodeTags;
	std::vector<double> coord;
	std::vector<double> parametric_coord;
	gmsh::model::mesh::getNodes(nodeTags, coord, parametric_coord, -1, -1, false, false);

	std::vector<node> nodes_to_return(nodeTags.size());

	int i = 0;
	for (auto n : nodeTags) {
		node nn{ { coord[3 * i], coord[3 * i + 1], coord[3 * i + 2] }, {}, FREE_NODE, FREE_NODE, {} };
		nodes_to_return[n - 1] = nn;
		i++;
	}

	return nodes_to_return;
}

std::vector<size_t> mesher_interface::get_node_ids_in_volume(int id)
{
	std::vector<size_t> nodeTags4;
	std::vector<double> coord4;
	std::vector<double> parametric_coord4;
	gmsh::model::mesh::getNodes(nodeTags4, coord4, parametric_coord4, 3, id, true, false);
	return nodeTags4;
}

void mesher_interface::label_boundary_nodes(std::vector<node>& nodes, std::vector<int> surface_ids)
{
	for (auto id : surface_ids)
	{
		std::vector<size_t> nodeTags;
		std::vector<double> coord;
		std::vector<double> parametric_coord;
		gmsh::model::mesh::getNodes(nodeTags, coord, parametric_coord, 2, id, true, false);

		for (auto n : nodeTags)
		{
			nodes[n - 1].type_3d = BOUNDARY_NODE;
			nodes[n - 1].type_2d = BOUNDARY_NODE;
			nodes[n - 1].surface_entities.push_back(id);
		}

		std::vector<size_t> nodeTags_boundary;
		std::vector<double> coord_boundary;
		std::vector<double> parametric_coord_boundary;
		gmsh::model::mesh::getNodes(nodeTags_boundary, coord_boundary, parametric_coord_boundary, 2, id, false, false);

		for (auto n : nodeTags_boundary) nodes[n - 1].type_2d = FREE_NODE;
	}
}

tet mesher_interface::assemble_tet(size_t n1, size_t n2, size_t n3, size_t n4)
{
	std::array<size_t, 4> nodes{ n1, n2, n3, n4 };
	std::sort(nodes.begin(), nodes.end());

	std::vector<size_t> edge_tags;
	std::vector<int> edge_orientations;
	gmsh::model::mesh::getEdges(
		{
			nodes[0], nodes[1],
			nodes[0], nodes[2],
			nodes[0], nodes[3],
			nodes[1], nodes[2],
			nodes[1], nodes[3],
			nodes[2], nodes[3]
		},
		edge_tags, edge_orientations);

	std::vector<size_t> face_tags;
	std::vector<int> face_orientations;
	gmsh::model::mesh::getFaces(3,
		{
			nodes[0], nodes[1], nodes[2],
			nodes[0], nodes[1], nodes[3],
			nodes[0], nodes[2], nodes[3],
			nodes[1], nodes[2], nodes[3]
		},
		face_tags, face_orientations);

	return { nodes,
			{edge_tags[0], edge_tags[1], edge_tags[2], edge_tags[3], edge_tags[4], edge_tags[5]},
			{face_tags[0], face_tags[1], face_tags[2], face_tags[3], },
			0 };
}

std::vector<tet> mesher_interface::get_volume_elems(int id)
{
	std::vector<int> elementTypes;
	std::vector<std::vector<std::size_t>> elementTags;
	std::vector<std::vector<std::size_t>> nodeTags;
	gmsh::model::mesh::getElements(elementTypes, elementTags, nodeTags, 3, id);

	std::vector<tet> elems_to_return;
	elems_to_return.reserve(elementTags[0].size());
	for (int i = 0; i < elementTags[0].size(); i++)
	{
		size_t n1 = nodeTags[0][i * 4];
		size_t n2 = nodeTags[0][i * 4 + 1];
		size_t n3 = nodeTags[0][i * 4 + 2];
		size_t n4 = nodeTags[0][i * 4 + 3];
		elems_to_return.push_back(assemble_tet(n1, n2, n3, n4));
	}

	return elems_to_return;
}

int mesher_interface::get_surface_from_com(point_3d p)
{
	std::vector<std::pair<int, int>> entities;
	gmsh::model::occ::getEntities(entities, 2);

	for (auto e : entities)
	{
		double x, y, z;
		gmsh::model::occ::getCenterOfMass(e.first, e.second, x, y, z);
		if (helpers::is_approx_equal(p.x, x) && helpers::is_approx_equal(p.y, y) && helpers::is_approx_equal(p.z, z))
		{
			return e.second;
		}
	}
	return -1;
}

std::vector<int> mesher_interface::get_surface_from_com(std::vector<point_3d> coms)
{
	std::vector<int> ids;

	for (const auto& p : coms)
	{
		auto id = get_surface_from_com(p);
		if (id != -1)
		{
			ids.push_back(id);
		}
	}
	return ids;
}

tri mesher_interface::assemble_tri(size_t n1, size_t n2, size_t n3)
{
	std::array<size_t, 3> nodes{ n1, n2, n3 };
	std::sort(nodes.begin(), nodes.end());

	std::vector<size_t> edge_tags;
	std::vector<int> edge_orientations;
	gmsh::model::mesh::getEdges(
		{ nodes[0], nodes[1],
		  nodes[0], nodes[2],
		  nodes[1], nodes[2] },
		edge_tags, edge_orientations);

	std::vector<size_t> face_tags;
	std::vector<int> face_orientations;
	gmsh::model::mesh::getFaces(3,
		{
			nodes[0], nodes[1], nodes[2]
		},
		face_tags, face_orientations);

	return { nodes, {edge_tags[0], edge_tags[1], edge_tags[2]}, face_tags[0], 0 };
}

std::vector<tri> mesher_interface::get_surface_elems(int id)
{
	std::vector<int> elementTypes;
	std::vector<std::vector<std::size_t>> elementTags;
	std::vector<std::vector<std::size_t>> nodeTags;
	gmsh::model::mesh::getElements(elementTypes, elementTags, nodeTags, 2, id);

	std::vector<tri> elems_to_return;
	elems_to_return.reserve(elementTags[0].size());

	for (size_t i = 0; i < elementTags[0].size(); i++)
	{
		size_t n1 = nodeTags[0][i * 3];
		size_t n2 = nodeTags[0][i * 3 + 1];
		size_t n3 = nodeTags[0][i * 3 + 2];
		elems_to_return.push_back(assemble_tri(n1, n2, n3));
	}

	return elems_to_return;
}

std::vector<std::vector<tri>> mesher_interface::get_surface_elems(std::vector<int> ids)
{
	std::vector<std::vector<tri>> elems_to_return;
	for (auto id : ids)
	{
		elems_to_return.push_back(get_surface_elems(id));
	}
	return elems_to_return;
}

tri mesher_interface::get_surface_element_by_coordinate(point_3d p)
{
	double u, v, w;
	size_t element_tag;
	int element_type;
	std::vector<size_t> node_tags;
	gmsh::model::mesh::getElementByCoordinates(p.x, p.y, p.z, element_tag, element_type, node_tags, u, v, w, 2, false);

	size_t n1 = node_tags[0];
	size_t n2 = node_tags[1];
	size_t n3 = node_tags[2];

	return assemble_tri(n1, n2, n3);
}

std::vector<tri> mesher_interface::get_surface_element_by_coordinate(std::vector<point_3d> points)
{
	std::vector<tri> elements;
	elements.reserve(points.size());
	for (const auto& p : points)
	{
		elements.push_back(get_surface_element_by_coordinate(p));
	}
	return elements;
}

tri mesher_interface::get_surface_element_by_parametric_coordinate(point_2d point, int id)
{
	std::vector<double> coord;
	gmsh::model::getValue(2, id, { point.u, point.v }, coord);
	return get_surface_element_by_coordinate({ coord[0], coord[1], coord[2] });
}

rectangle mesher_interface::get_surface_bounds(int id)
{
	std::vector<double> min;
	std::vector<double> max;
	gmsh::model::getParametrizationBounds(2, id, min, max);
	return { min[0], min[1], max[0], max[1] };
}

std::vector<rectangle> mesher_interface::get_surface_bounds(std::vector<int> id)
{
	std::vector<rectangle> dim;
	for (auto i : id)
	{
		dim.push_back(get_surface_bounds(i));
	}
	return dim;
}

point_2d mesher_interface::parameterize_on_surface(point_3d coord, int id)
{
	std::vector<double> param_coords;
	gmsh::model::getParametrization(2, id, { coord.x, coord.y, coord.z }, param_coords);
	return { param_coords[0], param_coords[1] };
}

std::vector<point_2d> mesher_interface::parameterize_on_surface(std::vector <point_3d> coord, int id)
{
	std::vector<point_2d> param_coords;
	for (const auto& p : coord)
	{
		param_coords.push_back(parameterize_on_surface(p, id));
	}
	return param_coords;
}

void mesher_interface::parameterize_surface_nodes(std::vector<node>& nodes, int surface_id, const std::vector<tri> elements)
{
	for (const auto& e : elements)
	{
		for (auto n : e.nodes)
		{
			if (!nodes[n - 1].point_2d)
			{
				auto param_normalized = parameterize_on_surface(nodes[n - 1].coords, surface_id);
				nodes[n - 1].point_2d = { param_normalized.u, param_normalized.v };
			}
		}
	}
}

void mesher_interface::parameterize_surface_nodes(std::vector<node>& nodes, std::vector <int> surface_id, const std::vector<std::vector<tri>> elements)
{
	for (int i = 0; i < surface_id.size(); i++)
	{
		parameterize_surface_nodes(nodes, surface_id[i], elements[i]);
	}
}

std::optional<tet> mesher_interface::get_volume_element_by_coordinate(point_3d points)
{
	double u, v, w;
	size_t element_tag;
	int element_type;
	std::vector<size_t> node_tags;

	try
	{
		gmsh::model::mesh::getElementByCoordinates(points.x, points.y, points.z, element_tag, element_type, node_tags, u, v, w, 3, true);
	}
	catch (...)
	{
		return {};
	}

	size_t n1 = node_tags[0];
	size_t n2 = node_tags[1];
	size_t n3 = node_tags[2];
	size_t n4 = node_tags[3];

	return assemble_tet(n1, n2, n3, n4);
}

std::vector <int> mesher_interface::get_boundary_surfaces()
{
	std::vector<std::pair<int, int> > all_3d_dim_tags;
	gmsh::model::getEntities(all_3d_dim_tags, 3);

	std::vector<std::pair<int, int> > shared_dim_tags;
	gmsh::model::getBoundary(all_3d_dim_tags, shared_dim_tags, true, false, false);
	
	std::vector <int> shared_surfaces;
	for (const auto& t : shared_dim_tags)
	{
		shared_surfaces.push_back(t.second);
	}

	return shared_surfaces;
}
