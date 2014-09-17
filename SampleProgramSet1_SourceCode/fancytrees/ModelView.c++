// ModelView.c++ - a basic combined Model and View for OpenGL

#include <iostream>

#include "Controller.h"
#include "ModelView.h"
#include "ShaderIF.h"

ShaderIF* ModelView::shaderIF = NULL;
int ModelView::numInstances = 0;
GLuint ModelView::shaderProgram = 0;
GLint ModelView::pvaLoc_vertexColor = -2;
GLint ModelView::pvaLoc_vertexFraction = -2;
GLint ModelView::pvaLoc_mcPosition = -2;
GLint ModelView::ppuLoc_colorMode = -2;
GLint ModelView::ppuLoc_scaleTrans = -2;

// Current MC Region of interest
double ModelView::mcRegionOfInterest[6] = { -1.0, 1.0, -1.0, 1.0, -1.0, 1.0 };

ModelView::ModelView(vec2* coords, vec3* colors, float* fractions, int nVertices) :
	colorMode(0), numVertices(nVertices), serialNumber(numInstances)
{
	if (ModelView::shaderProgram == 0)
	{
		// The first time an instance of this class is generated, create the common shader program:
		ModelView::shaderIF = new ShaderIF("fancytree.vsh", "fancytree.fsh");
		ModelView::shaderProgram = shaderIF->getShaderPgmID();
		fetchGLSLVariableLocations();
	}

	// Now define the geometry
	defineGeometry(coords, colors, fractions);
	numInstances++;
}

ModelView::~ModelView()
{
	glDeleteBuffers(3, vbo);
	glDeleteVertexArrays(1, vao);
	if (--numInstances == 0)
	{
		ModelView::shaderIF->destroy();
		delete ModelView::shaderIF;
		ModelView::shaderIF = NULL;
		ModelView::shaderProgram = 0;
	}
}

// computeScaleTrans determines the current model coordinate region of
// interest and then uses linearMap to determine how to map coordinates
// in the region of interest to their proper location in Logical Device
// Space. (Returns float[] because glUniform currently favors float[].)
void ModelView::computeScaleTrans(float* scaleTransF) // CLASS METHOD
{
	// preserve aspect ratio. Make "region of interest" wider or taller to
	// match the Controller's viewport aspect ratio.
	double vAR = Controller::getCurrentController()->getViewportAspectRatio();
	double xmin = mcRegionOfInterest[0];
	double xmax = mcRegionOfInterest[1];
	double ymin = mcRegionOfInterest[2];
	double ymax = mcRegionOfInterest[3];

	double wHeight = ymax - ymin;
	double wWidth = xmax - xmin;
	double wAR = wHeight / wWidth;
	if (wAR > vAR)
	{
		// make window wider
		wWidth = wHeight / vAR;
		double xmid = 0.5 * (xmin + xmax);
		xmin = xmid - 0.5*wWidth;
		xmax = xmid + 0.5*wWidth;
	}
	else
	{
		// make window taller
		wHeight = wWidth * vAR;
		double ymid = 0.5 * (ymin + ymax);
		ymin = ymid - 0.5*wHeight;
		ymax = ymid + 0.5*wHeight;
	}

    // We are only concerned with the xy extents for now, hence we will
    // ignore mcRegionOfInterest[4] and mcRegionOfInterest[5].
    // Map the overall limits to the -1..+1 range expected by the OpenGL engine:
	double scaleTrans[4];
	linearMap(xmin, xmax, -1.0, 1.0, scaleTrans[0], scaleTrans[1]);
	linearMap(ymin, ymax, -1.0, 1.0, scaleTrans[2], scaleTrans[3]);
	for (int i=0 ; i<4 ; i++)
		scaleTransF[i] = static_cast<float>(scaleTrans[i]);
}

void ModelView::defineGeometry(vec2* coords, vec3* colors, float* fractions)
{
	glGenVertexArrays(1, vao);
	glBindVertexArray(vao[0]);

	// create three per-vertex attribute buffers for the set of triangles
	// [0] - color buffer; [1] - fraction; [2] - vertex coordinate buffer
	glGenBuffers(3, vbo);

	// Allocate space for and copy CPU color information to GPU
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	int numBytesColors = numVertices * sizeof(vec3);
	glBufferData(GL_ARRAY_BUFFER, numBytesColors, colors, GL_STATIC_DRAW);
	glVertexAttribPointer(ModelView::pvaLoc_vertexColor, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(ModelView::pvaLoc_vertexColor);

	// Allocate space for and copy CPU fraction information to GPU
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	int numBytesCodeData = numVertices * sizeof(float);
	glBufferData(GL_ARRAY_BUFFER, numBytesCodeData, fractions, GL_STATIC_DRAW);
	glVertexAttribPointer(ModelView::pvaLoc_vertexFraction, 1, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(ModelView::pvaLoc_vertexFraction);

	// Allocate space for and copy CPU coordinate data to GPU
	glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
	int numBytesCoordinateData = numVertices * sizeof(vec2);
	glBufferData(GL_ARRAY_BUFFER, numBytesCoordinateData, coords, GL_STATIC_DRAW);
	glVertexAttribPointer(ModelView::pvaLoc_mcPosition, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(ModelView::pvaLoc_mcPosition);

	// make note of the min/max coordinates
	xmin = xmax = coords[0][0];
	ymin = ymax = coords[0][1];
	for (int i=1 ; i<numVertices ; i++)
	{
		if (coords[i][0] < xmin)
			xmin = coords[i][0];
		else if (coords[i][0] > xmax)
			xmax = coords[i][0];
		if (coords[i][1] < ymin)
			ymin = coords[i][1];
		else if (coords[i][1] > ymax)
			ymax = coords[i][1];
	}

	// REMEMBER:
	// All three array buffers here - along with their memory layout information and
	// enable statuses - will be reestablished in ModelView::render with the single
	// call to glBindVertexArray(va0).
}

void ModelView::fetchGLSLVariableLocations()
{
	if (ModelView::shaderProgram > 0)
	{
		ModelView::pvaLoc_vertexColor = pvAttribLocation(shaderProgram, "vertexColor");
		ModelView::pvaLoc_vertexFraction = pvAttribLocation(shaderProgram, "vertexFraction");
		ModelView::pvaLoc_mcPosition = pvAttribLocation(shaderProgram, "mcPosition");
		ModelView::ppuLoc_scaleTrans = ppUniformLocation(shaderProgram, "scaleTrans");
		ModelView::ppuLoc_colorMode = ppUniformLocation(shaderProgram, "colorMode");
	}
}

// xyzLimits: {mcXmin, mcXmax, mcYmin, mcYmax, mcZmin, mcZmax}
void ModelView::getMCBoundingBox(double* xyzLimits) const
{
	xyzLimits[0] = xmin; xyzLimits[1] = xmax;
	xyzLimits[2] = ymin; xyzLimits[3] = ymax;
	xyzLimits[4] = -1.0; xyzLimits[5] = 1.0; // (zmin, zmax) (really 0..0)
}

void ModelView::handleCommand(unsigned char key, double ldsX, double ldsY)
{
	if ((key >= '0') && (key <= '9'))
	{
		int which = static_cast<int>(key) - static_cast<int>('0');
		if (which == serialNumber) // was this message intended for me???
			colorMode = (colorMode + 1) % 3; // cycle among 0, 1, 2
	}
}

// linearMap determines the scale and translate parameters needed in
// order to map a value, f (fromMin <= f <= fromMax) to its corresponding
// value, t (toMin <= t <= toMax). Specifically: t = scale*f + trans.
void ModelView::linearMap(double fromMin, double fromMax, double toMin, double toMax,
						  double& scale, double& trans) // CLASS METHOD
{
	scale = (toMax - toMin) / (fromMax - fromMin);
	trans = toMin - scale*fromMin;
}

GLint ModelView::ppUniformLocation(GLuint glslProgram, const std::string& name)
{
	GLint loc = glGetUniformLocation(glslProgram, name.c_str());
	if (loc < 0)
		std::cerr << "Could not locate per-primitive uniform: '" << name << "'\n";
	return loc;
}

GLint ModelView::pvAttribLocation(GLuint glslProgram, const std::string& name)
{
	GLint loc = glGetAttribLocation(glslProgram, name.c_str());
	if (loc < 0)
		std::cerr << "Could not locate per-vertex attribute: '" << name << "'\n";
	return loc;
}

void ModelView::render() const
{
	// save the current GLSL program in use
	GLint pgm;
	glGetIntegerv(GL_CURRENT_PROGRAM, &pgm);
	// draw the triangles using our vertex and fragment shaders
	glUseProgram(shaderProgram);

	// define the mapping from MC to -1..+1 Logical Device Space:
	float scaleTrans[4];
	computeScaleTrans(scaleTrans);
	glUniform4fv(ModelView::ppuLoc_scaleTrans, 1, scaleTrans);

	glBindVertexArray(vao[0]); // reestablishes all buffer settings as noted above
	
	glUniform1i(ModelView::ppuLoc_colorMode, colorMode);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, numVertices);

	// restore the previous program
	glUseProgram(pgm);
}

void ModelView::setMCRegionOfInterest(double xyz[6])
{
	for (int i=0 ; i<6 ; i++)
		mcRegionOfInterest[i] = xyz[i];
}
