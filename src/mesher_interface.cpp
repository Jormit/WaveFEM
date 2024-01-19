#include <string>
#include "mesher_interface.h"
#include <algorithm>
#include <unordered_map>
#include <iostream>
#include "helpers.h"
#include <cmath>

void mesher_interface::initialize() {
    gmsh::initialize();
}

int mesher_interface::import_model(const std::string &filename)
{
    std::vector<std::pair<int, int> > v;
    try 
    {
        gmsh::model::occ::importShapes(filename, v);
    }
    catch (...) 
    {
        gmsh::logger::write("Could not load STEP file: bye!");
        gmsh::finalize();
        return -1;
    }
    
    auto ids = std::vector<int>();
    for (auto i : v)
    {
        ids.push_back(i.second);
    }

    int sl = gmsh::model::occ::addSurfaceLoop(ids, -1, true);
    int vol = gmsh::model::occ::addVolume({sl});    
    gmsh::model::occ::synchronize();

    return vol;
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

box mesher_interface::get_bounding_box(int dim, size_t tag)
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

std::vector<box> mesher_interface::get_bounding_box(int dim, std::vector<size_t> tags)
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

int mesher_interface::subtract(int id1, int id2)
{
    std::vector<std::pair<int, int> > ov;
    std::vector<std::vector<std::pair<int, int> > > ovv;
    gmsh::model::occ::cut({ {3, id1} }, { {3, id2} }, ov, ovv, 3);
    gmsh::model::occ::synchronize();
    return ov[0].second;
}

std::vector<node> mesher_interface::get_all_nodes()
{
    // Get total number of nodes
    std::vector<size_t> nodeTags;
    std::vector<double> coord;
    std::vector<double> parametric_coord;
    gmsh::model::mesh::getNodes(nodeTags, coord, parametric_coord, -1, -1, false, false);

    // Create node array object
    std::vector<node> nodes_to_return(nodeTags.size());

    // Get 1d nodes (nodes on lines).
    std::vector<size_t> nodeTags1;
    std::vector<double> coord1;
    std::vector<double> parametric_coord1;
    gmsh::model::mesh::getNodes(nodeTags1, coord1, parametric_coord1, 1, -1, true, false);
   
    // Get 2d nodes (nodes on surfaces).
    std::vector<size_t> nodeTags2;
    std::vector<double> coord2;
    std::vector<double> parametric_coord2;
    gmsh::model::mesh::getNodes(nodeTags2, coord2, parametric_coord2, 2, -1, true, true);

    // Get 3d nodes (nodes on volumes).
    std::vector<size_t> nodeTags3;
    std::vector<double> coord3;
    std::vector<double> parametric_coord3;
    gmsh::model::mesh::getNodes(nodeTags3, coord3, parametric_coord3, 3, -1, false, false);

    // Insert 3d nodes.
    int i = 0;
    for (auto n : nodeTags3) {
        node nn{ { coord3[3 * i], coord3[3 * i + 1], coord3[3 * i + 2] }, {}, FREE_NODE, FREE_NODE };
        nodes_to_return[n - 1] = nn;
        i++;
    }

    // Insert 2d nodes
    i = 0;
    for (auto n : nodeTags2) {
        node nn{ 
            { coord2[3 * i], coord2[3 * i + 1], coord2[3 * i + 2] },
            { parametric_coord2[2 * i], parametric_coord2[2 * i + 1] },
            FREE_NODE, BOUNDARY_NODE};
        nodes_to_return[n - 1] = nn;
        i++;
    }

    // Insert 1d nodes
    for (auto n : nodeTags1) {
        nodes_to_return[n - 1].type_2d = BOUNDARY_NODE;
    }
    return nodes_to_return;
}

std::vector<tet> mesher_interface::get_all_volume_elems()
{
    std::vector<int> elementTypes;
    std::vector<std::vector<std::size_t>> elementTags;
    std::vector<std::vector<std::size_t>> nodeTags;
    gmsh::model::mesh::getElements(elementTypes, elementTags, nodeTags, 3);

    std::vector<tet> elems_to_return;
    elems_to_return.reserve(elementTags[0].size());
    for (int i = 0; i < elementTags[0].size(); i++)
    {
        // Get nodes and sort so that edges are always in same direction.
        size_t n1 = nodeTags[0][i * 4];
        size_t n2 = nodeTags[0][i * 4 + 1];
        size_t n3 = nodeTags[0][i * 4 + 2];
        size_t n4 = nodeTags[0][i * 4 + 3];

        std::array<size_t, 4> nodes{ n1, n2, n3, n4 };
        std::sort(nodes.begin(), nodes.end());

        // Get global edges in conventional order.
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

        // Get global faces in conventional order.
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

        elems_to_return.push_back({ nodes,
            {edge_tags[0], edge_tags[1], edge_tags[2], edge_tags[3], edge_tags[4], edge_tags[5]},
            {face_tags[0], face_tags[1], face_tags[2], face_tags[3], },
            1});
    }

    return elems_to_return;
}

int mesher_interface::get_surface_from_com(point p)
{
    std::vector<std::pair<int, int>> entities;
    gmsh::model::occ::getEntities(entities, 2);

    for (auto e : entities)
    {
        double x, y, z;
        gmsh::model::occ::getCenterOfMass(e.first, e.second, x, y, z);
        if (helpers::isEqual(p.x, x) && helpers::isEqual(p.y, y) && helpers::isEqual(p.z, z))
        {
            return e.second;
        }
    }
    return -1;
}

std::vector<int> mesher_interface::get_surface_from_com(std::vector<point> coms)
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

std::vector<tri> mesher_interface::get_surface_elems_by_id(int id)
{
    std::vector<int> elementTypes;
    std::vector<std::vector<std::size_t>> elementTags;
    std::vector<std::vector<std::size_t>> nodeTags;
    gmsh::model::mesh::getElements(elementTypes, elementTags, nodeTags, 2, id);

    std::vector<tri> elems_to_return;
    elems_to_return.reserve(elementTags[0].size());

    for (size_t i = 0; i < elementTags[0].size(); i++)
    {
        // Get nodes and sort so that edges are always in same direction.
        size_t n1 = nodeTags[0][i * 3];
        size_t n2 = nodeTags[0][i * 3 + 1];
        size_t n3 = nodeTags[0][i * 3 + 2];

        std::array<size_t, 3> nodes{ n1, n2, n3 };
        std::sort(nodes.begin(), nodes.end());

        // Get global edges in conventional order.
        std::vector<size_t> edge_tags;
        std::vector<int> edge_orientations;
        gmsh::model::mesh::getEdges(
            { nodes[0], nodes[1],
              nodes[0], nodes[2],
              nodes[1], nodes[2] },
            edge_tags, edge_orientations);

        // Get single face
        std::vector<size_t> face_tags;
        std::vector<int> face_orientations;
        gmsh::model::mesh::getFaces(3,
            {
                nodes[0], nodes[1], nodes[2]
            },
            face_tags, face_orientations);

        elems_to_return.push_back({ nodes, {edge_tags[0], edge_tags[1], edge_tags[2]}, face_tags[0], 1 });
    }

    return elems_to_return;
}

std::vector<std::vector<tri>> mesher_interface::get_surface_elems_by_id(std::vector<int> ids)
{
    std::vector<std::vector<tri>> elems_to_return;
    for (auto id : ids)
    {
        elems_to_return.push_back(get_surface_elems_by_id(id));
    }
    return elems_to_return;
}

size_t mesher_interface::get_element_by_coordinate(point p, int dim)
{
    double u, v, w;
    size_t element_tag;
    int element_type;
    std::vector<size_t> node_tags;
    gmsh::model::mesh::getElementByCoordinates(p.x, p.y, p.z, element_tag, element_type, node_tags, u, v, w, dim, false);
    return element_tag;
}

std::vector<size_t> mesher_interface::get_elements_by_coordinate(std::vector<point> points, int dim)
{
    std::vector<size_t> elements;
    elements.reserve(points.size());
    for (const auto& p : points)
    {
        elements.push_back(get_element_by_coordinate(p, 2));
    }
    return elements;
}

dimensions mesher_interface::get_surface_dimensions(int id)
{
    std::vector <double> corner_param_coords{ 0, 0, 1, 0, 0, 1 };
    std::vector <double> coords;
    gmsh::model::getValue(2, id, corner_param_coords, coords);
    double width = std::sqrt(std::pow(coords[3] - coords[0], 2) + std::pow(coords[4] - coords[1], 2) + std::pow(coords[5] - coords[2], 2));
    double height = std::sqrt(std::pow(coords[6] - coords[0], 2) + std::pow(coords[7] - coords[1], 2) + std::pow(coords[8] - coords[2], 2));
    return { width, height };
}

std::vector<dimensions> mesher_interface::get_surface_dimensions(std::vector<int> id)
{
    std::vector<dimensions> dim;
    for (auto i : id)
    {
        dim.push_back(get_surface_dimensions(i));
    }
    return dim;
}
