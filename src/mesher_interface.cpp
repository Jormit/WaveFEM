#include <string>
#include <algorithm>
#include <unordered_map>
#include <iostream>
#include <cmath>

#include "mesher_interface.h"
#include "helpers.h"

size_t volume_elem_offset = 0;

void mesher_interface::initialize() {
	gmsh::initialize();
	gmsh::option::setNumber("General.Terminal", 0);
}

std::vector<int> mesher_interface::import_step(const std::string& filename)
{
	gmsh::vectorpair imported_dim_tags;
	gmsh::model::occ::importShapes(filename, imported_dim_tags);
	gmsh::model::occ::synchronize();
	std::vector<int> ret;
	std::transform(imported_dim_tags.cbegin(), imported_dim_tags.cend(), std::back_inserter(ret), [](std::pair<int, int> c) { return c.second; });
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

geo::box mesher_interface::get_bounding_box(int dim, int tag)
{
	double xmin = 0, ymin = 0, zmin = 0, xmax = 0, ymax = 0, zmax = 0;
	try
	{
		gmsh::model::getBoundingBox(dim, tag, xmin, ymin, zmin, xmax, ymax, zmax);
	}
	catch (...)
	{
	}
	return geo::box(xmin, ymin, zmin, xmax, ymax, zmax);
}

std::vector<geo::box> mesher_interface::get_bounding_box(int dim, std::vector<int> tags)
{
	std::vector<geo::box> boxes;
	for (auto t : tags)
	{
		boxes.push_back(get_bounding_box(dim, t));
	}
	return boxes;
}

std::vector<int> mesher_interface::get_entities_in_bounding_box(int dim, geo::box box)
{
	gmsh::vectorpair obj_dim_tags;
	gmsh::model::getEntitiesInBoundingBox(box.xmin, box.ymin, box.zmin, box.xmax, box.ymax, box.zmax, obj_dim_tags, dim);
	std::vector<int> ids;
	std::transform(obj_dim_tags.cbegin(), obj_dim_tags.cend(), std::back_inserter(ids), [](std::pair<int, int> o) { return o.second; });
	return ids;
}

std::vector<std::vector<int>> mesher_interface::get_entities_in_bounding_box(int dim, std::vector<geo::box> box)
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

geo::box mesher_interface::get_bounding_box()
{
	return get_bounding_box(-1, -1);
}

int mesher_interface::add_box(geo::box b)
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
	gmsh::vectorpair out_dim_tags;
	std::vector<gmsh::vectorpair> out_dim_tags_map;
	gmsh::vectorpair in_dim_tags;
	std::transform(obj.cbegin(), obj.cend(), std::back_inserter(in_dim_tags), [](int o) { return std::pair<int, int>{ 3, o }; });

	gmsh::model::occ::cut(in_dim_tags, { {3, tool} }, out_dim_tags, out_dim_tags_map, -1, true, remove_tool);
	gmsh::model::occ::synchronize();

	std::vector<int> new_ids;
	std::transform(out_dim_tags.cbegin(), out_dim_tags.cend(), std::back_inserter(new_ids), [](std::pair<int, int> o) { return o.second; });

	return new_ids;
}

int mesher_interface::fuse_surfaces(std::vector<int> obj, bool remove_obj)
{
	gmsh::vectorpair objs;
	std::transform(obj.cbegin(), obj.cend(), std::back_inserter(objs), [](int o) { return std::pair<int, int>{ 2, o }; });

	gmsh::vectorpair out_dim_tags;
	std::vector<gmsh::vectorpair> out_dim_tags_map;
	gmsh::model::occ::fuse(objs, objs, out_dim_tags, out_dim_tags_map, -1, false, false);
	gmsh::model::occ::synchronize();

	if (out_dim_tags.size() > 1)
	{
		std::cerr << "Requested fuse operation failed because objects were not touching!" << std::endl;
		exit(1);
	}

	return out_dim_tags[0].second;
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

geo::point_3d mesher_interface::evaluate_surface_parameterization(int surface_id, geo::point_2d point)
{
	std::vector<double> coord;
	gmsh::model::getValue(2, surface_id, { point.u, point.v }, coord);
	return { coord[0], coord[1], coord[2] };
}

Eigen::Matrix<double, 3, 2> mesher_interface::evaluate_surface_derivative(int surface_id, geo::point_2d point)
{
	std::vector<double> result;
	gmsh::model::getDerivative(2, surface_id, { point.u, point.v }, result);

	Eigen::Matrix<double, 3, 2> deriv_matrix;
	deriv_matrix <<
		result[0], result[3],
		result[1], result[4],
		result[2], result[5];

	return deriv_matrix;
}

std::vector<geo::node> mesher_interface::get_all_nodes()
{
	std::vector<size_t> nodeTags;
	std::vector<double> coord;
	std::vector<double> parametric_coord;
	gmsh::model::mesh::getNodes(nodeTags, coord, parametric_coord, -1, -1, true, false);

	std::vector<geo::node> nodes_to_return(nodeTags.size());

	int i = 0;
	for (auto n : nodeTags) {
		geo::node nn{ { coord[3 * i], coord[3 * i + 1], coord[3 * i + 2] }, {}, false };
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
	std::vector<std::vector<size_t>> elementTags;
	std::vector<std::vector<size_t>> nodeTags;
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

geo::tet mesher_interface::assemble_tet(size_t n1, size_t n2, size_t n3, size_t n4)
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

std::vector<geo::tet> mesher_interface::get_all_volume_elems()
{
	std::vector<int> elementTypes;
	std::vector<std::vector<size_t>> elementTags;
	std::vector<std::vector<size_t>> nodeTags;
	gmsh::model::mesh::getElements(elementTypes, elementTags, nodeTags, 3, -1);

	auto elementTags_flattened = helpers::flatten_vector(elementTags);
	auto nodeTags_flattened = helpers::flatten_vector(nodeTags);
	size_t num_elems = elementTags_flattened.size();
	size_t offset = elementTags_flattened[0];

	std::vector<geo::tet> elems_to_return(num_elems);
	size_t i = 0;
	for (auto elem : elementTags_flattened)
	{
		size_t n1 = nodeTags_flattened[i * 4];
		size_t n2 = nodeTags_flattened[i * 4 + 1];
		size_t n3 = nodeTags_flattened[i * 4 + 2];
		size_t n4 = nodeTags_flattened[i * 4 + 3];
		elems_to_return[elem - elementTags_flattened[0]] = assemble_tet(n1, n2, n3, n4);
		i++;
	}

	volume_elem_offset = offset;
	return elems_to_return;
}

std::vector<size_t> mesher_interface::get_volume_elem_ids(int id)
{
	std::vector<int> elementTypes;
	std::vector<std::vector<size_t>> elementTags;
	std::vector<std::vector<size_t>> nodeTags;
	gmsh::model::mesh::getElements(elementTypes, elementTags, nodeTags, 3, id);

	for (size_t i = 0; i < elementTags[0].size(); i++)
	{
		elementTags[0][i] -= volume_elem_offset;
	}

	return elementTags[0];
}

std::vector<std::vector<size_t>> mesher_interface::get_volume_elem_ids(std::vector<int> id)
{
	std::vector<std::vector<size_t>> elems_to_return;
	for (auto i : id)
	{
		elems_to_return.push_back(get_volume_elem_ids(i));
	}
	return elems_to_return;
}

void mesher_interface::label_elems_in_volume(int id, size_t label, std::vector<geo::tet>& elems)
{
	auto ids = get_volume_elem_ids(id);
	for (auto id : ids)
	{
		elems[id].material_id = label;
	}
}

void mesher_interface::label_elems_in_volume(std::vector<int> ids, size_t label, std::vector<geo::tet>& elems)
{
	for (auto id : ids)
	{
		label_elems_in_volume(id, label, elems);
	}
}

geo::tri mesher_interface::assemble_tri(size_t n1, size_t n2, size_t n3)
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

std::vector<geo::tri> mesher_interface::get_surface_elems(int id)
{
	std::vector<int> elementTypes;
	std::vector<std::vector<size_t>> elementTags;
	std::vector<std::vector<size_t>> nodeTags;
	gmsh::model::mesh::getElements(elementTypes, elementTags, nodeTags, 2, id);

	std::vector<geo::tri> elems_to_return;
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

std::vector<std::vector<geo::tri>> mesher_interface::get_surface_elems(std::vector<int> ids)
{
	std::vector<std::vector<geo::tri>> elems_to_return;
	for (auto id : ids)
	{
		elems_to_return.push_back(get_surface_elems(id));
	}
	return elems_to_return;
}

geo::tri mesher_interface::get_surface_element_by_coordinate(geo::point_3d p)
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

std::vector<geo::tri> mesher_interface::get_surface_element_by_coordinate(std::vector<geo::point_3d> points)
{
	std::vector<geo::tri> elements;
	elements.reserve(points.size());
	for (const auto& p : points)
	{
		elements.push_back(get_surface_element_by_coordinate(p));
	}
	return elements;
}

geo::tri mesher_interface::get_surface_element_by_parametric_coordinate(geo::point_2d point, int id)
{
	return get_surface_element_by_coordinate(evaluate_surface_parameterization(id, point));
}

geo::rectangle mesher_interface::get_surface_parametric_bounds(int id)
{
	std::vector<double> min;
	std::vector<double> max;
	gmsh::model::getParametrizationBounds(2, id, min, max);
	return { min[0], min[1], max[0], max[1] };
}

std::vector<geo::rectangle> mesher_interface::get_surface_parametric_bounds(std::vector<int> id)
{
	std::vector<geo::rectangle> dim;
	for (auto i : id)
	{
		dim.push_back(get_surface_parametric_bounds(i));
	}
	return dim;
}

geo::point_2d mesher_interface::parameterize_on_surface(geo::point_3d coord, int id)
{
	std::vector<double> param_coords;
	gmsh::model::getParametrization(2, id, { coord.x, coord.y, coord.z }, param_coords);
	return { param_coords[0], param_coords[1] };
}

std::vector<geo::point_2d> mesher_interface::parameterize_on_surface(std::vector <geo::point_3d> coord, int id)
{
	std::vector<geo::point_2d> param_coords;
	for (const auto& p : coord)
	{
		param_coords.push_back(parameterize_on_surface(p, id));
	}
	return param_coords;
}

void mesher_interface::parameterize_surface_nodes(std::vector<geo::node>& nodes, int surface_id, const std::vector<geo::tri> elements)
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

void mesher_interface::parameterize_surface_nodes(std::vector<geo::node>& nodes,
	std::vector <int> surface_id, const std::vector<std::vector<geo::tri>> elements)
{
	for (int i = 0; i < surface_id.size(); i++)
	{
		parameterize_surface_nodes(nodes, surface_id[i], elements[i]);
	}
}

std::optional<size_t> mesher_interface::get_volume_element_by_coordinate(geo::point_3d point)
{
	double u, v, w;
	size_t element_tag;
	int element_type;
	std::vector<size_t> node_tags;

	try
	{
		gmsh::model::mesh::getElementByCoordinates(point.x, point.y, point.z, 
			element_tag, element_type, node_tags, u, v, w, 3, true);
	}
	catch (...)
	{
		return {};
	}

	return element_tag - volume_elem_offset;
}

std::optional<size_t> mesher_interface::get_volume_entity_by_coordinate(geo::point_3d point)
{
	gmsh::vectorpair dim_tags;
	gmsh::model::getEntities(dim_tags, 3);

	for (const auto& obj : dim_tags)
	{
		if (gmsh::model::isInside(3, obj.second, { point.x, point.y, point.z }, false))
		{
			return obj.second;
		}
	}

	return {};

}

std::vector <int> mesher_interface::get_boundary_surfaces()
{
	gmsh::vectorpair all_3d_dim_tags;
	gmsh::model::getEntities(all_3d_dim_tags, 3);

	gmsh::vectorpair shared_dim_tags;
	gmsh::model::getBoundary(all_3d_dim_tags, shared_dim_tags, true, false, false);
	
	std::vector <int> shared_surfaces;
	for (const auto& t : shared_dim_tags)
	{
		shared_surfaces.push_back(t.second);
	}

	return shared_surfaces;
}

std::vector<int> mesher_interface::get_bounding_box_surfaces(geo::box b)
{
	double tol = 1e-4;

	// Ordered per enum box_face
	geo::box xy_bottom (b.xmin - tol, b.ymin - tol, b.zmin - tol, b.xmax + tol, b.ymax + tol, b.zmin + tol);
	geo::box xy_top (b.xmin - tol, b.ymin - tol, b.zmax - tol, b.xmax + tol, b.ymax + tol, b.zmax + tol);

	geo::box xz_bottom(b.xmin - tol, b.ymin - tol, b.zmin - tol, b.xmax + tol, b.ymin + tol, b.zmax + tol);
	geo::box xz_top(b.xmin - tol, b.ymax - tol, b.zmin - tol, b.xmax + tol, b.ymax + tol, b.zmax + tol);

	geo::box yz_bottom(b.xmin - tol, b.ymin - tol, b.zmin - tol, b.xmin + tol, b.ymax + tol, b.zmax + tol);
	geo::box yz_top(b.xmax - tol, b.ymin - tol, b.zmin - tol, b.xmax + tol, b.ymax + tol, b.zmax + tol);

	auto ids = get_entities_in_bounding_box(2, { xy_bottom, xy_top, xz_bottom, xz_top, yz_bottom, yz_top});

	auto flat_ids = helpers::flatten_vector(ids);

	return {flat_ids[0], flat_ids[1], flat_ids[2], flat_ids[3], flat_ids[4], flat_ids[5]};
}

void mesher_interface::write_vtk(std::string filename)
{
	gmsh::write(filename + ".vtk");
}
