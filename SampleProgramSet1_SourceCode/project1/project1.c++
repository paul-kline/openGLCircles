// project1.c++

#include "GLFWController.h"
#include "ModelView.h"

int main(int argc, char* argv[])
{
 // std::cout << "starting main program";
	GLFWController c(argv[0]);
	c.reportVersions(std::cout);
	//std::cout << "just after reporting versions";
	// TODO: one or more ModelView dynamic allocations, adding
	//       each to the Controller using "c.addModel(...);"
	
	
	 float squareVertices[4][2]=
    {
      // {-10,-10}, {-10,10},{10,10},{10,-10}
      {-1.0,-1.0}, {-1.0,1.0},{1.0,1.0},{1.0,-1.0}
    };
    
     float squareVertices2[4][2]= { {5,5}, {5,10},{10,10},{10, 5}};
	c.addModel(new ModelView(squareVertices2));
	// initialize 2D viewing information:
	// Get the overall scene bounding box in Model Coordinates:
	double xyz[6]; // xyz limits, even though this is 2D
	c.getOverallMCBoundingBox(xyz);
	std::cout << "overallbox: " << xyz[0] << ", " << xyz[1] << "\n";
       	// Simplest case: Just tell the ModelView we want to see it all:
	double testBounds[6] = {-10,10, -10,10,-10,10};
	ModelView::setMCRegionOfInterest(testBounds);
	glClearColor(1.0, 1.0, 0.5 , 1.0);
	c.run();    

	return 0;
}
