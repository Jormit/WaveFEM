#include <string>
#include "../include/gmsh.h_cwrap"
#include "mesher_interface.h"
#include <algorithm>
#include <unordered_map>
#include <iostream>
#include "helpers.h"

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
}

void mesher_interface::view_model()
{
    gmsh::fltk::run();
}

box mesher_interface::get_bounding_box()
{
    double xmin = 0, ymin = 0, zmin = 0, xmax = 0, ymax = 0, zmax = 0;
    try
    {
        gmsh::model::getBoundingBox(-1, -1, xmin, ymin, zmin, xmax, ymax, zmax);
    }
    catch (...)
    {
    }
    return { xmin, ymin, zmin, xmax, ymax, zmax };
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

std::unordered_map<int, int> mesher_interface::get_node_map()
{
    std::vector<size_t> nodeTags;
    std::vector<double> coord;
    std::vector<double> parametric_coord;
    gmsh::model::mesh::getNodes(nodeTags, coord, parametric_coord, -1, -1, false, false);

    std::unordered_map<int, int> node_map;
    node_map.reserve(nodeTags.size());
    for (int i = 0; i < nodeTags.size(); i++)
    {
        int node = nodeTags[i];
        node_map[node] = i;
    }

    return node_map;
}

std::vector<node> mesher_interface::get_all_nodes(std::unordered_map<int, int> node_map)
{
    // Create node array object
    std::vector<node> nodes_to_return(node_map.size());
   
    // Get nodes on 2d surfaces.
    std::vector<size_t> nodeTags2;
    std::vector<double> coord2;
    std::vector<double> parametric_coord2;
    gmsh::model::mesh::getNodes(nodeTags2, coord2, parametric_coord2, 2, -1, true, false);

    // Get nodes in 3d volume (not on boundary).
    std::vector<size_t> nodeTags3;
    std::vector<double> coord3;
    std::vector<double> parametric_coord3;
    gmsh::model::mesh::getNodes(nodeTags3, coord3, parametric_coord3, 3, -1, false, false);

    // Insert 3d nodes.
    int i = 0;
    for (auto e : nodeTags3) {
        node nn({ coord3[3 * i], coord3[3 * i + 1], coord3[3 * i + 2] }, FREE_NODE);
        int new_number = node_map[e];
        nodes_to_return[new_number] = nn;
        i++;
    }

    // Insert 2d nodes
    i = 0;
    for (auto e : nodeTags2) {
        int new_number = node_map[e];
        node nn({ coord2[3 * i], coord2[3 * i + 1], coord2[3 * i + 2] }, BOUNDARY_NODE);
        nodes_to_return[new_number] = nn;
        i++;
    }
    return nodes_to_return;
}

std::vector<tet> mesher_interface::get_volume_elems(std::unordered_map<int, int> node_map)
{
    std::vector<int> elementTypes;
    std::vector<std::vector<std::size_t>> elementTags;
    std::vector<std::vector<std::size_t>> nodeTags;
    gmsh::model::mesh::getElements(elementTypes, elementTags, nodeTags, 3);

    std::vector<tet> elems_to_return;
    elems_to_return.reserve(elementTags[0].size());
    for (int i = 0; i < elementTags[0].size(); i++)
    {
        int n1 = nodeTags[0][i * 4];
        int n2 = nodeTags[0][i * 4 + 1];
        int n3 = nodeTags[0][i * 4 + 2];
        int n4 = nodeTags[0][i * 4 + 3];

        n1 = node_map[n1];
        n2 = node_map[n2];
        n3 = node_map[n3];
        n4 = node_map[n4];

        std::array<int, 4> nodes{ n1, n2, n3, n4 };
        std::sort(nodes.begin(), nodes.end());
        elems_to_return.push_back({ nodes, 1 });
    }

    return elems_to_return;
}

std::vector<int> mesher_interface::get_surface_ids_from_coms(std::vector<std::vector<double>> coms)
{
    std::vector<std::pair<int, int>> entities;
    gmsh::model::occ::getEntities(entities, 2);
    std::vector<int> ids;

    for (auto p : coms)
    {
        for (auto e : entities)
        {
            double x, y, z;
            gmsh::model::occ::getCenterOfMass(e.first, e.second, x, y, z);        
            if (helpers::isEqual(p[0], x) && helpers::isEqual(p[1], y) && helpers::isEqual(p[2], z))
            {
                ids.push_back(e.second);
                break;
            }
        }
    }
    return ids;
}

std::vector<std::vector<tri>> mesher_interface::get_surface_elems_by_id(std::vector<int> ids, std::unordered_map<int, int> node_map)
{
    std::vector<std::vector<tri>> elems_to_return(ids.size());
    int index = 0;
    for (auto id : ids)
    {
        std::vector<int> elementTypes;
        std::vector<std::vector<std::size_t>> elementTags;
        std::vector<std::vector<std::size_t>> nodeTags;
        gmsh::model::mesh::getElements(elementTypes, elementTags, nodeTags, 2, id);
       
        elems_to_return[index].reserve(elementTags[0].size());
        for (int i = 0; i < elementTags[0].size(); i++)
        {
            int n1 = nodeTags[0][i * 3];
            int n2 = nodeTags[0][i * 3 + 1];
            int n3 = nodeTags[0][i * 3 + 2];

            n1 = node_map[n1];
            n2 = node_map[n2];
            n3 = node_map[n3];

            std::array<int, 3> nodes{n1, n2, n3};
            std::sort(nodes.begin(), nodes.end());
            elems_to_return[index].push_back({ nodes, 1 });
        }
        index++;
    }
    return elems_to_return;
}