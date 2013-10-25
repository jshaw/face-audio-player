#include "ofMain.h"
#include "testApp.h"
#include "ofAppGlutWindow.h"

int main() {
    ofAppGlutWindow window;
//	ofSetupOpenGL(&window, 960, 720, OF_WINDOW);
//   	ofSetupOpenGL(960, 720, OF_WINDOW);
    ofSetupOpenGL(&window, 640, 480, OF_WINDOW);
	ofRunApp(new testApp());
}
