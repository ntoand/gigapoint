#include "Material.h"
#include "Utils.h"

Material::Material(Option* opt): option(opt) {
	//color texture
	texture = new ColorTexture("colortable.dat");

	//shader
	attributes.clear(); uniforms.clear();
	//attributes.push_back("VertexPosition");
	//attributes.push_back("VertexColor");

	uniforms.push_back("uScreenHeight");
	uniforms.push_back("uSpacing");
	uniforms.push_back("uPointScale");
	uniforms.push_back("uPointSizeRange");
	uniforms.push_back("uColorTexture");
	uniforms.push_back("uHeightMinMax");

	shader = new Shader("point");
	shaderstr = "shaders/point";
	shader->load(shaderstr, attributes, uniforms, option);
}

Shader* Material::bind() {
	shader->bind();
	return shader;
}

void Material::reloadShader() {
	shader->load(shaderstr, attributes, uniforms, option);
}
