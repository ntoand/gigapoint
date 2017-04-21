#ifndef GLUTILS_H
#define GLUTILS_H

#include "app/GLInclude.h"

namespace GLUtils
{
    int checkForOpenGLError(const char *, int);
    
    void dumpGLInfo(bool dumpExtensions = false);
    
    void APIENTRY debugCallback( GLenum source, GLenum type, GLuint id,
		GLenum severity, GLsizei length, const GLchar * msg, void * param );

}

#endif // GLUTILS_H
