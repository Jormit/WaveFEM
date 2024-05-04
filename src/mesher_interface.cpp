#include <string>
#include <algorithm>
#include <unordered_map>
#include <iostream>
#include <cmath>

#include "mesher_interface.h"
#include "helpers.h"

void mesher_interface::initialize() {
	gmsh::initialize();
	gmsh::option::setNumber("General.Terminal", 0);
}

std::vector<int> mesher_interface::import_model(const std::string& filename)
{
	std::vector<std::pair<int, int> > v;
	gmsh::model::occ::importShapes(filename, v);
	gmsh::model::occ::synchronize();
	std::vector<int> ret;
	std::transform(v.cbegin(), v.cend(), std::back_inserter(ret), [](std::pair<int, int> c) { return c.second; });
	return ret;
}

void mesher_interface::mesh_model(double mesh_size_min, double mesh_size_max)
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

std::vector<int> mesher_interface::get_entities_in_bounding_box(int dim, box box)
{
	std::vector<std::pair<int, int>> entities;
	gmsh::model::getEntitiesInBoundingBox(box.xmin, box.ymin, box.zmin, box.xmax, box.ymax, box.zmax, entities, dim);
	std::vector<int> ids;
	std::transform(entities.cbegin(), entities.cend(), std::back_inserter(ids), [](std::pair<int, int> o) { return o.second; });
	return ids;
}

std::vector<std::vector<int>> mesher_interface::get_entities_in_bounding_box(int dim, std::vector<box> box)
{
	std::vector<std::vector<int>> ids;
	for (const auto& b : box)
	{
		ids.push_back(get_entities_in_bounding_box(dim, b));
	}
	return ids;
}

int mesher_interface::get_surface_parent(int id)
{
	std::vector<int> up_adj;
	std::vector<int> down_adj;
	gmsh::model::getAdjacencies(2, id, up_adj, down_adj);
	return up_adj[0];
}

std::vector<int> mesher_interface::get_surface_parent(std::vector<int> id)
{
	std::vector<int> ids;
	for (auto i : id)
	{
		ids.push_back(get_surface_parent(i));
	}
	return ids;
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

std::vector<int> mesher_interface::subtract(int obj, int tool, bool remove_tool)
{
	return subtract(std::vector<int>{obj}, tool, remove_tool);
}

std::vector<int> mesher_interface::subtract(std::vector<int> obj, int tool, bool remove_tool)
{
	std::vector<std::pair<int, int>> ov;
	std::vector<std::vector<std::pair<int, int>>> ovv;
	std::vector<std::pair<int, int>> objs;
	std::transform(obj.cbegin(), obj.cend(), std::back_inserter(objs), [](int o) { return std::pair<int, int>{ 3, o }; });

	gmsh::model::occ::cut(objs, { {3, tool} }, ov, ovv, -1, true, remove_tool);
	gmsh::model::occ::synchronize();

	std::vector<int> new_ids;
	std::transform(ov.cbegin(), ov.cend(), std::back_inserter(new_ids), [](std::pair<int, int> o) { return o.second; });

	return new_ids;
}

int mesher_interface::fuse_surfaces(std::vector<int> obj, bool remove_obj)
{
	std::vector<std::pair<int, int>> objs;
	std::transform(obj.cbegin(), obj.cend(), std::back_inserter(objs), [](int o) { return std::pair<int, int>{ 2, o }; });

	std::vector<std::pair<int, int>> objs_out;
	std::vector<std::vector<std::pair<int, int>>> objs_out_map;
	gmsh::model::occ::fuse(objs, objs, objs_out, objs_out_map, -1, false, false);
	gmsh::model::occ::synchronize();

	if (objs_out.size() > 1)
	{
		std::cerr << "Requested fuse operation failed because objects were not touching!" << std::endl;
		exit(1);
	}

	return objs_out[0].second;
}

std::vector<int> mesher_interface::fuse_surfaces(std::vector <std::vector<int>> obj, bool remove_obj)
{
	std::vector<int> ids;
	for (const auto& o : obj)
	{
		ids.push_back(fuse_surfaces(o, remove_obj));
	}
	return ids;
}

void mesher_interface::remove_duplicates()
{
	gmsh::model::occ::removeAllDuplicates();
	gmsh::model::occ::synchronize();
}

std::vector<node> mesher_interface::get_all_nodes()
{
	std::vector<size_t> nodeTags;
	std::vector<double> coord;
	std::vector<double> parametric_coord;
	gmsh::model::mesh::getNodes(nodeTags, coord, parametric_coord, -1, -1, true, false);

	std::vector<node> nodes_to_return(nodeTags.size());

	int i = 0;
	for (auto n : nodeTags) {
		node nn{ { coord[3 * i], coord[3 * i + 1], coord[3 * i + 2] }, {}, false };
		nodes_to_return[n - 1] = nn;
		i++;
	}

	return nodes_to_return;
}

std::vector<size_t> mesher_interface::get_node_ids_in_line(int id)
{
	std::vector<size_t> nodeTags;
	std::vector<double> coord;
	std::vector<double> parametric_coord;
	gmsh::model::mesh::getNodes(nodeTags, coord, parametric_coord, 1, id, true, false);
	return nodeTags;
}

std::vector<size_t> mesher_interface::get_node_ids_in_volume(int id)
{
	std::vector<size_t> nodeTags;
	std::vector<double> coord;
	std::vector<double> parametric_coord;
	gmsh::model::mesh::getNodes(nodeTags, coord, parametric_coord, 3, id, true, false);
	return nodeTags;
}

std::pair<std::unordered_set<size_t>, std::unordered_set<size_t>> mesher_interface::get_surface_edges_and_faces(int surface_id)
{
	return get_surface_edges_and_faces(std::vector <int> {surface_id});
}

std::pair<std::unordered_set<size_t>, std::unordered_set<size_t>> mesher_interface::get_surface_edges_and_faces(std::vector<int> surface_ids)
{
	std::unordered_set<size_t> edges;
	std::unordered_set<size_t> faces;
	for (auto id : surface_ids)
	{
		auto elems = get_surface_elems(id);
		for (const auto & e : elems)
		{
			edges.insert(e.edges[0]);
			edges.insert(e.edges[1]);
			edges.insert(e.edges[2]);
			faces.insert(e.face);
		}
	}

	return { edges, faces };
}

std::vector<int> mesher_interface::get_surface_boundary_entities(int surface_id)
{
	return get_surface_boundary_entities(std::vector<int>{ surface_id });
}

std::vector<int> mesher_interface::get_surface_boundary_entities(std::vector<int> surface_ids)
{
	std::unordered_set<size_t> edges;
	std::vector<std::pair<int, int>> objs;
	std::transform(surface_ids.cbegin(), surface_ids.cend(), std::back_inserter(objs), [](int o) { return std::pair<int, int>{ 2, o }; });

	std::vector<std::pair<int, int>> boundary_entities;
	gmsh::model::getBoundary(objs, boundary_entities, true, false, false);
	std::vector<int> line_ids;
	std::transform(boundary_entities.cbegin(), boundary_entities.cend(), std::back_inserter(line_ids), [](std::pair<int, int> c) { return c.second; });

	return line_ids;
}

std::unordered_set<size_t> mesher_interface::get_edges_on_line(int line_id)
{
	std::unordered_set<size_t> edges;
	std::vector<int> elementTypes;
	std::vector<std::vector<std::size_t>> elementTags;
	std::vector<std::vector<std::size_t>> nodeTags;
	gmsh::model::mesh::getElements(elementTypes, elementTags, nodeTags, 1, line_id);
	std::vector<size_t> edge_tags;
	std::vector<int> edge_orientations;
	gmsh::model::mesh::getEdges(nodeTags[0], edge_tags, edge_orientations);
	for (auto edge : edge_tags)
	{
		edges.insert(edge);
	}
	return edges;
}

std::unordered_set<size_t> mesher_interface::get_edges_on_line(std::vector<int> line_id)
{
	std::unordered_set<size_t> edges;
	for (auto id : line_id)
	{
		edges.merge(get_edges_on_line(id));
	}
	return edges;
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

std::vector<std::vector<tet>> mesher_interface::get_volume_elems(std::vector<int> id)
{
	std::vector<std::vector<tet>> elems_to_return;
	for (auto i : id)
	{
		elems_to_return.push_back(get_volume_elems(i));
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

rectangle mesher_interface::get_surface_parametric_bounds(int id)
{
	std::vector<double> min;
	std::vector<double> max;
	gmsh::model::getParametrizationBounds(2, id, min, max);
	return { min[0], min[1], max[0], max[1] };
}

std::vector<rectangle> mesher_interface::get_surface_parametric_bounds(std::vector<int> id)
{
	std::vector<rectangle> dim;
	for (auto i : id)
	{
		dim.push_back(get_surface_parametric_bounds(i));
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

void mesher_interface::write_vtk(std::string filename)
{
	gmsh::write(filename + ".vtk");
}
