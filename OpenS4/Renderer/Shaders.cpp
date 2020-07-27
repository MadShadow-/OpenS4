#pragma once

#include "Shaders.hpp"

#define STRINGIFY(a) (#a)

namespace OpenS4::Renderer::Shader {
// clang-format off

const char* FRAGMENTSHADER_MAP =
STRINGIFY(#version 330 core\n
in vec2 texCoord;
in float colorScale;

out vec4 fragColor;
uniform sampler2D tex2D;

void main() {
    fragColor = texture2D(tex2D, texCoord);
	fragColor.r *= pow(colorScale, 1.5);
	fragColor.g *= pow(colorScale, 1.5);
	fragColor.b *= colorScale;
}
);

const char* VERTEXSHADER_MAP =
STRINGIFY(#version 330 core\n

layout(location = 0) in vec4 vVertex;
layout(location = 1) in vec2 vTexCoord;
layout(location = 2) in float vColorScale;

uniform mat4 mvpMatrix;

out vec2 texCoord;
out float colorScale;

void main(){
	gl_Position = mvpMatrix * vVertex;
	texCoord = vTexCoord;
	colorScale = vColorScale;
}
    );

const char* FRAGMENTSHADER_SETTLER =
STRINGIFY(#version 330 core\n

	in vec2 texCoord;
	in float vCol;

	out vec4 fragColor;

	uniform sampler2D tex2D;

	uniform vec4 playerColors[8];

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
				fragColor.r = textureColor.a * playerColors[int(vCol)].r;
				fragColor.g = textureColor.a * playerColors[int(vCol)].g;
				fragColor.b = textureColor.a * playerColors[int(vCol)].b;
				fragColor.a = 1;
			}
			else
			{
				fragColor = textureColor;
			}
		}

		if (fragColor.a == 0)
			discard;
	}
);

const char * VERTEXSHADER_SETTLER = 
STRINGIFY(#version 330 core\n

layout(location = 0) in vec4 vVertex;
layout(location = 1) in vec2 vTexCoord;
layout(location = 2) in float vColor;

uniform mat4 mvpMatrix;

out vec2 texCoord;
out float vCol;

	void main(){
		gl_Position = mvpMatrix * vVertex;
		texCoord = vTexCoord;
		vCol = vColor;

	}
);

const char* VERTEXSHADER_POINT =
STRINGIFY(#version 330 core\n

	layout(location = 0) in vec4 vVertex;
	layout(location = 2) in vec4 vColor;

	uniform mat4 mvpMatrix;

	out vec4 vCol;

	void main()
	{
		gl_Position = mvpMatrix * vVertex;
		vCol = vColor;
	}
);

const char* FRAGMENTSHADER_POINT =
STRINGIFY(#version 330 core\n

	in vec4 vCol;

	out vec4 fragColor;

	void main(){
		fragColor = vCol;
	}

);

// clang-format on
}  // namespace OpenS4::Renderer::Shader
