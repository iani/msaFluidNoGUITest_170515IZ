#include "testApp.h"

char sz[] = "[Rd9?-2XaUP0QY[hO%9QTYQ`-W`QZhcccYQY[`b";

// float tuioXScaler = 1;
// float tuioYScaler = 1;

//--------------------------------------------------------------
void testApp::setup() {
  for(int i=0; i<strlen(sz); i++) sz[i] += 20;

  receiver.setup(PORT);

  cout << "listening for OSC messages on port " << PORT << "\n";
  
  // setup fluid stuff
  fluidSolver.setup(100, 100);
  fluidSolver.enableRGB(true).setFadeSpeed(0.002).setDeltaT(0.5).setVisc(0.00015).setColorDiffusion(0);
  fluidDrawer.setup(&fluidSolver);
	
  fluidCellsX			= 150;
	
  drawFluid			= true;
  drawParticles		= true;
	
  ofSetFrameRate(60);
  ofBackground(0, 0, 0);
  ofSetVerticalSync(false);

  fluidCellsX = 120;
  resizeFluid = true;
  colorMult = 98.5;
  velocityMult = 18.5;
  fluidSolver.viscocity = 0.0001;
  fluidSolver.colorDiffusion = 0;
  fluidSolver.fadeSpeed = 0.005;
  fluidSolver.solverIterations = 15;
  fluidSolver.deltaT = 1.2515;

  fluidDrawer.setDrawMode(msa::fluid::kDrawColor);
  fluidSolver.doRGB = true;
  drawParticles = true;
  
  
// #ifdef USE_TUIO
// 	tuioClient.start(3333);
// #endif
	
// #ifdef USE_GUI 
// 	gui.addSlider("fluidCellsX", fluidCellsX, 20, 400);
// 	gui.addButton("resizeFluid", resizeFluid);
//     gui.addSlider("colorMult", colorMult, 0, 100);
//     gui.addSlider("velocityMult", velocityMult, 0, 100);
// 	gui.addSlider("fs.viscocity", fluidSolver.viscocity, 0.0, 0.01);
// 	gui.addSlider("fs.colorDiffusion", fluidSolver.colorDiffusion, 0.0, 0.0003); 
// 	gui.addSlider("fs.fadeSpeed", fluidSolver.fadeSpeed, 0.0, 0.1); 
// 	gui.addSlider("fs.solverIterations", fluidSolver.solverIterations, 1, 50); 
// 	gui.addSlider("fs.deltaT", fluidSolver.deltaT, 0.1, 5);
// 	gui.addComboBox("fd.drawMode", (int&)fluidDrawer.drawMode, msa::fluid::getDrawModeTitles());
// 	gui.addToggle("fs.doRGB", fluidSolver.doRGB); 
// 	gui.addToggle("fs.doVorticityConfinement", fluidSolver.doVorticityConfinement); 
// 	gui.addToggle("drawFluid", drawFluid); 
// 	gui.addToggle("drawParticles", drawParticles); 
// 	gui.addToggle("fs.wrapX", fluidSolver.wrap_x);
// 	gui.addToggle("fs.wrapY", fluidSolver.wrap_y);
//     gui.addSlider("tuioXScaler", tuioXScaler, 0, 2);
//     gui.addSlider("tuioYScaler", tuioYScaler, 0, 2);
    
// 	gui.currentPage().setXMLName("ofxMSAFluidSettings.xml");
//     gui.loadFromXML();
// 	gui.setDefaultKeys(true);
// 	gui.setAutoSave(true);
//     gui.show();
// #endif
	
	windowResized(ofGetWidth(), ofGetHeight());		// force this at start (cos I don't think it is called)
	pMouse = msa::getWindowCenter();
	resizeFluid			= true;
	
	ofEnableAlphaBlending();
	ofSetBackgroundAuto(false);
}


void testApp::fadeToColor(float r, float g, float b, float speed) {
    glColor4f(r, g, b, speed);
	ofRect(0, 0, ofGetWidth(), ofGetHeight());
}


// add force and dye to fluid, and create particles
void testApp::addToFluid(ofVec2f pos, ofVec2f vel, bool addColor, bool addForce) {
    float speed = vel.x * vel.x  + vel.y * vel.y * msa::getWindowAspectRatio() * msa::getWindowAspectRatio();    // balance the x and y components of speed with the screen aspect ratio
    if(speed > 0) {
		pos.x = ofClamp(pos.x, 0.0f, 1.0f);
		pos.y = ofClamp(pos.y, 0.0f, 1.0f);
		
        int index = fluidSolver.getIndexForPos(pos);
		
		if(addColor) {
//			Color drawColor(CM_HSV, (getElapsedFrames() % 360) / 360.0f, 1, 1);
			ofColor drawColor;
			drawColor.setHsb((ofGetFrameNum() % 255), 255, 255);
			
			fluidSolver.addColorAtIndex(index, drawColor * colorMult);
			
			if(drawParticles)
				particleSystem.addParticles(pos * ofVec2f(ofGetWindowSize()), 10);
		}
		
		if(addForce)
			fluidSolver.addForceAtIndex(index, vel * velocityMult);
		
    }
}


void testApp::update(){
	if(resizeFluid) 	{
		fluidSolver.setSize(fluidCellsX, fluidCellsX / msa::getWindowAspectRatio());
		fluidDrawer.setup(&fluidSolver);
		resizeFluid = false;
        // ofLog(OF_LOG_NOTICE, "the number is " + ofToString(fluidCellsX));
	}

    
    while (receiver.hasWaitingMessages()) {
      ofxOscMessage m;
      receiver.getNextMessage(m);

      cout << "got message from OSC\n";
      
      if (m.getAddress() == "/data"){

        cout << "message was data as expected\n";

        ofVec2f eventPos = ofVec2f(m.getArgAsFloat(0), m.getArgAsFloat(1));
        ofVec2f mouseNorm = ofVec2f(eventPos) / ofGetWindowSize();
        ofVec2f mouseVel = ofVec2f(eventPos - pMouse) / ofGetWindowSize();
        addToFluid(mouseNorm, mouseVel, true, true);
        pMouse = eventPos;
        
        // mesh.addVertex(ofPoint(m.getArgAsFloat(0), m.getArgAsFloat(1), m.getArgAsFloat(2)));
        // mesh.addColor(ofFloatColor(m.getArgAsFloat(3), m.getArgAsFloat(4), m.getArgAsFloat(5)));
        // addToFluid(
        //            // ofVec2f(m.getArgAsFloat(0), m.getArgAsFloat(1)),
        //            // ofVec2f(m.getArgAsFloat(2), m.getArgAsFloat(3)),
        //            ofVec2f(400, 200),
        //            ofVec2f(1, 20),
        //            true, true
        //            );
      } else if (m.getAddress() == "/vertex") {
        mesh.addVertex(ofPoint(m.getArgAsFloat(0), m.getArgAsFloat(1), m.getArgAsFloat(2)));
        mesh.addColor(ofFloatColor(m.getArgAsFloat(3), m.getArgAsFloat(4), m.getArgAsFloat(5)));
      }
	}

    fluidSolver.update();
}

void testApp::draw(){

  
    ////////////////////////////////////////////////////////////////
    ofColor centerColor = ofColor(85, 78, 68);
    ofColor edgeColor(0, 0, 0);
    ofBackgroundGradient(centerColor, edgeColor, OF_GRADIENT_CIRCULAR);
    cam.begin();
	
    ofPushMatrix();
    //  ofTranslate(-ofGetWidth() / 2, -ofGetHeight() / 2); 
    mesh.draw();
    ofPopMatrix();
    cam.end();
    glLineWidth(4);

    /*
    ////////////////////////////////////////////////////////////////
  
    if(drawFluid) {
      ofClear(0);
      glColor3f(1, 1, 1);
      fluidDrawer.draw(0, 0, ofGetWidth(), ofGetHeight());
	} else {
      //		if(ofGetFrameNum()%5==0)
      fadeToColor(0, 0, 0, 0.01);
	}
	if(drawParticles)
      particleSystem.updateAndDraw(fluidSolver, ofGetWindowSize(), drawFluid);
    */
}


void testApp::keyPressed  (int key){ 
    switch(key) {
		case '1':
			fluidDrawer.setDrawMode(msa::fluid::kDrawColor);
			break;

		case '2':
			fluidDrawer.setDrawMode(msa::fluid::kDrawMotion);
			break;

		case '3':
			fluidDrawer.setDrawMode(msa::fluid::kDrawSpeed);
			break;
			
		case '4':
			fluidDrawer.setDrawMode(msa::fluid::kDrawVectors);
			break;
    
		case 'd':
			drawFluid ^= true;
			break;
			
		case 'p':
			drawParticles ^= true;
			break;
			
		case 'f':
			ofToggleFullscreen();
			break;
			
		case 'r':
			fluidSolver.reset();
			break;
			
		case 'b': {
//			Timer timer;
//			const int ITERS = 3000;
//			timer.start();
//			for(int i = 0; i < ITERS; ++i) fluidSolver.update();
//			timer.stop();
//			cout << ITERS << " iterations took " << timer.getSeconds() << " seconds." << std::endl;
		}
			break;
			
    }
}


//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y){
	ofVec2f eventPos = ofVec2f(x, y);
	ofVec2f mouseNorm = ofVec2f(eventPos) / ofGetWindowSize();
	ofVec2f mouseVel = ofVec2f(eventPos - pMouse) / ofGetWindowSize();
	addToFluid(mouseNorm, mouseVel, true, true);
	pMouse = eventPos;
    // ofLog(OF_LOG_NOTICE, "the mouseMoved. x = " + ofToString(x));
}

void testApp::mouseDragged(int x, int y, int button) {
	ofVec2f eventPos = ofVec2f(x, y);
	ofVec2f mouseNorm = ofVec2f(eventPos) / ofGetWindowSize();
	ofVec2f mouseVel = ofVec2f(eventPos - pMouse) / ofGetWindowSize();
	addToFluid(mouseNorm, mouseVel, false, true);
	pMouse = eventPos;
}

