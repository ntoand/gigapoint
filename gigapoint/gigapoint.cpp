#include <omega.h>
#include <omegaGl.h>
#include <iostream>

#include "PointCloud.h"

using namespace std;
using namespace omega;

///////////////////////////////////////////////////////////////////////////////
class GigapointRenderModule : public EngineModule
{
public:
    GigapointRenderModule() :
        EngineModule("GigapointRenderModule")
    {
    	
    }

    virtual void initializeRenderer(Renderer* r);

    virtual void update(const UpdateContext& context)
    {
        // After a frame all render passes had a chance to update their
        // textures. reset the raster update flag.
       
    }

    void initPotree(const string& option_file)
    {
    	//init values here
    	//option = gigapoint::Utils::loadOption("opotree.json");
    	option = gigapoint::Utils::loadOption(option_file);
    	pointcloud = new gigapoint::PointCloud(option, SystemManager::instance()->isMaster());
    	pointcloud->initPointCloud();

    	//Camera
	    cout << "Update cam..." << endl;
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

    gigapoint::PointCloud* pointcloud;
    gigapoint::Option* option;
};

///////////////////////////////////////////////////////////////////////////////
class GigapointRenderPass : public RenderPass
{
public:
    GigapointRenderPass(Renderer* client, GigapointRenderModule* prm) : 
        RenderPass(client, "GigapointRenderPass"), 
        module(prm) {}
    
    virtual void initialize()
    {
        RenderPass::initialize();

        //graphics
		glEnable(GL_POINT_SPRITE);
		glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
    }

    virtual void render(Renderer* client, const DrawContext& context)
    {
        // Do we have new raster data?
        /*
        if(module->rasterUpdated)
        {
            oglError;
            
        }
        */
     
    	if(context.task == DrawContext::SceneDrawTask)
        {
            glPushAttrib(GL_TEXTURE_BIT | GL_ENABLE_BIT | GL_CURRENT_BIT);
            client->getRenderer()->beginDraw3D(context);

          	// Enable depth testing and lighting.
            glEnable(GL_DEPTH_TEST);
			if(oglError) return;
			glEnable(GL_LIGHTING);
			if(oglError) return;
		   
			// Test and draw
			// get camera location in world coordinate
            if(context.eye == DrawContext::EyeLeft || context.eye == DrawContext::EyeCyclop)
            {
                Vector3f cp = context.camera->getPosition();
                float campos[3] = {cp[0], cp[1], cp[2]};
                float* MVP = (context.projection*context.modelview).cast<float>().data();
                module->pointcloud->updateVisibility(MVP, campos);
            }

			module->pointcloud->draw();
			if(oglError) return;

            client->getRenderer()->endDraw();
            glPopAttrib();
        }
        
    }

private:
    GigapointRenderModule* module;

};

///////////////////////////////////////////////////////////////////////////////
void GigapointRenderModule::initializeRenderer(Renderer* r)
{
    r->addRenderPass(new GigapointRenderPass(r, this));
}

///////////////////////////////////////////////////////////////////////////////
GigapointRenderModule* initialize()
{
    GigapointRenderModule* prm = new GigapointRenderModule();
    ModuleServices::addModule(prm);
    prm->doInitialize(Engine::instance());
    return prm;
}

///////////////////////////////////////////////////////////////////////////////
// Python API
#include "omega/PythonInterpreterWrapper.h"
BOOST_PYTHON_MODULE(gigapoint)
{
    //
    PYAPI_REF_BASE_CLASS(GigapointRenderModule)
        PYAPI_METHOD(GigapointRenderModule, initPotree)
        //PYAPI_METHOD(VolumeRenderModule, setSliceBoundX)
        //PYAPI_METHOD(VolumeRenderModule, setSliceBoundY)
        //PYAPI_METHOD(VolumeRenderModule, setSliceBoundZ)
        //.add_property("node", make_getter(&VolumeRenderModule::node, return_value_policy<reference_existing_object>()), make_setter(&VolumeRenderModule::node, PYAPI_RETURN_VALUE))
        ;

    def("initialize", initialize, PYAPI_RETURN_REF);
}
