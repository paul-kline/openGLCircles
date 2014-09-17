// GLUTController.c++: A concrete Controller subclass based on GLUT

#include "GLUTController.h"
#include "ModelView.h"

bool GLUTController::glutInitialized = false;

static int zero = 0;

GLUTController::GLUTController(const std::string& windowTitle, int rcFlags,
	int* argcP, char**argv)
{
	if (!glutInitialized)
	{
		if (argcP == NULL)
			argcP = &zero;
		glutInit(argcP, argv);
		glutInitialized = true;
	}
	// First create the window and its Rendering Context (RC)
	createWindow(windowTitle, rcFlags);
}

GLUTController::~GLUTController()
{
}

void GLUTController::createWindow(const std::string& windowTitle, int rcFlags)
{
	// The following calls enforce use of only non-deprecated functionality.
	glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutInitWindowSize(newWindowWidth, newWindowHeight);

	glutInitDisplayMode(GLUT_DOUBLE);

	windowID = glutCreateWindow(titleString(windowTitle).c_str());
}

void GLUTController::displayCB() // CLASS METHOD
{
	if (curController != NULL)
		dynamic_cast<GLUTController*>(curController)->handleDisplay();
}

void GLUTController::establishInitialCallbacksForRC()
{
	glutDisplayFunc(displayCB);
	glutReshapeFunc(reshapeCB);
	glutKeyboardFunc(keyboardCB);
}

void GLUTController::handleDisplay()
{
	// clear the frame buffer
	glClear(glClearFlags);

	// draw the collection of models
	int which = 0;
	for (std::vector<ModelView*>::iterator it=models.begin() ; it<models.end() ; it++)
		(*it)->render();

	glutSwapBuffers();

	checkForErrors(std::cout, "GLUTController::handleDisplay");
}

void GLUTController::keyboardCB(unsigned char key, int x, int y)
{
	GLUTController* c = dynamic_cast<GLUTController*>(curController);
	if (c != NULL)
		c->handleAsciiChar(key, x, y);
}

void GLUTController::reportWindowInterfaceVersion(std::ostream& os) const
{
	int glutVer = glutGet(GLUT_VERSION);
	int glutVerMajor = glutVer/10000;
	int glutVerMinor = (glutVer % 10000) / 100;
	int glutVerPatch = glutVer % 100;
	os << "        GLUT: " << glutVerMajor << '.';
	if (glutVerMinor < 10)
		os << '0';
	os << glutVerMinor << '.';
	if (glutVerPatch < 10)
		os << '0';
	os << glutVerPatch << " (" << glutVer << ")\n";
}

void GLUTController::reshapeCB(int width, int height)
{
	GLUTController* c = dynamic_cast<GLUTController*>(curController);
	c->handleReshape(width, height);
}

void GLUTController::run()
{
	establishInitialCallbacksForRC();
	glutMainLoop();
}

void GLUTController::setWindowTitle(const std::string& title)
{
	glutSetWindowTitle(title.c_str());
}
