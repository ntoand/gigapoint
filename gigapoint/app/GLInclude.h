#ifndef _GLINCLUDE_H
#define _GLINCLUDE_H

//#include "gl_core_4_4.h"
#include <stdio.h>
#include <stdlib.h>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <glfw3.h>

#define GLM_FORCE_RADIANS

#define GL_Error_Check \
  { \
    GLenum error = GL_NO_ERROR; \
    while ((error = glGetError()) != GL_NO_ERROR) { \
      fprintf(stderr, "OpenGL error [ %s : %d ] \"%s\".\n",  \
            __FILE__, __LINE__, gluErrorString(error)); \
    } \
  }


//virtual key codes
const int VK_W = 0x57;
const int VK_S = 0x53;
const int VK_A = 0x41;
const int VK_D = 0x44;
const int VK_Q = 0x51;
const int VK_Z = 0x5a;

#endif
