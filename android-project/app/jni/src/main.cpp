//
// MAIN FILE
//
#include "includes.h"

#ifdef _WIN32
constexpr unsigned int wndWidth{ 270 }, wndHeight{ 480 };
#else
constexpr unsigned int wndWidth{ 1080 }, wndHeight{ 1920 };
#endif

// An array of 3 vectors which represents 3 vertices
static const GLfloat g_vertex_buffer_data[] = {
	-1.0f, -1.0f, 0.0f,
	1.0f, -1.0f, 0.0f,
	0.0f,  1.0f, 0.0f,
};

/**
* Log an SDL error with some error message to the output stream of our choice
* @param os The output stream to write the message to
* @param msg The error message to write, format will be msg error: SDL_GetError()
*/
void logSDLError(std::ostream &os, const std::string &msg)
{
    os << msg << " error: " << SDL_GetError() << std::endl;
}

//
//
//
void logMessage(const std::string &msg)
{
#if __ANDROID__
    __android_log_write(ANDROID_LOG_INFO, "MOOSE", msg.c_str());
#endif
#ifdef _WIN32
    std::cout << "INFO: MOOSE: " << msg << std::endl;
#endif
}

//
//
//
class Game
{
private:
    const Uint8* pCurrentKeyStates = nullptr;
    SDL_Window *window = nullptr;
	SDL_GLContext glContext;
	GLuint VertexArrayID;
	GLuint vertexbuffer;
    GLuint programID;

#if __ANDROID__
    AAssetManager *pAssetManager = nullptr;
    jobject global_asset_manager;

    //
    // Gets the Android Asset Manager object, used for load asset files
    //
    void getAssetManager()
    {
        JNIEnv* env = (JNIEnv*)SDL_AndroidGetJNIEnv();
        jobject activity = (jobject)SDL_AndroidGetActivity();
        jclass activity_class = env->GetObjectClass(activity);
        jmethodID activity_class_getAssets = env->GetMethodID(activity_class, "getAssets", "()Landroid/content/res/AssetManager;");
        jobject asset_manager = env->CallObjectMethod(activity, activity_class_getAssets); // activity.getAssets();
        global_asset_manager = env->NewGlobalRef(asset_manager);

        pAssetManager = AAssetManager_fromJava(env, global_asset_manager);
        if (pAssetManager == nullptr)
        {
            logMessage("Failed getting AssetManager");
        }
    }
#endif

    //
	// init openGL stuff for SDL
	// return -1 on error
	//
	int initOpenGLAttrs()
	{
		// TODO - set this to enable GLES for mobile
		// Set our OpenGL version.
		// SDL_GL_CONTEXT_CORE gives us only the newer version, deprecated functions are disabled
		if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
#ifdef USE_GLES
								SDL_GL_CONTEXT_PROFILE_ES
#else
								SDL_GL_CONTEXT_PROFILE_CORE
#endif
		)
			!= 0)
		{
			return -1;
		}

		// Set up so that we use version 3.2 of OpenGL.
		// 3.2 is part of the modern versions of OpenGL, but most video cards would be able to run it
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

		// Turn on double buffering with a 24bit Z buffer.
		if (SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1) != 0)
		{
			return -1;
		}
		if (SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24) != 0)
		{
			return -1;	
		}

		return 0;	// ok
	}

public:
	Game()	{ }

	// returns -1 on error
	int init()
	{
		// init SDL
		if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
		{
			logSDLError(std::cout, "SDL_Init");
			return -1;
		}

		if (initOpenGLAttrs() < 0)
		{
			logSDLError(std::cout, "initOpenGLAttrs");
			return -1;
		}

		// Create window
		window = SDL_CreateWindow("SDL2_OPENGL", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                  wndWidth, wndHeight, SDL_WINDOW_OPENGL
#if __ANDROID__
									| SDL_WINDOW_FULLSCREEN
#endif
		);
		if (window == nullptr)
		{
			logSDLError(std::cout, "CreateWindow");
			SDL_Quit();
			return -1;
		}

		glContext = SDL_GL_CreateContext(window);
		if (glContext == nullptr)
		{
			return -1;
		}

#ifdef USE_GLEW
		// This tells OpenGL that we want to use OpenGL 3.0 stuff and later.
		//glewExperimental = GL_TRUE;
		glewInit();
#endif

        // This makes our buffer swap syncronized with the monitor's vertical refresh
        SDL_GL_SetSwapInterval(1);

        // create a Vertex Array Object and set it as the current one :
		glGenVertexArrays(1, &VertexArrayID);
		glBindVertexArray(VertexArrayID);

        // Create and compile our GLSL program from the shaders
#ifdef _WIN32
        programID = LoadShaders( "assets/simple_vertex_shader.vs", "assets/simple_fragment_shader.fs" );
#endif
#if __ANDROID__
        getAssetManager();
        programID = LoadShadersAndroid(pAssetManager, "simple_vertex_shader.vs", "simple_fragment_shader.fs" );
#endif
        if (programID == 0)
        {
            logMessage("Failed loading shaders");
            return -1;
        }
		//
		// create triangle buffer
		//
		// Generate 1 buffer, put the resulting identifier in vertexbuffer
		glGenBuffers(1, &vertexbuffer);
		// The following commands will talk about our 'vertexbuffer' buffer
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		// Give our vertices to OpenGL.
		glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

		return 0;	// ok
	}

	void run()
	{
		SDL_Event e;
		bool quit = false;
		while (!quit)
		{
			while (SDL_PollEvent(&e))
			{
				//If user closes the window, presses escape
				if (e.type == SDL_QUIT ||
					(e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE))
				{
					quit = true;
				}
			}

			// key state
			pCurrentKeyStates = SDL_GetKeyboardState(NULL);
			if (pCurrentKeyStates[SDL_SCANCODE_ESCAPE])
				break;

            // Clear the screen
            glClear( GL_COLOR_BUFFER_BIT );

            // Use our shader
            glUseProgram(programID);

            // 1rst attribute buffer : vertices
            glEnableVertexAttribArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
            glVertexAttribPointer(
                    0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
                    3,                  // size
                    GL_FLOAT,           // type
                    GL_FALSE,           // normalized?
                    0,                  // stride
                    (void*)0            // array buffer offset
            );

            // Draw the triangle !
            glDrawArrays(GL_TRIANGLES, 0, 3); // 3 indices starting at 0 -> 1 triangle

            glDisableVertexAttribArray(0);

			// Update window with OpenGL rendering
			SDL_GL_SwapWindow(window);
		}

		// cleanup

        // Cleanup VBO
        glDeleteBuffers(1, &vertexbuffer);
        glDeleteVertexArrays(1, &VertexArrayID);
        glDeleteProgram(programID);

		SDL_GL_DeleteContext(glContext);
		SDL_DestroyWindow(window);
		SDL_Quit();
	}

};

int main(int, char **)
{
	Game game;
	if (game.init() < 0)
	{
		return -1;
	}
	game.run();
	return 0;
}

