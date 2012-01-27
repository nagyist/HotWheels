#include "stdafx.h"
#include "Player.h"


Player::Player()
{

}

Player::~Player(void)
{
}


void Player::init(Critter::RenderSystem* render, Ogre::SceneManager* mSceneMgr){
	pCar = Vehicle::createVehicle("camaro",render,  mSceneMgr);

	pCar->getBody()->setGlobalPosition(Vector3(300,300,-550));
}


void Player::onKeyPress(OIS::Keyboard* pKeyBoard){

	if (pKeyBoard->isKeyDown(OIS::KC_I)){
		pCar->accelerate();
	}else if(pKeyBoard->isKeyDown(OIS::KC_K)){
		pCar->deaccelerate();
	}
	
	if(pKeyBoard->isKeyDown(OIS::KC_J))
	{
		pCar->steerRight();

	}else if (pKeyBoard->isKeyDown(OIS::KC_L)){
		

		pCar->steerLeft();

	}


}

Vehicle* Player::getVechicle(){
	return pCar;
}