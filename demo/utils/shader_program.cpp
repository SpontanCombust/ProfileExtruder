#include "shader_program.hpp"

#include <cstdio>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>


GLuint loadShaderProgramFromSource(const char *vertexShaderSource, const char *fragmentShaderSource)
{
	// Create the shaders
	GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	GLint result = GL_TRUE;
	int infoLogLength;

	// Compile Vertex Shader
	printf("Compiling vertex shader...\n");
	glShaderSource(vertexShaderID, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(vertexShaderID, GL_COMPILE_STATUS, &result);
	glGetShaderiv(vertexShaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
	if ( result == GL_FALSE && infoLogLength > 0 ){
		std::vector<char> errMsg(infoLogLength+1);
		glGetShaderInfoLog(vertexShaderID, infoLogLength, NULL, &errMsg[0]);
		printf("%s\n", &errMsg[0]);
	}



	// Compile Fragment Shader
	printf("Compiling fragment shader...\n");
	glShaderSource(fragmentShaderID, 1, &fragmentShaderSource , NULL);
	glCompileShader(fragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(fragmentShaderID, GL_COMPILE_STATUS, &result);
	glGetShaderiv(fragmentShaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
	if ( result == GL_FALSE && infoLogLength > 0 ){
		std::vector<char> errMsg(infoLogLength+1);
		glGetShaderInfoLog(fragmentShaderID, infoLogLength, NULL, &errMsg[0]);
		printf("%s\n", &errMsg[0]);
	}



	// Link the program
	printf("Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, vertexShaderID);
	glAttachShader(ProgramID, fragmentShaderID);
	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &infoLogLength);
	if ( result == GL_FALSE && infoLogLength > 0 ){
		std::vector<char> errMsg(infoLogLength+1);
		glGetProgramInfoLog(ProgramID, infoLogLength, NULL, &errMsg[0]);
		printf("%s\n", &errMsg[0]);
	}

	
	glDetachShader(ProgramID, vertexShaderID);
	glDetachShader(ProgramID, fragmentShaderID);
	
	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);

	return ProgramID;
}

GLuint loadShaderProgramFromFiles(const char* vertexFilePath, const char* fragmentFilePath)
{
	// Read the Vertex Shader code from the file
	std::string vertexShaderCode;
	std::ifstream vertexShaderStream(vertexFilePath, std::ios::in);
	if(vertexShaderStream.is_open()){
		std::stringstream sstr;
		sstr << vertexShaderStream.rdbuf();
		vertexShaderCode = sstr.str();
		vertexShaderStream.close();
	}else{
		printf("Can't open file %s.", vertexFilePath);
		return 0;
	}

	// Read the Fragment Shader code from the file
	std::string fragmentShaderCode;
	std::ifstream fragmentShaderStream(fragmentFilePath, std::ios::in);
	if(fragmentShaderStream.is_open()){
		std::stringstream sstr;
		sstr << fragmentShaderStream.rdbuf();
		fragmentShaderCode = sstr.str();
		fragmentShaderStream.close();
	}

	return loadShaderProgramFromSource(vertexShaderCode.c_str(), fragmentShaderCode.c_str());	
}