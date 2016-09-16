#include "Utils.h"
#include "MyTriangle.h"

using namespace omega;

MyTriangle::MyTriangle(){

}

MyTriangle::~MyTriangle() {

}

int MyTriangle::initResources() {

	GLfloat triangle_vertices[] = {
			0.0,  2.8, -2,
			-0.8, 1.2, -2,
			0.8, 0.2, -2
	};

    //bbox
    bbox[0] = -0.8; bbox[1] = 0.2; bbox[2] = -2;
    bbox[3] = 0.8; bbox[4] = 2.8; bbox[5] = -2;

	glGenBuffers(1, &vbo_triangle);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_triangle);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangle_vertices), triangle_vertices, GL_STATIC_DRAW);

	GLint link_ok = GL_FALSE;

	// shader
	list<string> attributes;
	list<string> uniforms;
	attributes.clear(); uniforms.clear();
	attributes.push_back("VertexPosition");
	uniforms.push_back("MVP");
	shader = new Shader("mytriangle");
	shader->load("shaders/mytriangle", attributes, uniforms);

	unsigned int attribute_vertext_pos = shader->attribute("VertexPosition");
    glEnableVertexAttribArray(attribute_vertext_pos);  // Vertex position
    glBindBuffer(GL_ARRAY_BUFFER, vbo_triangle);
    glVertexAttribPointer(
        attribute_vertext_pos, // attribute
        3,                 // number of elements per vertex, here (x,y,z)
        GL_FLOAT,          // the type of each element
        GL_FALSE,          // take our values as-is
        0,                 // no extra data between each position
        0                  // offset of first element
    );
    
	return 1;
}

int MyTriangle::testFrustum(float V[6][4]) {
    return Utils::testFrustum(V, bbox);
}

void MyTriangle::useProgram() {
    glUseProgram(program);
}

void MyTriangle::draw(const DrawContext& context) {

    float V[6][4];
    float* X = (context.projection*context.modelview).cast<float>().data();
    Utils::getFrustum(V, X);

    if(testFrustum(V) < 0) {
        owarn("Outside V");
        return;
    }

    //glUseProgram(program);
    shader->bind();
    shader->transmitUniform("MVP", X);

    //glUniformMatrix4fv(glGetUniformLocation(program,"MVP"), 1, GL_FALSE, X);
    
    glBindBuffer(GL_ARRAY_BUFFER, vbo_triangle);
	glDrawArrays(GL_TRIANGLES, 0, 3);

	//glUseProgram(0);
}
