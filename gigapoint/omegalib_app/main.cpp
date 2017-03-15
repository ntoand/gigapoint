#include <omega.h>
#include <omegaGl.h>
#include <omegaToolkit.h>

#include <iostream>
#include <list>
#include <string>

#include "../PointCloud.h"
#include "../Menu.h"

using namespace omega;
using namespace omegaToolkit;
using namespace omegaToolkit::ui;
using namespace std;
using namespace gigapoint;


class GigapointRenderPass: public RenderPass
{
public:
	GigapointRenderPass(Renderer* client, Option* opt, PointCloud* pc): 
						RenderPass(client, "GigapointRenderPass"), option(opt), pointcloud(pc) {}
	virtual void initialize();
	virtual void render(Renderer* client, const DrawContext& context);

private:
	PointCloud* pointcloud;
	Option* option;
	int framecount;
};

class GigapointApplication: public EngineModule
{
private:
	PointCloud* pointcloud;
	Option* option;
	PCMenu* menu;
	Ref<Label> menuLabel;
	// The ui manager
  	Ref<UiModule> myUiModule;
  	// The root ui container
  	Ref<Container> myUi;  

public:
	GigapointApplication(): EngineModule("GigapointApplication") {}

	virtual void initializeRenderer(Renderer* r)  { 
		r->addRenderPass(new GigapointRenderPass(r, option, pointcloud));
	}

	virtual void initialize();

	void menuAction(int type);
	void resetCamera();
	void printInfo();
	virtual void handleEvent(const Event& evt);

	virtual void dispose()
	{
		if(pointcloud)
			delete pointcloud;
		if(menu)
			delete menu;
		if(option)
			delete option;
	}

    virtual bool handleCommand(const String& cmd);


};


int main(int argc, char** argv)
<<<<<<< HEAD:gigapoint/omegalib_app/main.cpp
{

    Application<GigapointApplication> app("OPotree");
    return omain(app, argc, argv);
}


//========== GigapointRenderPass ==========
void GigapointRenderPass::initialize()
=======
{    
    Application<OPotreeApplication> app("OPotree");
    return omain(app, argc, argv);
}

//========== OPotreeRenderPass ==========
void OPotreeRenderPass::initialize()
>>>>>>> need to avoid too many filesystem reads, recieving MC commands to flag nodes as dirty:opotree/main.cpp
{
	RenderPass::initialize();

	// Initialize
	pointcloud->initPointCloud();
	//graphics
	glEnable(GL_POINT_SPRITE);
	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);

	framecount = 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void GigapointRenderPass::render(Renderer* client, const DrawContext& context)
{
    if(context.task == DrawContext::SceneDrawTask)
	{
		client->getRenderer()->beginDraw3D(context);
		if(oglError) return;

		// Enable depth testing and lighting.
		glEnable(GL_DEPTH_TEST);
		if(oglError) return;
		glEnable(GL_LIGHTING);
		if(oglError) return;
	
		// Test and draw
		// get camera location in world coordinate

		Vector3f cp = context.camera->getPosition();
		float campos[3] = {cp[0], cp[1], cp[2]};

		float* MVP = (context.projection*context.modelview).cast<float>().data();
        pointcloud->updateVisibility(MVP, campos);
		pointcloud->draw();
		
		/*
		framecount++;
		if(framecount > 500) {
			if(SystemManager::instance()->isMaster()) {
				Quaternion q = context.camera->getOrientation();
				cout << "Camara pos: " << cp[0] << "," << cp[1] << "," << cp[2] << " ";
				cout << "orientation: " << q.w() << "," << q.x() << "," << q.y() << "," << q.z() << endl;
			}
			framecount = 0;
		} 
		*/  		

		if(oglError) return;
		client->getRenderer()->endDraw();
		if(oglError) return;
	}
}


//========== GigapointApplication ==========
void GigapointApplication::initialize() {


	option = Utils::loadOption("gigapoint.json");
	//UI
	//Create a label for text info
	DisplaySystem* ds = SystemManager::instance()->getDisplaySystem();
	//Create and initialize the UI management module.
	myUiModule = UiModule::createAndInitialize();
	myUi = myUiModule->getUi();
	int sz = 100;
	menuLabel = Label::create(myUi);
	menuLabel->setText("");
	menuLabel->setColor(Color::Red);
	menuLabel->setFont(ostr("gigapoint_resource/fonts/arial.ttf %1%", %option->menuOption[2]));
	menuLabel->setHorizontalAlign(Label::AlignLeft);
	menuLabel->setPosition(Vector2f(option->menuOption[0],option->menuOption[1]));


    if (SystemManager::instance()->isMaster())
        cout << "TODO update PCINFO" << endl;
    //PointCloud
	pointcloud = new PointCloud(option, SystemManager::instance()->isMaster());
	menu = new PCMenu(option);
	cout << "menu: " << menu->getString() << endl;
	menuLabel->setText(menu->getString());
	
	//Camera
	Camera* cam = getEngine()->getDefaultCamera();
	cam->getController()->setSpeed(option->cameraSpeed);
	cam->getController()->setFreeFlyEnabled(true);
	cam->setNearFarZ(1.0, 10000.0);

	if(option->cameraUpdatePosOri) {
		cam->setPosition(Vector3f(option->cameraPosition[0], option->cameraPosition[1], option->cameraPosition[2]));
		cam->setOrientation(Quaternion(option->cameraOrientation[0], option->cameraOrientation[1], 
										option->cameraOrientation[2], option->cameraOrientation[3]));
	}	
}

void GigapointApplication::menuAction(int type) {
	switch(type) {
		case 0: 
			menu->prev();
        	cout << "left " << menu->getString() << endl;
        	menuLabel->setText(menu->getString());
        	pointcloud->setReloadShader(menu->updateOption(option));
			break;
		case 1:
			menu->next();
        	cout << "right " << menu->getString() << endl;
        	menuLabel->setText(menu->getString());
        	pointcloud->setReloadShader(menu->updateOption(option));
			break;
		case 2:
			menu->down();
        	cout << "down " << menu->getString() << endl;
        	menuLabel->setText(menu->getString());
        	pointcloud->setReloadShader(menu->updateOption(option));
			break;
		case 3:
			menu->up();
        	cout << "up " << menu->getString() << endl;
        	menuLabel->setText(menu->getString());
        	pointcloud->setReloadShader(menu->updateOption(option));
			break;
		default:
			break;
	}
}

void GigapointApplication::resetCamera() {
	Camera* cam = getEngine()->getDefaultCamera();

    if(option->cameraUpdatePosOri) {
        cam->setPosition(Vector3f(option->cameraPosition[0], option->cameraPosition[1], option->cameraPosition[2]));
        cam->setOrientation(Quaternion(option->cameraOrientation[0], option->cameraOrientation[1],
                                        option->cameraOrientation[2], option->cameraOrientation[3]));
    }
}

void GigapointApplication::printInfo() {
    if(!SystemManager::instance()->isMaster())
        return;
    pointcloud->setPrintInfo(true);
    Camera* cam = getEngine()->getDefaultCamera();
    Vector3f cp = cam->getPosition();
    Quaternion q = cam->getOrientation();
    cout << "pos: " << cp[0] << "," << cp[1] << "," << cp[2] << " ";
    cout << "orientation: " << q.w() << "," << q.x() << "," << q.y() << "," << q.z() << endl;
}


bool GigapointApplication::handleCommand(const String& cmd) { // ":dirty NODENAME eg: ':dirty r0012' "
    //cout <<  "got Command: " << cmd.substr(0,5) << endl;    
    if (0==cmd.substr(0,5).compare("dirty"))
    {        
        cout << "testing substring" << cmd.substr(6,cmd.size()-1);
        pointcloud->flagNodeAsDirty(cmd.substr(6,cmd.size()));
    }
    return true;
}

void GigapointApplication::handleEvent(const Event& evt) {
    if(evt.getServiceType() == Service::Keyboard) {
        if (evt.isKeyDown('j')) {
        	menuAction(0);
        }
        else if (evt.isKeyDown('l')) {
        	menuAction(1);
        }
        else if (evt.isKeyDown('k')) {
        	menuAction(2);
        }
        else if (evt.isKeyDown('i')) {
        	menuAction(3);
        }
        else if (evt.isKeyDown('r')) {
                resetCamera();
        }
        else if (evt.isKeyDown('p')) {
			printInfo();
            cout << "#####################################" << endl;
            cout << "#####################################" << endl;
            cout << "#####################################" << endl;
        }

        else if (evt.isKeyDown('m')) {
            if(pointcloud->getInteractMode() == INTERACT_NONE)
                pointcloud->setInteractMode(INTERACT_POINT);
            else
                pointcloud->setInteractMode(INTERACT_NONE);
        }

        else if (evt.isKeyDown('e')) {
            cout << "reloading everything" << endl;
            cout << "TODO print before after in reloading" << endl;
            pointcloud->setReloading(true);
                printInfo();
        }
        else if (evt.isKeyDown('e')) {            
            pointcloud->setReloading(true);            

        }
        else if (evt.isKeyDown('u')) {
            pointcloud->togglePauseUpdate();
        }
        else if (evt.isKeyDown('g')) {
            pointcloud->setUnloading(true);
        }
        else if (evt.isKeyDown('h')) {
            pointcloud->resetRootHierarchy();

        }
<<<<<<< HEAD:gigapoint/omegalib_app/main.cpp

=======
/*
		else if (evt.isKeyDown('p')) {
			if(pointcloud->getInteractMode() == INTERACT_NONE)
				pointcloud->setInteractMode(INTERACT_POINT);
			else
				pointcloud->setInteractMode(INTERACT_NONE);
		}
*/
>>>>>>> interactMode is now part of the menu:opotree/main.cpp

    }
    
    else if(evt.getServiceType() == Service::Pointer ) {
		DisplaySystem* ds = SystemManager::instance()->getDisplaySystem();
	    Ray r;
	    bool res = ds->getViewRayFromEvent(evt, r);
	    if(res)
	    	pointcloud->updateRay(r);

	    if(evt.getType() == Event::Down && evt.getFlags() & 2) {
	    	pointcloud->findHitPoint();
	    }
		
    }

    else if(evt.getServiceType() == Service::Wand) {
        if(option->interactMode != INTERACT_NONE) {
    		DisplaySystem* ds = SystemManager::instance()->getDisplaySystem();
		    Ray r;
		    bool res = ds->getViewRayFromEvent(evt, r);
		    if(res)
		    	pointcloud->updateRay(r);
    	}
    	
    	if (evt.isButtonDown(Event::ButtonLeft )) {
      		menuAction(0);
    	}
    	else if (evt.isButtonDown(Event::ButtonRight )) {
      		menuAction(1);
    	}
    	else if (evt.isButtonDown(Event::ButtonDown)) {
    		menuAction(2);
    	}
        else if (evt.isButtonDown(Event::ButtonUp)) {
      		menuAction(3);
        }
		else if (evt.isButtonDown(Event::Button3)) { // cross
			resetCamera();
		}
		else if (evt.isButtonDown(Event::Button2)) { // circle
			printInfo();
		}
<<<<<<< HEAD:gigapoint/omegalib_app/main.cpp
		else if (evt.isButtonDown(Event::Button1) || evt.isButtonDown(Event::Button4)) {
=======
        else if (evt.isButtonDown(Event::Button4)) { // circle
            pointcloud->setReloading(true);
        }
        /*
        else if (evt.isButtonDown(Event::Button1) ) {
>>>>>>> interactMode is now part of the menu:opotree/main.cpp
			if(pointcloud->getInteractMode() == INTERACT_NONE)
				pointcloud->setInteractMode(INTERACT_POINT);
			else
				pointcloud->setInteractMode(INTERACT_NONE);
		}
        */
		else if (evt.isButtonDown(Event::Button5)) {  // L1
			pointcloud->findHitPoint();
		}
	else if (evt.isButtonDown(Event::Button3)) { // cross
		resetCamera();
	}
	else if (evt.isButtonDown(Event::Button2)) { // circle
		printInfo();
	}
        //else if (evt.isButtonDown(Event::Button4)) { // X
        //
	}
        else if (evt.isButtonDown(Event::Button4)) {
		cout << "got Event Button 4" << endl;
                pointcloud->setReloading(true);
	}

    }
}
