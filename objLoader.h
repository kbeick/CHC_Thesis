#ifndef OBJ_LOADER_H
#define OBJ_LOADER_H
//scratch pixel
//http://www.scratchapixel.com/lessons/3d-advanced-lessons/obj-file-format/reading-an-obj-file/


/*!
    \file objloader.cpp
    \brief load an OBJ file and store its geometry/material in memory

    This code was adapted from the project Embree from Intel.
    Copyright 2009-2012 Intel Corporation

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
 
    Copyright 2013 Scratchapixel

    Compile with: clang++/c++ -o objloader objloader.cpp -O3 -Wall -std=c++0x
 */


#include <vector>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <cstdio>
#include <sstream>
#include <map>
#include <memory>
#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>

#include "utils.h"

#define MAX_LINE_LENGTH 10000

/*! returns the path of a file */
std::string getFilePath(const std::string &filename)
{
    size_t pos = filename.find_last_of('/');
    if (pos == std::string::npos) return filename;
    return filename.substr(0, pos);
}

/*! \struct Material
 *  \brief a simple structure to store material's properties
 */
struct Material
{
    Vec3f Ka, Kd, Ks;   /*! ambient, diffuse and specular rgb coefficients */
    float d;            /*! transparency */
    float Ns, Ni;       /*! specular exponent and index of refraction */
};

/*! \class TriangleMesh
 *  \brief a basic class to store a triangle mesh data
 */
class TriangleMesh
{
public:
    Vec3f *positions;   /*! position/vertex array */
    Vec3f *normals;     /*! normal array (can be null) */
    Vec2f *texcoords;   /*! texture coordinates (can be null) */
    int numTriangles;   /*! number of triangles */
    int *triangles;     /*! triangle index list */
    TriangleMesh() : positions(NULL), normals(NULL), texcoords(NULL), triangles(NULL) {}
    ~TriangleMesh()
    {
        if (positions) delete [] positions;
        if (normals)   delete [] normals;
        if (texcoords) delete [] texcoords;
        if (triangles) delete [] triangles;
    }
};

// trim from start
static inline std::string &ltrim(std::string &s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
        return s;
}

// trim from end
static inline std::string &rtrim(std::string &s) {
        s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
        return s;
}

// trim from both ends
static inline std::string &trim(std::string &s) {
        return ltrim(rtrim(s));
}

/*! \class Primitive
 *  \brief a basic class to store a primitive (defined by a mesh and a material)
 */
struct Primitive
{
    Primitive( TriangleMesh *m, Material *mat) : 
        mesh(m), material(mat) {}
     TriangleMesh* mesh;   /*! the object's geometry */
     Material* material;   /*! the object's material */
};

/*! Three-index vertex, indexing start at 0, -1 means invalid vertex. */
struct Vertex {
    int v, vt, vn;
    Vertex() {};
    Vertex(int v) : v(v), vt(v), vn(v) {};
    Vertex(int v, int vt, int vn) : v(v), vt(vt), vn(vn) {};
};

// need to declare this operator if we want to use Vertex in a map
static inline bool operator < ( const Vertex& a, const Vertex& b ) {
    if (a.v  != b.v)  return a.v  < b.v;
    if (a.vn != b.vn) return a.vn < b.vn;
    if (a.vt != b.vt) return a.vt < b.vt;
    return false;
}

/*! Parse separator. */
static inline const char* parseSep(const char*& token) {
    size_t sep = strspn(token, " \t");
    if (!sep) throw std::runtime_error("separator expected");
    return token+=sep;
}

/*! Read float from a string. */
static inline float getFloat(const char*& token) {
    token += strspn(token, " \t");
    float n = (float)atof(token);
    token += strcspn(token, " \t\r");
    return n;
}

/*! Read Vec2f from a string. */
static inline Vec2f getVec2f(const char*& token) {
    float x = getFloat(token);
    float y = getFloat(token);
    return Vec2f(x,y);
}

/*! Read Vec3f from a string. */
static inline Vec3f getVec3f(const char*& token) {
    float x = getFloat(token);
    float y = getFloat(token);
    float z = getFloat(token);
    return Vec3f(x, y, z);
}

/*! Parse optional separator. */
static inline const char* parseSepOpt(const char*& token) {
    return token+=strspn(token, " \t");
}

/*! Determine if character is a separator. */
static inline bool isSep(const char c) {
    return (c == ' ') || (c == '\t');
}

class ObjReader
{
public:
    ObjReader(const char *filename);
    Vertex getInt3(const char*& token);
    int fix_v(int index)  { return(index > 0 ? index - 1 : (index == 0 ? 0 : (int)v .size() + index)); }
    int fix_vt(int index) { return(index > 0 ? index - 1 : (index == 0 ? 0 : (int)vt.size() + index)); }
    int fix_vn(int index) { return(index > 0 ? index - 1 : (index == 0 ? 0 : (int)vn.size() + index)); }
    std::vector<Vec3f> v, vn;
    std::vector<Vec2f> vt;
    std::vector<std::vector<Vertex> > curGroup;
    std::map<std::string, Material*> materials;
    Material* curMaterial;
    void loadMTL(const std::string &mtlFilename);
    void flushFaceGroup();
    unsigned int getVertex(std::map<Vertex, unsigned int>&, std::vector<Vec3f>&, std::vector<Vec3f>&, std::vector<Vec2f>&, const Vertex&);
    std::vector<Primitive* > model;
    void printStats();
    void extractNormals();
    bool hasNormals;
    int totalTriangles;
    void getRawData(float *&verts, float *&normals);
};


/*! Parse differently formated triplets like: n0, n0/n1/n2, n0//n2, n0/n1.          */
/*! All indices are converted to C-style (from 0). Missing entries are assigned -1. */
Vertex ObjReader::getInt3(const char*& token)
{
    Vertex v(-1);
    v.v = fix_v(atoi(token));
    token += strcspn(token, "/ \t\r");
    if (token[0] != '/') return(v);
    token++;
    
    // it is i//n
    if (token[0] == '/') {
        token++;
        v.vn = fix_vn(atoi(token));
        token += strcspn(token, " \t\r");
        return(v);
    }
    
    // it is i/t/n or i/t
    v.vt = fix_vt(atoi(token));
    token += strcspn(token, "/ \t\r");
    if (token[0] != '/') return(v);
    token++;
    
    // it is i/t/n
    v.vn = fix_vn(atoi(token));
    token += strcspn(token, " \t\r");
    return(v);
}

/*! \brief load a OBJ material file
 *  \param mtlFilename is the full path to the material file
 */
void ObjReader::loadMTL(const std::string &mtlFilename)
{
    std::ifstream ifs;
    std::string trimmedFilename=mtlFilename;
    trimmedFilename=trim(trimmedFilename);
    ifs.open( trimmedFilename.c_str());
    if (!ifs.is_open()) {
        std::cerr << "can't open " << mtlFilename << std::endl;
        return;
    }
    Material* mat;
    while (ifs.peek() != EOF) {
        char line[MAX_LINE_LENGTH];
        ifs.getline(line, sizeof(line), '\n');
        const char* token = line + strspn(line, " \t"); // ignore spaces and tabs
        if (token[0] == 0) continue; // ignore empty lines
        if (token[0] == '#') continue; // ignore comments

        if (!strncmp(token, "newmtl", 6)) {
            parseSep(token += 6);
            std::string name(token); printf("Name of the material %s\n", name.c_str());
            mat = new Material;
            materials[name] = mat;
            continue;
        }

        if (!mat) throw std::runtime_error("invalid material file: newmtl expected first");
        
        if (!strncmp(token, "d", 1))  { parseSep(token += 1); mat->d = getFloat(token); continue; }
        if (!strncmp(token, "Ns", 1)) { parseSep(token += 2); mat->Ns = getFloat(token); continue; }
        if (!strncmp(token, "Ns", 1)) { parseSep(token += 2); mat->Ni = getFloat(token); continue; }
        if (!strncmp(token, "Ka", 2)) { parseSep(token += 2); mat->Ka = getVec3f(token); continue; }
        if (!strncmp(token, "Kd", 2)) { parseSep(token += 2); mat->Kd = getVec3f(token); continue; }
        if (!strncmp(token, "Ks", 2)) { parseSep(token += 2); mat->Ks = getVec3f(token); continue; }
    }
    ifs.close();
}

/*! \brief load the geometry defined in an OBJ/Wavefront file
 *  \param filename is the path to the OJB file
 */
ObjReader::ObjReader(const char *filename)
{
    std::ifstream ifs;
    // extract the path from the filename (used to read the material file)
    std::string path = getFilePath(filename);
    try {
        ifs.open(filename);
        if (ifs.fail()) throw std::runtime_error("can't open file " + std::string(filename));

        // create a default material
        Material* defaultMaterial(new Material);
        curMaterial = defaultMaterial;

        char line[MAX_LINE_LENGTH]; // line buffer
        
        while (ifs.peek() != EOF) // read each line until EOF is found
        {
            ifs.getline(line, sizeof(line), '\n');
            const char* token = line + strspn(line, " \t"); // ignore space and tabs

            if (token[0] == 0) continue; // line is empty, ignore
            // read a vertex
            if (token[0] == 'v' && isSep(token[1])) { v.push_back(getVec3f(token += 2)); continue; }
            // read a normal
            if (!strncmp(token, "vn",  2) && isSep(token[2])) { vn.push_back(getVec3f(token += 3)); continue; }
            // read a texture coordinates
            if (!strncmp(token, "vt",  2) && isSep(token[2])) { vt.push_back(getVec2f(token += 3)); continue; }
            // read a face
            if (token[0] == 'f' && isSep(token[1])) {
                //cerr<<"f!!"<<endl;
                parseSep(token += 1);
                std::vector<Vertex> face;
                while (token[0]!=13 && token[0]!=0) {// 13= CR, \n stripped?
                    //cerr<<"w!!"<<endl;
                    //int t=(int)token[0];
                   // cerr<<t<<endl;
                    face.push_back(getInt3(token));
                    parseSepOpt(token);
                }
                curGroup.push_back(face);
                continue;
            }
            
            /*! use material */
            if (!strncmp(token, "usemtl", 6) && isSep(token[6]))
            {
                //cerr<<"usemtl!!"<<endl;
                flushFaceGroup();
                std::string name(parseSep(token += 6));
                if (materials.find(name) == materials.end()) curMaterial = defaultMaterial;
                else curMaterial = materials[name];
                continue;
            }
            
            /* load material library */
            if (!strncmp(token, "mtllib", 6) && isSep(token[6])) {
                loadMTL(path + "/" + std::string(parseSep(token += 6)));
                continue;
            }
        }
    }
    catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        cerr<<"Here!!"<<endl;
    }
    
    flushFaceGroup(); // flush the last loaded object
    ifs.close();
    printStats();
}


/*! \brief utility function to keep track of the vertex already used while creating a new mesh
 *  \param vertexMap is a map used to keep track of the vertices already inserted in the position list
 *  \param position is a position list for the newly created mesh
 *  \param normals is a normal list for the newly created mesh
 *  \param texcoords is a texture coordinate list for the newly created mesh
 *  \param i is the Vertex looked for or inserted in vertexMap
 *  \return the index of this Vertex in the position vector list.
 */
unsigned int ObjReader::getVertex(
    std::map<Vertex, unsigned int> &vertexMap, 
    std::vector<Vec3f> &positions, 
    std::vector<Vec3f> &normals,
    std::vector<Vec2f> &texcoords,
    const Vertex &i)
{
    const std::map<Vertex, unsigned int>::iterator& entry = vertexMap.find(i);
    if (entry != vertexMap.end()) return(entry->second);
    
    positions.push_back(v[i.v]);
    if (i.vn >= 0) normals.push_back(vn[i.vn]);
    if (i.vt >= 0) texcoords.push_back(vt[i.vt]);
    return (vertexMap[i] = int(positions.size()) - 1);
}

/*! \brief flush the current content of currGroup and create new mesh 
 */
void ObjReader::flushFaceGroup()
{
    if (curGroup.empty()) return;
    
    // temporary data arrays
    std::vector<Vec3f> positions;
    std::vector<Vec3f> normals;
    std::vector<Vec2f> texcoords;
    std::vector<Vec3i> triangles;
    std::map<Vertex, unsigned int> vertexMap;
    
    // merge three indices into one
    for (size_t j = 0; j < curGroup.size(); j++)
    {
        /* iterate over all faces */
        const std::vector<Vertex>& face = curGroup[j];
        Vertex i0 = face[0], i1 = Vertex(-1), i2 = face[1];
        
        /* triangulate the face with a triangle fan */
        for (size_t k = 2; k < face.size(); k++) {
            i1 = i2; i2 = face[k];
            unsigned int v0 = getVertex(vertexMap, positions, normals, texcoords, i0);
            unsigned int v1 = getVertex(vertexMap, positions, normals, texcoords, i1);
            unsigned int v2 = getVertex(vertexMap, positions, normals, texcoords, i2);
            triangles.push_back(Vec3i(v0, v1, v2));
        }
    }
    curGroup.clear();

    // create new triangle mesh, allocate memory and copy data
    TriangleMesh* mesh = new TriangleMesh;
    mesh->numTriangles = triangles.size();
    mesh->triangles = new int[mesh->numTriangles * 3];
    memcpy(mesh->triangles, &triangles[0], sizeof(Vec3i) * mesh->numTriangles);
    mesh->positions = new Vec3f[positions.size()];
    memcpy(mesh->positions, &positions[0], sizeof(Vec3f) * positions.size());
    if (normals.size()) {
        mesh->normals = new Vec3f[normals.size()];
        memcpy(mesh->normals, &normals[0], sizeof(Vec3f) * normals.size());
    }
    if (texcoords.size()) {
        mesh->texcoords = new Vec2f[texcoords.size()];
        memcpy(mesh->texcoords, &texcoords[0], sizeof(Vec2f) * texcoords.size());
    }
    model.push_back(new Primitive(mesh, curMaterial));
}

void ObjReader::printStats()
{
    int  size               =model.size();   
    int  totalTri           =0;
    bool hasPositions       =true;
    hasNormals              =true;
    bool hasTextureCoords   =true;
    for(int i=0; i<size;i++)
    {
        int s=model[i]->mesh->numTriangles;
        totalTri+=model[i]->mesh->numTriangles;
        if(model[i]->mesh->positions==NULL)   hasPositions      =false;
        if(model[i]->mesh->normals  ==NULL)   hasNormals        =false;
        if(model[i]->mesh->texcoords==NULL)   hasTextureCoords  =false;
        if(model[i]->mesh->normals  ==NULL){
          //cerr<<" "<<i;  
        }
        //cerr<<"Begin Tris: "<<endl;
        //if(i%34==0)
        //{

            //cerr<<s<<endl;
        int count=0;
        //cerr<<"Has "<<hasNormals<<endl;
        for (int j=0;j<s*3;j++)
        {
            count=std::max(model[i]->mesh->triangles[j],count);
            //int t=model[i]->mesh->triangles[j];
            //cerr<<t<<" ";
            
            //cerr<<model[i]->mesh->positions[t]<<endl;
            //if((j+1)%3==0 && j!=0) cerr<<endl;
        }
        count++;

        //cerr<<"Number of verts : "<<count<<endl;
       // }

    }
    cerr<<"Model Stats: "<<endl;
    cerr<<"Meshes in model : "<<size<<endl;
    cerr<<"Total triangles: "<<totalTri<<endl;
    if(hasPositions)        cerr<<"Positions included"<<endl;
    if(hasNormals)          cerr<<"Normals included"<<endl;
    if(hasTextureCoords)    cerr<<"Texture Coords included"<<endl;
    totalTriangles=totalTri;

}

void ObjReader::extractNormals()
{
    
    int  size               =model.size();   
    int  totalTri           =0;

    cerr<<"Extracting Normals..."<<endl;
    for(int i=0; i<size;i++)
    {
        int s=model[i]->mesh->numTriangles;
        int count=0;//find the number of verticies
        //cerr<<" "<<i;
        for (int j=0;j<s*3;j++)
        {
            count=std::max(model[i]->mesh->triangles[j],count);
            
        }
        if(count>0)
        {   
            //allocate space
            //cerr<<"Extracting "<<count<<endl;
            //cerr<<model[i]->mesh->normals<<endl;
            count++;//????
            Vec3f* norms= new Vec3f[count];
            model[i]->mesh->normals = &norms[0];
            //cerr<<" "<<model[i]->mesh->normals<<endl;
            //cerr<<"after Countaa "<<count<<endl;
            int* sharedVertexCount  = new int[count]; //TODO DELETE
            memset(sharedVertexCount,0,count*sizeof(int));
            Vec3f *norm;
            if(model[i]->mesh==NULL) cerr<<" mesh"<<model[i]->mesh<<endl;
            for(int j=0;j<s;j++)
            {
                //extract the normals
                int v0,v1,v2;
                v0=model[i]->mesh->triangles[j*3  ];
                v1=model[i]->mesh->triangles[j*3+1];
                v2=model[i]->mesh->triangles[j*3+2];
                //cerr<<"Vs "<<v0<<" "<<v1<<" "<<v2<<endl;
                sharedVertexCount[v0]++;
                sharedVertexCount[v1]++;
                sharedVertexCount[v2]++;

                Vec3f a;
                Vec3f b;
                Vec3f n;
                //a=v1-v0
                a.x=model[i]->mesh->positions[v1].x-model[i]->mesh->positions[v0].x;
                a.y=model[i]->mesh->positions[v1].y-model[i]->mesh->positions[v0].y;
                a.z=model[i]->mesh->positions[v1].z-model[i]->mesh->positions[v0].z;
                ///cerr<<"a "<<a<<endl;
                //b=v2-v0
                b.x=model[i]->mesh->positions[v2].x-model[i]->mesh->positions[v0].x;
                b.y=model[i]->mesh->positions[v2].y-model[i]->mesh->positions[v0].y;
                b.z=model[i]->mesh->positions[v2].z-model[i]->mesh->positions[v0].z;
                //cerr<<"b "<<b<<endl;
                //a cross b
                n.x = a.y*b.z - a.z*b.y;
                n.y = a.z*b.x - a.x*b.z;
                n.z = a.x*b.y - a.y*b.x;
                //cerr<<"n "<<n<<endl;
                //add normals
                
                model[i]->mesh->normals[v0].x+=n.x;
                //cerr<<"kKMMMMM "<< model[i]->mesh->normals[v0]<<endl;
                
                model[i]->mesh->normals[v0].y+=n.y;
                model[i]->mesh->normals[v0].z+=n.z;
                
                model[i]->mesh->normals[v1].x+=n.x;
                model[i]->mesh->normals[v1].y+=n.y;
                model[i]->mesh->normals[v1].z+=n.z;

                model[i]->mesh->normals[v2].x+=n.x;
                model[i]->mesh->normals[v2].y+=n.y;
                model[i]->mesh->normals[v2].z+=n.z;
                
            }

            //cycle back over the normals and average them, then normalize
            //cerr<<"COUNT "<<count<<endl;
            for (int j=0;j<count;j++)
            {
                norm=&model[i]->mesh->normals[j];
                norm->x/=sharedVertexCount[j];
                norm->y/=sharedVertexCount[j];
                norm->z/=sharedVertexCount[j];

                float m=sqrt(norm->x*norm->x+norm->y*norm->y+norm->z*norm->z);

                if(m==0) {cerr<<"DIVIDE BY ZER0"<<endl; exit(1);}
                

                norm->x*=m;
                norm->y*=m;
                norm->z*=m;
                //cerr<<"Shared "<<sharedVertexCount[j]<<endl;
                //cerr<<j<<" Norms : "<<*norm<<" "<<model[i]->mesh->normals[j]<<endl;
            }
            norm=NULL;
           //delete sharedVertexCount;

        } 

    }

}

void ObjReader::getRawData(float *&verts, float *&normals)
{
    if(totalTriangles>0) 
    {
        verts    = new float[totalTriangles*9];
        normals  = new float[totalTriangles*9];

        int     size = model.size();
        int     index= 0;

        for(int i=0; i<size;i++) //for each model
        {
            int s=model[i]->mesh->numTriangles;
            for(int j=0;j<s;j++) //for each triangle
            {
                int v0,v1,v2;
                v0=model[i]->mesh->triangles[j*3  ];
                v1=model[i]->mesh->triangles[j*3+1];
                v2=model[i]->mesh->triangles[j*3+2];
                //v0
                verts[index]=model[i]->mesh->positions[v0].x;
                normals[index]=model[i]->mesh->normals[v0].x;
                index++;
                verts[index]=model[i]->mesh->positions[v0].y;
                normals[index]=model[i]->mesh->normals[v0].y;
                index++;
                verts[index]=model[i]->mesh->positions[v0].z;
                normals[index]=model[i]->mesh->normals[v0].z;
                index++;
                //v1
                verts[index]=model[i]->mesh->positions[v1].x;
                normals[index]=model[i]->mesh->normals[v1].x;
                index++;
                verts[index]=model[i]->mesh->positions[v1].y;
                normals[index]=model[i]->mesh->normals[v1].y;
                index++;
                verts[index]=model[i]->mesh->positions[v1].z;
                normals[index]=model[i]->mesh->normals[v1].z;
                index++;
                //v2 
                verts[index]=model[i]->mesh->positions[v2].x;
                normals[index]=model[i]->mesh->normals[v2].x;
                index++;
                verts[index]=model[i]->mesh->positions[v2].y;
                normals[index]=model[i]->mesh->normals[v2].y;
                index++;
                verts[index]=model[i]->mesh->positions[v2].z;
                normals[index]=model[i]->mesh->normals[v2].z;
                index++;
            }

        }//for
        cerr<<"Number of Verts output : "<<index<<" for "<<totalTriangles<<" triangles"<<endl;
    }//if
    else{
        cerr<<"No Triangles to output"<<endl;
        verts=NULL;
    }
}

#endif
