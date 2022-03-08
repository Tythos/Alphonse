/**
 * Single-file "Alphonse" variation of beginner game engine architectures.
 * 
 * @author <code@tythos.net>
 */

#include <string>
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <map>
#include <iostream>
#include <SDL.h>
#include <gl\glew.h>
#include <gl\glu.h>
#include <SDL_opengl.h>

 /**
  * Defines the basic vertex structure at a byte level
  *
  * @property {float[3]} position - Triplet of x/y/z coordinates
  * @property {float[3]} color - Triplet of r/g/b color values
 */
struct BasicVertex {
	float position[3];
	float color[3];

	/**
	 * Defines a constructor to streamline basic vertex array data instantiation
	 *
	 * @param {float} x - First axis coordinate of vertex
	 * @param {float} y - Second axis coordinate of vertex
	 * @param {float} z - Third axis coordinate of vertex
	 * @param {float} r - Red value [0=black, 1=red] at vertex
	 * @param {float} g - Green value [0=black, 1=green] at vertex
	 * @param {float} b - Blue value [0=black, 1=blue] at vertex
	 */
	BasicVertex(float x, float y, float z, float r, float g, float b) {
		position[0] = x;
		position[1] = y;
		position[2] = z;
		color[0] = r;
		color[1] = g;
		color[2] = b;
	}
};

/**
 * Global state
 */
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
SDL_Window* WINDOW = NULL;
SDL_GLContext CONTEXT;
bool IS_POLYGON_RENDERED = true;
std::map<std::string, GLuint> PROGRAMS;
std::map<std::string, GLint> ATTRIBS;
std::map<std::string, GLuint> BUFFERS;
int NUM_VERTICES = 3;
int NUM_ATTRIBUTES = sizeof(BasicVertex) / sizeof(float);

/**
 * Helper method for reading a std::string from file contents
 * 
 * @param {std::string} filePath - Indicates the path of the resource to read
 * @returns {std::string} - Contents of file indicated by input
 */
std::string readFile(std::string filePath) {
	std::ifstream f;
	std::ostringstream sstr;
	f.open(filePath);
	sstr << f.rdbuf();
	return sstr.str();
}

/**
 * Retrieves and prints log contents for the given GL program.
 *
 * @param {GLuint} program - Defines a unique GL program instance
 */
void printProgramLog(GLuint program) {
	if (glIsProgram(program)) {
		int bufferLength = 0;
		int infoLogLength = bufferLength;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);
		char* infoLog = new char[infoLogLength];
		glGetProgramInfoLog(program, infoLogLength, &bufferLength, infoLog);
		if (bufferLength > 0) {
			std::cout << infoLog << std::endl;
		}
		delete[] infoLog;
	}
	else {
		throw std::runtime_error("Invalid GL program ID");
	}
}

/**
 * Retrieves and prints log contents for the given GL shader.
 *
 * @param {GLuint} shader - Identifies a unique GL shader instance
 */
void printShaderLog(GLuint shader) {
	if (glIsShader(shader)) {
		int bufferLength = 0;
		int infoLogLength = bufferLength;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);
		char* infoLog = new char[infoLogLength];
		glGetShaderInfoLog(shader, infoLogLength, &bufferLength, infoLog);
		if (bufferLength > 0) {
			std::cout << infoLog << std::endl;
		}
		delete[] infoLog;
	}
	else {
		throw std::runtime_error("Invalid GL shader ID");
	}
}

/**
 * Initializes fundamental application states, including window/context and
 * extension support.
 */
void startApp() {
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		throw std::runtime_error("SDL initialization failed");
	}
	WINDOW = SDL_CreateWindow("Artemis - Alphonse", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
	if (WINDOW == NULL) {
		throw std::runtime_error("Window initialization failed");
	}
	CONTEXT = SDL_GL_CreateContext(WINDOW);
	if (CONTEXT == NULL) {
		throw std::runtime_error("Context initialization failed");
	}
	glewExperimental = GL_TRUE;
	GLenum glewError = glewInit();
	if (glewError != GLEW_OK) {
		throw std::runtime_error("GLEW initialization failed");
	}
	SDL_StartTextInput();
}

/**
 * Initializes GL data, including shader program (compiled/linked from source
 * files); buffer contents; and variable bindings.
 */
void startGraphics() {
	// define graphics program
	PROGRAMS["basic"] = glCreateProgram();

	// load and compile vertex shader
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	std::string vertexString = readFile("../basic.v.glsl");
	const char* vertexRaw = vertexString.c_str();
	glShaderSource(vertexShader, 1, &vertexRaw, NULL);
	glCompileShader(vertexShader);
	GLint vsCompileStatus = GL_FALSE;
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &vsCompileStatus);
	if (vsCompileStatus != GL_TRUE) {
		printShaderLog(vertexShader);
		throw std::runtime_error("Vertex shader failed to compile");
	}
	glAttachShader(PROGRAMS["basic"], vertexShader);

	// load and compile fragment shader
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	std::string fragmentString = readFile("../basic.f.glsl");
	const char* fragmentRaw = fragmentString.c_str();
	glShaderSource(fragmentShader, 1, &fragmentRaw, NULL);
	glCompileShader(fragmentShader);
	GLint fsCompileStatus = GL_FALSE;
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &fsCompileStatus);
	if (fsCompileStatus != GL_TRUE) {
		printShaderLog(fragmentShader);
		throw std::runtime_error("Fragment shader failed to compile");
	}
	glAttachShader(PROGRAMS["basic"], fragmentShader);

	// link shader program and bind program variables
	glLinkProgram(PROGRAMS["basic"]);
	GLint programSuccess = GL_TRUE;
	glGetProgramiv(PROGRAMS["basic"], GL_LINK_STATUS, &programSuccess);
	if (programSuccess != GL_TRUE) {
		printProgramLog(PROGRAMS["basic"]);
		throw std::runtime_error("Shader program failed to link");
	}
	ATTRIBS["position"] = glGetAttribLocation(PROGRAMS["basic"], "position");
	ATTRIBS["color"] = glGetAttribLocation(PROGRAMS["basic"], "color");
	if (ATTRIBS["position"] < 0 || ATTRIBS["color"] < 0) {
		throw std::runtime_error("Shader program failed to expose attributes");
	}

	// populate buffer objects
	BUFFERS["vertex"] = 0;
	BUFFERS["index"] = 0;
	glClearColor(0.f, 0.f, 0.f, 1.f);
	BasicVertex vertexData[] = {
		BasicVertex(-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f),
		BasicVertex(0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f),
		BasicVertex(0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f)
	};
	GLuint indexData[] = {0, 1, 2};
	glGenBuffers(1, &BUFFERS["vertex"]);
	glBindBuffer(GL_ARRAY_BUFFER, BUFFERS["vertex"]);
	glBufferData(GL_ARRAY_BUFFER, NUM_ATTRIBUTES * NUM_VERTICES * sizeof(GLfloat), vertexData, GL_STATIC_DRAW);
	glGenBuffers(1, &BUFFERS["index"]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BUFFERS["index"]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, NUM_VERTICES * sizeof(GLuint), indexData, GL_STATIC_DRAW);

	// now we can bind specific attributes from the vertex array/format and program variables
	glVertexAttribPointer(ATTRIBS["position"], 3, GL_FLOAT, GL_FALSE, sizeof(BasicVertex), NULL);
	glVertexAttribPointer(ATTRIBS["color"], 3, GL_FLOAT, GL_FALSE, sizeof(BasicVertex), (void*)sizeof(float[3]));
}

/**
 * Invoked when SDL text input produces a keypress event.
 * 
 * @param {unsigned char} key - Indicates what key was pressed
 * @returns {bool} - Defaults to true, but will return false if a keypress results in a program interrupt
 */
bool onTextInput(unsigned char key) {
	bool isRunning = true;
	if (key == ' ') {
		IS_POLYGON_RENDERED = !IS_POLYGON_RENDERED;
	}
	if (key == 'q') {
		isRunning = false;
	}
	return isRunning;
}

/**
 * Invoked once a frame to update the frame buffer. In addition to clearing
 * the color buffer, draws the polygon if the appropriate flag is true.
 */
void render() {
	glClear(GL_COLOR_BUFFER_BIT);
	if (IS_POLYGON_RENDERED) {
		glUseProgram(PROGRAMS["basic"]);
		glEnableVertexAttribArray(ATTRIBS["position"]);
		glEnableVertexAttribArray(ATTRIBS["color"]);
		glBindBuffer(GL_ARRAY_BUFFER, BUFFERS["vertex"]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BUFFERS["index"]);
		glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, NULL);
		glDisableVertexAttribArray(ATTRIBS["position"]);
		glDisableVertexAttribArray(ATTRIBS["color"]);
		glUseProgram(NULL);
	}
	SDL_GL_SwapWindow(WINDOW);
}

/**
 * Destroys app-level (e.g., non-GL state) variables and other behaviors.
 */
void stopApp() {
	SDL_StopTextInput();
	SDL_DestroyWindow(WINDOW);
	WINDOW = NULL;
	SDL_Quit();
}

/**
 * Frees up GL state (in this case, just programs).
 */
void stopGraphics() {
	glDeleteProgram(PROGRAMS["basic"]);
}

/**
 * Defines a single iteration of the main application/game loop.
 * 
 * @returns {bool} Indicates if the loop should continue running.
 */
bool mainLoop() {
	bool isRunning = true;
	SDL_Event event;
	while (SDL_PollEvent(&event) != 0) {
		if (event.type == SDL_QUIT) {
			isRunning = false;
		}
		else if (event.type == SDL_TEXTINPUT) {
			isRunning = onTextInput(event.text.text[0]);
		}
	}
	render();
	return isRunning;
}

/**
 * Primary entry point. After initialization, this largely consists of calling
 * the main loop so long as it returns true. Once it returns false, resources
 * are cleaned up and the entry point returns.
 * 
 * @param {int} nArgs - Not used
 * @param {char**} vArgs - Not used
 */
int main(int nArgs, char** vArgs) {
	startApp(); {
		startGraphics(); {
			bool isRunning = true;
			while (isRunning) {
				isRunning = mainLoop();
			}
		} stopGraphics();
	} stopApp();
	return 0;
}
