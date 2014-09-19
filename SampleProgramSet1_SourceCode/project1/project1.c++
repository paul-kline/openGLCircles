// project1.c++

#include "GLFWController.h"
#include "ModelView.h"
#include <cmath>
typedef float vec2[2];

int main(int argc, char* argv[])
{    
  
  std::cout << sqrt(exp(abs(7-4)) + exp(abs(4-8)));
 // std::cout << "starting main program";
	GLFWController c(argv[0]);
	c.reportVersions(std::cout);
	//std::cout << "just after reporting versions";
	// TODO: one or more ModelView dynamic allocations, adding
	//       each to the Controller using "c.addModel(...);"
	
	
	 float squareVertices[6][4][2]=
    {
      {{-10,-10}, {-10,-5},{-5,-5},{-5,-10}},
      {{-1.0,-1.0}, {-1.0,1.0},{1.0,1.0},{1.0,-1.0}},
      {{5,5}, {5,10},{10,10},{10, 5}},
      {{-8,5}, {-9,9},{-4,5},{-4, 0}},
      {{5,-10}, {5,6},{10,-5},{10, -10}},
      {{-4.8,-10}, {-4.8,-2},{4.8,-2},{4.8, -10}}
    };
    
     float squareVertices2[4][2]= { };
	c.addModel(new ModelView(squareVertices[0],5));
	c.addModel(new ModelView(squareVertices[1],7));
	c.addModel(new ModelView(squareVertices[2],7));
	c.addModel(new ModelView(squareVertices[3],7));
	c.addModel(new ModelView(squareVertices[4],7));
	c.addModel(new ModelView(squareVertices[5],7));
	// initialize 2D viewing information:
	// Get the overall scene bounding box in Model Coordinates:
	double xyz[6]; // xyz limits, even though this is 2D
	c.getOverallMCBoundingBox(xyz);
	std::cout << "overallbox: " << xyz[0] << ", " << xyz[1] << "\n";
       	// Simplest case: Just tell the ModelView we want to see it all:
	double testBounds[6] = {-10,10, -10,10,-10,10};
	ModelView::setMCRegionOfInterest(testBounds);
	glClearColor(0.0980392, 0.0980392, 0.439216, 1.0);
	c.run();    

	return 0;
}

