// GLUTController.h - A concrete Controller subclass based on GLUT

#ifndef GLUTCONTROLLER_H
#define GLUTCONTROLLER_H

#include <GL/freeglut.h>
#include "Controller.h"

class GLUTController : public Controller
{
public:
	GLUTController(const std::string& windowTitle, int rcFlags = 0,
		int* argc=NULL, char** argv=NULL);
	virtual ~GLUTController();

	void redraw() const { glutPostRedisplay(); }
	void run();
	void setWindowTitle(const std::string& title);

protected:
	GLUTController(const GLUTController& c) : Controller(c) {}

	void establishInitialCallbacksForRC();
	virtual void handleDisplay();
	void reportWindowInterfaceVersion(std::ostream& os) const;
	void swapBuffers() const { glutSwapBuffers(); }

private:
	void createWindow(const std::string& windowTitle, int rcFlags);
	int windowID;

	static int getModifiers();

	static bool glutInitialized;

	static void displayCB();
	static void keyboardCB(unsigned char key, int x, int y);
	static void reshapeCB(int width, int height);
};

#endif
