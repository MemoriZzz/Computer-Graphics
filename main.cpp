#include <iostream>
#define GLFW_STATIC
#include <GL\glew.h>
#include <GLFW\glfw3.h>



float vertices[] = {
	-0.5f, -0.5f, 0.0f, //0
	0.5f, -0.5f, 0.0f,  //1
	0.0f, 0.5f, 0.0f,   //2
	//0.5f, -0.5f, 0.0f,  //1
	//0.0f, 0.5f, 0.0f,   //2
	0.8f, 0.8f, 0.0f    //3
};
//节省数据：EBO element buffer object
//0,1,2三个点构成了第一个三角形; 1,2,3三个点构成了第二个三角形
unsigned int indices[] = {
	0,1,2,
	2,1,3
};


//把VOA中的东西倒进vertex shader(location=0指的是从VOA中的index/attribute=0的位置开始往里倒)
//在Notepad++中按住ALT可以选出一列然后输入\n
//复制到VS中内容会紊乱，按CONTROL Z即恢复
//vertex shader (GLSL源代码)
const char* vertexShaderSource = "						  \n\
#version 330 core                                         \n\
layout (location = 0) in vec3 aPos;                       \n\
void main(){                                              \n\
    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);}     \n\
";
//fragment shader (GLSL源代码)
const char* fragmentShaderSource = "				\n\
#version 330 core                                   \n\
out vec4 FragColor;                                 \n\
void main(){                                        \n\
    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);}      \n\
";




//自己写一个获取用户动作的方法
void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE)) {//是不是获取到了ESC
		glfwSetWindowShouldClose(window, true);
	}
}


int main() //返回0是正常退出，返回-1是非正常推出
{	
	// 初始化GLFW
	glfwInit();
	// 使用3.3版本的glfw
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);//使用此profile

	//造窗子-给大小和名字，返回一个指针。
	GLFWwindow* window = glfwCreateWindow(800, 600, "My OpenGL Game", NULL, NULL);

	if (window == NULL) {//空指针，造窗失败
		printf("open window failed");
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);//造窗成功，那么主要上下文就用这个window

	// 初始化GLEW
	glewExperimental = true;
	if (glewInit() != GLEW_OK) {//GLEW失败，没有返回GLEW_OK
		printf("init glew failed");
		glfwTerminate();
		return -1;
	}

	//都成功，虾面就可以开窗口辣
	glViewport(0, 0, 800, 600); //起始坐标点是（0，0） ，可以绘制的像素宽高（800，600）

	//glEnable(GL_CULL_FACE); //开启面剔除功能
	//glCullFace(GL_FRONT);//使用面剔除方法剔除背面（逆时针画的叫正面-GL_FRONT；顺时针画出来的面叫背面-GL_BACK）

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);//只有框线

	//创造并且绑定VAO和VBO
	unsigned int VAO;
	glGenVertexArrays(1, &VAO);//用这个方法产生（多个）VAO，然后把第一个的位置传过去。在此只造一个VAO。
	glBindVertexArray(VAO);//VAO绑定位置结束
	unsigned int VBO;
	glGenBuffers(1,&VBO);//用这个方法可以产生（多个）VBO，然后把第一个的位置传过去。在此只造一个VBO。
	glBindBuffer(GL_ARRAY_BUFFER, VBO);//VBO同VAO绑定
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); //把CPU的东西塞进GPU
	unsigned int EBO;
	glGenBuffers(1, &EBO);//用这个方法可以产生（多个）EBO，然后把第一个的位置传过去。在此只造一个EBO。
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);



	//利用OPENGL现场把上述源代码compile了
	unsigned int vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);//创造一个vertexshader
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);//1个字符串
	glCompileShader(vertexShader);
	//利用OPENGL现场把上述源代码compile了
	unsigned int fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);//创造一个fragmentshader
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);//1个字符串
	glCompileShader(fragmentShader);
	//把这两个shader组装成一个program
	unsigned int shaderProgram;
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	//从第几个开始绘制（0），三个数据是一个顶点/资料（3），每个数值的形式是什么（float），
	//要不要帮你aline到正负1之间（不用 GL_FALSE），每笔资料之间间隔的长度是多少（3个float），第一笔资料的偏移量是多少（(void*)0）
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0); 
	glEnableVertexAttribArray(0);//把第0号打开

	//我们不能让程序运行完成即结束，需要一直画一直收集用户动作，直到用户明确表示结束程序
	while (!glfwWindowShouldClose(window)) {//render loop 渲染循环 (没有要关窗)
		glfwSwapBuffers(window);//切换色彩缓存区，涉及到double buffer(front buffer&backbuffer)的问题，具体参阅网站

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);//red green blue, alpha 都是低档0满档1
		glClear(GL_COLOR_BUFFER_BIT);//只清掉COLOR BUFFER就可以了

		glBindVertexArray(VAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

		glUseProgram(shaderProgram);
		//glDrawArrays(GL_TRIANGLES, 0, 3); //从第0个点开始画，一共画3个点
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);//要画6个顶点，值的类型是Unsigned int，偏移量是0


		glfwPollEvents();//把用户动作获取出来
		processInput(window);//自己写一个获取用户动作的方法
		
	}

	glfwTerminate();
	return 0;
}

