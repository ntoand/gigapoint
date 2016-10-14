#include "Material.h"
#include "Utils.h"

Material::Material(Shader* _shader): shader(_shader), quality(QUALITY_SQUARE),
									sizeType(SIZE_FIXED), material(MATERIAL_RGB) {

}

Shader* Material::bind() {
	shader->bind();
	return shader;
}