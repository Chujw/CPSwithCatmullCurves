#include "Particle.h"


//--------------------------------------------------
// small handy functions
//--------------------------------------------------
float dist(Particle* A, Particle* B)
{
	//float test = sqrt((A->pos.x-B->pos.x)*(A->pos.x-B->pos.x)+(A->pos.y-B->pos.y)*(A->pos.y-B->pos.y));
	return sqrt((A->pos.x-B->pos.x)*(A->pos.x-B->pos.x)+(A->pos.y-B->pos.y)*(A->pos.y-B->pos.y));
}

Particle midp(Particle* A, Particle* B)
{
	Particle midpoint;
	midpoint.pos.x = (A->pos.x+B->pos.x)/2;
	midpoint.pos.y = (A->pos.y+B->pos.y)/2;
	return midpoint;
}



Particle::Particle(float theta, float sp)
{
	linewidth = DEFAULT_LINEWIDTH;
	bearing=theta;
	speed=sp;
	pos.x=speed*sin(bearing);
	pos.y=speed*cos(bearing);
	Is_released = false;
	Is_newborn = false;
	Is_dying = false;
	Is_visible = false;
	musthavepoint = false;
	crushAt = pos;
	grid_id = -1;
}

//---------------------------------------------
//
// Add random factors to direction and speed
//
//---------------------------------------------

void Particle::advance()
{
	float dtheta;
	dtheta = ofRandom(-15,15);    // randomzie direction
	if(id != -1 && !Is_newborn && !Is_dying)	// for general particles
	{
		//if(dtheta*last_tilt<0)
		//	dtheta = -dtheta;
		last_tilt = dtheta;
		bearing = bearing + 0.03*radians(dtheta);
		bearing_vec = ofVec2f(sin(bearing),cos(bearing));
		angle = degrees(bearing);

	}
}



//----------------------------------------------------------------------
//
// Move B to the middle of A and C
// now suits the situation of a par is a head of rear in open-area mode
//----------------------------------------------------------------------

void Particle::Move2Middle(bool IsOpenMode, int head_par, int rear_par)
{
	if(!this->Is_dying)
	{
		//-------------------------------------
		// Calculate the sigmoid bearing
		//-------------------------------------
		float d_sigmoid = GetSigmoidBearing(IsOpenMode, head_par, rear_par, this);
		//-------------------------------------
		// Calculate the average bearing
		//-------------------------------------
		float d_aver = GetAverageBearing(IsOpenMode, head_par, rear_par, this);
		//---------------------------------------
		// Add the new bearing to the old one
		//---------------------------------------
		float new_aver_sigmoid = d_aver+d_sigmoid;
		ofVec2f new_bearing_vec = ofVec2f(sin(new_aver_sigmoid),cos(new_aver_sigmoid));
		new_bearing_vec.normalize();
		this->bearing_vec = 0.999*this->bearing_vec + 0.001*new_bearing_vec;		// adjust the bearing through vector
		this->bearing_vec.normalize();
		this->bearing = acos(this->bearing_vec.dot(ofVec2f(0,1)));
		if(this->bearing_vec.x<0)
			this->bearing = 2*PI-this->bearing;
		this->angle = degrees(this->bearing);
				float deg_sig_temp = degrees(this->bearing);
				ofVec2f test_bearing_vec = ofVec2f(sin(this->bearing),cos(this->bearing));
				deg_sig_temp = degrees(this->bearing);		
	}
}

//------------------------------------------------------------
//
// return the direction of sigmoid movement
//------------------------------------------------------------
float Particle::GetSigmoidBearing(bool IsOpenMode, int head_par, int rear_par, Particle* thispar)
{
	float x;
	float d_sigmoid = 0;
	//-------------------------------------------------------------------------
	bool Is_headpar = false, Is_rearpar = false;
	if(/*IsOpenMode && */thispar->id==head_par)	// for the head particle in open-area mode
	{
		Is_headpar = true;
	}
	if(/*IsOpenMode && */thispar->id==rear_par)	// for the rear particle in open-area mode
	{
		Is_rearpar = true;
	}

	//--------------SETUP A, B, C--------------------------------------
	Particle *A; 
	Particle *B;
	Particle *C;
	if(!Is_headpar&& !Is_rearpar) // head and rear are always false when in close-area mode
	{
		A = thispar->prev;
		B = thispar;
		C = thispar->next;
	}
	else if(Is_headpar)
	{
		if(thispar->next->id==rear_par)	// do not compute the sigmoid direction when there are only 2 pars
			return 0;
		A = thispar;
		B = thispar->next;
		C = thispar->next->next;
	}
	else if(Is_rearpar)
	{
		if(thispar->prev->id==head_par)	// do not compute the sigmoid direction when there are only 2 pars
			return 0;
		A = thispar->prev->prev;
		B = thispar->prev;
		C = thispar;
	}
	//-------------------------------------
	// Calculate the sigmoid ratio
	//-------------------------------------
	float AB = dist(A,B);
	float BC = dist(B,C);
	float mid = (AB+BC)/2;
	float ratio = AB/mid;
	ratio = floorf(ratio * 100) / 100;	// round down to two decimal places

	//----------------------------------------------------------------
	if(ratio<1.0)			// tangent-like shape (move to right)
	{
		x = (PI-PI/6)*ratio - PI/3;	// -60 < x < 90
		x = floorf(x * 100) / 100;
		float slope = 1/(cos(x)*cos(x)); // sec square
		d_sigmoid = atan2(1,slope);
		if(d_sigmoid>0)
			d_sigmoid = -d_sigmoid;
	}
	else if(ratio>1.0)			// inverse-tangent-like (move to left)
	{
		ratio = 2-ratio;
		ratio = floorf(ratio * 100) / 100;
		x = (PI-PI/6)*ratio - PI/3;	// -60 < x < 90
		x = floorf(x * 100) / 100;
		float slope = 1/(cos(x)*cos(x)); // sec square
		d_sigmoid = atan2(-1,slope);
		if(d_sigmoid<0)
			d_sigmoid = -d_sigmoid;
	}
	if(!Is_headpar && !Is_rearpar)	// for regular pars, the sigmoid direction is done computing now
		return d_sigmoid;
	else
	{
		return -d_sigmoid;	// for the head and rear pars, the direction is the opposite of the regular one
	}
}



//------------------------------------------------------------
//
// return the average of directions
//------------------------------------------------------------
float Particle::GetAverageBearing(bool IsOpenMode, int head_par, int rear_par, Particle* thispar)
{
	bool Is_headpar = false, Is_rearpar = false;
	if(/*IsOpenMode && */thispar->id==head_par)	// for the head particle in open-area mode
	{
		Is_headpar = true;
	}
	if(/*IsOpenMode && */thispar->id==rear_par)	// for the rear particle in open-area mode
	{
		Is_rearpar = true;
	}
	ofVec2f mid_vec = (thispar->prev->bearing_vec + thispar->next->bearing_vec)/2;
	if(Is_headpar)
		mid_vec = (thispar->next->bearing_vec + thispar->next->next->bearing_vec)/2;
	else if(Is_rearpar)
		mid_vec = (thispar->prev->bearing_vec + thispar->prev->prev->bearing_vec)/2;
	mid_vec.normalize();
	float d_aver = acos(mid_vec.dot(ofVec2f(0,1)));
	if(mid_vec.x<0)
		d_aver = 2*PI-d_aver;	// adjust signs
			float test_aver = degrees(d_aver);
	return d_aver;

}

//------------------------------------------------------------
//
// Sigmoid movement for death control	(C--A--B--D, kill B and C)
//------------------------------------------------------------
void Particle::SigmoidDeath(int head_par, int rear_par)
{
	Particle *A = this;
	Particle *B = this->next;
	Particle *C = this->prev;
	Particle *D = this->next->next;

	if(A->id!=head_par && B->id!=rear_par)
	{
		float x;
		float d_sigmoid = 0;
		float CA = dist(C,A);
		float AB = dist(A,B);
		float CB = CA+AB;
		float BD = dist(B,D);
		float mid = (CA+AB+BD)/2;

		//-----------------------------------------------------------------
		//------------------------- For A:---------------------------------
		//-----------------------------------------------------------------
		float Aratio = CA/mid;
		Aratio = floorf(Aratio * 100) / 100;	// round down to two decimal places
		//----------------------------------------------------------------
		if(Aratio<1.0)			// tangent-like shape (move to right)
		{
			x = (PI-PI/6)*Aratio - PI/3;	// -60 < x < 90
			x = floorf(x * 100) / 100;
			float slope = 1/(cos(x)*cos(x)); // sec square
			d_sigmoid = atan2(1,slope);
			if(d_sigmoid>0)
				d_sigmoid = -d_sigmoid;
		}
		else if(Aratio>1.0)			// inverse-tangent-like (move to left)
		{
			Aratio = 2-Aratio;
			Aratio = floorf(Aratio * 100) / 100;
			x = (PI-PI/6)*Aratio - PI/3;	// -60 < x < 90
			x = floorf(x * 100) / 100;
			float slope = 1/(cos(x)*cos(x)); // sec square
			d_sigmoid = atan2(-1,slope);
			if(d_sigmoid<0)
				d_sigmoid = -d_sigmoid;
		}

		float d_Anew = A->bearing + d_sigmoid;
		ofVec2f Anew_bearing_vec = ofVec2f(sin(d_Anew),cos(d_Anew));
		Anew_bearing_vec.normalize();
		A->bearing_vec = 0.999*A->bearing_vec + 0.001*Anew_bearing_vec;		// adjust the bearing through vector
		A->bearing_vec.normalize();
		A->bearing = acos(A->bearing_vec.dot(ofVec2f(0,1)));
		if(A->bearing_vec.x<0)
			A->bearing = 2*PI-A->bearing;
		A->angle = degrees(A->bearing);

		//-----------------------------------------------------------------
		//--------------For B:---------------------------------------------
		//-----------------------------------------------------------------
		float Bratio = CB/mid;
		Bratio = floorf(Bratio * 100) / 100;	// round down to two decimal places
		//----------------------------------------------------------------
		if(Bratio<1.0)			// tangent-like shape (move to right)
		{
			x = (PI-PI/6)*Bratio - PI/3;	// -60 < x < 90
			x = floorf(x * 100) / 100;
			float slope = 1/(cos(x)*cos(x)); // sec square
			d_sigmoid = atan2(1,slope);
			if(d_sigmoid>0)
				d_sigmoid = -d_sigmoid;
		}
		else if(Bratio>1.0)			// inverse-tangent-like (move to left)
		{
			Bratio = 2-Bratio;
			Bratio = floorf(Bratio * 100) / 100;
			x = (PI-PI/6)*Bratio - PI/3;	// -60 < x < 90
			x = floorf(x * 100) / 100;
			float slope = 1/(cos(x)*cos(x)); // sec square
			d_sigmoid = atan2(-1,slope);
			if(d_sigmoid<0)
				d_sigmoid = -d_sigmoid;
		}

		float d_Bnew = B->bearing + d_sigmoid;
		ofVec2f Bnew_bearing_vec = ofVec2f(sin(d_Bnew),cos(d_Bnew));
		Bnew_bearing_vec.normalize();
		B->bearing_vec = 0.999*B->bearing_vec + 0.001*Bnew_bearing_vec;		// adjust the bearing through vector
		B->bearing_vec.normalize();
		B->bearing = acos(B->bearing_vec.dot(ofVec2f(0,1)));
		if(B->bearing_vec.x<0)
			B->bearing = 2*PI-B->bearing;
		B->angle = degrees(B->bearing);
	}
}


bool Particle::PushBackOrFwd(int head, int rear, bool IsOpenMode)
{
	// In an open area mode, do not puch back or foward a head or rear
	// NOTE that the head and rear par will still be pushed through their neighbors
	if(this->id == head || this->id == rear)	
		return false;
	else
	{
		// NOTE: be careful with the situation where the vec of 
		// AB pars is not perpendicular to the initial direction_vec of A
		ofVec2f ABvec;
		ABvec = this->prev->pos - this->pos;
		ABvec.normalize();
		//calculate dirA's projection on AB
		float dirA_dot_AB = this->bearing_vec.dot(ABvec);
		/*--------------------------------------------------
				if dirA_dot_AB>0, then A is left behind
				if dirA_dot_AB<0, then A is in front
				push or drag A and B 
		--------------------------------------------------*/
		this->pos.x += 0.01 * sin(this->prev->bearing) * dirA_dot_AB/2;
		this->pos.y += 0.01 * cos(this->prev->bearing)* dirA_dot_AB/2;
		this->prev->pos.x -= 0.01 * sin(this->prev->bearing) *dirA_dot_AB/2;
		this->prev->pos.y -= 0.01 * cos(this->prev->bearing) *dirA_dot_AB/2;
		return true;
	}
}

void Particle::Update()
{
	last_pos = pos;
	pos.x += 0.1 * speed * sin(bearing);
	pos.y += 0.1 * speed * cos(bearing);
}

void Particle::storepathforPDF()
{
	makepdf.growpath(pos.x,pos.y,musthavepoint,Is_visible);
	musthavepoint = false;	// reset the bool
}

bool Particle::OutOfBoudaryKill()	
{
	
	if(this->pos.x < 0 || floor(this->pos.x+0.5) >= ofGetWidth() || this->pos.y <0 || floor(this->pos.y+0.5) >= ofGetHeight())
	{
		return true; // if the particle is out of boundary
	}
	else	
	{
		return false;
	}
}


//----------------------------------------------------------------------
//
// update spacing threshold (it'll update the linewidth automatically)
//
//----------------------------------------------------------------------
bool Particle::UpdateSpacingTd(float Td)
{
	
	float currentTd = Td;
	currentTd = floorf(currentTd * 1000) / 1000;	// round down to two decimal places
	last_spacing_threshold = floorf(last_spacing_threshold * 1000) / 1000; 
	if(last_spacing_threshold < currentTd)
	{
		last_spacing_threshold += 0.015;
		UpdateLineWidth(currentTd,MIN_SP_VIABLE,MAX_SP_VIABLE);
		return false;
	}
	else if(last_spacing_threshold > currentTd)
	{
		last_spacing_threshold -= 0.015;
		UpdateLineWidth(currentTd,MIN_SP_VIABLE,MAX_SP_VIABLE);
		return false;
	}
	else if(last_spacing_threshold == currentTd)
		return true;

	return false;
}

float Particle::UpdateLineWidth(float currentTd, float minTd, float maxTd)
{
	// rescale a new linewidth based on the scale of threshold
	//float new_linewidth = ((currentTd/BASE_SPACING-minTd)*(LINEMAX-LINEMIN)) / (maxTd-minTd) + LINEMIN;
	float new_linewidth = -((currentTd/BASE_SPACING-minTd)*(LINEMAX-LINEMIN)) / (maxTd-minTd) + LINEMAX;// larger spacing, smaller linewidth
	
	//if(currentTd>= 4*BASE_SPACING*MAX_SP_VIABLE/5 && linewidth_timer_reg >= LINEWD_TIMER_LIMIT_REG)	// if current threshold exceed 4/5 max spacing threshold && it's not in zero linewidth mode
	//{
	//	linewidth_timer_reg = 0;
	//	linewidth_timer_zero++;
	//	new_linewidth = 0;
	//}
	//else if(currentTd<4*BASE_SPACING*MAX_SP_VIABLE/5 && linewidth_timer_zero >= LINEWD_TIMER_LIMIT_ZERO)	//  if current threshold not exceed 4/5 max spacing threshold && it's in zero linewidth mode
	//{
	//	linewidth_timer_zero = 0;
	//	linewidth_timer_reg++;
	//}

	//if(currentTd>= 5*BASE_SPACING*MAX_SP_VIABLE/6)	// if current threshold exceed 4/5 max spacing threshold && it's not in zero linewidth mode
	//{
	//	new_linewidth = 0;
	//}


	if(this->linewidth < new_linewidth)
	{
		linewidth += 0.5;
	}
	else if(this->linewidth > new_linewidth)
	{
		linewidth -= 0.5;
		if(linewidth<1 && linewidth > 0)
			linewidth = 1;
	}
	return new_linewidth;
}