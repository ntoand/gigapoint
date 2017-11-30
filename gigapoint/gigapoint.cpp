#include <omega.h>
#include <omegaGl.h>
#include <iostream>

#include "PointCloud.h"
#include "Interaction.h"

using namespace std;
using namespace omega;

///////////////////////////////////////////////////////////////////////////////
class GigapointRenderModule : public EngineModule
{
public:
    GigapointRenderModule() :
        EngineModule("GigapointRenderModule"), pointcloud(0), option(0), interaction(0), visible(true)
    {
    	
    }

#ifdef ONLINEUPDATE
    virtual bool handleCommand(const String& cmd) { // ":dirty NODENAME eg: ':dirty r0012' "
        if (!option->onlineUpdate) {
            cout <<  "recieving dirty command while onlineupdate is disabled. nothing will happen" << endl;
        }
        cout <<  "got Command: " << cmd.substr(0,5) << endl;
        if (0==cmd.substr(0,5).compare("dirty"))
        {
            cout << "testing substring" << cmd.substr(6,cmd.size()-1);
            return pointcloud->flagNodeAsDirty(cmd.substr(6,cmd.size()));
        }
        return false;
    }
#endif

    virtual void initializeRenderer(Renderer* r);

    virtual void update(const UpdateContext& context)
    {
        // After a frame all render passes had a chance to update their
        // textures. reset the raster update flag.
       #ifdef  INTERACTION
        interaction->traceAllFractures();
       #endif
    }
    
    virtual void dispose()
    {
    	if(pointcloud)
            delete pointcloud;
        if(option)
            delete option;
        if (interaction)
            delete interaction;
    }

    void initPotree(const string& option_file)
    {
    	//init values here
    	//option = gigapoint::Utils::loadOption("opotree.json");
    	option = gigapoint::Utils::loadOption(option_file);
    	pointcloud = new gigapoint::PointCloud(option, SystemManager::instance()->isMaster());
    	pointcloud->initPointCloud();

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
#ifdef INTERACTION
        interaction = new gigapoint::Interaction(pointcloud);
#endif
    }

    void updateMaterial(const string& material)
    {  
        if(material.compare("rgb") == 0)
            option->material = MATERIAL_RGB;
        else if (material.compare("elevation") == 0)
            option->material = MATERIAL_ELEVATION;
        else
            return;
        pointcloud->setReloadShader(true);
    }

    void updateInteractionMode(const string& mode)
    {
        interaction->updateInteractionMode(mode);
    }

    void updateQuality(const string& quality)
    {
        if(quality.compare("square") == 0)
            option->quality = QUALITY_SQUARE;
        else if (quality.compare("circle") == 0)
            option->quality = QUALITY_CIRCLE;
        else
            return;
        pointcloud->setReloadShader(true);
    }

    void updateSizeType(const string& type)
    {
        if(type.compare("fixed") == 0)
            option->sizeType = SIZE_FIXED;
        else if (type.compare("adaptive") == 0)
            option->sizeType = SIZE_ADAPTIVE;
        else
            return;
        pointcloud->setReloadShader(true);
    }

    void updatePointScale(const float val)
    {
        option->pointScale[0] = val;
        pointcloud->setReloadShader(false);
    }

    void updateVisible(const bool b)
    {
	   visible = b;
    }

    void updateFilter(const string& filter)
    {
        if(filter.compare("none") == 0)
            option->filter = FILTER_NONE;
        else if (filter.compare("edl") == 0)
            option->filter = FILTER_EDL;
        pointcloud->setReloadShader(true);
    }

    void updateEdl(const float strength, const float radius) 
    {
        option->filterEdl[0] = strength;
        option->filterEdl[1] = radius;
        pointcloud->setReloadShader(false);
    }

    void printInfo()
    {
	if(pointcloud)
		pointcloud->setPrintInfo(true);
    }


    gigapoint::PointCloud* pointcloud;
    gigapoint::Interaction* interaction;
    gigapoint::Option* option; 
    bool visible;

#if defined INTERACTION
    void traceFracture(int playerid){interaction->traceFracture(playerid);}

    void test(int playerid)
    {
        interaction->test(playerid);
        printInfo();
    }
    void setDrawTrace(bool b) {interaction->setDrawTrace(b);}
    void pickPointFromRay(const omega::Vector3f &origin,const omega::Vector3f &direction,int playerid)
        { interaction->pickPointFromRay(origin,direction,playerid);}
    void useSelectedPointAsTracePoint() {interaction->useSelectedPointAsTracePoint();}
    void resetTracer(int playerid) {interaction->resetTracer(playerid);}
    void next() {interaction->next();}

#endif

}; //class GigapointRenderModule

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
    	if(context.task == DrawContext::SceneDrawTask)
        {
            glPushAttrib(GL_TEXTURE_BIT | GL_ENABLE_BIT | GL_CURRENT_BIT);
            client->getRenderer()->beginDraw3D(context);

	    if(module->visible)
	    { 
			// Test and draw
			// get camera location in world coordinate
            	//if(context.eye == DrawContext::EyeLeft || context.eye == DrawContext::EyeCyclop)
            	{
                	Vector3f cp = context.camera->getPosition();
                	float campos[3] = {cp[0], cp[1], cp[2]};
                	float* MVP = (context.projection*context.modelview).cast<float>().data();
                    module->pointcloud->updateVisibility(MVP, campos, context.viewport.width(), context.viewport.height());
            	}

        module->pointcloud->draw();
#ifdef INTERACTION
        module->interaction->draw();
#endif

		if(oglError) return;
	    }
            client->getRenderer()->endDraw();
            glPopAttrib();
        }
        
    }

private:
    GigapointRenderModule* module;

}; //class GigapointRenderPass

///////////////////////////////////////////////////////////////////////////////
void GigapointRenderModule::initializeRenderer(Renderer* r)
{
    r->addRenderPass(new GigapointRenderPass(r, this));
}

///////////////////////////////////////////////////////////////////////////////
GigapointRenderModule* initialize()
{

    string RED="\033[0;31m";
    string NOCOLOR="\033[0m";
    string INDENT1="\t";
    cout << RED
         << "Andreas TODO List" << endl
         << " 3 controllers and tracers \n"                  
         << INDENT1 << "3 colors for drawing traces and interaction \n"
         << INDENT1 << " implement fixed nodelevel in memory and for search to keep all nodes euqal\n"
         << "onlineupdate demo using a lasfile ordered by scantime \n"
         << NOCOLOR << endl;

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
        PYAPI_METHOD(GigapointRenderModule, updateMaterial)
        PYAPI_METHOD(GigapointRenderModule, updateQuality)
        PYAPI_METHOD(GigapointRenderModule, updateSizeType)
        PYAPI_METHOD(GigapointRenderModule, updatePointScale)
        PYAPI_METHOD(GigapointRenderModule, updateVisible)
        PYAPI_METHOD(GigapointRenderModule, printInfo)        
        PYAPI_METHOD(GigapointRenderModule, updateFilter)
        PYAPI_METHOD(GigapointRenderModule, updateEdl)

        #if defined ONLINEUPDATE
        PYAPI_METHOD(GigapointRenderModule, handleCommand)
        #endif


#if defined INTERACTION
        PYAPI_METHOD(GigapointRenderModule, updateInteractionMode)
        PYAPI_METHOD(GigapointRenderModule, traceFracture)
        PYAPI_METHOD(GigapointRenderModule, test)
        PYAPI_METHOD(GigapointRenderModule, setDrawTrace)
        PYAPI_METHOD(GigapointRenderModule, pickPointFromRay)
        PYAPI_METHOD(GigapointRenderModule, useSelectedPointAsTracePoint)
        PYAPI_METHOD(GigapointRenderModule, resetTracer)
        //PYAPI_METHOD(GigapointRenderModule, next)
    ;


#endif
    ;

    def("initialize", initialize, PYAPI_RETURN_REF);


}
