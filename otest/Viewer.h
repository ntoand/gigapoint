#ifndef __VIEWER_H__
#define __VIEWER_H__

#include "Points.h"

class Viewer {

private:
	Points* points;
	Shader* shader;
	ColorTexture* texture;

	bool checkCollision;
	omega::Ray ray;

public:
	Viewer();
	~Viewer();
	void init();
	void draw();
	void setCheckCollision(bool c, const omega::Ray& r);
};

#endif