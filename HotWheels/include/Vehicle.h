#pragma once
#include "stdafx.h"
#include "VehicleWheel.h"
#include "VehicleDescription.h"
#include "VehicleGears.h"
#include "VehicleMotor.h"
#include "VehicleGearsListener.h"
#include "SoundManager.h"
#include "Utilities.h"
#include "Track.h"


using namespace Ogre;
using namespace NxOgre;

enum VehiclePartIds
{
    ID_Chassis1     = 1,
    ID_Chassis2     = 2,
    ID_FLWheel      = 3,
    ID_FRWheel      = 4,
    ID_BLWheel      = 5,
    ID_BRWheel      = 6,
};


const float VEHICLE_STEERING_COEF = 0.08f;
const NxOgre::MaterialIdentifier VEHICLE_MATERIAL_ID = 128;

class Vehicle
{

    Vehicle(std::string name, VehicleDescription vehDesc, Critter::RenderSystem* critter, Ogre::SceneManager* mSceneMgr);

public:
	static Vehicle* createVehicle(std::string vehicleName, Critter::RenderSystem* critter, Ogre::SceneManager* mSceneMgr);
	static VehicleDescription readFromFile(std::string filename);
    ~Vehicle();

    virtual void update(float diff);
    void postPhysics(float diff);

    void accelerate();
    void deaccelerate();
    void steerLeft(float userDefinedSteeringRate = 1);
    void steerRight(float userDefinedSteeringRate = 1);

    int getCurrentGear() { return gears->getCurrentGear(); }
    float getRpm() { return motor->getRpm(); }
    Critter::Body* getBody() { return body; }

    void addGearsListener(VehicleGearsListener* gListener) { gearListeners.push_back(gListener); }
    void destroyGearsListener(VehicleGearsListener* gListener);

	void attachCamera(Ogre::Camera* pCamera);

	Ogre::Vector3 getWheelDirection(){

		Ogre::Node* g = this->getBody()->getNode()->getSceneNode()->getChild(3);

		return Vector3(0,0,0);
	}

	VehicleWheel* getWheel(int index){

		assert(index < 4);
		return wheels[index];
	}

	void setMotorToqureMultplier(float m){
		this->motorToqureMultplier = m;
	}



	VehicleWheel* wheels[4];

protected:
    bool steering;
    float accelerationPedal;
    float steer;
	float motorToqureMultplier;

	bool revingSound;

    VehicleDescription vDesc;
    std::string vehicleName;
    Critter::RenderSystem* critter;
    Critter::Body* body;
    
    VehicleGears* gears;
    VehicleMotor* motor;
	Ogre::Light* directionalLight;

	ParticleSystem* rightExhaust;
	ParticleSystem* leftExhaust;

	static int vehicleCount;

    std::vector<VehicleGearsListener*> gearListeners;

    float computeRpmFromWheels();
    float computeMotorRpm(float rpm);
    float computeAxisTorque();
};