#include "Mesh.h"
#include "Color.h"
#include "Applog.h"
#include "Material.h"
#include "ModuleScene.h"
#include "Application.h"
#include "MathGeoLib\MathGeoLib.h"
#include "glew-2.1.0\include\GL\glew.h"
#include "ModuleResourcesManager.h"
#include "ResourceTexture.h"


#include "Assimp\include\scene.h"

Mesh::Mesh(const aiMesh& imported_mesh, const aiScene& scene,const char* file_name) : id(App->scene->last_mesh_id++)
{
	if (LoadFromAssimpMesh(imported_mesh, scene))	LoadDataToVRAM();
	else											app_log->AddLog("error loading mesh for the component %s", imported_mesh.mName.C_Str());

	mesh_name = file_name;
	
}

Mesh::Mesh(float* _vertices, Tri* _tris, uint _num_vertices, uint _num_tris, const float3& centroid): id(App->scene->last_mesh_id++)
{
	num_vertices		= _num_vertices;
	vertex_one_buffer	= _vertices;
	num_tris			= _num_tris;
	tris				= _tris;
	


	calculateCentroidandHalfsize();
	this->centroid = centroid;
	LoadDataToVRAM();
}

Mesh::Mesh(PrimitiveTypes primitive) : id(App->scene->last_mesh_id++)
{
	switch (primitive)
	{
	case Primitive_Cube:	 BuildCube(); break;
	case Primitive_Plane:	 BuildPlane(); break;
	//case Primitive_Sphere:	 BuildSphere(); break;
	//case Primitive_Cylinder: BuildCylinder(); break;
	default:
		break;
	}

	calculateCentroidandHalfsize();
	LoadDataToVRAM();

}


Mesh::~Mesh()
{
	if(iboId != 0)
		glDeleteBuffers(1, &iboId);
	if (vboId != 0)
		glDeleteBuffers(1, &vboId);
	if (vaoId != 0)
		glDeleteBuffers(1, &vaoId);

	if (vertex_one_buffer)	delete vertex_one_buffer;
	if (tris)	delete tris;

}

void Mesh::LoadDataToVRAM()
{
	
	// create VBO
	glGenBuffers(1, &vboId);    // for vertex buffer

	// copy vertex attribs data to VBO
	glBindBuffer(GL_ARRAY_BUFFER, vboId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*num_vertices * 12, vertex_one_buffer, GL_STATIC_DRAW); // reserve space

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &iboId);    // for index buffer
	// copy index data to VBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Tri) * num_tris, tris, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


	
	//create VAO
	glGenVertexArrays(1, &vaoId);
	//bind VAO
	glBindVertexArray(vaoId);
	//bind vbo
	glBindBuffer(GL_ARRAY_BUFFER, vboId);
	

	//vertex pos
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 13 * sizeof(GLfloat), (void*)(0 * sizeof(GLfloat)));
	glEnableVertexAttribArray(0);
	//colors
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 13 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	//normals
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 13 * sizeof(GLfloat), (void*)(7 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);
	//texcoords
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 13 * sizeof(GLfloat), (void*)(10 * sizeof(GLfloat)));
	glEnableVertexAttribArray(3);

	//Unbind
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}



void Mesh::Draw(Material* mat, bool draw_as_selected)  const
{
	
	//Texture* diffuse_tex = mat ? mat->getTexture(DIFFUSE) : nullptr;
	Texture* diffuse_tex = nullptr;
	if(mat){
		uint diffuse_resource_id = mat->getTextureResource(DIFFUSE);
		if (diffuse_resource_id != 0) {
			ResourceTexture* diffuse_resource = (ResourceTexture*)App->resources->getResource(diffuse_resource_id);
			if(diffuse_resource)
				diffuse_tex = diffuse_resource->texture;
		}
	}

	if (diffuse_tex)			glEnable(GL_TEXTURE_2D);
	else if(!draw_as_selected)	glEnableClientState(GL_COLOR_ARRAY);

	// bind VBOs before drawing
	glBindVertexArray(vaoId);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboId);
	

	// enable vertex arrays
	/*glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);*/

	if (draw_as_selected)
	{
		glColor3f(0.0f, 1.0f, 0.0f);
		glLineWidth(2.5f);
	}

	if (diffuse_tex)		glBindTexture(GL_TEXTURE_2D, diffuse_tex->getGLid());

	//size_t Offset = sizeof(float3) * num_vertices;

	// specify vertex arrays with their offsets
	/*glVertexPointer(3, GL_FLOAT, 0, (void*)0);
	glNormalPointer(GL_FLOAT, 0, (void*)Offset);
	glColorPointer(3, GL_FLOAT, 0, (void*)(Offset * 2));
	glTexCoordPointer(2, GL_FLOAT, 0, (void*)(Offset * 3));*/
	glDrawElements(GL_TRIANGLES, num_tris * 3, GL_UNSIGNED_INT, NULL);


	if (diffuse_tex)		glBindTexture(GL_TEXTURE_2D, 0);
	else					glDisableClientState(GL_COLOR_ARRAY);

	if (draw_as_selected)
	{
		glColor3f(1.0f, 1.0f, 1.0f);
		glLineWidth(1.0f);
	}
	 
	/*glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);*/

	// unbind VBOs
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glUseProgram(0);

	if (diffuse_tex)		glDisable(GL_TEXTURE_2D);

}


void Mesh::DrawNormals() const
{
	glBegin(GL_LINES);

	float3 centroid = float3::zero;
	float3 h_s = float3::zero;

	for (int i = 0; i < num_vertices; i+=13)
	{
		glVertex3f(vertex_one_buffer[i], vertex_one_buffer[i+1], vertex_one_buffer[i+2]);
		glVertex3f(vertex_one_buffer[i] + (vertex_one_buffer[i+7] * 0.2f), vertex_one_buffer[i+1] + (vertex_one_buffer[i+8] * 0.2f), vertex_one_buffer[i+2] + (vertex_one_buffer[i+9] * 0.2f));
	}

	glEnd();
}

void Mesh::BuildCube()
{
	uint ind[6 * 6] =
	{
		2, 7, 6, 2, 3, 7, // front
		11, 9, 10, 11, 8, 9, // right
		1, 4, 0, 1, 5, 4, // back
		15, 13, 12, 15, 14, 13, // left
		1, 3, 2, 1, 0, 3, // top
		7, 5, 6, 7, 4, 5  // bottom
	};

	uint num_indices = 6 * 6;
	num_tris = num_indices / 3;
	uint size = sizeof(Tri) * num_tris;
	tris = new Tri[num_tris];
	memcpy(tris, ind, size);

	//  vertices ------------------------
	float vert[16 * 3] =
	{
		0.5f,  0.5f,  0.5f, // 0
		-0.5f,  0.5f,  0.5f, // 1
		-0.5f,  0.5f, -0.5f, // 2
		0.5f,  0.5f, -0.5f, // 3

		0.5f, -0.5f,  0.5f, // 4
		-0.5f, -0.5f,  0.5f, // 5
		-0.5f, -0.5f, -0.5f, // 6
		0.5f, -0.5f, -0.5f, // 7

		0.5f,  0.5f,  0.5f,  // 8
		0.5f, -0.5f,  0.5f,  // 9
		0.5f, -0.5f, -0.5f,  //10
		0.5f,  0.5f, -0.5f,  //11

		-0.5f,  0.5f,  0.5f,  //12
		-0.5f, -0.5f,  0.5f,  //13
		-0.5f, -0.5f, -0.5f,  //14
		-0.5f,  0.5f, -0.5f,  //15
	};

	num_vertices = 16;
	size = sizeof(float) * num_vertices * 3;
	float* vertices = new float[num_vertices * 3];
	memcpy(vertices, vert, size);

	// Load texture coords
	float texture_coords[16 * 3] =
	{
		1.f,  1.f,  0.f,
		0.f,  1.f,  0.f,
		0.f,  0.f,  0.f,
		1.f,  0.f,  0.f,

		1.f,  0.f,  0.f,
		0.f,  0.f,  0.f,
		0.f,  1.f,  0.f,
		1.f,  1.f,  0.f,

		1.f,  1.f,  0.f,
		0.f,  1.f,  0.f,
		0.f,  0.f,  0.f,
		1.f,  0.f,  0.f,

		0.f,  1.f,  0.f,
		1.f,  1.f,  0.f,
		1.f,  0.f,  0.f,
		0.f,  0.f,  0.f,
	};

	float* tex_coords = new float[num_vertices * 3];
	memcpy(tex_coords, texture_coords, size);

	vertex_one_buffer = new float[num_vertices * 13]; 

	float* normals = nullptr;
	float null[3] = { 0.f,0.f,0.f };
	float null_color[4] = { 1.f,1.f,1.f,1.f };
	for (int i = 0; i < num_vertices; ++i)
	{

		memcpy(vertex_one_buffer + i * 13, vertices + i * 3, sizeof(float) * 3);

		memcpy(vertex_one_buffer + i * 13 + 3, null_color, sizeof(float) * 4);

		if (normals != nullptr)
			memcpy(vertex_one_buffer + i * 13 + 7, normals + i * 3, sizeof(float) * 3);
		else
			memcpy(vertex_one_buffer + i * 13 + 7, null, sizeof(float) * 3);

		if (tex_coords != nullptr)
			memcpy(vertex_one_buffer + i * 13 + 10, tex_coords + i * 3, sizeof(float) * 3);
		else
			memcpy(vertex_one_buffer + i * 13 + 10, null, sizeof(float) * 3);

	}

}

void Mesh::BuildPlane(float sx, float sy)
{
	float length = sx;
	float width = sy;
	int resX = 2; 
	int resZ = 2;

	num_vertices = resX * resZ;
	float3 ver[4];
	for (int i = 0; i < resZ; ++i)
	{
		// [ -length / 2, length / 2 ]
		float zPos = ((float)i / (resZ - 1) - .5f) * length;
		for (int j = 0; j < resX; ++j)
		{
			// [ -width / 2, width / 2 ]
			float xPos = ((float)j / (resX - 1) - .5f) * width;
			ver[j + i * resX] = float3(xPos, 0.f, zPos);
		}
	}

	float* vertices = new float[num_vertices * 3];
	for (int i = 0; i < num_vertices; ++i)
	{
		memcpy(vertices + i * 3, ver[i].ptr(), sizeof(float) * 3);
	}

	uint num_indices = (resX - 1) * (resZ - 1) * 6;
	uint ind[6];
	int t = 0;
	for (int k = 0; k < num_indices / 6; ++k)
	{
		int l = k % (resX - 1) + (k / (resZ - 1) * resX);

		ind[t++] = l + resX;
		ind[t++] = l + 1;
		ind[t++] = l;
				   
		ind[t++] = l + resX;
		ind[t++] = l + resX + 1;
		ind[t++] = l + 1;
	}
	tris = new Tri[num_indices/3];
	memcpy(tris, ind, sizeof(Tri)*(num_indices/3));

	//uv
	float3 uvs[4];
	for (int v = 0; v < resZ; v++)
	{
		for (int u = 0; u < resX; u++)
		{
			uvs[u + v * resX] = float3((float)u / (resX - 1), (float)v / (resZ - 1), 0.f);
		}
	}

	float* tex_coords = new float[num_vertices * 3];
	for (int i = 0; i < num_vertices; ++i)
	{
		memcpy(tex_coords + i * 3, uvs[i].ptr(), sizeof(float) * 3);
	}

	vertex_one_buffer = new float[num_vertices * 13];
	float null[3] = { 0.f,0.f,0.f };
	float null_color[4] = { 1.f,1.f,1.f,1.f };
	for (int i = 0; i < num_vertices; ++i)
	{

		memcpy(vertex_one_buffer + i * 13, vertices + i * 3, sizeof(float) * 3);

		
		memcpy(vertex_one_buffer + i * 13 + 3, null_color, sizeof(float) * 4); //colors

		
		memcpy(vertex_one_buffer + i * 13 + 7, null, sizeof(float) * 3); //normals

		if (tex_coords != nullptr)
			memcpy(vertex_one_buffer + i * 13 + 10, tex_coords + i * 3, sizeof(float) * 3);
		else
			memcpy(vertex_one_buffer + i * 13 + 10, null, sizeof(float) * 3);

	}
}

/*void Mesh::BuildSphere(float radius, float sectorCount, float stackCount) {

	// Sphere (code from http://www.songho.ca/opengl/gl_sphere.html)

	mesh_name = "SPHERE";	
	float x, y, z, xy;                              // vertex position
	float nx, ny, nz, lengthInv = 1.0f / radius;    // vertex normal
	float s, t;                                     // vertex texCoord

	float sectorStep = 2 * PI / sectorCount;
	float stackStep = PI / stackCount;
	float sectorAngle, stackAngle;

	float sector_size = sectorCount + 1;
	float stack_size = stackCount + 1;

	num_vertices = sector_size * stack_size;
	num_tris = stackCount * sectorCount * 2 - (2 * sectorCount);

	vertices = new float3[num_vertices];
	normals = new float3[num_vertices];
	tex_coords = new float2[num_vertices];
	tris = new Tri[num_tris];

	int array_pos = 0;
	for (int i = 0; i <= stackCount; ++i) {
		stackAngle = PI / 2 - i * stackStep;        // starting from pi/2 to -pi/2
		xy = radius * cosf(stackAngle);             // r * cos(u)
		z = radius * sinf(stackAngle);              // r * sin(u)

													// add (sectorCount+1) vertices per stack
													// the first and last vertices have same position and normal, but different tex coods
		for (int j = 0; j <= sectorCount; ++j) {
			sectorAngle = j * sectorStep;

			// vertex position (x, y, z)
			x = xy * cosf(sectorAngle);             // r * cos(u) * cos(v)
			y = xy * sinf(sectorAngle);             // r * cos(u) * sin(v)
			vertices[array_pos] = { x, y, z };

			// vertex normal (nx, ny, nz)
			nx = x * lengthInv;
			ny = y * lengthInv;
			nz = z * lengthInv;
			normals[array_pos] = { nx, ny, nz };

			// vertex tex coord (s, t)
			s = (float)j / sectorCount;
			t = (float)i / stackCount;
			tex_coords[array_pos] = { s, t };
			array_pos++;
		}
	}

	array_pos = 0;
	int k1, k2;
	for (int i = 0; i < stackCount; ++i) {
		k1 = i * (sectorCount + 1);     // beginning of current stack
		k2 = k1 + sectorCount + 1;      // beginning of next stack

		for (int j = 0; j < sectorCount; ++j, ++k1, ++k2) {
			// 2 triangles per sector excluding 1st and last stacks
			if (i != 0) {
				tris[array_pos] = { (uint)k1, (uint)k2, (uint)k1 + 1 };
				array_pos++;
			}

			if (i != (stackCount - 1)) {
				tris[array_pos] = { (uint)k1 + 1, (uint)k2, (uint)k2 + 1 };
				array_pos++;
			}
		}
	}

	colors = new float3[num_vertices];
	Color random_color;
	random_color.setRandom();

	for (int i = 0; i < num_vertices; i++)
		colors[i] = { random_color.r, random_color.g, random_color.b };


}

void Mesh::BuildCylinder(float radius, float length, int numSteps) {


	// code belongs to Zakuayada https://www.gamedev.net/forums/topic/359467-draw-cylinder-with-triangle-strips/

	mesh_name = "CYLINDER";
	num_vertices = numSteps * 2 + 2;
	vertices = new float[num_vertices*3];
	normals = new float[num_vertices*3];
	tex_coords = new float[num_vertices*2];
	colors = new float3[num_vertices];

	Color random_color;
	random_color.setRandom();
	for (int i = 0; i < num_vertices; i++)
		colors[i] = { random_color.r, random_color.g, random_color.b };

	float hl = length * 0.5f;
	float a = 0.0f;
	float step = PI * 2 / (float)numSteps;
	for (int i = 0; i < numSteps; ++i) {
		float x = cos(a) * radius;
		float y = sin(a) * radius;
		vertices[i] = { x, y, hl };
		tex_coords[i] = { 1.0f / numSteps * i , 1.0f };
		vertices[i + numSteps] = { x, y, -hl };
		tex_coords[i + numSteps] = { 1.0f / numSteps * i , 0.0f };

		a += step;
	}

	vertices[numSteps * 2 + 0] = {0.0f, 0.0f, +hl};
	vertices[numSteps * 2 + 1] = {0.0f, 0.0f, -hl};

	num_tris = 4 * numSteps * 3;
	tris = new Tri[num_tris];

	for (int i = 0; i < numSteps; ++i) {
		unsigned int i1 = i;
		unsigned int i2 = (i1 + 1) % numSteps;
		unsigned int i3 = i1 + numSteps;
		unsigned int i4 = i2 + numSteps;

		// Sides

		tris[i * 6 + 0] = { i1, i3, i2};
		tris[i * 6 + 3] = { i4, i2, i3};
		// Caps
		tris[numSteps * 6 + i * 6 + 0] = { (uint)numSteps * 2 + 0, i1, i2};
		tris[numSteps * 6 + i * 6 + 3] = { (uint)numSteps * 2 + 1, i4, i3};
	}

	for (int i = 0; i < num_vertices; i++)
		normals[i] = vertices[i].Normalized();
}*/



bool Mesh::LoadFromAssimpMesh(const aiMesh& imported_mesh, const aiScene& scene)
{
	//vertices
	float* vertices = nullptr;
	if (imported_mesh.mNumVertices)
	{
		num_vertices = imported_mesh.mNumVertices;
		vertices = new float[num_vertices*3];
		memcpy(vertices, imported_mesh.mVertices, sizeof(float) * num_vertices* 3);
	}
	else
		return false;

	// faces
	if (imported_mesh.HasFaces())
	{
		num_tris = imported_mesh.mNumFaces;
		tris = new Tri[num_tris]; // assume each face is a triangle
		for (uint i = 0; i < num_tris; ++i)
		{
			if (imported_mesh.mFaces[i].mNumIndices == 3)
				memcpy(&tris[i], imported_mesh.mFaces[i].mIndices, sizeof(Tri));
			else
				app_log->AddLog("WARNING, geometry face with != 3 indices!");
		}
	}
	else
		return false;

	// normals
	float* normals = nullptr;
	normals = new float[num_vertices*3 ];
	if (imported_mesh.HasNormals())
	{
		imported_normals = true;
		memcpy(normals, imported_mesh.mNormals, sizeof(float) * num_vertices* 3);
	}

	// colors
	float* colors = nullptr;
	if (imported_mesh.HasVertexColors(0))
	{
		imported_colors = true;
		colors = new float[num_vertices * 4];
		memcpy(colors, imported_mesh.mColors[0], sizeof(float) * num_vertices * 4);
	}

	// texture coordinates
	float* tex_coords = nullptr;
	
	if (imported_mesh.HasTextureCoords(0))
	{
		imported_tex_coords = true;
		tex_coords = new float[num_vertices*2];
		memcpy(tex_coords, imported_mesh.mTextureCoords[0], sizeof(float)* num_vertices * 2);
	}
	
	
	vertex_one_buffer = new float[num_vertices * 12];

	

	float null_coords[2] = { 0.f,0.f };
	float null[3] = { 0.f,0.f,0.f };
	float null_color[4] = { 1.f,1.f,1.f,1.f };
	for (int i = 0; i < num_vertices; ++i)
	{

		memcpy(vertex_one_buffer + i * 13, vertices + i , sizeof(float) * 3);

		if (colors != nullptr)
			memcpy(vertex_one_buffer + i * 13 + 3, colors + i , sizeof(float) * 4);
		else
			memcpy(vertex_one_buffer + i * 13 + 3, null_color, sizeof(float) * 4);

		if (normals != nullptr)
			memcpy(vertex_one_buffer + i * 13 + 7, normals + i , sizeof(float) * 3);
		else
			memcpy(vertex_one_buffer + i * 13 + 7, null, sizeof(float) * 3);

		if (tex_coords != nullptr)
			memcpy(vertex_one_buffer + i * 13 + 10, tex_coords + i , sizeof(float) * 3);
		else
			memcpy(vertex_one_buffer + i * 13 + 10, null_coords, sizeof(float) * 3);
	
	}

	calculateCentroidandHalfsize();
	for (int i = 0; i < num_vertices; i += 13)
	{
		vertex_one_buffer[i] -= centroid.x;
		vertex_one_buffer[i + 1] -= centroid.y;
		vertex_one_buffer[i + 2] -= centroid.z;
	}

	return true;
}

void Mesh::calculateCentroidandHalfsize()
{
	float3 lowest_p = float3::inf;
	float3 highest_p = -float3::inf;

	if (vertex_one_buffer)
	{
		for (int i = 0; i < num_vertices; i+=13)
		{
			if (lowest_p.x > vertex_one_buffer[i]) lowest_p.x = vertex_one_buffer[i];
			if (lowest_p.y > vertex_one_buffer[i+1]) lowest_p.y = vertex_one_buffer[i+1];
			if (lowest_p.z > vertex_one_buffer[i+2]) lowest_p.z = vertex_one_buffer[i+2];

			if (highest_p.x < vertex_one_buffer[i]) highest_p.x = vertex_one_buffer[i];
			if (highest_p.y < vertex_one_buffer[i+1]) highest_p.y = vertex_one_buffer[i+1];
			if (highest_p.z < vertex_one_buffer[i+2]) highest_p.z = vertex_one_buffer[i+2];
		}
	}

	centroid = ((lowest_p + highest_p) * 0.5f);
	half_size = highest_p - centroid;

}

void Mesh::getData(uint& vert_num, uint& poly_count, bool& has_normals, bool& has_colors, bool& has_texcoords) const
{
	vert_num = num_vertices;
	poly_count = num_tris;
	has_normals = imported_normals;
	has_colors = imported_colors;
	has_texcoords = imported_tex_coords;
}

void Mesh::ClearData()
{
	glDeleteBuffers(1, &iboId);
	glDeleteBuffers(1, &vboId);
	glDeleteBuffers(1, &vaoId);

	if (vertex_one_buffer)	delete vertex_one_buffer;
	if (tris)	delete tris;

	imported_normals = imported_colors = imported_tex_coords = false;
	num_vertices = num_tris = iboId = vboId = vaoId = 0;
}
