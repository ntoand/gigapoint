#include "Material.h"

Material::Material(Shader* _shader): shader(_shader) {

}

Shader* Material::bind() {
	shader->bind();
	return shader;
}