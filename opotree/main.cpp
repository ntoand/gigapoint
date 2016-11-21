#include <omega.h>
#include <omegaGl.h>
#include <omegaToolkit.h>

#include <iostream>
#include <list>
#include <string>

#include "PointCloud.h"
#include "Menu.h"

using namespace omega;
using namespace omegaToolkit;
using namespace omegaToolkit::ui;
using namespace std;

class OPotreeRenderPass: public RenderPass
{
public:
	OPotreeRenderPass(Renderer* client, Option* opt, PointCloud* pc): 
						RenderPass(client, "OPotreeRenderPass"), option(opt), pointcloud(pc) {}
	virtual void initialize();
	virtual void render(Renderer* client, const DrawContext& context);

private:
	PointCloud* pointcloud;
	Option* option;
	int framecount;
};

class OPotreeApplication: public EngineModule
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
	OPotreeApplication(): EngineModule("OPotreeApplication") {}

	virtual void initializeRenderer(Renderer* r)  { 
		r->addRenderPass(new OPotreeRenderPass(r, option, pointcloud));
	}

	virtual void initialize();

	void menuAction(int type);
	void resetCamera();
	virtual void handleEvent(const Event& evt);
	
};


int main(int argc, char** argv)
{
    Application<OPotreeApplication> app("OPotree");
    return omain(app, argc, argv);
}


//========== OPotreeRenderPass ==========
void OPotreeRenderPass::initialize()
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
void OPotreeRenderPass::render(Renderer* client, const DrawContext& context)
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

		framecount++;
		if(framecount > 500) {
			if(SystemManager::instance()->isMaster()) {
				Quaternion q = context.camera->getOrientation();
				cout << "Camara pos: " << cp[0] << " " << cp[1] << " " << cp[2] << " ";
				cout << "orientation: " << q.w() << " " << q.x() << " " << q.y() << " " << q.z() << endl;
			}
			framecount = 0;
		}   		

		if(oglError) return;
		client->getRenderer()->endDraw();
		if(oglError) return;
	}
}


//========== OPotreeApplication ==========
void OPotreeApplication::initialize() {

	option = Utils::loadOption("opotree.json");

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
	menuLabel->setFont(ostr("fonts/arial.ttf %1%", %option->menuOption[2]));
	menuLabel->setHorizontalAlign(Label::AlignLeft);
	menuLabel->setPosition(Vector2f(option->menuOption[0],option->menuOption[1]));

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

void OPotreeApplication::menuAction(int type) {
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

void OPotreeApplication::resetCamera() {
	Camera* cam = getEngine()->getDefaultCamera();

        if(option->cameraUpdatePosOri) {
                cam->setPosition(Vector3f(option->cameraPosition[0], option->cameraPosition[1], option->cameraPosition[2]));
                cam->setOrientation(Quaternion(option->cameraOrientation[0], option->cameraOrientation[1],
                                                                                option->cameraOrientation[2], option->cameraOrientation[3]));
        }
}

void OPotreeApplication::handleEvent(const Event& evt) {
    if(evt.getServiceType() == Service::Keyboard) {
        if(evt.isKeyDown('c')) {
        	if(!SystemManager::instance()->isMaster())
        		return;
        	Camera* cam = getEngine()->getDefaultCamera();
        	Vector3f cp = cam->getPosition();
			Quaternion q = cam->getOrientation();
			cout << "pos: " << cp[0] << " " << cp[1] << " " << cp[2] << " ";
			cout << "orientation: " << q.w() << " " << q.x() << " " << q.y() << " " << q.z() << endl;
        }
        else if (evt.isKeyDown('j')) {
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

    }
    else if(evt.getServiceType() == Service::Wand) {
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
    }
}
