// ModelView.c++ - a basic combined Model and View for OpenGL

#include <iostream>
#include <cmath>

#include "ModelView.h"
#include "Controller.h"
#include "ShaderIF.h"
#include "GLFWController.h"
double delayInSeconds = 0.050;
int delayInMilliseconds = 50;
GLboolean ModelView::animating= false;
ShaderIF* ModelView::shaderIF = NULL;
int ModelView::numInstances = 0;


GLint numCircs=-1;

GLfloat** ModelView::circleCenters;

GLuint ModelView::shaderProgram = 0;
GLint ModelView::ppuLoc_scaleTrans = -2;
GLint ModelView::pvaLoc_mcPosition = -2;
GLint ModelView::ppuLoc_radius = -2;
GLint ModelView::ppuLoc_creationDistance = -2;
GLint ModelView::pvaLoc_refCoord = -2;
GLint ModelView::ppuLoc_numCircles = -2;
GLfloat ModelView::radius =0.4;
GLfloat ModelView::creationDistance = 0.4;


double ModelView::mcRegionOfInterest[6] = { -1.0, 1.0, -1.0, 1.0, -1.0, 1.0 };

// ModelView::ModelView()
// {
//   // std::cout << "Making Model View";
// 	std::cout << "This is the shaderProgram value: " << shaderProgram;
// 	if (ModelView::shaderProgram == 0)
// 	{
// 		// Create the common shader program:
// 	 	ModelView::shaderIF = new ShaderIF("project1.vsh", "project1.fsh");
// 		ModelView::shaderProgram = shaderIF->getShaderPgmID();
// 		fetchGLSLVariableLocations();
// 	}
//         ppuLoc_numCircles = ppUniformLocation(shaderProgram, "numCircs");
// 	// TODO: define and call method(s) to initialize your model and send data to GPU
// 	defineSquare();
// 	ModelView::numInstances++;
// }
ModelView::ModelView(float mcCoords[4][2],GLint numCircs_){
	
	 numCircs=numCircs_;
	
      if (ModelView::shaderProgram == 0)
	{
		// Create the common shader program:
	 	ModelView::shaderIF = new ShaderIF("project1.vsh", "project1.fsh");
	  //ModelView::shaderIF = ShaderIF::initShader("project1.vsh", "project1.fsh");
		ModelView::shaderProgram = shaderIF->getShaderPgmID();
// 		 numCircs=numCircs_;
		// initializeCircleRadii();
		fetchGLSLVariableLocations();
		
	}
	
	
	// TODO: define and call method(s) to initialize your model and send data to GPU

	ModelView::numInstances++;
	double xBounds[2]={mcCoords[0][0], mcCoords[0][0]}; //arbitrarily starting by setting to two real x values.
	double yBounds[2]={mcCoords[0][1],mcCoords[0][1]};
  for( int i = 0; i < 4; i = i + 1 )
   {
     //std::cout << "You are passing in to the constructor: mcCoords[" << i << "][0]=" << mcCoords[i][0]<<'\n';
     //next 4 ifs are to help set the bounding box.
     if(xBounds[0]>mcCoords[i][0]){
       xBounds[0]=mcCoords[i][0];
     }
     if(xBounds[1]<mcCoords[i][1]){
       xBounds[1]=mcCoords[i][1];
     }
     
     if(yBounds[0]>mcCoords[i][0]){
       yBounds[0]=mcCoords[i][0];
     }
     if(yBounds[1]<mcCoords[i][1]){
       yBounds[1]=mcCoords[i][1];
     }
     mcCorners[i][0]=mcCoords[i][0];
     mcCorners[i][1]=mcCoords[i][1];
     
   }
   double bounds[6] = {xBounds[0],xBounds[1],yBounds[0],yBounds[1],-1,1};
   setMCRegionOfInterest(bounds);
  
  defineSquare();
  
}
ModelView::~ModelView()
{
	// TODO: delete the vertex array objects and buffers here

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
	// TODO: This code can be used as is, BUT be absolutely certain you
	//       understand everything about how it works.

	double xmin = mcRegionOfInterest[0];
	double xmax = mcRegionOfInterest[1];
	double ymin = mcRegionOfInterest[2];
	double ymax = mcRegionOfInterest[3];

	// BEGIN: ASPECT RATIO CONSIDERATIONS
	// If we wish to preserve aspect ratios, make "region of interest"
	// wider or taller to match the Controller's viewport aspect ratio.
	double vAR = Controller::getCurrentController()->getViewportAspectRatio();

	double wHeight = ymax - ymin;
	double wWidth = xmax - xmin;
	double wAR = wHeight / wWidth; // wAR == MC Window Aspect Ratio
	if (wAR > vAR)
	{
		// make MC window wider
		wWidth = wHeight / vAR;
		double xmid = 0.5 * (xmin + xmax);
		xmin = xmid - 0.5*wWidth;
		xmax = xmid + 0.5*wWidth;
	}
	else
	{
		// make MC window taller
		wHeight = wWidth * vAR;
		double ymid = 0.5 * (ymin + ymax);
		ymin = ymid - 0.5*wHeight;
		ymax = ymid + 0.5*wHeight;
	}
	// END: ASPECT RATIO CONSIDERATIONS

    // We are only concerned with the xy extents for now, hence we will
    // ignore mcRegionOfInterest[4] and mcRegionOfInterest[5].
    // Map the overall limits to the -1..+1 range expected by the OpenGL engine:
	double scaleTrans[4];
	linearMap(xmin, xmax, -1.0, 1.0, scaleTrans[0], scaleTrans[1]);
	linearMap(ymin, ymax, -1.0, 1.0, scaleTrans[2], scaleTrans[3]);
	for (int i=0 ; i<4 ; i++)
		scaleTransF[i] = static_cast<float>(scaleTrans[i]);
}

void ModelView::fetchGLSLVariableLocations()
{
	if (ModelView::shaderProgram > 0)
	{
		// TODO: Set GLSL variable locations here
	    ModelView::ppuLoc_radius = ppUniformLocation(shaderProgram, "radius");
	  ModelView::ppuLoc_scaleTrans = ppUniformLocation(shaderProgram, "scaleTrans");
	  
	  ModelView::ppuLoc_creationDistance = ppUniformLocation(shaderProgram, "creationDistance");
	  ModelView::pvaLoc_mcPosition = pvAttribLocation(shaderProgram, "mcPosition");
	  ModelView::pvaLoc_refCoord = pvAttribLocation(shaderProgram, "refCoord");
 	  ModelView::ppuLoc_numCircles = ppUniformLocation(shaderProgram, "numCircs");
	  std::cout << ppuLoc_radius << ", " << ppuLoc_numCircles<< ", "<<ppuLoc_creationDistance << ", " << ppuLoc_scaleTrans <<"\n\n\n";
	}
}

// xyzLimits: {mcXmin, mcXmax, mcYmin, mcYmax, mcZmin, mcZmax}
void ModelView::getMCBoundingBox(double* xyzLimits) const
{
	// TODO:
	// Put this ModelView instance's min and max x, y, and z extents
	// into xyzLimits[0..5]. (-1 .. +1 is OK for z direction for 2D models)
  for( int i = 0; i < 6; i = i + 1 )
   {
     xyzLimits[i]=mcRegionOfInterest[i];
   }
}

void ModelView::handleCommand(unsigned char key, double ldsX, double ldsY)
{
  if (key == 'a')
	{
	  std::cout << "You clicked a!!!!!!\n\n\n";
		animating = !animating;
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
	

	glBindVertexArray(vao[0]);

//
	// TODO: set scaleTrans (and all other needed) uniform(s)

	// TODO: make require primitive call(s)
	float scaleTrans[4];
	computeScaleTrans(scaleTrans);
	//std::cout << "SCALETRANS: " << scaleTrans[0] << ", " << scaleTrans[1];
	glUniform4fv(ModelView::ppuLoc_scaleTrans, 1, scaleTrans);
	glUniform1f(ModelView::ppuLoc_radius, radius);
	glUniform1f(ModelView::ppuLoc_creationDistance, creationDistance);
	//if(numSurroundingCircles!=6){ std::cout << "not 6 \n\n\n\n\n\n";}
	glUniform1i(ModelView::ppuLoc_numCircles, numCircs);
	if(animating){
	  std::cout << "animating!!!\n\n\n";
	  creationDistance = 0.6;
	 glUniform1f(ModelView::ppuLoc_creationDistance, creationDistance); 
	  
	}else{
	//  std::cout << "not animatinganimating!!!\n\n\n";
	  creationDistance=radius;
	  glUniform1f(ModelView::ppuLoc_creationDistance, creationDistance);
	  
	}
	/*
	GLfloat flattened[(ModelView::numSurroundingCircles+1)*2];
	for( int i = 0; i <(ModelView::numSurroundingCircles+1)*2; i = i + 1 )
	{
	  flattened[i]= circleCenters[i/2][i %2];
	  std::cout << flattened[i] << "\n";
     
	}
	
	
	
	glUniform1fv(ModelView::ppuLoc_circleCenters,(ModelView::numSurroundingCircles+1)*2,flattened);
	
	*/
	
	
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	// restore the previous program
	
	glUseProgram(pgm);
	
}

void ModelView::setMCRegionOfInterest(double xyz[6])
{
	for (int i=0 ; i<6 ; i++)
		mcRegionOfInterest[i] = xyz[i];
	
	
}





void ModelView::defineSquare(){
  typedef float vec2[2];
  std::cout << "mcCorners: " << mcCorners[0][0] << ", " << mcCorners[0][1];
  vec2 squareVertices[4]=
    {{mcCorners[0][0],mcCorners[0][1]},{mcCorners[1][0],mcCorners[1][1]},{mcCorners[2][0],mcCorners[2][1]},{mcCorners[3][0],mcCorners[3][1]}};
   // {-1.0,-1.0}, {-0.50,0.50},{0.50,0.70},{0.50,-0.50}
     // {-1.0,-1.0}, {-1.0,1.0},{1.0,1.0},{1.0,-1.0}
    //};
  glGenVertexArrays(1,vao);
  glBindVertexArray(vao[0]);
  glGenBuffers(2,vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
  int numBytes = 4*sizeof(vec2);
  glBufferData(GL_ARRAY_BUFFER,numBytes,squareVertices, GL_DYNAMIC_DRAW);
  glVertexAttribPointer(ModelView::pvaLoc_mcPosition, 2, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(ModelView::pvaLoc_mcPosition);
  
  
  vec2 refCorners[4] = { {-1.0,-1.0}, {-1.0,1.0},{1.0,1.0},{1.0,-1.0} };
  glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
  int numBytes2 = 4*sizeof(vec2);
  glBufferData(GL_ARRAY_BUFFER,numBytes2,refCorners, GL_STATIC_DRAW);
  glVertexAttribPointer(ModelView::pvaLoc_refCoord, 2, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(ModelView::pvaLoc_refCoord);
  
  
}

// void ModelView::initializeCircleRadii()
// {
//  // float circleCenters[numSurroundingCircles+1][2];
//   circleCenters = new float* [numSurroundingCircles+1]();
//  for( int i = 0; i < numSurroundingCircles+1; i = i + 1 )
//   {
//     circleCenters[i]= new float[2]();
//      
//    }
//   
//   circleCenters[0][0]=0.0; //circle in the middle;
//   circleCenters[0][1]=0.0;
//   
//   float tau = 2 * M_PI;
//   for( int i = 1; i < numSurroundingCircles+1; i = i + 1 )
//   {
//     circleCenters[i][0]=radius* sin((tau/numSurroundingCircles)*i);
//     circleCenters[i][1]=radius* cos((tau/(float)numSurroundingCircles)*i);
//     //  std::cout << circleCenters[i][0] << ", " << circleCenters[i][1] << "\n";
//    }
//    
//    
//  //  centers[0] = centers[0];
// 
// }

