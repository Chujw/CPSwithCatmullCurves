#include "makePDF.h"


makePDF::makePDF()
{
	path.reserve(200);
	//linewidth.reserve(200);
}

void makePDF::growpath(float x, float y, bool musthave, bool parvisible)
{
	// store a point if it is a musthave one
	if(parvisible)
	{
		if(path.size() == 0 || musthave || (path.back() - ofPoint(x,y)).length() > pathrecordstep)
		{
			path.push_back(ofPoint(x,y));
			//linewidth.push_back(lwidth);
		}
	}

}

void makePDF::drawpath(bool darker)
{

	if(path.size() > 0)
	{
		int numPts = path.size();
		output.setColor(0);
		output.noFill();
		output.beginShape();
		

		//catmull curves create nice smooth curves between points
		//so actually a lower resolution creates a smoother line
		//and reduce the number of points in our file.
		int rescaleRes = 6;

		for(int i = 0; i < numPts; i++){

			//we need to draw the first and last point
			//twice for a catmull curve
			if(i == 0 || i == numPts -1){
				//output.setLineWidth(linewidth[i]);
				if(darker)
					output.setLineWidth(5);
				else
					output.setLineWidth(4);
				output.curveVertex(path[i].x, path[i].y);
			}

			if(i % rescaleRes == 0) 
			{
				//output.setLineWidth(linewidth[i]);
				if(darker)
					output.setLineWidth(5);
				else
					output.setLineWidth(4);
				output.curveVertex(path[i].x, path[i].y);
			}
		}
		output.endShape();
	}
}

void makePDF::growfromanotherpath(vector <ofPoint>* p)
{
	if(p->size()>4)
	{
		path.push_back(p->at(p->size()-4));
		path.push_back(p->at(p->size()-3));
		path.push_back(p->at(p->size()-2));
		path.push_back(p->at(p->size()-1));

		//linewidth.push_back(plinewidth->at(plinewidth->size()-4));
		//linewidth.push_back(plinewidth->at(plinewidth->size()-3));
		//linewidth.push_back(plinewidth->at(plinewidth->size()-2));
		//linewidth.push_back(plinewidth->at(plinewidth->size()-1));
	}
}

int makePDF::GetPathSize()
{
	return path.size();
}

ofPoint* makePDF::GetPoint(int index)
{
	return &path.at(index);
}

vector<ofPoint>* makePDF::GetPath()
{
	return &path;
}

void makePDF::ClearPath()
{
	path.clear();
}