#include "Mesh.h"
#include "Globals.h"

#include "glew-2.1.0\include\GL\glew.h"

Mesh::Mesh(PrimitiveTypes primitive)
{
	switch (primitive)
	{
	case Primitive_Cube:	BuildCube();
		break;

	default:
		break;
	}

	LoadDataToVRAM();
}

Mesh::~Mesh()
{
	if (vertices) delete vertices;
	if (tris) delete tris;
	if (normals) delete normals;
}

void Mesh::LoadDataToVRAM()
{
	if (num_tris > 0)
	{
		glGenBuffers(1, (GLuint*) &(id_tris));
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_tris);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Point3i) * num_tris, tris, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	}

	if (num_vertices > 0) 
	{
		glGenBuffers(1, (GLuint*) &(id_vertices));
		glBindBuffer(GL_ARRAY_BUFFER, id_vertices);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Point3f) * num_vertices, vertices, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

	}

	if (num_normals > 0) {

		glGenBuffers(1, (GLuint*) &(id_normals));
		glBindBuffer(GL_ARRAY_BUFFER, id_normals);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Point3f) * num_normals, normals, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
}



void Mesh::Draw() {

	// early exit if empty mesh
	if (num_tris == 0 || num_vertices == 0)
		return;

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_tris);
	glBindBuffer(GL_ARRAY_BUFFER, id_vertices);
	glBindBuffer(GL_NORMAL_ARRAY, id_normals);

	glColor3f(1.0f, 1.0f, 1.0f);

	glVertexPointer(3, GL_FLOAT, 0, NULL);
	glTexCoordPointer(2, GL_FLOAT, 0, NULL);
	glNormalPointer(GL_FLOAT, 0, NULL);
	glDrawElements(GL_TRIANGLES, num_tris * 3, GL_UNSIGNED_INT, NULL);

	glBindBuffer(GL_NORMAL_ARRAY, 0);
	glBindBuffer(GL_TEXTURE_COORD_ARRAY, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);

}


void Mesh::BuildCube(float sx, float sy, float sz)
{
	sx *= 0.5f, sy *= 0.5f, sz *= 0.5f;

	num_vertices = 8;
	vertices = new Point3f[num_vertices];

	vertices[0].set(-sx, -sy, -sz);
	vertices[1].set(sx, -sy, -sz);
	vertices[2].set(-sx, -sy, sz);
	vertices[3].set(sx, -sy, sz);

	vertices[4].set(-sx, sy, -sz);
	vertices[5].set(sx, sy, -sz);
	vertices[6].set(-sx, sy, sz);
	vertices[7].set(sx, sy, sz);

	num_tris = 12;
	tris = new Point3i[num_tris];

	tris[0].set(0, 1, 2);	tris[1].set(3, 2, 1);	//front
	tris[2].set(6, 5, 4);	tris[3].set(5, 6, 7);	//back
	tris[4].set(5, 3, 1);	tris[5].set(3, 5, 7);	//up
	tris[6].set(0, 2, 4);	tris[7].set(6, 4, 2);	//down
	tris[8].set(4, 1, 0);	tris[9].set(1, 4, 5);	//left
	tris[10].set(2, 3, 6);	tris[11].set(7, 6, 3);	//right  
}
