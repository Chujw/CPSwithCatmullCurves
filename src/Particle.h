#pragma once
#include "ofMain.h"
#include "common.h"
#include "SpacingMap.h"
#include "makePDF.h"

#define		BIRTH	1
//#define NUM 3
#define DEFAULT_SPEED		0.5 
#define DEFAULT_LINEWIDTH	0
#define LINEMAX		4
#define	LINEMIN		1
//#define	BEARING_LIMIT	
//#define VISIBILITY_TIMER_LIMIT	0
//#define LINEWD_TIMER_LIMIT_REG	0
//#define LINEWD_TIMER_LIMIT_ZERO 0	// must larger than SPACING_TIMER_LIMIT_ON
class Particle {

public:

	ofVec2f pos;
	float angle;
	float bearing;	// the rotate angle in clockwise direction from the north line
	ofVec2f bearing_vec;	// the vector of bearing
	ofVec2f last_pos;
	ofVec2f crushAt; // the location it killed or died with neighbors
	float speed;
	int id;
	int beacon_id;
	Particle* prev;
	Particle* next;
	int parentbeacon;
	int childbeacon;
	int dyingwith;
	int diedwith;
	bool Is_released;
	bool Was_released;
	bool Is_visible;
	bool Was_visible;
	bool Is_newborn;
	bool Is_dying;
	float last_tilt;
	int tilt_control;
	int tilt_limit;
	ofColor c;
	float linewidth;
	float last_spacing_threshold;
	int grid_id;
	makePDF makepdf;
	bool musthavepoint;

	Particle(float theta=0, float sp=0.5);
	void Move2Middle(bool IsOpenMode, int head_par, int rear_par);
	float GetSigmoidBearing(bool IsOpenMode, int head_par, int rear_par, Particle* thispar);
	float GetAverageBearing(bool IsOpenMode, int head_par, int rear_par, Particle* thispar);
	void SigmoidDeath(int head_par, int rear_par);
	//int BirthOrDeath(int b_stepsize, int d_stepsize, int b_min_num, int d_max_num);
	void Update();
	void advance();	// add some random direction factors
	bool OutOfBoudaryKill();
	bool PushBackOrFwd(int head, int rear,bool IsOpenMode);
	bool UpdateSpacingTd(float currentTd);// is the local last_spacing_threshold same as the threshold in the current pixel
	float UpdateLineWidth(float currentTd, float minTd, float maxTd);
	void storepathforPDF();
};