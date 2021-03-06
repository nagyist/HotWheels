/*
-----------------------------------------------------------------------------
Filename:    BaseApplication.cpp
-----------------------------------------------------------------------------


This source file is generated by the
   ___                   _              __    __ _                  _ 
  /___\__ _ _ __ ___    /_\  _ __  _ __/ / /\ \ (_)______ _ _ __ __| |
 //  // _` | '__/ _ \  //_\\| '_ \| '_ \ \/  \/ / |_  / _` | '__/ _` |
/ \_// (_| | | |  __/ /  _  \ |_) | |_) \  /\  /| |/ / (_| | | | (_| |
\___/ \__, |_|  \___| \_/ \_/ .__/| .__/ \/  \/ |_/___\__,_|_|  \__,_|
      |___/                 |_|   |_|                                 
      Ogre 1.7.x Application Wizard for VC10 (July 2011)
      http://code.google.com/p/ogreappwizards/
-----------------------------------------------------------------------------
*/
#include "stdafx.h"
#include "BaseApplication.h"

#include "Settings.h"


#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#include "../res/resource.h"
#endif


//-------------------------------------------------------------------------------------
BaseApplication::BaseApplication(void)
    : mRoot(0),
    mCamera(0),
    mSceneMgr(0),
    mWindow(0),
    mResourcesCfg(Ogre::StringUtil::BLANK),
    mPluginsCfg(Ogre::StringUtil::BLANK),
    mUserInterface(0),
    mCameraMan(0),
    mDetailsPanel(0),
    mCursorWasVisible(false),
    mShutDown(false),
    mInputManager(0),
    mMouse(0),
    mKeyboard(0)
{

}

//-------------------------------------------------------------------------------------
BaseApplication::~BaseApplication(void)
{
	 delete  mUserInterface;
    if (mCameraMan) delete mCameraMan;

	NxOgre::World::destroyWorld();

    //Remove ourself as a Window listener
    Ogre::WindowEventUtilities::removeWindowEventListener(mWindow, this);
    windowClosed(mWindow);
    delete mRoot;
}

//-------------------------------------------------------------------------------------
bool BaseApplication::configure(void)
{
    // Show the configuration dialog and initialise the system
    // You can skip this and use root.restoreConfig() to load configuration
    // settings if you were sure there are valid ones saved in ogre.cfg
    if(true)
    {	//mRoot->showConfigDialog()
			mRoot->restoreConfig();
        // If returned true, user clicked OK so initialise
        // Here we choose to let the system create a default rendering window by passing 'true'
        mWindow = mRoot->initialise(true, "OgreAppTest Render Window");

        // Let's add a nice window icon
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
        HWND hwnd;
        mWindow->getCustomAttribute("WINDOW", (void*)&hwnd);
        LONG iconID   = (LONG)LoadIcon( GetModuleHandle(0), MAKEINTRESOURCE(IDI_APPICON) );
        SetClassLong( hwnd, GCL_HICON, iconID );
#endif
        return true;
    }
    else
    {
        return false;
    }
}
//-------------------------------------------------------------------------------------
void BaseApplication::chooseSceneManager(void)
{
    // Get the SceneManager, in this case a generic one
    mSceneMgr = mRoot->createSceneManager(Ogre::ST_GENERIC);
}
//-------------------------------------------------------------------------------------
void BaseApplication::createCamera(void)
{
    // Create the camera
    mCamera = mSceneMgr->createCamera("PlayerCam");

    // Position it at 500 in Z direction
    mCamera->setPosition(Ogre::Vector3(0,0,0));
    // Look back along -Z
    mCamera->lookAt(Ogre::Vector3(0,0,0));
    mCamera->setNearClipDistance(5);

    mCameraMan = new OgreBites::SdkCameraMan(mCamera);   // create a default camera controller
}
//-------------------------------------------------------------------------------------
void BaseApplication::createFrameListener(void)
{
    Ogre::LogManager::getSingletonPtr()->logMessage("*** Initializing OIS ***");
    OIS::ParamList pl;
    size_t windowHnd = 0;
    std::ostringstream windowHndStr;

    mWindow->getCustomAttribute("WINDOW", &windowHnd);
    windowHndStr << windowHnd;
    pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));

    mInputManager = OIS::InputManager::createInputSystem( pl );

    mKeyboard = static_cast<OIS::Keyboard*>(mInputManager->createInputObject( OIS::OISKeyboard, true ));
    mMouse = static_cast<OIS::Mouse*>(mInputManager->createInputObject( OIS::OISMouse, true ));

    mMouse->setEventCallback(this);
    mKeyboard->setEventCallback(this);

    //Set initial mouse clipping size
    windowResized(mWindow);

    //Register as a Window listener
    Ogre::WindowEventUtilities::addWindowEventListener(mWindow, this);

    mUserInterface = new GUI( mWindow, mMouse);
	mUserInterface->hide();
	mUserInterface->getTrayManager()->showFrameStats(OgreBites::TL_BOTTOMLEFT);

		/* Create background rectangle covering the whole screen
		Rectangle2D* rect = new Rectangle2D(true);
		
		rect->setCorners(-0.5, 0.5, 0.5, -0.5);
		rect->setMaterial("Examples/RustySteel");

		// Render the background before everything else
		rect->setRenderQueueGroup(RENDER_QUEUE_BACKGROUND);

		// Hacky, but we need to set the bounding box to something big
		// NOTE: If you are using Eihort (v1.4), please see the note below on setting the bounding box
		rect->setBoundingBox(AxisAlignedBox(-100000.0*Vector3::UNIT_SCALE, 100000.0*Vector3::UNIT_SCALE));

		// Attach background to the scene
		SceneNode* node = mSceneMgr->getRootSceneNode()->createChildSceneNode("Background2dstuff");
		node->attachObject(rect);

		node->setPosition(5,5,5);
		

		// Example of background scrolling
		*/


    mRoot->addFrameListener(this);
}
//-------------------------------------------------------------------------------------
void BaseApplication::destroyScene(void)
{
}
//-------------------------------------------------------------------------------------
void BaseApplication::createViewports(void)
{
    // Create one viewport, entire window
	vp = mWindow->addViewport(mCamera);
    vp->setBackgroundColour(Ogre::ColourValue(0,0,0));

    // Alter the camera aspect ratio to match the viewport
    mCamera->setAspectRatio(
        Ogre::Real(vp->getActualWidth()) / Ogre::Real(vp->getActualHeight()));
}
//-------------------------------------------------------------------------------------
void BaseApplication::setupResources(void)
{
    // Load resource paths from config file
    Ogre::ConfigFile cf;
    cf.load(mResourcesCfg);

    // Go through all sections & settings in the file
    Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();

    Ogre::String secName, typeName, archName;
    while (seci.hasMoreElements())
    {
        secName = seci.peekNextKey();
        Ogre::ConfigFile::SettingsMultiMap *settings = seci.getNext();
        Ogre::ConfigFile::SettingsMultiMap::iterator i;
        for (i = settings->begin(); i != settings->end(); ++i)
        {
            typeName = i->first;
            archName = i->second;
            Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
                archName, typeName, secName);
        }
    }
}
//-------------------------------------------------------------------------------------
void BaseApplication::createResourceListener(void)
{

}
//-------------------------------------------------------------------------------------
void BaseApplication::loadResources(void)
{
    Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
}
//-------------------------------------------------------------------------------------
void BaseApplication::go(void)
{
#ifdef _DEBUG
    mResourcesCfg = "resources_d.cfg";
    mPluginsCfg = "plugins_d.cfg";
#else
    mResourcesCfg = "resources.cfg";
    mPluginsCfg = "plugins.cfg";
#endif

    if (!setup())
        return;

    mRoot->startRendering();

    // clean up
    destroyScene();
}
//-------------------------------------------------------------------------------------
bool BaseApplication::setup(void)
{
	mRoot = new Ogre::Root(mPluginsCfg);

	setupResources();

	bool carryOn = configure();
    if (!carryOn) return false;

	chooseSceneManager();
	createCamera();
	createViewports();

    // Set default mipmap level (NB some APIs ignore this)
    Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);

    // Create any resource listeners (for loading screens)
    createResourceListener();
    // Load resources
    loadResources();

	//Phyxs
	setupPhysics();
	createScene();

    createFrameListener();
 //   
	return true;
};

void BaseApplication::setupPhysics()
{
	mWorld = NxOgre::World::createWorld();
	NxOgre::ResourceSystem::getSingleton()->openProtocol(new Critter::OgreResourceProtocol());
	mWorld->getRemoteDebugger()->connect();
	NxOgre::SceneDescription scene_description;
	
	scene_description.mGravity = Constants::MEAN_EARTH_GRAVITY;
	scene_description.mUseHardware = true;
	mScene = mWorld->createScene(scene_description);
	//mScene->createSceneGeometry(NxOgre::PlaneGeometryDescription());
	mRenderSystem = new Critter::RenderSystem(mScene, mSceneMgr);
	mScene->getMaterial(0)->setAll(0.1, 0.5,0.5);



	if(Settings::debugMode){
	//Set debug render
		NxOgre::VisualDebuggerDescription vd_desc;
		vd_desc.showDebug();

		mRenderSystem->createVisualDebugger(vd_desc);
	}
}

//-------------------------------------------------------------------------------------
bool BaseApplication::frameRenderingQueued(const Ogre::FrameEvent& evt)
{

	//if(!mUserInterface->isMenuShowing()){

		//advance physics
		mLastTimeStep = mScene->getTimeStep().getModified();

		// Don't let the camera go underground.
		if (mCamera->getPosition().y < 0.5f)
		{
			Ogre::Vector3 pos = mCamera->getPosition();
			pos.y = 0.5f;
			mCamera->setPosition(pos);
		}
	
		if(mWindow->isClosed())
			return false;


		//Need to capture/update each device
		mKeyboard->capture();
		mMouse->capture();


	

	//}

	mCameraMan->frameRenderingQueued(evt);
	mUserInterface->getTrayManager()->frameRenderingQueued(evt);
	mUserInterface->update(mShutDown);

    return true;
}
//-------------------------------------------------------------------------------------
bool BaseApplication::keyPressed( const OIS::KeyEvent &arg )
{

    if (arg.key == OIS::KC_ESCAPE)
        mUserInterface->show();

	if(!mUserInterface->isMenuShowing() && Settings::debugMode)
		mCameraMan->injectKeyDown(arg);

    return true;
}

bool BaseApplication::keyReleased( const OIS::KeyEvent &arg )
{
	if(!mUserInterface->isMenuShowing() && Settings::debugMode)
		mCameraMan->injectKeyUp(arg);
    return true;
}

bool BaseApplication::mouseMoved( const OIS::MouseEvent &arg )
{
    if ( mUserInterface->getTrayManager()->injectMouseMove(arg)) return true;
   
	
	if(!mUserInterface->isMenuShowing() && Settings::debugMode)
		mCameraMan->injectMouseMove(arg);
    return true;
}

bool BaseApplication::mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
    if ( mUserInterface->getTrayManager()->injectMouseDown(arg, id)) return true;
    mCameraMan->injectMouseDown(arg, id);
    return true;
}

bool BaseApplication::mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
    if ( mUserInterface->getTrayManager()->injectMouseUp(arg, id)) return true;
    mCameraMan->injectMouseUp(arg, id);
    return true;
}

//Adjust mouse clipping area
void BaseApplication::windowResized(Ogre::RenderWindow* rw)
{
    unsigned int width, height, depth;
    int left, top;
    rw->getMetrics(width, height, depth, left, top);

    const OIS::MouseState &ms = mMouse->getMouseState();
    ms.width = width;
    ms.height = height;
}

//Unattach OIS before window shutdown (very important under Linux)
void BaseApplication::windowClosed(Ogre::RenderWindow* rw)
{
    //Only close for window that created OIS (the main window in these demos)
    if( rw == mWindow )
    {
        if( mInputManager )
        {
            mInputManager->destroyInputObject( mMouse );
            mInputManager->destroyInputObject( mKeyboard );

            OIS::InputManager::destroyInputSystem(mInputManager);
            mInputManager = 0;
        }
    }
}
