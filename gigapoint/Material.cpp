#include "Material.h"
#include "Utils.h"
#include <iostream>

namespace gigapoint {

Material::Material(Option* opt, string name): option(opt), texture(0) {

	shaderstr = opt->shaderDir;
	attributes.clear();
	uniforms.clear();

	if(name.compare("point") == 0) {
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
	
#ifdef OMEGALIB_APP
		shaderstr.append("_app");
#endif
	}

	else if(name.compare("edl") == 0) {

		//shader
		attributes.clear(); uniforms.clear();
		attributes.push_back("VertexPosition");
		attributes.push_back("VertexTexCoord");

		uniforms.push_back("uColorTexture");
	
		shader = new Shader(name);
		shaderstr.append("edl");
	}

	else {
		cout << "ERROR: invalid material name " << name << endl;
		return;
	}

	cout << "shaderstr: " << shaderstr << endl;
	
	shader->load(shaderstr, attributes, uniforms, option);
}

Shader* Material::bind() {
	shader->bind();
	return shader;
}

void Material::reloadShader() {
	shader->load(shaderstr, attributes, uniforms, option);
}

}; //namespace gigapoint
