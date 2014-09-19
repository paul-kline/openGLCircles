#version 410 core

// replace the placeholder implementation here
//layout (location = 0) in vec2 vertexCoordinates;

uniform vec4 scaleTrans;



in vec2 mcPosition;

in vec2 refCoord;
out vec2 refCoordToFS;
out float ldsX, ldsY;
void main()
{

	ldsX = scaleTrans[0]*mcPosition.x + scaleTrans[1];
	ldsY = scaleTrans[2]*mcPosition.y + scaleTrans[3];
	gl_Position = vec4(ldsX,ldsY, 0, 1);
	refCoordToFS=refCoord;
	
	
	
}

