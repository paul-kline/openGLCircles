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

class ModelView
{
public:
	ModelView();
	ModelView(float cmCoords[4][2],int surroundingCircleNum);
	virtual ~ModelView();
	GLuint vao[1];
	GLuint vbo[2];
	// xyzLimits: {mcXmin, mcXmax, mcYmin, mcYmax, mcZmin, mcZmax}
	void getMCBoundingBox(double* xyzLimits) const;
	void handleCommand(unsigned char key, double ldsX, double ldsY);
	void render() const;
	
	GLint numCircs;
	
	static void setMCRegionOfInterest(double xyz[6]);
	//animation related things
	static void animate();
	static void maybeAnimate();
	
	static double lastAnimationUpdate; //animation
	static bool animating_inout;//animation
	static bool animating_radically;//animation
	static float stepSize; //this is used to animate radically and in&out-ly.
	static GLfloat radicalOffset; //this is used for animation.
	
private:
	
	//float* x; //pointer of any kind here seg faults.
	float mcCorners[4][2]; //the mc locations of the corners of this model view.
	
	static GLfloat radius; //the size of the circles to be drawn.
	static GLfloat creationDistance; // this is used for animation.
	
	// TODO: VAO(s), VBO(s), and other relevant INSTANCE variables

	// we assume all instances share the same shader program:
	static ShaderIF* shaderIF;
	static int numInstances;
	static GLuint shaderProgram;
	
	// TODO: add uniform and attribute variable location CLASS variables
	static GLint ppuLoc_scaleTrans;
	static GLint ppuLoc_radius;
	static GLint ppuLoc_creationDistance;
	static GLint pvaLoc_mcPosition;
	static GLint pvaLoc_refCoord;
	
	
	static GLint ppuLoc_numCircles;
	static GLint ppuLoc_radicalOffset;
	// "pp": "per-primitive"; "pv": "per-vertex"
	static GLint ppUniformLocation(GLuint glslProgram, const std::string& name);
	static GLint pvAttribLocation(GLuint glslProgram, const std::string& name);

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
	
	
	static void fetchGLSLVariableLocations();

	void defineSquare();
	
   
protected:
    static GLfloat** circleCenters;
};

#endif
