#pragma once

#include <GL/glew.h>

GLuint loadShaderProgramFromSource(const char *vertexShaderSource, const char *fragmentShaderSource);
GLuint loadShaderProgramFromFiles(const char *vertexFilePath, const char *fragmentFilePath);