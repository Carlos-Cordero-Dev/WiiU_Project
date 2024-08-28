
#define TINYOBJLOADER_IMPLEMENTATION 1

#include <string>

#include "tinyobjloader/tiny_obj_loader.h"
#include "ufbx/ufbx.h"


void LoadFBX(const std::string& inputfile)
{
    ufbx_load_opts opts = { 0 }; // Optional, pass NULL for defaults
    ufbx_error error; // Optional, pass NULL if you don't care about errors
    ufbx_scene* scene = ufbx_load_file(inputfile.c_str(), &opts, &error);
    if (!scene) {
        VORP_LOG("Failed to load: %s\n", error.description.data);
        return;
    }

    // Use and inspect `scene`, it's just plain data!

    // Let's just list all objects within the scene for example:
    for (size_t i = 0; i < scene->nodes.count; i++) {
        ufbx_node* node = scene->nodes.data[i];
        if (node->is_root) continue;

        printf("Object: %s\n", node->name.data);
        if (node->mesh) {
            VORP_LOG("-> mesh with %zu faces\n", node->mesh->faces.count);
        }
    }

    ufbx_free_scene(scene);
}

//=========================
//TINYOBJLOADER

struct Mesh
{
    std::vector<unsigned int> indices;
    std::vector<float> vertexData;
    //std::shared_ptr<Vorpal::Material> material = nullptr;
    std::string name;
};

struct ObjInfo
{
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
};

std::shared_ptr<ObjInfo> LoadObjFromFile(const std::string& inputfile)
{
    std::shared_ptr<ObjInfo> objInfo = std::make_shared<ObjInfo>();

    std::string err = "";

    bool ret = tinyobj::LoadObj(&objInfo->attrib, &objInfo->shapes, &objInfo->materials,
        &err, inputfile.c_str(),nullptr,false);


    if (!err.empty()) {
        VORP_LOG("Tinyobj error %s",err.c_str());
    }

    if (!ret) {
        return nullptr;
    }
    VORP_LOG("Loading Obj with path %s", inputfile.c_str());

    // Loop over shapes
    for (size_t s = 0; s < objInfo->shapes.size(); s++) {
        // Loop over faces(polygon)
        size_t index_offset = 0;
        for (size_t f = 0; f < objInfo->shapes[s].mesh.num_face_vertices.size(); f++) {
            size_t fv = size_t(objInfo->shapes[s].mesh.num_face_vertices[f]);

            // Loop over vertices in the face.
            for (size_t v = 0; v < fv; v++) {
                // access to vertex
                tinyobj::index_t idx = objInfo->shapes[s].mesh.indices[index_offset + v];

                tinyobj::real_t vx = objInfo->attrib.vertices[3 * size_t(idx.vertex_index) + 0];
                tinyobj::real_t vy = objInfo->attrib.vertices[3 * size_t(idx.vertex_index) + 1];
                tinyobj::real_t vz = objInfo->attrib.vertices[3 * size_t(idx.vertex_index) + 2];

                // Check if `normal_index` is zero or positive. negative = no normal data
                if (idx.normal_index >= 0) {
                    tinyobj::real_t nx = objInfo->attrib.normals[3 * size_t(idx.normal_index) + 0];
                    tinyobj::real_t ny = objInfo->attrib.normals[3 * size_t(idx.normal_index) + 1];
                    tinyobj::real_t nz = objInfo->attrib.normals[3 * size_t(idx.normal_index) + 2];
                }

                // Check if `texcoord_index` is zero or positive. negative = no texcoord data
                if (idx.texcoord_index >= 0) {
                    tinyobj::real_t tx = objInfo->attrib.texcoords[2 * size_t(idx.texcoord_index) + 0];
                    tinyobj::real_t ty = objInfo->attrib.texcoords[2 * size_t(idx.texcoord_index) + 1];
                }
                // Optional: vertex colors
                // tinyobj::real_t red   = attrib.colors[3*size_t(idx.vertex_index)+0];
                // tinyobj::real_t green = attrib.colors[3*size_t(idx.vertex_index)+1];
                // tinyobj::real_t blue  = attrib.colors[3*size_t(idx.vertex_index)+2];
            }
            index_offset += fv;

            // per-face material
            objInfo->shapes[s].mesh.material_ids[f];
        }
    }
    return objInfo;
}

void UploadModel()
{


}