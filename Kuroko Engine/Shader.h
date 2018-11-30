#ifndef _SHADER_H_
#define _SHADER_H_

#include "Globals.h"

enum ShaderType{ UNKNOWN, VERTEX, FRAGMENT, TESSELLATION, GEOMETRY};

class Shader {
public:
	Shader(ShaderType type);
	~Shader();

	uint CreateVertexShader(const char* ShaderSource);
	uint CreateFragmentShader(const char* ShaderSource);

	uint getId() const { return id; };
	ShaderType getType() const { return type; };

private:
	
	ShaderType type = UNKNOWN;
	const uint id = 0;
	bool compile_success = false;
	char* source = nullptr;

private:

	//DEFAULT VERTEX SHADER CODE
	char vertex_default[512] =

		"#version 330 core\n\
	layout(location = 0) in vec3 position;\n\
	layout(location = 1) in vec3 color;\n\
	layout(location = 2) in vec2 texCoord;\n\\n\
	out vec3 ourColor;\n\
	out vec2 TexCoord;\n\
	uniform mat4 model;\n\
	uniform mat4 view;\n\
	uniform mat4 projection;\n\
	void main()\n\
	{\n\\n\
		gl_Position = projection * view * model * vec4(position, 1.0f);\n\
		ourColor = color;\n\
		TexCoord = texCoord;\n\
	}";

	//DEFAULT FRAGMENT SHADER CODE
	char fragment_default[512] =
		"#version 330 core\n\
	in vec3 ourColor;\n\
	in vec2 TexCoord;\n\\n\
	out vec4 color;\n\
	uniform sampler2D ourTexture;\n\
	void main()\n\
	{\n\\n\
		color = texture(ourTexture, TexCoord);\n\
	}";

	
};

#endif // !_SHADER_H_
