
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <cmath>


const GLint WIDTH = 800, HEIGHT = 600; // window size 800*600
GLuint VAO, VBO, shader;//GL unisigned integer

GLuint uniformXMove;
bool direction = true;
float triOffset = 0.0f;
float triMaxoffset = 0.7f;
float triIncrement = 0.005f;




//Create vertex shader
static const char* vShader = "										\n\
#version 330														\n\
																	\n\
layout(location = 0) in vec3 pos;									\n\
uniform float xMove;												\n\
																	\n\
void main(){														\n\
	gl_Position = vec4(0.4*pos.x + xMove, 0.4*pos.y, pos.z, 1.0);	\n\
}";
//Create fragment Shader
static const char* fShader = "										\n\
#version 330														\n\
																	\n\
out vec4 colour;													\n\
																	\n\
void main(){														\n\
	colour = vec4(1.0, 0.0, 0.0, 1.0);								\n\
}";



//Vertex Specification: Create VAO, VBO
void createTriangle() {

	GLfloat vertices[] = {
		-1.0f, -1.0f, 0.0f, 
		1.0f, -1.0f, 0.0f, 
		0.0f, 1.0f, 0.0f
	};
	glGenVertexArrays(1, &VAO); //generate a vao id
	glBindVertexArray(VAO); //bind the vao with its id
	glGenBuffers(1, &VBO); //generate a vbo id
	glBindBuffer(GL_ARRAY_BUFFER, VBO); //bind vbo with its id, attached to the chosen VAO

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); //connect data and buffer
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0); //define the attribute pointer formatting, location = 0, size of one value = 3 
	glEnableVertexAttribArray(0); //enable the attibute pointer, location = 0
	//unbind VAO and VBO
	glBindBuffer(GL_ARRAY_BUFFER, 0); //0 means bind with nothing
	glBindVertexArray(0); //0 means bind with nothing
}
void addShader(GLuint theProgram, const char* shaderCode, GLenum shaderType) {
	GLuint theShader = glCreateShader(shaderType);// create an empty shader

	const GLchar* theCode[1];
	theCode[0] = shaderCode;

	GLint codeLength[1];
	codeLength[0] = strlen(shaderCode);

	glShaderSource(theShader, 1, theCode, codeLength); //get the code
	glCompileShader(theShader); //compile the code
	//another error check
	GLint result = 0;//result of the two functions
	GLchar eLog[1024] = { 0 };//a place to log the error
	glGetShaderiv(theShader, GL_COMPILE_STATUS, &result);//make sure it is LINDED
	if (!result) {
		glGetShaderInfoLog(theShader, 1024, NULL, eLog);
		printf("Error compiling the %d shader: '%s'\n", shaderType, eLog);
		return;
	}
	//attach the shader
	glAttachShader(theProgram, theShader);

}
void compileShaders() {
	shader = glCreateProgram();
	if (!shader) { // if nothing is returned from the shader string
		printf("Error Creating shader programming\n");
		return;
	}
	addShader(shader, vShader, GL_VERTEX_SHADER);
	addShader(shader, fShader, GL_FRAGMENT_SHADER);

	GLint result = 0;//result of the two functions
	GLchar eLog[1024] = { 0 };//a place to log the error
	//LINK
	glLinkProgram(shader);
	glGetProgramiv(shader, GL_LINK_STATUS, &result);//make sure it is LINDED
	if (!result) {
		glGetProgramInfoLog(shader, sizeof(eLog), NULL, eLog);
		printf("Error linking programm: '%s'\n", eLog);
		return;
	}
	//VALIDATE
	glValidateProgram(shader);
	glGetProgramiv(shader, GL_VALIDATE_STATUS, &result);//make sure it is VAILDATE
	if (!result) {
		glGetProgramInfoLog(shader, sizeof(eLog), NULL, eLog);
		printf("Error validating programm: '%s'\n", eLog);
		return;
	}

	uniformXMove = glGetUniformLocation(shader, "xMove");

}


int main()
{
	// Initialize GLFW
	if (!glfwInit()) { //glfw failed
		printf("GLFW INITIALISATION FIALED");
		glfwTerminate();
		return 1; 
	}
	//GLEW window properties
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);//the major version is going to be 3;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);// the minor version is going to be 3;
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);//no backwards compatibility
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);//allow forwards compatibility
	GLFWwindow* mainWindow = glfwCreateWindow(WIDTH, HEIGHT, "Test window", NULL, NULL); //window name
	//Create window
	if (!mainWindow) {//glfw window failed
		printf("GLFW window creation failed");
		glfwTerminate();
		return 1;
	}
	//Window size
	int bufferWidth, bufferHeight;// get buffer size information
	glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);//put them into &bufferWidth and &bufferHeight
	glfwMakeContextCurrent(mainWindow);//everything will be draw in this window(current window)
	glewExperimental = GLU_TRUE;//allow modern extention features
	

	// Initialize GLEW
	if (glewInit() != GLEW_OK) { //GLEW failed
		printf("GLFW INITIALISATION FAILED");
		glfwDestroyWindow(mainWindow);
		glfwTerminate();
		return 1;
	}
	//Viewport size
	glViewport(0, 0, bufferWidth, bufferHeight);

	//Create Triangle and compile the code
	createTriangle();
	compileShaders();


	//Loop until window closed
	while (!glfwWindowShouldClose(mainWindow)) {

		//get and handle user input events
		glfwPollEvents(); //check all kinds of inputs: keyboard, mouse, resize the window...

		if (direction) {
			triOffset += triIncrement; //if we hit the right direction, we will add increment to the triOffset 偏离量
		}
		else {
			triOffset -= triIncrement; //if we hit the left direction, we will minus incremant from the triOffset
		}

		if (abs(triOffset) >= triMaxoffset) { 
			direction = !direction; //if the trioffset hit the max offset, we need to change the direction
		}
		

		//clear the window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f); //0%red, 0green, 0blue, fully opaque (black)
		glClear(GL_COLOR_BUFFER_BIT);


		glUseProgram(shader);//start using shader

		glUniform1f(uniformXMove, triOffset); //assign a value to the shader
		 
		glBindVertexArray(VAO);

		glDrawArrays(GL_TRIANGLES, 0, 3); //there are 3 points in a triangle

		glBindVertexArray(0);

		glUseProgram(0);//stop using shader

		//swap buffer
		glfwSwapBuffers(mainWindow);
	}




	return 0;

}

