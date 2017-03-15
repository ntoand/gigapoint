#include "Material.h"
#include "Utils.h"
#include <iostream>
#include <math.h>

namespace gigapoint {

Material::Material(Option* opt): option(opt) {

	shaderstr = opt->shaderDir;
	attributes.clear();
	uniforms.clear();

}

Shader* Material::bind() {
	shader->bind();
	return shader;
}

void Material::reloadShader() {
	shader->load(shaderstr, attributes, uniforms, option);
}

//================================
MaterialPoint::MaterialPoint(Option* option) : Material(option), texture(0) {

	name = "point";

	//color texture
	std::cout << "gigapoint_resource/colortable.dat" << std::endl;
	texture = new ColorTexture("gigapoint_resource/colortable.dat");
	
	//shader
	attributes.clear(); uniforms.clear();
	attributes.push_back("VertexPosition");
	attributes.push_back("VertexColor");

	uniforms.push_back("uScreenHeight");
	uniforms.push_back("uSpacing");
	uniforms.push_back("uPointScale");
	uniforms.push_back("uPointSizeRange");
	uniforms.push_back("uColorTexture");
	uniforms.push_back("uHeightMinMax");

	shader = new Shader(name);
	shaderstr.append("point");

	cout << "shaderstr: " << shaderstr << endl;
	shader->load(shaderstr, attributes, uniforms, option);
}


//================================
MaterialEdl::MaterialEdl(Option* option) : Material(option) {

	name = "edl";

	//shader
	attributes.clear(); uniforms.clear();
	attributes.push_back("VertexPosition");
	attributes.push_back("VertexTexCoord");

	uniforms.push_back("uColorTexture");
	uniforms.push_back("uScreenWidth");
	uniforms.push_back("uScreenHeight");
	uniforms.push_back("uNeighbours");
	uniforms.push_back("uEdlStrength");
	uniforms.push_back("uRadius");
	uniforms.push_back("uOpacity");

	shader = new Shader(name);
	shaderstr.append("edl");

	cout << "shaderstr: " << shaderstr << endl;
	shader->load(shaderstr, attributes, uniforms, option);

	// setup other parameters
	for(int c = 0; c < NUM_NEIGHBOURS; c++){
		neighbours[2*c+0] = cos(2 * c * PI / NUM_NEIGHBOURS);
		neighbours[2*c+1] = sin(2 * c * PI / NUM_NEIGHBOURS);
	}
}


}; //namespace gigapoint
