#include "Terrain.h"
#include <stb_image.h>
#include <cmath>
namespace CoreTech {

bool Terrain::loadHeightmap(const std::string& path, int& w, int& h, std::vector<float>& data){
    int n=0;
    unsigned char* img = stbi_load(path.c_str(), &w, &h, &n, 1);
    if(!img) return false;
    data.resize(w*h);
    for(int y=0;y<h;y++) for(int x=0;x<w;x++){
        data[y*w+x] = img[y*w+x] / 255.0f;
    }
    stbi_image_free(img);
    return true;
}

TerrainMesh Terrain::generateMesh(const std::vector<float>& heightmap, int w, int h, float scale, int skip){
    TerrainMesh m; m.width = w; m.height = h;
    int W = (w + skip -1)/skip; int H = (h + skip -1)/skip;
    m.vertices.reserve(W*H*3); m.uvs.reserve(W*H*2);
    for(int y=0;y<h;y+=skip){
        for(int x=0;x<w;x+=skip){
            float hx = (float)x; float hy = (float)y;
            float hval = heightmap[y*w + x];
            m.vertices.push_back(hx * scale);
            m.vertices.push_back(hval * 10.0f); // height scale
            m.vertices.push_back(hy * scale);
            m.uvs.push_back((float)x / (float)w); m.uvs.push_back((float)y / (float)h);
        }
    }
    // indices
    for(int y=0;y<H-1;y++){
        for(int x=0;x<W-1;x++){
            int i0 = y*W + x;
            int i1 = y*W + (x+1);
            int i2 = (y+1)*W + x;
            int i3 = (y+1)*W + (x+1);
            // two triangles per quad
            m.indices.push_back(i0); m.indices.push_back(i2); m.indices.push_back(i1);
            m.indices.push_back(i1); m.indices.push_back(i2); m.indices.push_back(i3);
        }
    }
    computeNormals(m);
    return m;
}

void Terrain::computeNormals(TerrainMesh& m){
    m.normals.assign(m.vertices.size(), 0.0f);
    // accumulate face normals
    for(size_t i=0;i+2<m.indices.size(); i+=3){
        unsigned int ia = m.indices[i]*3;
        unsigned int ib = m.indices[i+1]*3;
        unsigned int ic = m.indices[i+2]*3;
        glm::vec3 a(m.vertices[ia+0], m.vertices[ia+1], m.vertices[ia+2]);
        glm::vec3 b(m.vertices[ib+0], m.vertices[ib+1], m.vertices[ib+2]);
        glm::vec3 c(m.vertices[ic+0], m.vertices[ic+1], m.vertices[ic+2]);
        glm::vec3 n = glm::normalize(glm::cross(b-a, c-a));
        m.normals[ia+0]+=n.x; m.normals[ia+1]+=n.y; m.normals[ia+2]+=n.z;
        m.normals[ib+0]+=n.x; m.normals[ib+1]+=n.y; m.normals[ib+2]+=n.z;
        m.normals[ic+0]+=n.x; m.normals[ic+1]+=n.y; m.normals[ic+2]+=n.z;
    }
    // normalize
    for(size_t i=0;i<m.normals.size(); i+=3){
        glm::vec3 v(m.normals[i+0], m.normals[i+1], m.normals[i+2]);
        glm::vec3 n = glm::normalize(v);
        m.normals[i+0]=n.x; m.normals[i+1]=n.y; m.normals[i+2]=n.z;
    }
}

}