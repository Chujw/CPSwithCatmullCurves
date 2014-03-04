#pragma once

#include "ofxVectorGraphics.h"
#include "ofMain.h"

#define pathrecordstep 4.5

class makePDF {
private:	
	ofxVectorGraphics output;
	vector <ofPoint> path;
	//vector <float> linewidth;

public:
	makePDF();
	void growpath(float x, float y, bool musthave, bool parvisible);
	void growfromanotherpath(vector <ofPoint>* p);
	void drawpath(bool darker);
	int GetPathSize();
	ofPoint* GetPoint(int index);
	vector<ofPoint>* GetPath();
	void ClearPath();
	
};