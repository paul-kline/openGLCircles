// project1.c++

#include "GLFWController.h"
#include "ModelView.h"

int main(int argc, char* argv[])
{
  std::cout << "starting main program";
	GLFWController c(argv[0]);
	c.reportVersions(std::cout);
	std::cout << "just after reporting versions";
	// TODO: one or more ModelView dynamic allocations, adding
	//       each to the Controller using "c.addModel(...);"
	c.addModel(new ModelView());
	// initialize 2D viewing information:
	// Get the overall scene bounding box in Model Coordinates:
	double xyz[6]; // xyz limits, even though this is 2D
	c.getOverallMCBoundingBox(xyz);
       	// Simplest case: Just tell the ModelView we want to see it all:
	//	//	ModelView::setMCRegionOfInterest(xyz);
	glClearColor(1.0, 1.0, 0.5 , 1.0);
	c.run();

	return 0;
}
