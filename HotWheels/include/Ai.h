#pragma once
#include "stdafx.h"
#include "Vehicle.h"
#include "Track.h"
#include <math.h>
#include "OgreException.h"
#include "Settings.h"

using namespace Ogre;

class Ai
{

private:
	Vehicle* pCar;
	Track* pTrack;
	SceneManager* mSceneMgr;

	Ogre::Vector3 * pTargetPosition;
	int mCurrentWayPiont;
	float count;

	Ai(Ai &); // disable copy construction

public:
	Ai(void);

	Vehicle* getVechicle();
	void setTrack(Track * pTrack);

	void init(Critter::RenderSystem* render, Ogre::SceneManager* mSceneMgr);

	void update(float timeDelta);
	//void attachCamera(Ogre::Camera* pCamera);
	~Ai(void);
};

