// ModelView.h - a basic combined Model and View for OpenGL

#ifndef MODELVIEW_H
#define MODELVIEW_H

class ShaderIF;

#include <string>

#ifdef __APPLE_CC__
#define GLFW_INCLUDE_GLCOREARB
#include "GLFW/glfw3.h"
#else
#include <GL/gl.h>
#endif

typedef float vec2[2];
typedef float vec3[3];

class ModelView
{
public:
	ModelView(vec2* coords, vec3* colors, float* temperatures, int nVertices);
	virtual ~ModelView();

	// xyzLimits: {mcXmin, mcXmax, mcYmin, mcYmax, mcZmin, mcZmax}
	void getMCBoundingBox(double* xyzLimits) const;
	void handleCommand(unsigned char key, double ldsX, double ldsY);
	void render() const;

	static void setMCRegionOfInterest(double xyz[6]);

private:
	GLuint vao[1];
	GLuint vbo[3]; // 0: colors; 1: fractions; 2: coordinates
	int colorMode; // how is color determined for this set of triangles
	int numVertices;
	int serialNumber;
	float xmin, xmax, ymin, ymax;

	// All instances share the same shader program:
	static ShaderIF* shaderIF;
	static int numInstances;
	static GLuint shaderProgram;
	static GLint pvaLoc_vertexColor, pvaLoc_vertexFraction;
	static GLint pvaLoc_mcPosition;
	static GLint ppuLoc_colorMode;
	static GLint ppuLoc_scaleTrans;

	// Routines for computing parameters necessary to map from arbitrary
	// model coordinate ranges into OpenGL's -1..+1 Logical Device Space.
	// 1. linearMap determines the scale and translate parameters needed in
	//    order to map a value, f (fromMin <= f <= fromMax) to its corresponding
	//    value, t (toMin <= t <= toMax). Specifically: t = scale*f + trans.
	static void linearMap(double fromMin, double fromMax, double toMin, double toMax,
						  double& scale, double& trans);
	// 2. computeScaleTrans determines the current model coordinate region of
	//    interest and then uses linearMap to determine how to map coordinates
	//    in the region of interest to their proper location in Logical Device
	//    Space. (Returns float[] because glUniform currently allows only float[].)
	static void computeScaleTrans(float* scaleTrans);

	static double mcRegionOfInterest[6];

	// "pp": "per-primitive"
	static GLint ppUniformLocation(GLuint glslProgram, const std::string& name);
	// "pv": "per-vertex"
	static GLint pvAttribLocation(GLuint glslProgram, const std::string& name);
	
	void defineGeometry(vec2* coords, vec3* colors, float* temperatures);
	static void fetchGLSLVariableLocations();
};

#endif
