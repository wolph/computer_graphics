#include "Mesh.hpp"
#include <ctime>

const unsigned int LINE_LEN = 256;

/************************************************************
 * draw
 ************************************************************/
void Mesh::drawSmooth(){

    glBegin(GL_TRIANGLES);

    for(unsigned int i = 0;i < triangles.size();++i){
        Vec3Df col = this->materials[triangleMaterials[i]].Kd();

        glColor3fv(col.pointer());
        for(int v = 0;v < 3;v++){
            glNormal3f(triangles[i].vertices[v].n[0],
                    triangles[i].vertices[v].n[1],
                    triangles[i].vertices[v].n[2]);
            glVertex3f(triangles[i].vertices[v].p[0],
                    triangles[i].vertices[v].p[1],
                    triangles[i].vertices[v].p[2]);
        }

    }
    glEnd();
}

void Mesh::draw(){
    glBegin(GL_TRIANGLES);

    for(unsigned int i = 0;i < triangles.size();++i){
        unsigned int triMat = triangleMaterials.at(i);
        Vec3Df col = this->materials.at(triMat).Kd();
        glColor3fv(col.pointer());
        const Vec3Df edge01 = triangles[i].vertices[1].p - triangles[i].vertices[0].p;
        const Vec3Df edge02 = triangles[i].vertices[2].p - triangles[i].vertices[0].p;
        Vec3Df n = Vec3Df::crossProduct(edge01, edge02);
        n.normalize();
        glNormal3f(n[0], n[1], n[2]);
        for(int v = 0;v < 3;v++){
            glVertex3f(triangles[i].vertices[v].p[0],
                    triangles[i].vertices[v].p[1],
                    triangles[i].vertices[v].p[2]);
        }

    }
    glEnd();
}

bool Mesh::loadMesh(const char * filename, bool randomizeTriangulation){
    vertices.clear();
    triangles.clear();
    texcoords.clear();

    std::vector<unsigned int> vhandles;
    std::vector<unsigned int> texhandles;
    std::vector<std::vector<unsigned int>> tempTriangles;

    if(randomizeTriangulation)
        /* Randomization should use an actual _RANDOM_ thing */
        srand((unsigned int)time(NULL));

    materials.clear();
    Material defaultMat;
    defaultMat.set_Kd(0.5f, 0.5f, 0.5f);
    defaultMat.set_Ka(0.f, 0.f, 0.f);
    defaultMat.set_Ks(0.5f, 0.5f, 0.5f);
    defaultMat.set_Ns(96.7f);
    //defaultMat.set_Ni();
    //defaultMat.set_Tr();
    defaultMat.set_illum(2);
    defaultMat.set_name(std::string("StandardMaterialInitFromTriMesh"));
    materials.push_back(defaultMat);

    std::map<std::string, unsigned int> materialIndex;
    char s[LINE_LEN];
    float x, y, z;

    //we replace the \ by /
    std::string realFilename(filename);
    for(unsigned int i = 0;i < realFilename.length();++i){
        if(realFilename[i] == '\\')
            realFilename[i] = '/';
    }

    std::vector<Vec3Df> normals;
    std::string matname;
    std::string path_;
    std::string temp(realFilename);
    int pos = (int)temp.rfind("/");

    if (pos < 0){
        path_ = "";
    } else {
        path_ = temp.substr(0, pos + 1);
    }
    memset(&s, 0, LINE_LEN);

    FILE * in;
    in = fopen(filename, "r");

    while(in && !feof(in) && fgets(s, LINE_LEN, in)){
        // comment
        if(s[0] == '#' || isspace(s[0]) || s[0] == '\0')
            continue;

        // material file
        else if(strncmp(s, "mtllib ", 7) == 0){
            // Which material file to include
            char *p0 = s + 6, *p1;
            while(isspace(*++p0))
                ;
            p1 = p0;
            std::string t = p1;
            unsigned int i;
            for(i = 0;i < t.length();++i){
                if(t[i] < 32 || t[i] == (char)255){
                    break;
                }
            }
            std::string file;
            if(t.length() == i)
                file = path_.append(t);
            else
                file = path_.append(t.substr(0, i));
            printf("Load material file %s\n", file.c_str());
            loadMtl(file.c_str(), materialIndex);
        }
        // usemtl
        else if(strncmp(s, "usemtl ", 7) == 0){
            /* The material to use for this object
             
             The use material command lets you name a material to use. All 'f'
             face commands that follow will use the same material, until
             another usemtl command is encountered. For all of the Poser OBJ
             files I've seen, all 'g' commands should be followed by a 'usemtl'
             command.
            */
            
            char *p0 = s + 6, *p1;
            while(isspace(*++p0))
                ;
            p1 = p0;
            while(!isspace(*p1))
                ++p1;
            *p1 = '\0';
            matname = p0;
            if(materialIndex.find(matname) == materialIndex.end()){
                printf(
                        "Warning! Material '%s' not defined in material file. Taking default!\n",
                        matname.c_str());
                throw std::runtime_error("Material is missing, exiting.");
                matname = "";
            }
        }

        // vertex
        else if(strncmp(s, "v ", 2) == 0){
            /* The vertices themselves, these are just points in the geometry
             
             The vertex command, this specifies a vertex by its three
             coordinates. The vertex is implicitly named by the order it is
             found in the file. For example, the first vertex in the file is
             referenced as '1', the second as '2' and so on. None of the vertex
             commands actually specify any geometry, they are just points in
             space.
             */
            sscanf(s, "v %f %f %f", &x, &y, &z);
			vertices.push_back(Vec3Df(x, y, z));
        }

        // texture coord
        else if(strncmp(s, "vt ", 3) == 0){
            /* Vertex texture mapping
             
             The vertex texture command specifies the UV (and optionally W)
             mapping. These will be floating point values between 0 and 1 which
             say how to map the texture. They really don't tell you anything by
             themselves, they must be grouped with a vertex in a 'f' face
             command.
            */
            //we do nothing
            Vec3Df textureCoordinates(0, 0, 0);

            //we only support 2d tex coords
            sscanf(s, "vt %f %f", &textureCoordinates[0], &textureCoordinates[1]);
            texcoords.push_back(textureCoordinates);
        }
        // normal
        else if(strncmp(s, "vn ", 3) == 0){
            /* The normals, we calculate them in the triangles
             
             The vertex normal command specifies a normal vector. A lot of
             times these aren't used, because the 'f' face command will use the
             order the 'v' commands are given to determine the normal instead.
             Like the 'vt' commands, they don't mean anything until grouped
             with a vertex in the 'f' face command.
            */
        }
        // face
        else if(strncmp(s, "f ", 2) == 0){
            /* Create a polygon from the points added to the vertices list
             
             The face command specifies a polygon made from the verticies
             listed. You may have as many verticies as you like. To reference a
             vertex you just give its index in the file, for example 'f 54 55
             56 57' means a face built from vertecies 54 - 57. For each vertex,
             there may also be an associated vt, which says how to map the
             texture at this point, and/or a vn, which specifies a normal at
             this point. If you specify a vt or vn for one vertex, you must
             specify one for all. If you want to have a vertex and a vertex
             normal, but no vertex texture, it will look like: 'f v1//vt1'. The
             normal is what tells it which way the polygon faces. If you don't
             give one, it is determined by the order the verticies are given.
             They are assumed to be in counter-clockwise direction. If you
             aren't using vn's to specify the normal and you wanted to 'flip
             the normal' you would reverse the order of the verticies. In most
             3D programs, if the normal points the wrong way, there will appear
             to be a hole in the object. Luckily, Poser 3 renders both sides of
             a polygon, so even if you are editing something in another program
             that looks like it has holes, they will effectively go away inside
             Poser (NOTE: I read that somewhere, but haven't personally
             confirmed that fact.) One more thing, if you ever see a negative
             v, vt, or vn, that is a relative offset. It means go back that
             many verticies from where you are now in the file to find the
             vertex. This is part of the OBJ file spec, but I haven't seen any
             Poser OBJs that use it.
             */

            int component(0), nV(0);
            bool endOfVertex(false);
            char *p0, *p1(s + 2); //place behind the "f "

            vhandles.clear();
            texhandles.clear();

            while(*p1 == ' ')
                ++p1; // skip white-spaces

            while(p1){
                p0 = p1;

                // overwrite next separator

                // skip '/', '\n', ' ', '\0', '\r' <-- don't forget Windows
                while(*p1 != '/' && *p1 != '\r' && *p1 != '\n' && *p1 != ' '
                        && *p1 != '\0')
                    ++p1;

                // detect end of vertex
                if(*p1 != '/')
                    endOfVertex = true;

                // replace separator by '\0'
                if(*p1 != '\0'){
                    *p1 = '\0';
                    p1++; // point to next token
                }

                // detect end of line and break
                if(*p1 == '\0' || *p1 == '\n')
                    p1 = 0;

                // read next vertex component
                if(*p0 != '\0'){
                    switch(component){
                        case 0: // vertex
                        {
                            int tmp = atoi(p0) - 1;
                            vhandles.push_back(tmp);
                        }
                            break;

                        case 1: // texture coord
                        {
                            int tmp = atoi(p0) - 1;
                            texhandles.push_back(tmp);
                        }
                            break;

                        case 2: // normal
                            //assert(!vhandles.empty());
                            //assert((unsigned int)(atoi(p0)-1) < normals.size());
                            //_bi.set_normal(vhandles.back(), normals[atoi(p0)-1]);
                            break;
                    }
                }

                ++component;

                if(endOfVertex){
                    component = 0;
                    nV++;
                    endOfVertex = false;
                }
            }

            if(vhandles.size() != texhandles.size())
                texhandles.resize(vhandles.size(), 0);

			unsigned int mat = 0;

			if (!materialIndex.count(matname))
				printf("Material %s not found!\n", matname.c_str());
			else
				mat = materialIndex.find(matname)->second;

            if(vhandles.size() > 3){
                //model is not triangulated, so let us do this on the fly...
                //to have a more uniform mesh, we add randomization
                unsigned int k = (false) ? (rand() % vhandles.size()) : 0;
                for(unsigned int i = 0;i < vhandles.size() - 2;++i){
                    const int v0 = (k + 0) % vhandles.size();
                    const int v1 = (k + i + 1) % vhandles.size();
                    const int v2 = (k + i + 2) % vhandles.size();

                    const int t0 = (k + 0) % vhandles.size();
                    const int t1 = (k + i + 1) % vhandles.size();
                    const int t2 = (k + i + 2) % vhandles.size();

                    tempTriangles.push_back(std::vector<unsigned int> {vhandles[v0],
                            vhandles[v1], vhandles[v2], texhandles[t0],
                            texhandles[t1], texhandles[t2],
                            mat});
                    triangleMaterials.push_back(mat);
                }
            }else if(vhandles.size() == 3){
                triangleMaterials.push_back(mat);
                tempTriangles.push_back(std::vector<unsigned int> {vhandles[0],
                        vhandles[1], vhandles[2], texhandles[0], texhandles[1],
                        texhandles[2], mat});

            }else{
                printf(
                        "TriMesh::LOAD: Unexpected number of face vertices (<3). Ignoring face");
            }
        }
        memset(&s, 0, LINE_LEN);
    }

	// calculate vertex normals
	for (Vertex& v : vertices)
		v.n = Vec3Df(0, 0, 0);

	for (auto& triangle : tempTriangles) {
		Triangle tr = Triangle(vertices[triangle[0]], vertices[triangle[1]],
			vertices[triangle[2]], materials[triangle[6]]);

		vertices[triangle[0]].n += tr.normal;
		vertices[triangle[1]].n += tr.normal;
		vertices[triangle[2]].n += tr.normal;
	}

	for (auto& v : vertices)
		v.n.normalize();

	// make triangles
    for(auto& triangle : tempTriangles){
		if (!texcoords.empty())
			triangles.push_back(Triangle(vertices[triangle[0]], vertices[triangle[1]],
                        vertices[triangle[2]], texcoords[triangle[3]],
						texcoords[triangle[4]], texcoords[triangle[5]], materials[triangle[6]]));
		else
			triangles.push_back(Triangle(vertices[triangle[0]], vertices[triangle[1]],
						vertices[triangle[2]], materials[triangle[6]]));
    }

    fclose(in);
    return true;
}

bool Mesh::loadMtl(const char * filename,
        std::map<std::string, unsigned int> & materialIndex){
    FILE * _in;
    _in = fopen(filename, "r");

    if(!_in){
        printf("  Warning! Material file '%s' not found!\n", filename);
        return false;
    }

    char line[LINE_LEN];
    std::string textureName;

    std::string key;
    Material mat;
    float f1, f2, f3;
    bool indef = false;

    memset(line, 0, LINE_LEN);
    while(_in && !feof(_in)){
        fgets(line, LINE_LEN, _in);
        if(line[0] == '#') // skip comments
                {
            memset(line, 0, LINE_LEN);
            continue;
        }

        else if(isspace(line[0]) || line[0] == '\0'){
            if(indef && !key.empty() && mat.is_valid()){
                if(materialIndex.find(key) == materialIndex.end()){
                    mat.set_name(key);
                    materials.push_back(mat);
                    materialIndex[key] = (unsigned int)materials.size() - 1;
                }
                mat.cleanup();
            }
            if(line[0] == '\0')
                break;
        }else if(strncmp(line, "newmtl ", 7) == 0) // begin new material definition
                {

            char *p0 = line + 6, *p1;
            while(isspace(*++p0))
                ;
            p1 = p0;
            while(!isspace(*p1))
                ++p1;
            *p1 = '\0';
            key = p0;
            indef = true;
        }else if(strncmp(line, "Kd ", 3) == 0) // diffuse color
                {
            sscanf(line, "Kd %f %f %f", &f1, &f2, &f3);
            mat.set_Kd(f1, f2, f3);
        }else if(strncmp(line, "Ka ", 3) == 0) // ambient color
                {
            sscanf(line, "Ka %f %f %f", &f1, &f2, &f3);
            mat.set_Ka(f1, f2, f3);
        }else if(strncmp(line, "Ks ", 3) == 0) // specular color
                {
            sscanf(line, "Ks %f %f %f", &f1, &f2, &f3);
            mat.set_Ks(f1, f2, f3);
        }else if(strncmp(line, "Ns ", 3) == 0) // Shininess [0..200]
                {
            sscanf(line, "Ns %f", &f1);
            mat.set_Ns(f1);
        }else if(strncmp(line, "Ni ", 3) == 0) // Shininess [0..200]
                {
            sscanf(line, "Ni %f", &f1);
            mat.set_Ni(f1);
        }else if(strncmp(line, "illum ", 6) == 0) // diffuse/specular shading model
                {
            int illum = -1;
            sscanf(line, "illum %i", &illum);
            mat.set_illum(illum);
        }else if(strncmp(line, "map_Kd ", 7) == 0) // map images
                {
            std::string t = &(line[7]);
            if(!t.empty() && t[t.length() - 1] == '\n'){
                t.erase(t.length() - 1);
            }

            // map_Kd, diffuse map
            // map_Ks, specular map
            // map_Ka, ambient map
            // map_Bump, bump map
            // map_d,  opacity map
            // just skip this
            mat.set_textureName(t);

        }else if(strncmp(line, "Tr ", 3) == 0) // transparency value
                {
            sscanf(line, "Tr %f", &f1);
            mat.set_Tr(f1);
        }else if(strncmp(line, "d ", 2) == 0) // transparency value
                {
            sscanf(line, "d %f", &f1);
            mat.set_Tr(f1);
        }

        if(feof(_in) && indef && mat.is_valid() && !key.empty()){
            if(materialIndex.find(key) == materialIndex.end()){
                mat.set_name(key);
                materials.push_back(mat);
                materialIndex[key] = (unsigned int)materials.size() - 1;
            }
        }
        memset(line, 0, LINE_LEN);
    }
    printf("%d  materials loaded.\n", (int)materials.size());
    fclose(_in);
    return true;
}
