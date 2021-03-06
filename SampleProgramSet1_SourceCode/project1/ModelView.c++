// ModelView.c++ - a basic combined Model and View for OpenGL

#include <iostream>
#include <cmath>

#include "ModelView.h"
#include "Controller.h"
#include "ShaderIF.h"
#include "GLFWController.h"
double delayInSeconds = 0.001;
int delayInMilliseconds = 1;
double ModelView::lastAnimationUpdate=0.0;
bool ModelView::animating_inout= false;
bool ModelView::animating_radically= false;
ShaderIF* ModelView::shaderIF = NULL;
int ModelView::numInstances = 0;
float ModelView::stepSize= 0.002;

GLint numCircs=-1;


GLuint ModelView::shaderProgram = 0;
GLint ModelView::ppuLoc_scaleTrans = -2;
GLint ModelView::pvaLoc_mcPosition = -2;
GLint ModelView::ppuLoc_radius = -2;
GLint ModelView::ppuLoc_creationDistance = -2;
GLint ModelView::pvaLoc_refCoord = -2;
GLint ModelView::ppuLoc_numCircles = -2;
GLint ModelView::ppuLoc_radicalOffset = -2;
GLfloat ModelView::radius =0.4;
GLfloat ModelView::creationDistance = 0.4;
GLfloat ModelView::radicalOffset = 0.0;



double ModelView::mcRegionOfInterest[6] = { -1.0, 1.0, -1.0, 1.0, -1.0, 1.0 };

ModelView::ModelView(float mcCoords[4][2],GLint numCircs_){	
      numCircs=numCircs_;	
      if (ModelView::shaderProgram == 0)
	{
		// Create the common shader program:
	 	ModelView::shaderIF = new ShaderIF("project1.vsh", "project1.fsh");
		ModelView::shaderProgram = shaderIF->getShaderPgmID();
		fetchGLSLVariableLocations();	
	}
	
	
	

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
      //Now that I have the new bounds, set 'em!
      setMCRegionOfInterest(bounds);
  
      //oh yeah, and define the square.
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
	  //Per primitive uniform stuff.
	  ModelView::ppuLoc_radius = ppUniformLocation(shaderProgram, "radius");
	  ModelView::ppuLoc_scaleTrans = ppUniformLocation(shaderProgram, "scaleTrans");
	  ModelView::ppuLoc_creationDistance = ppUniformLocation(shaderProgram, "creationDistance");
	  ModelView::ppuLoc_numCircles = ppUniformLocation(shaderProgram, "numCircs");
	  ModelView::ppuLoc_radicalOffset = ppUniformLocation(shaderProgram, "radicalOffset");
	  //per vertex stuff 
	  ModelView::pvaLoc_mcPosition = pvAttribLocation(shaderProgram, "mcPosition");
	  ModelView::pvaLoc_refCoord = pvAttribLocation(shaderProgram, "refCoord");
 	  
	  
	  //debugging
	  //std::cout << ppuLoc_radius << ", " << ppuLoc_numCircles<< ", "<<ppuLoc_creationDistance << ", " << ppuLoc_scaleTrans <<"\n\n\n";
	}
}

// xyzLimits: {mcXmin, mcXmax, mcYmin, mcYmax, mcZmin, mcZmax}
void ModelView::getMCBoundingBox(double* xyzLimits) const
{
  for( int i = 0; i < 6; i = i + 1 )
   {
     xyzLimits[i]=mcRegionOfInterest[i];
   }
}

void ModelView::handleCommand(unsigned char key, double ldsX, double ldsY)
{
  if (key == 'a')
	{

		animating_inout = true;
// 		if(ModelView::animating){
 			GLFWController* glfwC =
 				dynamic_cast<GLFWController*>(Controller::getCurrentController());
 			if (glfwC != NULL)
 			{
 				glfwC->setRunWaitsForAnEvent(false);
 			}
// 		}
// 		else
// 		{
// 			GLFWController* glfwC =
// 				dynamic_cast<GLFWController*>(Controller::getCurrentController());
// 			if (glfwC != NULL)
// 				glfwC->setRunWaitsForAnEvent(true);	
// 		}
	}
	if(key == 'o'){
	  animating_inout=false;
	  animating_radically=false;
	  ModelView::creationDistance=ModelView::radius;
	  
	}
	if(key == 'r'){
	    animating_radically=true;
	  
	}
	if(key == 'p'){
	 animating_inout=false; 
	 animating_radically=false; 
	}
  maybeAnimate();
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


	// TODO: make require primitive call(s)
	float scaleTrans[4];
	computeScaleTrans(scaleTrans);
	//std::cout << "SCALETRANS: " << scaleTrans[0] << ", " << scaleTrans[1];
	glUniform4fv(ModelView::ppuLoc_scaleTrans, 1, scaleTrans);
	glUniform1f(ModelView::ppuLoc_radius, radius);
	glUniform1f(ModelView::ppuLoc_creationDistance, creationDistance);
	glUniform1f(ModelView::ppuLoc_radicalOffset, radicalOffset);
	//if(numSurroundingCircles!=6){ std::cout << "not 6 \n\n\n\n\n\n";}
	glUniform1i(ModelView::ppuLoc_numCircles, numCircs);
	
	
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	// restore the previous program
	
	glUseProgram(pgm);
	maybeAnimate();
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



void ModelView::maybeAnimate()
{
  
  //std::cout << "Maybe animating!!!!!! animating: "  << ModelView::animating <<"\n\n";
	if (animating_inout || animating_radically)
	{
		double time = glfwGetTime();
		if (time > lastAnimationUpdate+delayInSeconds)
		{
			animate();
			lastAnimationUpdate = time;
		}
	}else{
	//  std::cout << " here1\n";
	//  ModelView::creationDistance=ModelView::radius;
	  //std::cout << " here2\n";
	//  glUniform1f(ModelView::ppuLoc_creationDistance, ModelView::creationDistance);
	  //std::cout << " here3\n";
	}
	  
	  
}
void ModelView::animate()
{
	//  std::cout << "animating!!!\n\n\n";
  if(animating_inout){
     if(abs(ModelView::creationDistance) >=ModelView::radius){
	    ModelView::stepSize = ModelView::stepSize * (-1);
	  };
	  ModelView::creationDistance = ModelView::creationDistance +
					ModelView::stepSize;
     
    
  }
  
  if(animating_radically){
    if((ModelView::radicalOffset) >=4*M_PI){
	    ModelView::radicalOffset = 0.0;//ModelView::stepSize * (-1);
	  };
	  ModelView::radicalOffset = ModelView::radicalOffset +
					ModelView::stepSize*3;
    
    
  }
	 
	
	  
	
  
}

