#pragma once

#include "Shaders.hpp"

#define STRINGIFY(a) (#a)

namespace OpenS4::Renderer::Shader {
// clang-format off

const char* FRAGMENTSHADER_MAP =
STRINGIFY(#version 330 core\n
in vec2 texCoord;

out vec4 fragColor;
uniform sampler2D tex2D;

void main() {
    fragColor = texture2D(tex2D, texCoord);
}
);

const char* VERTEXSHADER_MAP =
STRINGIFY(#version 330 core\n

layout(location = 0) in vec4 vVertex;
layout(location = 1) in vec2 vTexCoord;

uniform mat4 mvpMatrix;

out vec2 texCoord;

void main(){
	gl_Position = mvpMatrix * vVertex;
	texCoord = vTexCoord;
}
    );

const char* FRAGMENTSHADER_SETTLER =
STRINGIFY(#version 330 core\n

	in vec2 texCoord;
	in vec4 vCol;

	uniform vec4 playerColor;



	out vec4 fragColor;

	uniform sampler2D tex2D;

	void main(){

		vec4 textureColor = texture2D(tex2D, texCoord);
	
		if (textureColor.a == 1)
		{
			fragColor = textureColor;
		}
		else
		{
			if (textureColor.r == 0 && textureColor.g == 0 && textureColor.b == 0 && textureColor.a != 0)
			{
				fragColor = vCol;
				fragColor.r = textureColor.a * vCol.r;
				fragColor.g = textureColor.a * vCol.g;
				fragColor.b = textureColor.a * vCol.b;
				fragColor.a = 1;
			}
			else
			{
				fragColor = textureColor;
			}
		}
	}
);

const char * VERTEXSHADER_SETTLER = 
STRINGIFY(#version 330 core\n

layout(location = 0) in vec4 vVertex;
layout(location = 1) in vec2 vTexCoord;
layout(location = 2) in vec4 vColor;

uniform mat4 mvpMatrix;

out vec2 texCoord;
out vec4 vCol;

	void main(){
		gl_Position = mvpMatrix * vVertex;
		texCoord = vTexCoord;
		vCol = vColor;

	}
);

// clang-format on
}  // namespace OpenS4::Renderer::Shader
