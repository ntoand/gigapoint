#include "GLInclude.h"
#include "GLUtils.h"
#include "../PointCloud.h"
#include "Camera.h"

#include <sstream>
#include <iostream>
#include <vector>
using std::stringstream;
using std::string;
using std::cout;
using std::endl;

#include <glm/gtc/type_ptr.hpp>


#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION
#define NK_GLFW_GL2_IMPLEMENTATION
#include "nuklear.h"
#include "nuklear_glfw_gl2.h"

#define UNUSED(a) (void)a
#define LEN(a) (sizeof(a)/sizeof(a)[0])


using namespace gigapoint;

GLFWwindow* window;
string title;

//timing related variables
float last_time=0, current_time =0;
//delta time
float dt = 0;

#define WIDTH 1024
#define HEIGHT 768

int frame_width, frame_height;

Option* option = NULL;
PointCloud* pointcloud = NULL;
Camera* camera = NULL;

//camera / mouse
bool keys[1024];
GLfloat lastX = 400, lastY = 300;
bool firstmouse = true;
bool usemouse = false;

void doMovement();

// GUI
struct nk_context *ctx;


static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    //cout << key << endl;
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
    if (key >= 0 && key < 1024)
    {
        if(action == GLFW_PRESS)
            keys[key] = true;
        else if(action == GLFW_RELEASE)
            keys[key] = false;
    }
}

static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    //cout << "button: " << button << " action: " << action << endl;
    if (button == 0 && action == 1)
        usemouse = true;
    
    else if (button == 0 && action == 0)
        usemouse = false;
}


static void mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    //camera->processMouseScroll(yoffset);
}

static void mouse_position_callback(GLFWwindow* window, double xpos, double ypos) {
    
    if(firstmouse) {
        camera->move_camera = false;
        camera->Move2D((int)xpos, (int)ypos);
        firstmouse = false;
        return;
    }
    
    camera->move_camera = usemouse;
    camera->Move2D((int)xpos, (int)ypos);
}

static void window_size_callback(GLFWwindow* window, int width, int height) {
	
}

void init_resources(string configfile, bool zup = false)
{
    option = Utils::loadOption(configfile);
    Utils::printOption(option);
    
    camera = new Camera();
    if(option->cameraUpdatePosOri) {
        camera->SetPosition(glm::vec3(option->cameraPosition[0], option->cameraPosition[1], option->cameraPosition[2]));
        camera->SetLookAt(glm::vec3(option->cameraTarget[0], option->cameraTarget[1], option->cameraTarget[2]));
    }
    else {
        camera->SetPosition(glm::vec3(0, 0, 2));
        camera->SetLookAt(glm::vec3(0, 0, 0));
    }
    
    if(zup)
        camera->camera_up = glm::vec3(0, 0, 1);
    camera->SetViewport(0, 0, WIDTH, HEIGHT);
    camera->SetClipping(1, 1000000);
    camera->camera_scale = option->cameraSpeed;
    camera->Update();
    
    pointcloud = new PointCloud(option);
    pointcloud->initPointCloud();
    //pointcloud->setPrintInfo(true);
}

void free_resources()
{
    if(option)
        delete option;
    if(pointcloud)
        delete pointcloud;
    if(camera)
        delete camera;
}

void doMovement() {
    // Camera controls
    if(keys[GLFW_KEY_W])
        camera->Move(FORWARD);
    if(keys[GLFW_KEY_S])
        camera->Move(BACK);
    if(keys[GLFW_KEY_A])
        camera->Move(LEFT);
    if(keys[GLFW_KEY_D])
        camera->Move(RIGHT);
    if(keys[GLFW_KEY_Q])
        camera->Move(DOWN);
    if(keys[GLFW_KEY_E])
        camera->Move(UP);
   
    if(keys[GLFW_KEY_I]) {
        cout << "pos: " << camera->camera_position[0] << ", " << camera->camera_position[1] << ", " << camera->camera_position[2] << endl;
        cout << "direction: " << camera->camera_direction[0] << ", " << camera->camera_direction[1] << ", " << camera->camera_direction[2] << endl;
        cout << "lookat: " << camera->camera_look_at[0] << ", " << camera->camera_look_at[1] << ", " << camera->camera_look_at[2] << endl;
        cout << "up: " << camera->camera_up[0] << ", " << camera->camera_up[1] << ", " << camera->camera_up[2] << endl;
        pointcloud->setPrintInfo(true);
        keys[GLFW_KEY_I] = false;
    }
    if(keys[GLFW_KEY_T]) {
        keys[GLFW_KEY_T] = false;
    }
    if(keys[GLFW_KEY_N]) {
        keys[GLFW_KEY_N] = false;
    }
    camera->Update();
}

void mainLoop()
{
    const int samples = 50;
    float time[samples];
    int index = 0;

    do{
        //timing related calcualtion
        last_time = current_time;
        current_time = glfwGetTime();
        dt = current_time-last_time;
        
        glfwPollEvents();
        
        // GUI
        nk_glfw3_new_frame();
        if (nk_begin(ctx, "Settings", nk_rect(10, 10, 230, 150),
                     //NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_SCALABLE|
                     NK_WINDOW_MINIMIZABLE|NK_WINDOW_TITLE))
        {
            static int colormode = option->material;
            nk_layout_row_dynamic(ctx, 25, 2);
            //nk_label(ctx, "color mode: ", NK_TEXT_LEFT);
            if (nk_option_label(ctx, "rgb", colormode == MATERIAL_RGB)) colormode = MATERIAL_RGB;
            if (nk_option_label(ctx, "elevation", colormode == MATERIAL_ELEVATION)) colormode = MATERIAL_ELEVATION;
            
            static float pointscale = option->pointScale[0];
            nk_layout_row_dynamic(ctx, 25, 1);
            nk_property_float(ctx, "Pointscale:", option->pointScale[1], &pointscale, option->pointScale[2], 0.02, 1);
            
            static int quality = option->quality;
            nk_layout_row_dynamic(ctx, 15, 3);
            if (nk_option_label(ctx, "square", quality == QUALITY_SQUARE)) quality = QUALITY_SQUARE;
            if (nk_option_label(ctx, "circle", quality == QUALITY_CIRCLE)) quality = QUALITY_CIRCLE;
            if (nk_option_label(ctx, "sphere", quality == QUALITY_SPHERE)) quality = QUALITY_SPHERE;
            
            static int filter = option->filter;
            nk_layout_row_dynamic(ctx, 25, 1);
            nk_checkbox_label(ctx, "EDL", &filter);
            
            // update pointcloud
            if(option->material != colormode) {
                option->material = colormode;
                pointcloud->setReloadShader(true);
            }
            option->pointScale[0] = pointscale;
            if(option->quality != quality) {
                option->quality = quality;
                pointcloud->setReloadShader(true);
            }
            if(option->filter != filter) {
                option->filter = filter;
                pointcloud->setReloadShader(true);
            }
            
        }
        nk_end(ctx);
        
        // end GUI
        
        doMovement();
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        //graphics
        glEnable(GL_POINT_SPRITE);
        glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
        glEnable(GL_DEPTH_TEST);
       
        // Dark blue background
        //glClearColor(1.0f,0.5f,0.5f,1.0f);
    
        float* MVP, *MV;
        MVP = (float*)glm::value_ptr(camera->MVP);
        MV = (float*)glm::value_ptr(camera->MV);
        
        float campos[3] = {camera->camera_position[0], camera->camera_position[1], camera->camera_position[2]};
        
        pointcloud->updateVisibility(MVP, campos, frame_width, frame_height);
        pointcloud->draw(MV, MVP);
        
        
        // draw GUI
        /* IMPORTANT: `nk_glfw_render` modifies some global OpenGL state
         * with blending, scissor, face culling and depth test and defaults everything
         * back into a default state. Make sure to either save and restore or
         * reset your own state after drawing rendering the UI. */
        nk_glfw3_render(NK_ANTI_ALIASING_ON);
       

        glfwSwapBuffers(window);

        // Update FPS
        time[index] = glfwGetTime();
        index = (index + 1) % samples;

        if( index == 0 ) {
          float sum = 0.0f;
          for( int i = 0; i < samples-1 ; i++ )
            sum += time[i + 1] - time[i];
          float fps = samples / sum;

          stringstream strm;
          strm << title;
          strm.precision(4);
          strm << " (fps: " << fps << ")";
          glfwSetWindowTitle(window, strm.str().c_str());
        }

    } // Check if the ESC key was pressed or the window was closed
    while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
           glfwWindowShouldClose(window) == 0 );
}

int main(int argc, char* argv[]) {
    
    string configfile = "gigapoint_resource/config/gigapoint_century_local.json";
    //string configfile = "gigapoint_resource/config/gigapoint_WP_07_local.json";
    //string configfile = "gigapoint_resource/config/gigapoint_HoyoVerde_local.json";
    bool zup = true;
    
    if(argc == 2)
        configfile = string(argv[1]);

	// Initialise GLFW
	if( !glfwInit() )
	{
		fprintf( stderr, "Failed to initialize GLFW\n" );
		return -1;
	}

    glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
    
	// Open a window and create its OpenGL context
	window = glfwCreateWindow( WIDTH, HEIGHT, "OpenGL window with GLFW", NULL, NULL);
	if( window == NULL ){
		fprintf( stderr, "Failed to open GLFW window.\n" );
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, key_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetCursorPosCallback(window, mouse_position_callback);
    glfwSetScrollCallback(window, mouse_scroll_callback);
	glfwSetWindowSizeCallback(window, window_size_callback);

	// Initialize GLEW
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// print GL info
	GLUtils::dumpGLInfo();

	// init resources
    glfwGetFramebufferSize(window, &frame_width, &frame_height);
	init_resources(configfile, zup);
    
    /* GUI */
    ctx = nk_glfw3_init(window, NK_GLFW3_INSTALL_CALLBACKS);
    /* Load Fonts: if none of these are loaded a default font will be used  */
    /* Load Cursor: if you uncomment cursor loading please hide the cursor */
    {struct nk_font_atlas *atlas;
        nk_glfw3_font_stash_begin(&atlas);
        /*struct nk_font *droid = nk_font_atlas_add_from_file(atlas, "../../../extra_font/DroidSans.ttf", 14, 0);*/
        /*struct nk_font *roboto = nk_font_atlas_add_from_file(atlas, "../../../extra_font/Roboto-Regular.ttf", 14, 0);*/
        /*struct nk_font *future = nk_font_atlas_add_from_file(atlas, "../../../extra_font/kenvector_future_thin.ttf", 13, 0);*/
        /*struct nk_font *clean = nk_font_atlas_add_from_file(atlas, "../../../extra_font/ProggyClean.ttf", 12, 0);*/
        /*struct nk_font *tiny = nk_font_atlas_add_from_file(atlas, "../../../extra_font/ProggyTiny.ttf", 10, 0);*/
        /*struct nk_font *cousine = nk_font_atlas_add_from_file(atlas, "../../../extra_font/Cousine-Regular.ttf", 13, 0);*/
        nk_glfw3_font_stash_end();
        /*nk_style_load_all_cursors(ctx, atlas->cursors);*/
        /*nk_style_set_font(ctx, &droid->handle);*/
    }

	// Enter the main loop
	mainLoop();

	free_resources();
    
    //GUI
    nk_glfw3_shutdown();

	// Close window and terminate GLFW
	glfwTerminate();

	// Exit program
	return EXIT_SUCCESS;
}
