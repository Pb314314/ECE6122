// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <iostream>
// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include <common/shader.hpp>
#include <common/texture.hpp>
#include <common/controls.hpp>
#include <common/objloader.hpp>
#include <common/vboindexer.hpp>
#include <cstdlib> // for std::rand and std::srand
#include <ctime>   // for std::time
#include <random>
#include <thread>
#include <cmath>

float objectSpeed = 2.0f;
float deltaTime = 0.016f;
float rotate_rate = 5;
float collisionDistance = 2.4f;

float generateRandomLight_power(float min, float max) {
    return min + static_cast<float>(rand()) / static_cast<float>(RAND_MAX / (max - min));
}

float random_light_change() {
    return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
}

float calculateDistance(const glm::vec3& position1, const glm::vec3& position2) {
    float dx = position1.x - position2.x;
    float dy = position1.y - position2.y;
    float dz = position1.z - position2.z;

    return std::sqrt(dx * dx + dy * dy + dz * dz);
}

void updateObjectDirection(glm::vec3& objectDirection) {
    objectDirection = -objectDirection;
	objectDirection *=1.01;
}
float generate_init_angle(unsigned int seed) {
    // Set up a random number generator with the provided seed
    std::mt19937 gen(seed);

    // Define the range for floating-point numbers
    std::uniform_real_distribution<float> distribution(0.0f, 360.0f);

    // Generate a random float
    return distribution(gen);
}

glm::vec3 generateRandomDirection(int seed) {
    // Seed the random number generator using the current time
    std::srand(static_cast<unsigned>(seed));

    // Generate random values for x, y, and z within the range [-50, 49]
    glm::vec3 objectDirection = glm::normalize(glm::vec3(std::rand() % 100 - 50, std::rand() % 100 - 50, std::rand() % 100 - 50));

    return objectDirection;
}

float generateRandomFloat(float min, float max) {
    return min + static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX / (max - min));
}

// Function to move the object within the cube based on the random direction
glm::vec3 moveObject(glm::vec3& objectPosition,glm::vec3& objectDirection) {
    // Cube boundaries (adjust these according to your needs)
    glm::vec3 newPosition = objectPosition + objectSpeed * deltaTime * objectDirection;
	for (int i = 0; i < 3; ++i) {
			if (newPosition[i] < -4.5f || newPosition[i] > 5.0f) {
				objectDirection[i] = -objectDirection[i];
				newPosition[i] += objectSpeed * deltaTime * objectDirection[i];  // Move the object back inside the cube
			}
		}
    return newPosition;
}

glm::vec3 generateRandomAxis(unsigned int seed) {
    // Set up a random number generator with the provided seed
    std::mt19937 gen(seed);

    std::uniform_int_distribution<int> distribution(-1000, 1000);

    glm::vec3 axis;
    do {
        axis = glm::normalize(glm::vec3(distribution(gen), distribution(gen), distribution(gen)));
    } while (glm::length(axis) < 0.1f); // Ensure the axis is not too small

    return axis;
}

void updateModelMatrix(glm::mat4& ModelMatrix, int pressed_G, glm::vec3& objectPosition, glm::vec3& objectDirection, float& init_angle, glm::vec3 rotationAxis) {
    if (pressed_G) {
        objectPosition = moveObject(objectPosition, objectDirection);
    }

    float rotationAngle = generateRandomFloat(0.0f, 360.0f);
    ModelMatrix = glm::translate(glm::mat4(1.0), objectPosition);
    init_angle += rotate_rate;

    if (!pressed_G) {
        ModelMatrix = glm::translate(ModelMatrix, glm::vec3(1.8f, 0.0f, 0.99f));
        ModelMatrix = glm::rotate(ModelMatrix, glm::radians(90.0f), glm::vec3(0, 1, 0));
        ModelMatrix = glm::rotate(ModelMatrix, glm::radians(90.0f), glm::vec3(0, 0, 1));
    } else {
        ModelMatrix = glm::rotate(ModelMatrix, glm::radians(init_angle), rotationAxis);
    }
}


int main( void )
{
	// Initialise GLFW
	if( !glfwInit() )
	{
		fprintf( stderr, "Failed to initialize GLFW\n" );
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow( 1024, 768, "Tutorial 09 - Rendering several models", NULL, NULL);
	if( window == NULL ){
		fprintf( stderr, "Failed to open GLFW window.\n" );
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
    
	// Initialize GLEW
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    // Hide the mouse and enable unlimited mouvement
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    // Set the mouse at the center of the screen
    glfwPollEvents();
    glfwSetCursorPos(window, 1024/2, 768/2);

	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS); 

	// Cull triangles which normal is not towards the camera
	//glEnable(GL_CULL_FACE);

	// Create and compile our GLSL program from the shaders
	GLuint programID = LoadShaders( "StandardShading.vertexshader", "StandardShading.fragmentshader" );

	// Get a handle for our "MVP" uniform
	GLuint MatrixID = glGetUniformLocation(programID, "MVP");
	GLuint ViewMatrixID = glGetUniformLocation(programID, "V");
	GLuint ModelMatrixID = glGetUniformLocation(programID, "M");
	
	GLuint Pressed_L_ID = glGetUniformLocation(programID, "Pressed_L");
	GLuint Light_power0 = glGetUniformLocation(programID, "light_power0");
	GLuint Light_power1 = glGetUniformLocation(programID, "light_power1");
	GLuint Light_power2 = glGetUniformLocation(programID, "light_power2");
	GLuint Light_power3 = glGetUniformLocation(programID, "light_power3");

	// Get a handle for our buffers
	GLuint vertexPosition_modelspaceID = glGetAttribLocation(programID, "vertexPosition_modelspace");
	GLuint vertexUVID = glGetAttribLocation(programID, "vertexUV");
	GLuint vertexNormal_modelspaceID = glGetAttribLocation(programID, "vertexNormal_modelspace");
	GLuint vertexColorID = glGetAttribLocation(programID, "vertexColor");

	// Load the texture
	GLuint Texture1 = loadDDS("uvmap.DDS");
	GLuint TextureID1  = glGetUniformLocation(programID, "myTextureSampler1");
	GLuint Texture2 = loadDDS("try.DDS");
	GLuint TextureID2  = glGetUniformLocation(programID, "myTextureSampler2");
	

	// Read our .obj file
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;
	bool res = loadOBJ("suzanne.obj", vertices, uvs, normals);

	std::vector<unsigned short> indices;
	std::vector<glm::vec3> indexed_vertices;
	std::vector<glm::vec2> indexed_uvs;
	std::vector<glm::vec3> indexed_normals;
	indexVBO(vertices, uvs, normals, indices, indexed_vertices, indexed_uvs, indexed_normals);

	// Load it into a VBO
	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, indexed_vertices.size() * sizeof(glm::vec3), &indexed_vertices[0], GL_STATIC_DRAW);

	GLuint uvbuffer;
	glGenBuffers(1, &uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, indexed_uvs.size() * sizeof(glm::vec2), &indexed_uvs[0], GL_STATIC_DRAW);

	GLuint normalbuffer;
	glGenBuffers(1, &normalbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glBufferData(GL_ARRAY_BUFFER, indexed_normals.size() * sizeof(glm::vec3), &indexed_normals[0], GL_STATIC_DRAW);


	// Generate a buffer for the indices as well
	GLuint elementbuffer;
	glGenBuffers(1, &elementbuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short), &indices[0] , GL_STATIC_DRAW);

	
	// Read square.obj file
	std::vector<glm::vec3> vertices_rec;
	std::vector<glm::vec2> uvs_rec;
	std::vector<glm::vec3> normals_rec; // Won't be used at the moment.
	bool res1 = loadOBJ("square.obj", vertices_rec, uvs_rec, normals_rec);//通过cube.obj将vertices，

	// Load it into a VBO

	GLuint vertexbuffer_rec;
	glGenBuffers(1, &vertexbuffer_rec);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer_rec);
	glBufferData(GL_ARRAY_BUFFER, vertices_rec.size() * sizeof(glm::vec3), &vertices_rec[0], GL_STATIC_DRAW);

	GLuint uvbuffer_rec;
	glGenBuffers(1, &uvbuffer_rec);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer_rec);
	glBufferData(GL_ARRAY_BUFFER, uvs_rec.size() * sizeof(glm::vec2), &uvs_rec[0], GL_STATIC_DRAW);

	int pressed_L = 0; // Set your initial value here

	glUniform1i(Pressed_L_ID, pressed_L);

	// Get a handle for our "LightPosition" uniform
	glUseProgram(programID);
	//GLuint LightID = glGetUniformLocation(programID, "LightPosition_worldspace");

	GLuint LightID1 = glGetUniformLocation(programID, "Lightlocation1");
	GLuint LightID2 = glGetUniformLocation(programID, "Lightlocation2");
	GLuint LightID3 = glGetUniformLocation(programID, "Lightlocation3");
	GLuint LightID4 = glGetUniformLocation(programID, "Lightlocation4");

	int pressed_G = 0;

	// For speed computation
	double lastTime = glfwGetTime();
	int nbFrames = 0;

	// draw reatangle
	GLuint rec_programID = LoadShaders( "SimpleVertexShader.vertexshader", "SimpleFragmentShader.fragmentshader" );
	GLuint rec_MatrixID = glGetUniformLocation(rec_programID, "MVP");
	// Get a handle for our buffers
	GLuint rec_vertexPosition_modelspaceID = glGetAttribLocation(rec_programID, "vertexPosition_modelspace");

	static const GLfloat g_rec_vertex_buffer_data2[] = { 
		-3.0f,-2.5f, 0.0f,
		3.0f,-2.5f, 0.0f,
		-2.5f, 2.5f, 0.0f,
		 2.5f, 2.5f, 0.0f,
		-2.5f,2.5f, 0.0f,
		2.5f, -2.5f, 0.0f
	};
	GLuint rec_vertexbuffer;
	glGenBuffers(1, &rec_vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, rec_vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_rec_vertex_buffer_data2), g_rec_vertex_buffer_data2, GL_STATIC_DRAW);
	static int L_key_state = GLFW_RELEASE;
	static int G_key_state = GLFW_RELEASE;
	
	glm::vec3 objectPosition0 = glm::vec3(1.5f, 1.5f, 0.0f);
	glm::vec3 objectDirection0 = generateRandomDirection(1);
	float init_angle0 = generate_init_angle(34);
	glm::vec3 rotationAxis0 = generateRandomAxis(66);

	glm::vec3 objectPosition1 = glm::vec3(1.5f, -1.5f, 0.0f);
	glm::vec3 objectDirection1 = generateRandomDirection(2);
	float init_angle1 = generate_init_angle(10);
	glm::vec3 rotationAxis1 = generateRandomAxis(234);

	glm::vec3 objectPosition2 = glm::vec3(-1.5f, 1.5f, 0.0f);
	glm::vec3 objectDirection2 = generateRandomDirection(3);
	float init_angle2 = generate_init_angle(77);
	glm::vec3 rotationAxis2 = generateRandomAxis(786);

	glm::vec3 objectPosition3 = glm::vec3(-1.5f, -1.5f, 0.0f);
	glm::vec3 objectDirection3 = generateRandomDirection(4);
	float init_angle3 = generate_init_angle(98);
	glm::vec3 rotationAxis3 = generateRandomAxis(171);
	float Light_value0 = generateRandomLight_power(0,50);  // Change this to the desired value
	float Light_value1 = generateRandomLight_power(0,50);
	float Light_value2 = generateRandomLight_power(0,50);
	float Light_value3 = generateRandomLight_power(0,50);
	float light_change = 5.0;
	do{
		// Measure speed
		double currentTime = glfwGetTime();
		nbFrames++;
		if ( currentTime - lastTime >= 1.0 ){ // If last prinf() was more than 1sec ago
			// printf and reset
			printf("%f ms/frame\n", 1000.0/double(nbFrames));
			nbFrames = 0;
			lastTime += 1.0;
		}

		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		// Compute the MVP matrix from keyboard and mouse input
		computeMatricesFromInputs();
		glm::mat4 ProjectionMatrix = getProjectionMatrix();
		glm::mat4 ViewMatrix = getViewMatrix();

		if(random_light_change()< 0.5){
			Light_value0 -=light_change;
		}
		else{
			Light_value0 +=1.1*light_change;
		}
		if(random_light_change()< 0.5){
			Light_value1 +=1.4*light_change;
		}
		else{
			Light_value1 -=1.4*light_change;
		}
		if(random_light_change()< 0.5){
			Light_value2 +=2*light_change;
		}
		else{
			Light_value2 -=2*light_change;
		}
		if(random_light_change()< 0.5){
			Light_value3 -=2.6*light_change;
		}
		else{
			Light_value3 +=2.5*light_change;
		}
		
		glUseProgram(programID);
		glUniform1f(Light_power0, Light_value0);
		glUniform1f(Light_power1, Light_value1);
		glUniform1f(Light_power2, Light_value2);
		glUniform1f(Light_power3, Light_value3);

		// Bind our texture in Texture Unit 0
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Texture1);
		// Set our "myTextureSampler" sampler to user Texture Unit 0
		glUniform1i(TextureID1, 0);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, Texture2);
		// Set our "myTextureSampler" sampler to user Texture Unit 0
		glUniform1i(TextureID2, 1);

		glUniform1i(glGetUniformLocation(programID, "isRectangle"), 0);
		
		
		//Control whether move:
		if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) {
			pressed_G = 1;
		} 

		

		// Control the light.
		glUseProgram(programID);
		int i = 0;
		int result = 0;
		if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
    	if (L_key_state == GLFW_RELEASE) {
			printf("Press L !!!!!\n");
			pressed_L = !pressed_L; // Toggle the flag
			L_key_state = GLFW_PRESS;
    		}
		} 
		else 
		{
			L_key_state = GLFW_RELEASE;
		}
		glUniform1i(Pressed_L_ID, pressed_L);
		// Use our shader
		
		//glm::vec3 lightPos = glm::vec3(4,4,4);
		glUniform3f(LightID1, objectPosition0.x, objectPosition0.y, objectPosition0.z);
		glUniform3f(LightID2, objectPosition1.x, objectPosition1.y, objectPosition1.z);
		glUniform3f(LightID3, objectPosition2.x, objectPosition2.y, objectPosition2.z);
		glUniform3f(LightID4, objectPosition3.x, objectPosition3.y, objectPosition3.z);

		glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]); // This one doesn't change between objects, so this can be done once for all objects that use "programID"
		
		if (calculateDistance(objectPosition0, objectPosition1) < collisionDistance) {
        updateObjectDirection(objectDirection0);
        updateObjectDirection(objectDirection1);
    	}

		// Check distance between objectPosition0 and objectPosition2
		if (calculateDistance(objectPosition0, objectPosition2) < collisionDistance) {
			updateObjectDirection(objectDirection0);
			updateObjectDirection(objectDirection2);
		}

		// Check distance between objectPosition0 and objectPosition3
		if (calculateDistance(objectPosition0, objectPosition3) < collisionDistance) {
			updateObjectDirection(objectDirection0);
			updateObjectDirection(objectDirection3);
		}

		// Check distance between objectPosition1 and objectPosition2
		if (calculateDistance(objectPosition1, objectPosition2) < collisionDistance) {
			updateObjectDirection(objectDirection1);
			updateObjectDirection(objectDirection2);
		}

		// Check distance between objectPosition1 and objectPosition3
		if (calculateDistance(objectPosition1, objectPosition3) < collisionDistance) {
			updateObjectDirection(objectDirection1);
			updateObjectDirection(objectDirection3);
		}

		// Check distance between objectPosition2 and objectPosition3
		if (calculateDistance(objectPosition2, objectPosition3) < collisionDistance) {
			updateObjectDirection(objectDirection2);
			updateObjectDirection(objectDirection3);
		}

		glUniform3f(LightID1, objectPosition0.x, objectPosition0.y, objectPosition0.z);
		glUniform3f(LightID2, objectPosition1.x, objectPosition1.y, objectPosition1.z);
		glUniform3f(LightID3, objectPosition2.x, objectPosition2.y, objectPosition2.z);
		glUniform3f(LightID4, objectDirection3.x, objectDirection3.y, objectDirection3.z);

		////// Start of the rendering of the first object //////
		{
		glm::mat4 ModelMatrix1 = glm::mat4(1.0);
		// if press G should move the object
		updateModelMatrix(ModelMatrix1, pressed_G, objectPosition0, objectDirection0, init_angle0, rotationAxis0);
		glm::mat4 MVP1 = ProjectionMatrix * ViewMatrix * ModelMatrix1;

		// Send our transformation to the currently bound shader, 
		// in the "MVP" uniform
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP1[0][0]);
		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix1[0][0]);

		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(vertexPosition_modelspaceID);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(
			vertexPosition_modelspaceID, // The attribute we want to configure
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		// 2nd attribute buffer : UVs
		glEnableVertexAttribArray(vertexUVID);
		glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
		glVertexAttribPointer(
			vertexUVID,                       // The attribute we want to configure
			2,                                // size : U+V => 2
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
		);

		// 3rd attribute buffer : normals
		glEnableVertexAttribArray(vertexNormal_modelspaceID);
		glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
		glVertexAttribPointer(
			vertexNormal_modelspaceID,        // The attribute we want to configure
			3,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
		);

		// Index buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);

		// Draw the triangles !
		glDrawElements(
			GL_TRIANGLES,      // mode
			indices.size(),    // count
			GL_UNSIGNED_SHORT,   // type
			(void*)0           // element array buffer offset
		);

		}

		////// Start of the rendering of the second object //////
		{
		// BUT the Model matrix is different (and the MVP too)
		glm::mat4 ModelMatrix2 = glm::mat4(1.0);
		updateModelMatrix(ModelMatrix2, pressed_G, objectPosition1, objectDirection1, init_angle1, rotationAxis1);
		glm::mat4 MVP2 = ProjectionMatrix * ViewMatrix * ModelMatrix2;

		// Send our transformation to the currently bound shader, 
		// in the "MVP" uniform
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP2[0][0]);
		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix2[0][0]);


		// The rest is exactly the same as the first object
		
		// 1rst attribute buffer : vertices
		//glEnableVertexAttribArray(vertexPosition_modelspaceID); // Already enabled
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(vertexPosition_modelspaceID, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		// 2nd attribute buffer : UVs
		//glEnableVertexAttribArray(vertexUVID); // Already enabled
		glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
		glVertexAttribPointer(vertexUVID, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

		// 3rd attribute buffer : normals
		//glEnableVertexAttribArray(vertexNormal_modelspaceID); // Already enabled
		glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
		glVertexAttribPointer(vertexNormal_modelspaceID, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		// Index buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);

		// Draw the triangles !
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_SHORT, (void*)0);
		}

		////// Start of rendering of the third object //////
		{
		glm::mat4 ModelMatrix3 = glm::mat4(1.0);
		updateModelMatrix(ModelMatrix3, pressed_G, objectPosition2, objectDirection2, init_angle2, rotationAxis2);
		glm::mat4 MVP3 = ProjectionMatrix * ViewMatrix * ModelMatrix3;

		// Send our transformation to the currently bound shader, 
		// in the "MVP" uniform
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP3[0][0]);
		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix3[0][0]);


		// The rest is exactly the same as the first object
		
		// 1rst attribute buffer : vertices
		//glEnableVertexAttribArray(vertexPosition_modelspaceID); // Already enabled
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(vertexPosition_modelspaceID, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		// 2nd attribute buffer : UVs
		//glEnableVertexAttribArray(vertexUVID); // Already enabled
		glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
		glVertexAttribPointer(vertexUVID, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

		// 3rd attribute buffer : normals
		//glEnableVertexAttribArray(vertexNormal_modelspaceID); // Already enabled
		glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
		glVertexAttribPointer(vertexNormal_modelspaceID, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		// Index buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);

		// Draw the triangles !
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_SHORT, (void*)0);
		}

		////// Start of rendering of the fourth object //////
		{
		glm::mat4 ModelMatrix4 = glm::mat4(1.0);
		updateModelMatrix(ModelMatrix4, pressed_G, objectPosition3, objectDirection3, init_angle3, rotationAxis3);
		glm::mat4 MVP4 = ProjectionMatrix * ViewMatrix * ModelMatrix4;

		// Send our transformation to the currently bound shader, 
		// in the "MVP" uniform
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP4[0][0]);
		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix4[0][0]);

		// 1rst attribute buffer : vertices
		//glEnableVertexAttribArray(vertexPosition_modelspaceID); // Already enabled
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(vertexPosition_modelspaceID, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		// 2nd attribute buffer : UVs
		//glEnableVertexAttribArray(vertexUVID); // Already enabled
		glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
		glVertexAttribPointer(vertexUVID, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

		// 3rd attribute buffer : normals
		//glEnableVertexAttribArray(vertexNormal_modelspaceID); // Already enabled
		glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
		glVertexAttribPointer(vertexNormal_modelspaceID, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		// Index buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);


		// Draw the triangles !
		
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_SHORT, (void*)0);
		//End fourth Object
		}
		
		//Draw Rec
		glUniform1i(glGetUniformLocation(programID, "isRectangle"), 1);
		glm::mat4 ModelMatrix = glm::mat4(1.0);
		glm::mat4 MVP0 = ProjectionMatrix * ViewMatrix * ModelMatrix;

		// Send our transformation to the currently bound shader, 
		// in the "MVP" uniform
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP0[0][0]);

		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(vertexPosition_modelspaceID);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer_rec);
		glVertexAttribPointer(
			vertexPosition_modelspaceID,  // The attribute we want to configure
			3,                            // size
			GL_FLOAT,                     // type
			GL_FALSE,                     // normalized?
			0,                            // stride
			(void*)0                      // array buffer offset
		);

		// 2nd attribute buffer : UVs
		glEnableVertexAttribArray(vertexUVID);
		glBindBuffer(GL_ARRAY_BUFFER, uvbuffer_rec);
		glVertexAttribPointer(
			vertexUVID,                   // The attribute we want to configure
			2,                            // size : U+V => 2
			GL_FLOAT,                     // type
			GL_FALSE,                     // normalized?
			0,                            // stride
			(void*)0                      // array buffer offset
		);

		// Draw the triangles !
		glDrawArrays(GL_TRIANGLES, 0, vertices.size() );


		glDisableVertexAttribArray(rec_vertexPosition_modelspaceID);	//draw rectangle
		glDisableVertexAttribArray(vertexPosition_modelspaceID);
		glDisableVertexAttribArray(vertexUVID);
		glDisableVertexAttribArray(vertexNormal_modelspaceID);
		
		//glDisableVertexAttribArray(vertexPosition_modelspaceID);
		glDisableVertexAttribArray(vertexColorID);

		glDisableVertexAttribArray(vertexPosition_modelspaceID);
		glDisableVertexAttribArray(vertexUVID);


		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		   glfwWindowShouldClose(window) == 0 );

	// Cleanup VBO and shader
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &uvbuffer);
	glDeleteBuffers(1, &normalbuffer);
	glDeleteBuffers(1, &elementbuffer);
	glDeleteProgram(programID);
	glDeleteTextures(1, &Texture1);
	glDeleteTextures(1, &Texture2);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}

