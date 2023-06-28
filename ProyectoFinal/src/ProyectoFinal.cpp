#define _USE_MATH_DEFINES
#define _DEBUG_FLAG 0
#define _DEBUG_CAMERA 0
#include <cmath>
//glew include
#include <GL/glew.h>

//std includes
#include <string>
#include <iostream>
#include <random>

//glfw include
#include <GLFW/glfw3.h>

// program include
#include "Headers/TimeManager.h"

// Shader include
#include "Headers/Shader.h"

// Model geometric includes
#include "Headers/Sphere.h"
#include "Headers/Cylinder.h"
#include "Headers/Box.h"
#include "Headers/FirstPersonCamera.h"
#include "Headers/ThirdPersonCamera.h"

//GLM include
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Headers/Texture.h"

// Include loader Model class
#include "Headers/Model.h"

// Include Terrain
#include "Headers/Terrain.h"

#include "Headers/AnimationUtils.h"

// Include Colision headers functions
#include "Headers/Colisiones.h"

// Shadow Box Include
#include "Headers/ShadowBox.h"

// Include Sound headers functions
#include <AL/alut.h>

// Libreria de Texto
#include "Headers/FontTypeRendering.h"

// My includes for abstraccion
#include "Headers/GameObject.h"

#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))

// Constantes de juego.
#define VELOCIDAD_BALA 1.2f
#define VELOCIDAD_MOVIMIENTO_PERSONAJE 0.1f
#define VELOCIDAD_ROTACION_PERSONAJE 0.6f

#define GRAVEDAD_SALTO_PERSONAJE 0.5f
#define NUMERO_ENEMIGOS 30
#define VELOCIDAD_DE_GIRO_PERSONAJE 5.0f
int TIEMPO_ENTRE_ZOMBIES = 5;
int vidas = 250;
float VELOCIDAD_MOVIMIENTO_ZOMBIE = 0.001f;

// Constantes para la camara
#define SCREEN_WIDTH 1350
#define SCREEN_HEIGHT 768
#define CAMERA_PITCH 45.0f
#define CAMERA_ANGLE 45.0f
#define CAMERA_DISTANCE 15.0f

// Constantes para las sombras.
const unsigned int SHADOW_WIDTH = 4096, SHADOW_HEIGHT = 4096;

int screenWidth;
int screenHeight;

GLFWwindow* window;

Shader shader;
//Shader con skybox
Shader shaderSkybox;
//Shader con multiples luces
Shader shaderMulLighting;
//Shader para el terreno
Shader shaderTerrain;
//Shader para dibujar el buffer de profundidad.
Shader shaderDepth;
ShadowBox* shadowBox;
// Shader para dibujar la interfaz grafica
Shader shaderViewTexture;
// Shader para el sistema de particulas de zombies
Shader shaderZombieBlood;
// Shader para el sistema de particulas del jugador cuando camina
Shader shaderJugadorCamina;

// CAMARA:
std::shared_ptr<Camera> camera(new ThirdPersonCamera());
float distanceFromTarget = 7.0;
Sphere skyboxSphere(20, 20);
Box boxCollider;
Sphere sphereCollider(10, 10);
Cylinder RayModel(10, 10, 1.0, 1.0, 1.0); //Creamos modelo de cilindro que servira como nuestro "rayo"
Cylinder bala(10, 10, 1.0, 1.0, 1.0);
Box zombiePlaceHolder;
Box boxRenderImagen;

// Models complex instances
Model modelRock;

Model modelHeliChasis;
Model modelHeliHeli;


// Modelos animados: 
// Pruebas:
// Mayow
//Model mayowModelAnimate;

// Finales:
// Personaje:
Model playerModelAnimated;
// Zombie: 
Model zombieModel;
// Modelos de efectos:
Model modelDisparo;


// Pruebas:
// GameObjects:
GameObject* mayowGameObject;
GameObject* jugadorGameObject;
GameObject* zombieGameObject;
GameObject* bulletGameObject;

std::vector<GameObject*> enemyCollection;
std::vector<GameObject*> bulletCollection;


// menus
bool sw = false;
bool btnApress = false;
bool btnBackPress = false;
bool btnStartPress = false;

//std::vector<GameObject> zombieGameObjects;
std::vector<Box> zombieContainer;
int zombieOffset = 0;
std::vector<GameObject*> Lamparas;
std::vector<GameObject*> Lamparas2;

// Relevantes al terreno: 
// Terrain model instance
Terrain terrain(-1, -1, 200, 8, "../Textures/heightmap3.png");
GLuint textureCespedID, textureWallID, textureWindowID, textureHighwayID, textureLandingPadID;
GLuint textureTerrainBackgroundID, textureTerrainRID, textureTerrainGID, textureTerrainBID, textureTerrainBlendMapID;
GLuint skyboxTextureID;

// UI textures:
GLuint textureHealthBarID, scoreTextureID, texturePauseMenuID, textureGameOverID, textureItemContainer, textureTitleMenuID;
GLuint textureParticleZombieBloodID, textureParticleJugadorCaminaID;

FontTypeRendering::FontTypeRendering* modelText;

GLenum types[6] = {
GL_TEXTURE_CUBE_MAP_POSITIVE_X,
GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
GL_TEXTURE_CUBE_MAP_NEGATIVE_Z };


// SKYBOX
std::string fileNames[6] = { 
		"../Textures/skybox/SB_FT.png",	//FRONT
		"../Textures/skybox/SB_BK.png",	//BACK
		"../Textures/skybox/SB_UP.png",	//UP
		"../Textures/skybox/SB_DW.png",	//DOWN
		"../Textures/skybox/SB_R.png",	//RIGHT	
		"../Textures/skybox/SB_L.png" };	//LEFT

// Random Number Generator
std::random_device rd;
std::mt19937 generador(rd());
std::uniform_int_distribution<> distrib(1, 50);  //Tama�o del area de generaci�n del zombie en (X,Z)

// CONTROL DEL JUEGO:
bool exitApp = false;
int lastMousePosX, offsetX = 0;
int lastMousePosY, offsetY = 0;
bool isJump = false;
double startTimeJump = 0.0;
double tmv = 0.0;
float gravity = 1.3;

// BALA
std::vector<bool> isBalaColZombie = { false, false };
std::vector<bool> lanzarBala = { false, false };
float disMaxBala = 7.0f; // Distancia m�xima de la bala
std::vector<float> disBala = { 0.0f, 0.0f };
float desapareceObjeto = 0.0f;
float velBala = 0.09f;
float posturaJugadorX = 0.0f;
float posturaJugadorY = 0.0f;
float posturaJugadorZ = 0.0f;
std::vector<glm::mat4> vectorMatrixRayBala = { glm::mat4(0.0f), glm::mat4(0.0f), glm::mat4(0.0f) };
std::vector<glm::vec3> vectorVarRayBala = {
	glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f),	// rayDirectionBala, origenBala, targetRayBala para la bala 0
	glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f),	// rayDirectionBala, origenBala, targetRayBala para la bala 1
	glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f) };// rayDirectionBala, origenBala, targetRayBala para la bala 2
int numVarBala = 0;
int numEnemyImpacto = -1;
float escalaEnemy[NUMERO_ENEMIGOS];
float transladaEnemy[NUMERO_ENEMIGOS];


//FIN DE CONTROLES DE JUEGO.
glm::mat4 matrixModelRock = glm::mat4(1.0);

// CONTROLES DE ESCNEA
enum GameState {
	PLAY, PAUSE, GAMEOVER, TITLE
};
bool isPaused = false;
bool isGameOver = false;
GameState state = TITLE;


int animationIndex = 1;
int animationIndexPlayer = 1;

// Blending model unsorted
std::map<std::string, glm::vec3> blendingUnsorted = {
	{"particulasBlood", glm::vec3(0.0)},
	{"particulasCamina", glm::vec3(0.0)}
};

double deltaTime, diffTime;
double currTime, lastTime, startupTimer;

// Variables para el sistema de particulas de la sangre Zombie
GLuint initVelZB, startTimeZB, VAOZB, numParticulasZB = 20000;
double currTimeParticulasZombieBlood, lastTimeParticulasZombieBlood;
bool ZombieBloodBool = false;
int cTimeParticlesZombie = 0;

// Variables para el sistema de particulas de la sangre Zombie
GLuint initVelJC, startTimeJC, VAOJC, numParticulasJC = 10000;
double currTimeParticulasJugadorCamina, lastTimeParticulasJugadorCamina;
bool JugadorCaminaBool = false;
int cTimeParticlesJugadorCamina = 0;

// Colliders
std::map<std::string, std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> > collidersOBB;
std::map<std::string, std::tuple<AbstractModel::SBB, glm::mat4, glm::mat4> > collidersSBB;

// FrameBuffers
GLuint depthMap, depthMapFBO;

/**********************
 * OpenAL config
 **********************/

// OpenAL Defines
#define NUM_BUFFERS 3
#define NUM_SOURCES 2
#define NUM_ENVIRONMENTS 1
// Listener
ALfloat listenerPos[] = { 0.0, 0.0, 4.0 };
ALfloat listenerVel[] = { 0.0, 0.0, 0.0 };
ALfloat listenerOri[] = { 0.0, 0.0, 1.0, 0.0, 1.0, 0.0 };
// Source 0
ALfloat BGMusic0Pos[] = { -2.0, 0.0, 0.0 };
ALfloat BGMusic0Vel[] = { 0.0, 0.0, 0.0 };
// Source 1
ALfloat GunPos[] = { 2.0, 0.0, 0.0 };
ALfloat GunVel[] = { 0.0, 0.0, 0.0 };

// Buffers
ALuint buffer[NUM_BUFFERS];
ALuint source[NUM_SOURCES];
ALuint environment[NUM_ENVIRONMENTS];
//Configs
ALsizei size, freq;
ALenum format;
ALvoid *data;
int ch;
ALboolean loop;
std::vector<bool> sourcesPlay = { true, false };

// *-------------------------------------------------*
// *--------   PROTOTIPOS DE FUNCIONES        -------*
// *-------------------------------------------------*

// Se definen todos las funciones.
void reshapeCallback(GLFWwindow* Window, int widthRes, int heightRes);
void keyCallback(GLFWwindow* window, int key, int scancode, int action,
	int mode);
void mouseCallback(GLFWwindow* window, double xpos, double ypos);
void mouseButtonCallback(GLFWwindow* window, int button, int state, int mod);
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void loadTexture(std::string pathToTexture, GLuint* textureID);
void init(int width, int height, std::string strTitle, bool bFullScreen);
void destroy();
bool processInput(bool continueApplication = true);
void prepareScene();
void prepareDepthScene();
void renderScene(bool renderParticles = true);
// funciones para el comportamiento del juego


void generarZombie();
void generarBala();
void resetGame();

void inicializerParticulas(GLuint* initVel, GLuint* startTime, GLuint* VAO, GLuint* numParticulas);
void inicializerParticulasZB();
void inicializerParticulasJC();
void renderPaticulas(bool pBool, int contador, int maxContador, double curr, double last, float tAnim, float tPar, float altura,
	std::map<float, std::pair<std::string, glm::vec3> >::reverse_iterator itera, GLuint* nPar, GLuint* textureID, Shader shader);

void drawGUIElement(GLuint textureID, glm::vec3 scale, glm::vec3 pos);
void startScene(std::string sceneName);

void initOpenGL(int width, int height, std::string strTitle, bool bFullScreen) {

	if (!glfwInit()) {
		std::cerr << "Failed to initialize GLFW" << std::endl;
		exit(-1);
	}

	screenWidth = SCREEN_WIDTH;
	screenHeight = SCREEN_HEIGHT;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	if (bFullScreen)
		window = glfwCreateWindow(width, height, strTitle.c_str(),
			glfwGetPrimaryMonitor(), nullptr);
	else
		window = glfwCreateWindow(width, height, strTitle.c_str(), nullptr,
			nullptr);

	if (window == nullptr) {
		std::cerr
			<< "Error to create GLFW window, you can try download the last version of your video card that support OpenGL 3.3+"
			<< std::endl;
		destroy();
		exit(-1);
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(0);
	glfwSetWindowSizeCallback(window, reshapeCallback);
	glfwSetKeyCallback(window, keyCallback);
	glfwSetCursorPosCallback(window, mouseCallback);
	glfwSetMouseButtonCallback(window, mouseButtonCallback);
	glfwSetScrollCallback(window, scrollCallback);
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Init glew
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (GLEW_OK != err) {
		std::cerr << "Failed to initialize glew" << std::endl;
		exit(-1);
	}

	glViewport(0, 0, screenWidth, screenHeight);
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
}

// Implementacion de todas las funciones.
void init(int width, int height, std::string strTitle, bool bFullScreen) {

	initOpenGL(width, height,strTitle,bFullScreen);

	// Inicializaci�n de los shaders
	shader.initialize("../Shaders/colorShader.vs", "../Shaders/colorShader.fs");
	shaderSkybox.initialize("../Shaders/skyBox.vs", "../Shaders/skyBox.fs");
	shaderMulLighting.initialize("../Shaders/iluminacion_textura_animation_shadow.vs", "../Shaders/multipleLights_shadow.fs");
	shaderTerrain.initialize("../Shaders/terrain_shadow.vs", "../Shaders/terrain_shadow.fs");
	shaderDepth.initialize("../Shaders/shadow_mapping_depth.vs", "../Shaders/shadow_mapping_depth.fs");
	shaderViewTexture.initialize("../Shaders/texturizado.vs", "../Shaders/texturizado.fs");
	shaderZombieBlood.initialize("../Shaders/particlesBlood.vs", "../Shaders/particlesBlood.fs");
	shaderJugadorCamina.initialize("../Shaders/particlesCamina.vs", "../Shaders/particlesCamina.fs");

	// Inicializacion de los objetos.

	boxRenderImagen.init();
	boxRenderImagen.setShader(&shaderViewTexture);

	skyboxSphere.init();
	skyboxSphere.setShader(&shaderSkybox);
	skyboxSphere.setScale(glm::vec3(20.0f, 20.0f, 20.0f));

	boxCollider.init();
	boxCollider.setShader(&shader);
	boxCollider.setColor(glm::vec4(1.0, 1.0, 1.0, 1.0));

	sphereCollider.init();
	sphereCollider.setShader(&shader);
	sphereCollider.setColor(glm::vec4(1.0, 1.0, 1.0, 1.0));

	RayModel.init();
	RayModel.setShader(&shader);
	RayModel.setColor(glm::vec4(1.0));

	bala.init();
	bala.setShader(&shader);
	bala.setColor(glm::vec4(1.0, 0.0, 0.3765, 1.0));

	zombiePlaceHolder.init();
	zombiePlaceHolder.setShader(&shader);
	zombiePlaceHolder.setColor(glm::vec4(1.0, 0.0, 0.0, 1.0));


	modelRock.loadModel("../models/rock/rock.obj");
	

	// Terreno
	terrain.init();
	
	terrain.setPosition(glm::vec3(100, 0, 100));


	
	// Modelo de pruebas: Mayow

	mayowGameObject = new GameObject("Mayow", "../models/mayow/personaje2.fbx", &shaderMulLighting);

	// Modelo de juego: Jugador.
	jugadorGameObject = new GameObject("Player", "../models/Player/PlayerAnimated.fbx", &shaderMulLighting);

	// Modelo de juego: Zombie.
	zombieGameObject = new GameObject("Zombie", "../models/Zombie/ZombieAnimated.fbx", &shaderMulLighting);

	// Modelo de juego: Bala.
	bulletGameObject = new GameObject("Bala", "../models/bullet/bullet.fbx", &shaderMulLighting);

	// Contenedor de Zombies.
	for (int i = 0; i < 3; i++) {
		generarZombie();
	}

	// Camera
	camera->setPosition(glm::vec3(0.0, 0.0, 10.0));
	
	camera->setSensitivity(1.0);

	// Carga de texturas para el skybox
	int imageWidth, imageHeight;
	FIBITMAP* bitmap;
	unsigned char* data;

	// Carga de texturas para el skybox
	Texture skyboxTexture = Texture("");
	glGenTextures(1, &skyboxTextureID);
	// Tipo de textura CUBE MAP
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTextureID);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	for (int i = 0; i < ARRAY_SIZE_IN_ELEMENTS(types); i++) {
		skyboxTexture = Texture(fileNames[i]);
		FIBITMAP* bitmap = skyboxTexture.loadImage(true);
		unsigned char* data = skyboxTexture.convertToData(bitmap, imageWidth,
			imageHeight);
		if (data) {
			glTexImage2D(types[i], 0, GL_RGBA, imageWidth, imageHeight, 0,
				GL_BGRA, GL_UNSIGNED_BYTE, data);
		}
		else
			std::cout << "Failed to load texture" << std::endl;
		skyboxTexture.freeImage(bitmap);
	}
	// Fin de texturas skybox.

	loadTexture("../Textures/cesped.jpg", &textureCespedID);
	loadTexture("../Textures/whiteWall.jpg", &textureWallID);
	loadTexture("../Textures/ventana.png", &textureWindowID);
	loadTexture("../Textures/highway.jpg", &textureHighwayID);
	loadTexture("../Textures/landingPad.jpg", &textureLandingPadID);
	// --------- TEXTURAS DE PARA EL BLEND MAP ----------
	// 
	// **************************************************
	//						BACKGROUND:
	/// -------------------------------------------------
	loadTexture("../Textures/grassy2.png", &textureTerrainBackgroundID);
	// **************************************************
	//						CANAL ROJO:
	//						Banquetas
	/// -------------------------------------------------
	loadTexture("../Textures/path.png", &textureTerrainRID);
	// **************************************************
	//						CANAL VERDE:
	//						Flores / Basura
	/// -------------------------------------------------
	loadTexture("../Textures/path.png", &textureTerrainGID);
	// **************************************************
	//						CANAL AZUL:
	//					Carretera, camino. 
	/// -------------------------------------------------
	loadTexture("../Textures/asfalto.png", &textureTerrainBID);

	// **************************************************
	//						BLEND MAP:
	//
	/// -------------------------------------------------
	loadTexture("../Textures/blendMap2.png", &textureTerrainBlendMapID);

	//Texturas para interfaz gr�fica.
	loadTexture("../Textures/UI/health.png", &textureHealthBarID);
	loadTexture("../Textures/UI/score.png", &scoreTextureID);
	loadTexture("../Textures/UI/pauseMenu.png", &texturePauseMenuID);
	loadTexture("../Textures/UI/gameOver.png", &textureGameOverID);
	loadTexture("../Textures/UI/container.png", &textureItemContainer);
	loadTexture("../Textures/UI/title.png", &textureTitleMenuID);
	// **************************************************
	//						PARTICULAS:
	//					Zombies, jugador
	/// -------------------------------------------------
	loadTexture("../Textures/Blood-1.png", &textureParticleZombieBloodID);
	inicializerParticulas(&initVelZB, &startTimeZB, &VAOZB, &numParticulasZB);
	//inicializerParticulasZB();
	loadTexture("../Textures/smoke.png", &textureParticleJugadorCaminaID);
	inicializerParticulas(&initVelJC, &startTimeJC, &VAOJC, &numParticulasJC);
	//inicializerParticulasJC();

	/*******************************************
	 * Inicializacion del framebuffer para
	 * almacenar el buffer de profunidadad
	 *******************************************/
	glGenFramebuffers(1, &depthMapFBO);
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	/*glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);*/
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	modelText = new FontTypeRendering::FontTypeRendering(screenWidth, screenHeight);
	modelText->Initialize();


	/*******************************************
	 * OpenAL init
	 *******************************************/
	alutInit(0, nullptr);
	alListenerfv(AL_POSITION, listenerPos);
	alListenerfv(AL_VELOCITY, listenerVel);
	alListenerfv(AL_ORIENTATION, listenerOri);
	alGetError(); // clear any error messages
	if (alGetError() != AL_NO_ERROR) {
		printf("- Error creating buffers !!\n");
		exit(1);
	}
	else {
		printf("init() - No errors yet.");
	}

	// Generate buffers, or else no sound will happen!
	alGenBuffers(NUM_BUFFERS, buffer);
	buffer[0] = alutCreateBufferFromFile("../sounds/bg01.wav");
	buffer[1] = alutCreateBufferFromFile("../sounds/gunshot.wav");
	//buffer[1] = alutCreateBufferFromFile("../sounds/walk.wav");
	int errorAlut = alutGetError();
	if (errorAlut != ALUT_ERROR_NO_ERROR) {
		printf("- Error open files with alut %d !!\n", errorAlut);
		exit(2);
	}
	alGetError(); /* clear error */
	alGenSources(NUM_SOURCES, source);
	if (alGetError() != AL_NO_ERROR) {
		printf("- Error creating sources !!\n");
		exit(2);
	}
	else {
		printf("init - no errors after alGenSources\n");
	}
	alSourcef(source[0], AL_PITCH, 1.0f);
	alSourcef(source[0], AL_GAIN, 3.0f);
	alSourcefv(source[0], AL_POSITION, BGMusic0Pos);
	alSourcefv(source[0], AL_VELOCITY, BGMusic0Vel);
	alSourcei(source[0], AL_BUFFER, buffer[0]);
	alSourcei(source[0], AL_LOOPING, AL_TRUE);
	alSourcef(source[0], AL_MAX_DISTANCE, 2000);

	alSourcef( source[1], AL_PITCH, 1.0f);
	alSourcef( source[1], AL_GAIN, 3.0f);
	alSourcefv(source[1], AL_POSITION, GunPos);
	alSourcefv(source[1], AL_VELOCITY, GunVel);
	alSourcei( source[1], AL_BUFFER, buffer[1]);
	alSourcei( source[1], AL_LOOPING, AL_FALSE);
	alSourcef( source[1], AL_MAX_DISTANCE, 2000);

	startupTimer = TimeManager::Instance().GetTime();

}

void loadTexture(std::string pathToTexture, GLuint* textureID)
{
	//Definimos el tama�o de la imagen
	int imageWidth, imageHeight;
	FIBITMAP* bitmap;
	unsigned char* data;

	//FIBITMAP* bitmap; //Mapa de bits para cargar la textura
	//Definiendo la textura a utilizar
	Texture texture(pathToTexture);
	//Carga el mapa de bits (FIBITMAP es el tipo de dato de la libreria)
	bitmap = texture.loadImage();
	//Convertirmos el mapa de bits en un arreglo unidimensional de tipo unsigned char
	data = texture.convertToData(bitmap, imageWidth, imageHeight);
	//Creando la textura con id 1
	glGenTextures(1, textureID);
	//Enlazar esa textura a un tipo de textura de 2D.
	glBindTexture(GL_TEXTURE_2D, *textureID);
	//Set the texture wrapping parameters.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	//Set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//Verifica si se pudo abrir la textura.
	if (data) {
		// Transferis los datos de la imagen a memoria
		// Tipo de textura, Mipmaps, Formato interno de openGL, ancho, alto, Mipmaps,
		// Formato interno de la libreria de la imagen, el tipo de dato y al apuntador
		// a los datos
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
		// Generan los niveles del mipmap (OpenGL es el ecargado de realizarlos)
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;

	// Liberamos la memoria de la imagen, ya que ya fue transferida a memoria de video
	texture.freeImage(bitmap);
}


void destroy() {
	glfwDestroyWindow(window);
	glfwTerminate();
	// --------- IMPORTANTE ----------
	// Eliminar los shader y buffers creados.

	// Shaders Delete
	shader.destroy();
	shaderMulLighting.destroy();
	shaderSkybox.destroy();
	shaderTerrain.destroy();
	shaderZombieBlood.destroy();
	shaderJugadorCamina.destroy();

	// Basic objects Delete
	skyboxSphere.destroy();
	boxCollider.destroy();
	sphereCollider.destroy();
	RayModel.destroy();
	zombiePlaceHolder.destroy();
	bala.destroy();

	// Terrains objects Delete
	terrain.destroy();

	// Custom objects Delete
	modelHeliChasis.destroy();
	modelHeliHeli.destroy();
	modelRock.destroy();


	// Custom objects animate
	/*mayowModelAnimate.destroy();*/
	delete mayowGameObject;
	delete jugadorGameObject;
	delete zombieGameObject;
	
	


	// Textures Delete
	glBindTexture(GL_TEXTURE_2D, 0);
	glDeleteTextures(1, &textureCespedID);
	glDeleteTextures(1, &textureWallID);
	glDeleteTextures(1, &textureWindowID);
	glDeleteTextures(1, &textureHighwayID);
	glDeleteTextures(1, &textureLandingPadID);
	glDeleteTextures(1, &textureTerrainBackgroundID);
	glDeleteTextures(1, &textureTerrainRID);
	glDeleteTextures(1, &textureTerrainGID);
	glDeleteTextures(1, &textureTerrainBID);
	glDeleteTextures(1, &textureTerrainBlendMapID);
	glDeleteTextures(1, &textureParticleZombieBloodID);
	glDeleteTextures(1, &textureParticleJugadorCaminaID);

	// Cube Maps Delete
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	glDeleteTextures(1, &skyboxTextureID);

	// Eliminar el buffer del sistema de particulas Zombie Blood
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &initVelZB);
	glDeleteBuffers(1, &startTimeZB);
	glBindVertexArray(0);
	glDeleteVertexArrays(1, &VAOZB);

	// Eliminar el buffer del sistema de particulas Zombie Blood
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &initVelJC);
	glDeleteBuffers(1, &startTimeJC);
	glBindVertexArray(0);
	glDeleteVertexArrays(1, &VAOJC);
}

void reshapeCallback(GLFWwindow* Window, int widthRes, int heightRes) {
	screenWidth = widthRes;
	screenHeight = heightRes;
	glViewport(0, 0, widthRes, heightRes);
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action,
	int mode) {
	if (action == GLFW_PRESS) {
		switch (key) {
		case GLFW_KEY_ESCAPE:
			exitApp = true;
			break;
		}
	}
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
	offsetX = xpos - lastMousePosX;
	offsetY = ypos - lastMousePosY;
	lastMousePosX = xpos;
	lastMousePosY = ypos;
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
	distanceFromTarget -= yoffset;
	camera->setDistanceFromTarget(distanceFromTarget);
}

void mouseButtonCallback(GLFWwindow* window, int button, int state, int mod) {
	if (state == GLFW_PRESS) {
		switch (button) {
		case GLFW_MOUSE_BUTTON_RIGHT:
			std::cout << "lastMousePos.y:" << lastMousePosY << std::endl;
			break;
		case GLFW_MOUSE_BUTTON_LEFT:
			std::cout << "lastMousePos.x:" << lastMousePosX << std::endl;
			break;
		case GLFW_MOUSE_BUTTON_MIDDLE:
			std::cout << "lastMousePos.x:" << lastMousePosX << std::endl;
			std::cout << "lastMousePos.y:" << lastMousePosY << std::endl;
			break;
		}
	}
}

bool processInput(bool continueApplication) {
	if (exitApp || glfwWindowShouldClose(window) != 0) {
		return false;
	}


	// Controles con Joystick
	if (glfwJoystickPresent(GLFW_JOYSTICK_1) == GLFW_TRUE) {
		
		int axisCount = 0, buttonCount = 0;
		const float* axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &axisCount);
		const unsigned char* buttons = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &buttonCount);


#if _DEBUG_FLAG
		std::cout << "Existen:  " << axisCount << " ejes detectados. " << std::endl;
		std::cout << "Stick izquierdo horizontal " << axes[0] << std::endl;
		std::cout << "Stick izquierdo vertical  " << axes[1] << std::endl;

		std::cout << "Stick derecho horizontal " << axes[2] << std::endl;
		std::cout << "Stick derecho vertical " << axes[3] << std::endl;

		std::cout << "Trigger L " << axes[4] << std::endl;
		std::cout << "Trigger R " << axes[5] << std::endl;

		const unsigned char* buttons = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &buttonCount);
		std::cout << "Existen:  " << buttonCount << " botones detectados. " << std::endl;
#endif
		
		//Mover al personaje Stick izquierdo
		if (abs(axes[0]) >= 0.1 || abs(axes[1] >= 0.1) ) {
			jugadorGameObject->ModelMatrix[3][0] += -axes[0] * VELOCIDAD_MOVIMIENTO_PERSONAJE;
			jugadorGameObject->ModelMatrix[3][2] += axes[1] * VELOCIDAD_MOVIMIENTO_PERSONAJE;
			animationIndexPlayer = 2;
			JugadorCaminaBool = true;
		}

		//Rotar al personaje Stick Derecho
		if (abs(axes[2]) >= 0.3f   || abs(axes[3]) >= 0.3){
		
			//Obtenemos la posición del jugador y del stick
			glm::vec3 playerCenter = glm::vec3(jugadorGameObject->ModelMatrix[3][0], jugadorGameObject->ModelMatrix[3][1], jugadorGameObject->ModelMatrix[3][2]);
			glm::vec3 stickPosRelToPlayer = glm::vec3(playerCenter.x + axes[2], playerCenter.y, playerCenter.z + axes[3]);  //This is target position relative to player

			//Obtenemos la dirección hacia el stick
			glm::vec3 directionToStick = glm::normalize(stickPosRelToPlayer - playerCenter);
			glm::vec3 currentDir = glm::normalize(jugadorGameObject->ModelMatrix[2]);

			glm::mat4 currentTransform = jugadorGameObject->ModelMatrix; //Save current transform
			
			//Obtenemos el angulo entre la dirección actual y la dirección hacia el stick
			float dotProduct = glm::dot(currentDir, directionToStick);
			glm::vec3 crossProduct = glm::cross(directionToStick, currentDir);
			float stickAngle = (acos(dotProduct));

			//directionToStick != currentDir
			if (stickAngle > 3.0f || stickAngle < 2.9f)
			{
				if(crossProduct.y < 0)
					jugadorGameObject->ModelMatrix = glm::rotate(currentTransform, glm::radians(-VELOCIDAD_DE_GIRO_PERSONAJE), glm::vec3(0, 1, 0));
				if(crossProduct.y > 0)
					jugadorGameObject->ModelMatrix = glm::rotate(currentTransform, glm::radians(VELOCIDAD_DE_GIRO_PERSONAJE), glm::vec3(0, 1, 0));
			}
		
		}

		//Disparar balas, Boton RB.
		//Detect Press
		if(buttons[5] == GLFW_PRESS) {
			btnApress = true;
		}

		//Wait for Release
		if (btnApress && buttons[5] == GLFW_RELEASE) {
			generarBala();
			alSourcePlay(source[1]);
			btnApress = false;
		}
		

		//Menu pausa, Boton Back
		
		//Detect Press
		if(buttons[6] == GLFW_PRESS){
			btnBackPress = true;
		}

		//Wait for Release
		if (btnBackPress && buttons[6] == GLFW_RELEASE )
		{
			sw = !sw;
			if (sw) {
				state = PAUSE;

			}
			else {
				state = PLAY;
			}
			btnBackPress = false;
		}

		// Menu Inicio y reiniciar, Boton Back.
		if (state == TITLE) {
			if (buttons[7] == GLFW_PRESS) {
				btnStartPress = true;
			}
			if (btnStartPress && buttons[7] == GLFW_RELEASE) {
				state = PLAY;
				isPaused = false;
			}

		}

		if (state == GAMEOVER) {
			if (buttons[6] == GLFW_PRESS )
			{
				state = PLAY;
				resetGame();

			}
		}
		


	}



#if _DEBUG_CAMERA
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
		camera->mouseMoveCamera(offsetX, 0.0, deltaTime);
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
		camera->mouseMoveCamera(0.0, offsetY, deltaTime);	
	offsetX = 0;
	offsetY = 0;
#endif
	//Rotación del jugador
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
		jugadorGameObject->ModelMatrix = glm::rotate(jugadorGameObject->ModelMatrix, glm::radians(VELOCIDAD_ROTACION_PERSONAJE), glm::vec3(0, 1, 0));
		animationIndex = 0;
		animationIndexPlayer = 2;
		JugadorCaminaBool = true;
	}
	else if ( glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
	
		jugadorGameObject->ModelMatrix = glm::rotate(jugadorGameObject->ModelMatrix, glm::radians(-VELOCIDAD_ROTACION_PERSONAJE), glm::vec3(0, 1, 0));	
		animationIndex = 0;
		animationIndexPlayer = 2;

	//Movimiento del jugador
	}if ( glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
		jugadorGameObject->ModelMatrix = glm::translate(jugadorGameObject->ModelMatrix, glm::vec3(0, 0, VELOCIDAD_MOVIMIENTO_PERSONAJE));
		animationIndex = 0;
		animationIndexPlayer = 2;
		JugadorCaminaBool = true;
	}
	else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
		jugadorGameObject->ModelMatrix = glm::translate(jugadorGameObject->ModelMatrix, glm::vec3(0, 0, -VELOCIDAD_MOVIMIENTO_PERSONAJE));
		animationIndex = 0;
		animationIndexPlayer = 2;
		JugadorCaminaBool = true;
	}
	// Disparar bala
	else if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_B) == GLFW_RELEASE  ) {


		generarBala();

	}

	if (JugadorCaminaBool)
		blendingUnsorted.find("particulasCamina")->second = glm::vec3(jugadorGameObject->ModelMatrix[3]);

	bool statePause;
	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_P) == GLFW_RELEASE)
	{
		sw = !sw;
		if (sw) {
			state = PAUSE;
			
		}
		else {
			state = PLAY;
		}
	}

	if (state == TITLE) {
		if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS) {
			state = PLAY;
			isPaused = false;
		}

	}

	if (state == GAMEOVER) {
		if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_R) == GLFW_RELEASE)
		{
			state = PLAY;
			resetGame();
			
		}
	}

	glfwPollEvents();
	return continueApplication;
}

void applicationLoop() {
	bool psi = true;

	glm::mat4 view;
	glm::vec3 axis;
	glm::vec3 target;
	float angleTarget;

	//Initial Positions???
	matrixModelRock = glm::translate(matrixModelRock, glm::vec3(-3.0, 0.0, 2.0));
	jugadorGameObject->ModelMatrix = glm::translate(jugadorGameObject->ModelMatrix, glm::vec3(3.0, 0.0, 2.0));
	zombieGameObject->ModelMatrix = glm::translate(zombieGameObject->ModelMatrix, glm::vec3(5.0, 0.0, 2.0));
	
	lastTime = TimeManager::Instance().GetTime();

	//Setting up Initial camera Angle and position.
	camera->setAngleTarget(CAMERA_ANGLE);
	camera->setDistanceFromTarget(CAMERA_DISTANCE);
	camera->mouseMoveCamera(0.0, CAMERA_PITCH, 0.009);

	glm::vec3 lightPos = glm::vec3(10.0, 10.0, 0.0);
	shadowBox = new ShadowBox(-lightPos, camera.get(), 30.0f, 0.1f, 45.0f);

	std::cout << "State: " << state << std::endl;

	while (psi) {

		

		currTime = TimeManager::Instance().GetTime();
		if (currTime - lastTime < 0.0166667) {
			glfwPollEvents();
			continue;
		}
		lastTime = currTime;
		TimeManager::Instance().CalculateFrameRate(true);
		deltaTime = TimeManager::Instance().DeltaTime;
		psi = processInput(true);

		

		std::map<std::string, bool> collisionDetection;

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		
		if (state == PLAY) {

			diffTime = lastTime - startupTimer;

			glm::mat4 projection = glm::perspective(glm::radians(45.0f),
				(float)screenWidth / (float)screenHeight, 0.01f, 100.0f);


			axis = glm::axis(glm::quat_cast(jugadorGameObject->ModelMatrix));
			angleTarget = glm::angle(glm::quat_cast(jugadorGameObject->ModelMatrix));
			target = jugadorGameObject->ModelMatrix[3];


			if (std::isnan(angleTarget))
				angleTarget = 0.0;
			if (axis.y < 0)
				angleTarget = -angleTarget;

			camera->setCameraTarget(target);
			camera->updateCamera();
			view = camera->getViewMatrix();

			// Matriz de proyecci�n del shadow mapping
			glm::mat4 lightProjection = glm::mat4(1.0f), lightView = glm::mat4(1.0f);
			shadowBox->update(screenWidth, screenHeight);
			glm::vec3 centerBox = shadowBox->getCenter();
			glm::mat4 lightSpaceMatrix;
			lightView = glm::lookAt(centerBox, centerBox + glm::normalize(-lightPos), glm::vec3(0.0, 1.0, 0.0));
			lightProjection[0][0] = 2.0f / (shadowBox->getWidth());
			lightProjection[1][1] = 2.0f / (shadowBox->getHeight());
			lightProjection[2][2] = -2.0f / (shadowBox->getLength());
			lightProjection[3][3] = 1.0f;
			lightSpaceMatrix = lightProjection * lightView;
			shaderDepth.setMatrix4("lightSpaceMatrix", 1, false, glm::value_ptr(lightSpaceMatrix));


			// Settea la matriz de vista y projection al shader con solo color
			shader.setMatrix4("projection", 1, false, glm::value_ptr(projection));
			shader.setMatrix4("view", 1, false, glm::value_ptr(view));

			// Settea la matriz de vista y projection al shader con skybox
			shaderSkybox.setMatrix4("projection", 1, false,
				glm::value_ptr(projection));
			shaderSkybox.setMatrix4("view", 1, false,
				glm::value_ptr(glm::mat4(glm::mat3(view))));
			// Settea la matriz de vista y projection al shader con multiples luces
			shaderMulLighting.setMatrix4("projection", 1, false,
				glm::value_ptr(projection));
			shaderMulLighting.setMatrix4("view", 1, false,
				glm::value_ptr(view));
			shaderMulLighting.setMatrix4("lightSpaceMatrix", 1, false,
				glm::value_ptr(lightSpaceMatrix));
			// Settea la matriz de vista y projection al shader con multiples luces
			shaderTerrain.setMatrix4("projection", 1, false,
				glm::value_ptr(projection));
			shaderTerrain.setMatrix4("view", 1, false,
				glm::value_ptr(view));
			shaderTerrain.setMatrix4("lightSpaceMatrix", 1, false,
				glm::value_ptr(lightSpaceMatrix));
			// Settea la matriz de vista y projection al shader de particulas de Zombie blood
			shaderZombieBlood.setMatrix4("projection", 1, false, glm::value_ptr(projection));
			shaderZombieBlood.setMatrix4("view", 1, false, glm::value_ptr(view));
			// Settea la matriz de vista y projection al shader de particulas de Jugador caminando
			shaderJugadorCamina.setMatrix4("projection", 1, false, glm::value_ptr(projection));
			shaderJugadorCamina.setMatrix4("view", 1, false, glm::value_ptr(view));

		/*******************************************
		 * Propiedades Neblina
		 *******************************************/
		shaderMulLighting.setVectorFloat3("fogColor", glm::value_ptr(glm::vec3(0.5, 0.5, 0.4)));
		shaderMulLighting.setFloat("density", 0.01);
		shaderMulLighting.setFloat("gradient", 0.33);

		shaderTerrain.setVectorFloat3("fogColor", glm::value_ptr(glm::vec3(0.5, 0.5, 0.4)));
		shaderTerrain.setFloat("density", 0.01);
		shaderTerrain.setFloat("gradient", 0.33);

		shaderSkybox.setVectorFloat3("fogColor", glm::value_ptr(glm::vec3(0.5, 0.5, 0.4)));


			/*******************************************
			 * Propiedades Luz direccional
			 *******************************************/
			shaderMulLighting.setVectorFloat3("viewPos", glm::value_ptr(camera->getPosition()));
			shaderMulLighting.setVectorFloat3("directionalLight.light.ambient", glm::value_ptr(glm::vec3(0.2, 0.2, 0.2)));
			shaderMulLighting.setVectorFloat3("directionalLight.light.diffuse", glm::value_ptr(glm::vec3(0.5, 0.5, 0.5)));
			shaderMulLighting.setVectorFloat3("directionalLight.light.specular", glm::value_ptr(glm::vec3(0.2, 0.2, 0.2)));
			shaderMulLighting.setVectorFloat3("directionalLight.direction", glm::value_ptr(glm::vec3(-0.707106781, -0.707106781, 0.0)));

			/*******************************************
			 * Propiedades Luz direccional Terrain
			 *******************************************/
			shaderTerrain.setVectorFloat3("viewPos", glm::value_ptr(camera->getPosition()));
			shaderTerrain.setVectorFloat3("directionalLight.light.ambient", glm::value_ptr(glm::vec3(0.2, 0.2, 0.2)));
			shaderTerrain.setVectorFloat3("directionalLight.light.diffuse", glm::value_ptr(glm::vec3(0.5, 0.5, 0.5)));
			shaderTerrain.setVectorFloat3("directionalLight.light.specular", glm::value_ptr(glm::vec3(0.2, 0.2, 0.2)));
			shaderTerrain.setVectorFloat3("directionalLight.direction", glm::value_ptr(glm::vec3(-0.707106781, -0.707106781, 0.0)));

			
			

			/*******************************************
			 * 1.- We render the depth buffer
			 *******************************************/
			glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			// render scene from light's point of view
			glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
			glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
			glClear(GL_DEPTH_BUFFER_BIT);
			//glCullFace(GL_FRONT);
			prepareDepthScene();
			renderScene(false);
			//glCullFace(GL_BACK);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			/*******************************************
			 * 2.- We render the normal objects
			 *******************************************/
			glViewport(0, 0, screenWidth, screenHeight);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			prepareScene();
			glActiveTexture(GL_TEXTURE10);
			glBindTexture(GL_TEXTURE_2D, depthMap);
			shaderMulLighting.setInt("shadowMap", 10);
			shaderTerrain.setInt("shadowMap", 10);


			/*******************************************
			 * Skybox
			 *******************************************/
			GLint oldCullFaceMode;
			GLint oldDepthFuncMode;
			// deshabilita el modo del recorte de caras ocultas para ver las esfera desde adentro
			glGetIntegerv(GL_CULL_FACE_MODE, &oldCullFaceMode);
			glGetIntegerv(GL_DEPTH_FUNC, &oldDepthFuncMode);
			shaderSkybox.setFloat("skybox", 0);
			glCullFace(GL_FRONT);
			glDepthFunc(GL_LEQUAL);
			glActiveTexture(GL_TEXTURE0);
			skyboxSphere.render();
			glCullFace(oldCullFaceMode);
			glDepthFunc(oldDepthFuncMode);
			/*******************************************
			 * Update the position with alpha objects
			 *******************************************/
			//blendingUnsorted.find("particulasBlood")->second = glm::vec3(enemyCollection[0]->ModelMatrix[3]);
			//blendingUnsorted.find("particulasCamina")->second = glm::vec3(jugadorGameObject->ModelMatrix[3]);
			/*******************************************
			* Render the Scene Elements
			*******************************************/
			renderScene(true);

			/**********
			* Sorter with alpha objects
			*/
			std::map<float, std::pair<std::string, glm::vec3>> blendingSorted;
			std::map<std::string, glm::vec3>::iterator itblend;
			for (itblend = blendingUnsorted.begin(); itblend != blendingUnsorted.end(); itblend++) {
				float distanceFromView = glm::length(camera->getPosition() - itblend->second);
				blendingSorted[distanceFromView] = std::make_pair(itblend->first, itblend->second);
			}

			/**********
		 * Render de las transparencias
		 */
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glDisable(GL_CULL_FACE);
			for (std::map<float, std::pair<std::string, glm::vec3> >::reverse_iterator it = blendingSorted.rbegin(); it != blendingSorted.rend(); it++) {
				if ((it->second.first.compare("particulasBlood") == 0) && ZombieBloodBool) {
					cTimeParticlesZombie++;
					if (cTimeParticlesZombie >= 30) {
						ZombieBloodBool = false;
						cTimeParticlesZombie = 0;
					}
					//Render de particulas de sangre
					glm::mat4 modelMatrixParticulasBlood = glm::mat4(1.0);
					modelMatrixParticulasBlood = glm::translate(modelMatrixParticulasBlood, it->second.second);
					modelMatrixParticulasBlood[3][1] = terrain.getHeightTerrain(modelMatrixParticulasBlood[3][0],
						modelMatrixParticulasBlood[3][2]) + 2.0f;//+5.0f es para la altura
					currTimeParticulasZombieBlood = TimeManager::Instance().GetTime();//currTimeParticulasZombieBlood, lastTimeParticulasZombieBlood
					if (currTimeParticulasZombieBlood - lastTimeParticulasZombieBlood > 1.5) {//Tiempo de la animacion
						lastTimeParticulasZombieBlood = currTimeParticulasZombieBlood;
					}
					glDepthMask(GL_FALSE);
					glPointSize(10.0f);
					glActiveTexture(GL_TEXTURE0);
					glBindTexture(GL_TEXTURE_2D, textureParticleZombieBloodID);
					shaderZombieBlood.turnOn();
					// Llamando al shader
					shaderZombieBlood.setFloat("Time", float(currTimeParticulasZombieBlood - lastTimeParticulasZombieBlood));
					shaderZombieBlood.setFloat("ParticleLifetime", 1.25f);//Este valor es para que duren mas las particulas
					shaderZombieBlood.setInt("ParticleTex", 0);
					shaderZombieBlood.setVectorFloat3("Gravity", glm::value_ptr(glm::vec3(0.0f, 0.1f, 0.0f)));//Este valor es para que vayan hacia arriba las particulas
					shaderZombieBlood.setMatrix4("model", 1, false, glm::value_ptr(modelMatrixParticulasBlood));
					glBindVertexArray(VAOZB);
					glDrawArrays(GL_POINTS, 0, numParticulasZB);
					glDepthMask(GL_TRUE);
					shaderZombieBlood.turnOff();
				}
				else if ((it->second.first.compare("particulasCamina") == 0) && JugadorCaminaBool) {
					cTimeParticlesJugadorCamina++;
					if (cTimeParticlesJugadorCamina >= 30) {
						ZombieBloodBool = false;
						cTimeParticlesJugadorCamina = 0;
					}
					//Render de particulas de sangre
					glm::mat4 modelMatrixParticulasCamina = glm::mat4(1.0);
					modelMatrixParticulasCamina = glm::translate(modelMatrixParticulasCamina, it->second.second);
					modelMatrixParticulasCamina[3][1] = terrain.getHeightTerrain(modelMatrixParticulasCamina[3][0],
						modelMatrixParticulasCamina[3][2]) + 0.0f;//+5.0f es para la altura
					currTimeParticulasJugadorCamina = TimeManager::Instance().GetTime();//currTimeParticulasZombieBlood, lastTimeParticulasZombieBlood
					if (currTimeParticulasJugadorCamina - lastTimeParticulasJugadorCamina > 1.5) {//Tiempo de la animacion
						lastTimeParticulasJugadorCamina = currTimeParticulasJugadorCamina;
						std::cout << "Aqui entra 1 " << std::endl;
					}
					glDepthMask(GL_FALSE);
					glPointSize(10.0f);
					glActiveTexture(GL_TEXTURE0);
					glBindTexture(GL_TEXTURE_2D, textureParticleJugadorCaminaID);
					shaderJugadorCamina.turnOn();
					// Llamando al shader
					shaderJugadorCamina.setFloat("Time", float(currTimeParticulasJugadorCamina - lastTimeParticulasJugadorCamina));
					shaderJugadorCamina.setFloat("ParticleLifetime", 1.25f);//Este valor es para que duren mas las particulas
					shaderJugadorCamina.setInt("ParticleTex", 0);
					shaderJugadorCamina.setVectorFloat3("Gravity", glm::value_ptr(glm::vec3(0.0f, 0.1f, 0.0f)));//Este valor es para que vayan hacia arriba las particulas
					shaderJugadorCamina.setMatrix4("model", 1, false, glm::value_ptr(modelMatrixParticulasCamina));
					glBindVertexArray(VAOZB);
					glDrawArrays(GL_POINTS, 0, numParticulasJC);
					glDepthMask(GL_TRUE);
					shaderJugadorCamina.turnOff();
				}
			}
			glEnable(GL_CULL_FACE);

			/*******************************************
			 * Creacion de colliders
			 * IMPORTANT do this before interpolations
			 *******************************************/

			mayowGameObject->UpdateColliderOBB(-90.0f, glm::vec3(1, 0, 0), glm::vec3(0.021, 0.021, 0.021), glm::vec3(0.75));
			addOrUpdateColliders(collidersOBB, "mayow", mayowGameObject->GetOBB(), mayowGameObject->ModelMatrix);

			jugadorGameObject->UpdateColliderOBB(-90.0f, glm::vec3(1, 0, 0), glm::vec3(0.005, 0.015, 0.015), glm::vec3(1.0));
			addOrUpdateColliders(collidersOBB, "jugador", jugadorGameObject->GetOBB(), jugadorGameObject->ModelMatrix);

			for (size_t i = 0; i < enemyCollection.size(); i++)
			{
				enemyCollection[i]->UpdateColliderOBB(-90.0f, glm::vec3(1, 0, 0), glm::vec3(0.008, 0.062, 0.018), glm::vec3(1.0));
				addOrUpdateColliders(collidersOBB, "enemy" + std::to_string(i), enemyCollection[i]->GetOBB(), enemyCollection[i]->ModelMatrix);
			}

			for (size_t i = 0; i < bulletCollection.size(); i++)
			{

				bulletCollection[i]->UpdateColliderOBB(0.0f, glm::vec3(1, 0, 0), glm::vec3(0.5, 0.5, 0.5), glm::vec3(1.0));
				addOrUpdateColliders(collidersOBB, "bullet" + std::to_string(i), bulletCollection[i]->GetOBB(), bulletCollection[i]->ModelMatrix);

			}



			AbstractModel::SBB modelColliderRock;
			glm::mat4 modelMatrixColliderRock = glm::mat4(matrixModelRock); // Mantenemos las transformaciones
			modelMatrixColliderRock = glm::scale(modelMatrixColliderRock, glm::vec3(1.0));
			modelMatrixColliderRock = glm::translate(modelMatrixColliderRock, modelRock.getSbb().c); // Trasladamos el collider al modelo en si obteniendo el centro.
			modelColliderRock.c = modelMatrixColliderRock[3];
			modelColliderRock.ratio = modelRock.getSbb().ratio * 1.0; //Se multiplica por el escalamiento definido anteriormente. 
			addOrUpdateColliders(collidersSBB, "rock", modelColliderRock, matrixModelRock); //Metodo sobrecargado para actualizar los colliders con el estado anterior a la colision



			/*******************************************
			 * Render de colliders
			 *******************************************/

#if _DEBUG_FLAG
			for (std::map<std::string, std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> >::iterator it =
				collidersOBB.begin(); it != collidersOBB.end(); it++) {
				glm::mat4 matrixCollider = glm::mat4(1.0);
				matrixCollider = glm::translate(matrixCollider, std::get<0>(it->second).c);
				matrixCollider = matrixCollider * glm::mat4(std::get<0>(it->second).u);
				matrixCollider = glm::scale(matrixCollider, std::get<0>(it->second).e * 2.0f);
				boxCollider.setColor(glm::vec4(1.0, 1.0, 1.0, 1.0));
				boxCollider.enableWireMode();
				boxCollider.render(matrixCollider);
			}

			for (std::map<std::string, std::tuple<AbstractModel::SBB, glm::mat4, glm::mat4> >::iterator it =
				collidersSBB.begin(); it != collidersSBB.end(); it++) {
				glm::mat4 matrixCollider = glm::mat4(1.0);
				matrixCollider = glm::translate(matrixCollider, std::get<0>(it->second).c);
				matrixCollider = glm::scale(matrixCollider, glm::vec3(std::get<0>(it->second).ratio * 2.0f));
				sphereCollider.setColor(glm::vec4(1.0, 1.0, 1.0, 1.0));
				sphereCollider.enableWireMode();
				sphereCollider.render(matrixCollider);
			}
#endif

			/*******************************************
			 * Test Colisions
			 *******************************************/
			 // Detector de colisiones SBB
			for (std::map<std::string, std::tuple<AbstractModel::SBB, glm::mat4, glm::mat4>>::iterator it = collidersSBB.begin(); it != collidersSBB.end(); it++)
			{
				bool isCollision = false;
				for (std::map<std::string, std::tuple<AbstractModel::SBB, glm::mat4, glm::mat4>>::iterator jt = collidersSBB.begin(); jt != collidersSBB.end(); jt++)
				{
					bool isCollision = false;
					if (it != jt && testSphereSphereIntersection(
						std::get<0>(it->second), std::get<0>(jt->second))) {
#if _DEBUG_FLAG
						std::cout << "Hay colision entre: " << it->first << " y el modelo: " << jt->first << std::endl;
#endif																		
						isCollision = true;


					}
				}
				addOrUpdateCollisionDetection(collisionDetection, it->first, isCollision);
			}

			// Detector de colisiones OBB
			for (std::map<std::string, std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4>>::iterator it = collidersOBB.begin(); it != collidersOBB.end(); it++)
			{
				bool isCollision = false;
				//ZombieBlood = false;
				for (std::map<std::string, std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4>>::iterator jt = collidersOBB.begin(); jt != collidersOBB.end(); jt++)
				{

					if (it != jt && testOBBOBB(
						std::get<0>(it->second), std::get<0>(jt->second))) {

						//TODO: add debug flags
#if _DEBUG_FLAG
						std::cout << "Hay colision entre: " << it->first << " y el modelo: " << jt->first << std::endl;
#endif
						isCollision = true;

						//Colision enemygo con jugador hiere al jugador.
						if (it->first.compare("jugador") == 0 && jt->first.find("enemy") == 0) {
							vidas -= 1;
#if _DEBUG_FLAG
							std::cout << "JUGADOR HERIDO!: " << vidas << std::endl;
#endif
						}

						if (it->first.find("enemy") == 0 && jt->first.find("bullet") == 0) {
							
							

							if (bulletCollection.size() > 0) {

								//Obtenemos el indice de la bala por medio del indentificador generado por el collider.
								std::string strindex = jt->first.substr(6, 3);
								int index = std::stoi(strindex);

								//Validar que el indice este dentro del rango de las balas. pues esta sección se ejecuta por cada frame que hubo colisión.
								if (index >= 0 && index < bulletCollection.size()) {
									
									//Eliminar la bala de la colección de balas. y su collider
									bulletCollection.erase(bulletCollection.begin() + index);
									collidersOBB.erase(jt->first);

								}
								
							}


							if (enemyCollection.size() > 0) {
							
								//Obtenemos el indice del enemigo por medio del indentificador generado por el collider.
								std::string strindex = it->first.substr(5, 3);
								int index = std::stoi(strindex);


								blendingUnsorted.find("particulasBlood")->second = glm::vec3(enemyCollection[index]->ModelMatrix[3]);

								//Validar que el indice este dentro del rango de los enemigos. pues esta sección se ejecuta por cada frame que hubo colisión.
								if (index >= 0 && index < enemyCollection.size()) {

									//Marcar al zombie como muerto y borrar su collider.
									//enemyCollection.erase(enemyCollection.begin() + index);
									enemyCollection[index]->isDeath = true;
									collidersOBB.erase(it->first);
									
								}



								ZombieBloodBool = true;
							}



						}


					}
				}
				addOrUpdateCollisionDetection(collisionDetection, it->first, isCollision);
			}

			// Detector de colisiones Esfera Objeto
			for (std::map<std::string, std::tuple<AbstractModel::SBB, glm::mat4, glm::mat4>>::iterator it = collidersSBB.begin(); it != collidersSBB.end(); it++)
			{
				bool isCollision = false;
				for (std::map<std::string, std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4>>::iterator jt = collidersOBB.begin(); jt != collidersOBB.end(); jt++)
				{
					if (testSphereOBox(std::get<0>(it->second), std::get<0>(jt->second))) {
#if _DEBUG_FLAG
						std::cout << " Hay colision del " << it->first << " y el modelo: " << jt->first << std::endl;
#endif						
						isCollision = true;
						addOrUpdateCollisionDetection(collisionDetection, jt->first, true);
					}
				}
				addOrUpdateCollisionDetection(collisionDetection, it->first, isCollision);
			}

			/*******************************************
			 * Comportamiendo de Colliders
			 *******************************************/
			std::map<std::string, bool>::iterator itCollision; //Iterador de colisiones 
			for (itCollision = collisionDetection.begin(); itCollision != collisionDetection.end(); itCollision++)
			{
				//Recuperar el estado del mapa de colisiones. 
				std::map<std::string, std::tuple<AbstractModel::SBB, glm::mat4, glm::mat4>>::iterator sbbBuscado = collidersSBB.find(itCollision->first);
				std::map<std::string, std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4>>::iterator obbBuscado = collidersOBB.find(itCollision->first);


				// Se encontr�
				if (sbbBuscado != collidersSBB.end()) {
					//Determinar NO hubo colision 
					if (!itCollision->second) {
						//Se actualiza la matriz de transformacion. La actual pasa a ser la matriz anterior. 
						addOrUpdateColliders(collidersSBB, itCollision->first);
					}
				}

				// Se realiza el mismo proceso pero para la OBB
				if (obbBuscado != collidersOBB.end()) {
					if (!itCollision->second) {
						addOrUpdateColliders(collidersOBB, itCollision->first);
					}
					else {
						//Lista de todos los objetos con colision 
						if (itCollision->first.compare("mayow") == 0) {
							//Estamos obteniendo el valor de la primera matriz de transformacion de la tupla que forma parte del obbbuscado.
							//modelMatrixMayow = std::get<1>(obbBuscado->second);
							mayowGameObject->ModelMatrix = std::get<1>(obbBuscado->second);
						}

						if (itCollision->first.compare("jugador") == 0) {
							jugadorGameObject->ModelMatrix = std::get<1>(obbBuscado->second);

						}

						if (itCollision->first.find("enemy") == 0) {
							
						}

						if (itCollision->first.find("bullet") == 0) {

							//Borramos las balas al chocar con cualquier cosa
							collidersOBB.erase(itCollision->first);
						}

					}
				}



			}


			//Generacion de un rayo
			glm::mat4 modelMatrixRayMay = glm::mat4(mayowGameObject->ModelMatrix);
			modelMatrixRayMay = glm::translate(modelMatrixRayMay, glm::vec3(0, 1, 0)); //Trasladmos un poco para no quedar a la altura del pie
			float maxDistanceRay = 10.0;
			glm::vec3 rayDirection = modelMatrixRayMay[2];
			glm::vec3 origen = modelMatrixRayMay[3]; //Punto inicial
			glm::vec3 mid = origen + rayDirection * (maxDistanceRay / 2.0f); //Partiendo de la ecuacion canonica de la recta.
			glm::vec3 targetRay = origen + rayDirection * maxDistanceRay;

			modelMatrixRayMay[3] = glm::vec4(mid, 1.0);
			modelMatrixRayMay = glm::rotate(modelMatrixRayMay, glm::radians(90.0f), glm::vec3(1, 0, 0));
			modelMatrixRayMay = glm::scale(modelMatrixRayMay, glm::vec3(0.05f, maxDistanceRay, 0.05f));

			//RayModel.render(modelMatrixRayMay);

			//Rayo por modelos
			for (size_t i = 0; i < enemyCollection.size(); i++)
			{
				//Generacion de un rayo
				glm::mat4 modelMatrixRayEnemy = glm::mat4(enemyCollection[i]->ModelMatrix);
				modelMatrixRayEnemy = glm::translate(modelMatrixRayEnemy, glm::vec3(0, 1, 0)); //Trasladmos un poco para no quedar a la altura del pie
				float maxDistanceRay = 5.0;
				glm::vec3 rayDirection = modelMatrixRayEnemy[2];
				glm::vec3 origen = modelMatrixRayEnemy[3]; //Punto inicial
				glm::vec3 mid = origen + rayDirection * (maxDistanceRay / 2.0f); //Partiendo de la ecuacion canonica de la recta.
				glm::vec3 targetRay = origen + rayDirection * maxDistanceRay;

				modelMatrixRayEnemy[3] = glm::vec4(mid, 1.0);
				modelMatrixRayEnemy = glm::rotate(modelMatrixRayEnemy, glm::radians(90.0f), glm::vec3(1, 0, 0));
				modelMatrixRayEnemy = glm::scale(modelMatrixRayEnemy, glm::vec3(0.05f, maxDistanceRay, 0.05f));
				//modelMatrixRayEnemy = glm::rotate(modelMatrixRayEnemy, glm::radians(90.0f), glm::vec3(0, 0, 1));
				//RayModel.render(modelMatrixRayEnemy);
			}

			//Realizar la colision rayo contra esfera. 
			std::map<std::string, std::tuple<AbstractModel::SBB, glm::mat4, glm::mat4> >::iterator itSBB;
			for (itSBB = collidersSBB.begin(); itSBB != collidersSBB.end(); itSBB++)
			{
				float tRint;
				if (raySphereIntersect(origen, targetRay, rayDirection, std::get<0>(itSBB->second), tRint)) {
#if _DEBUG_FLAG
					std::cout << "Colision del rayo con el modelo" << itSBB->first << std::endl;
#endif
				}
			}

			//Realizar la colision rayo contra caja
			std::map<std::string, std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> >::iterator itOBB;
			for (itOBB = collidersOBB.begin(); itOBB != collidersOBB.end(); itOBB++)
			{
				if (testRayOBB(origen, targetRay, std::get<0>(itOBB->second))) {
#if _DEBUG_FLAG
					std::cout << "Colision del rayo con el modelo" << itOBB->first << std::endl;
#endif
				}
			}


			




			// Constantes de animaciones
			animationIndex = 1;
			animationIndexPlayer = 1;

			/*******************************************
			* Render UI Elements , ejes relevantes X,Y
			*******************************************/

			drawGUIElement(textureHealthBarID, glm::vec3(0.08f, 0.15f, 0.15f), glm::vec3(-0.925f, 0.75f, 0.0f));
			

			glEnable(GL_BLEND);
			
			char buff[100];

			int hours = (int)diffTime / 3600;
			int minutes = (int)diffTime / 60;
			int seconds = (int)diffTime % 60;
			snprintf(buff, 100, "Tiempo: %d:%2d:%2d",hours,minutes,seconds);

			//Cada 10 segundos incrementar la velocidad de los zombies.
			if (seconds % 6 == 0) {
				VELOCIDAD_MOVIMIENTO_ZOMBIE += 0.00019; //Incrementar levemente la velocidad de los zombies
			}


			if (TIEMPO_ENTRE_ZOMBIES >= 1)
			{
				if (seconds % TIEMPO_ENTRE_ZOMBIES == 0) {
					generarZombie();
					TIEMPO_ENTRE_ZOMBIES += -1;
					VELOCIDAD_MOVIMIENTO_ZOMBIE += 0.00001;
					if (TIEMPO_ENTRE_ZOMBIES <= 0)
					{
						TIEMPO_ENTRE_ZOMBIES = 1;
					}

				}
			}

			if (enemyCollection.size() == 0)
			{
				generarZombie();
			}


			
#if _DEBUG_FLAG
			std::cout << "tiempo entre zombies: " << TIEMPO_ENTRE_ZOMBIES << std::endl;
#endif
			std::string cadenaTiempo = buff;

			modelText->render((cadenaTiempo), -0.15, 0.8, 1.0, 0.0, 0.0, 42);
			
			modelText->render(("Salud: " + std::to_string(vidas)), -0.97, 0.4, 0.01, 0.0, 1.0, 32);
			
			
			glDisable(GL_BLEND);


			if (vidas < 0) {
				state = GAMEOVER;
			}
				


			glfwSwapBuffers(window);


			/*******************************************
			 * OpenAL sound data
			 *******************************************/

			BGMusic0Pos[0] = camera->getPosition().x;
			BGMusic0Pos[1] = camera->getPosition().y;
			BGMusic0Pos[2] = camera->getPosition().z;

			GunPos[0] = camera->getPosition().x;
			GunPos[1] = camera->getPosition().y;
			GunPos[2] = camera->getPosition().z;


			alSourcefv(source[0], AL_POSITION, BGMusic0Pos);
			alSourcefv(source[1], AL_POSITION, GunPos);



			// Listener for the Camera.
			listenerPos[0] = camera->getPosition().x;
			listenerPos[1] = camera->getPosition().y;
			listenerPos[2] = camera->getPosition().z;
			alListenerfv(AL_POSITION, listenerPos);
			listenerOri[0] = camera->getFront().x;
			listenerOri[1] = camera->getFront().y;
			listenerOri[2] = camera->getFront().z;
			listenerOri[3] = camera->getUp().x;
			listenerOri[4] = camera->getUp().y;
			listenerOri[5] = camera->getUp().z;
			alListenerfv(AL_ORIENTATION, listenerOri);

			for (unsigned int i = 0; i < sourcesPlay.size(); i++) {
				if (sourcesPlay[i]) {
					sourcesPlay[i] = false;
					alSourcePlay(source[i]);
				}
			}
			
		}
					
		if (state == PAUSE) {
			/*******************************************
			* Render UI Elements , ejes relevantes X,Y
			*******************************************/
			drawGUIElement(texturePauseMenuID, glm::vec3(1.99f), glm::vec3(0.0f));
			glfwSwapBuffers(window);
		}

		if (state == TITLE) {
			/*******************************************
			* Render UI Elements , ejes relevantes X,Y
			*******************************************/
			drawGUIElement(textureTitleMenuID, glm::vec3(1.99f), glm::vec3(0.0f));
			glfwSwapBuffers(window);
		}

		if (state == GAMEOVER) {
			/*******************************************
			* Render UI Elements , ejes relevantes X,Y
			*******************************************/
			char buff[100];

			int hours = (int)diffTime / 3600;
			int minutes = (int)diffTime / 60;
			int seconds = (int)diffTime % 60;
			snprintf(buff, 100, "Survived! : %d:%2d:%2d", hours, minutes, seconds);
			std::string cadenaTiempo = buff;

			
			drawGUIElement(textureGameOverID, glm::vec3(1.99f), glm::vec3(0.0f));
			glEnable(GL_BLEND);
			modelText->render((cadenaTiempo), -0.25, 0.5, 1.0, 0.0, 0.0, 45);
			glDisable(GL_BLEND);
			glfwSwapBuffers(window);
			
		}
		
	}

}

void resetGame() {
	//Initial Positions???
	matrixModelRock = glm::translate(matrixModelRock, glm::vec3(-3.0, 0.0, 2.0));
	jugadorGameObject->ModelMatrix = glm::translate(jugadorGameObject->ModelMatrix, glm::vec3(3.0, 0.0, 2.0));
	mayowGameObject->ModelMatrix = glm::translate(mayowGameObject->ModelMatrix, glm::vec3(13.0f, 0.05f, -5.0f));
	mayowGameObject->ModelMatrix = glm::rotate(mayowGameObject->ModelMatrix, glm::radians(-90.0f), glm::vec3(0, 1, 0));
	zombieGameObject->ModelMatrix = glm::translate(zombieGameObject->ModelMatrix, glm::vec3(5.0, 0.0, 2.0));

	vidas = 1000;
	diffTime = 0;
	VELOCIDAD_MOVIMIENTO_ZOMBIE = 0.001f;

	enemyCollection.clear();
	collidersOBB.clear();
	
	TIEMPO_ENTRE_ZOMBIES = 10;

}
	
void generarZombie() {

	if (enemyCollection.size() < NUMERO_ENEMIGOS)
	{
		// Zombie
		zombieGameObject->animationIndex = 3;

		//zombieGameObject->Transform = glm::mat4(zombieGameObject->ModelMatrix);
		int rnd_x = distrib(generador);
		int rnd_z = distrib(generador);
		zombieGameObject->ModelMatrix[3][0] = rnd_x;
		zombieGameObject->ModelMatrix[3][2] = rnd_z;
		//zombieGameObject->ModelMatrix[3][1] = terrain.getHeightTerrain(zombieGameObject->ModelMatrix[3][0], zombieGameObject->ModelMatrix[3][2]);
		//zombieGameObject->Translate(glm::vec3(rnd_x, 0.0, rnd_z));
		//zombieGameObject->SetScale(glm::vec3(0.00015, 0.00015, 0.00015));
		// GENERAR UN NUEVO ZOMBIE HASTA LA CANTIDAD M�XIMA DE ENEMIGOS.
		if (enemyCollection.size() < NUMERO_ENEMIGOS) {
			//Hacer una copia del zombie
			GameObject* temp = new GameObject();
			memcpy(temp, zombieGameObject, sizeof(GameObject));
			//Guardamos un zombie generico en el vector de enemigos
			enemyCollection.push_back(temp);
		}
	}
}

void generarBala() {

	bulletGameObject->ModelMatrix = glm::mat4(jugadorGameObject->ModelMatrix);
	bulletGameObject->ModelMatrix = glm::translate(bulletGameObject->ModelMatrix, glm::vec3(0, 1.8, 0.5));
	bulletGameObject->ModelMatrix = glm::rotate(bulletGameObject->ModelMatrix, glm::radians(90.0f), glm::vec3(1, 0, 0));
	bulletGameObject->ModelMatrix = glm::scale(bulletGameObject->ModelMatrix, glm::vec3(0.2f, 0.2, 0.2f));
	GameObject* temp = new GameObject();
	memcpy(temp, bulletGameObject, sizeof(GameObject));

	bulletCollection.push_back(temp);
}

void inicializerParticulas(GLuint* initVel, GLuint* startTime, GLuint* VAO, GLuint* numParticulas) {
	// Generar los buffers para la sangre de zombie
	glGenBuffers(1, initVel);//Buffer para las velocidades iniciales
	glGenBuffers(1, startTime);//Buffer para los tiempos iniciales

	// Reservar el espacio para buffers
	int size = (*numParticulas) * 3 * sizeof(float);
	glBindBuffer(GL_ARRAY_BUFFER, *initVel);
	glBufferData(GL_ARRAY_BUFFER, size, NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, *startTime);
	glBufferData(GL_ARRAY_BUFFER, (*numParticulas) * sizeof(float), NULL, GL_STATIC_DRAW);

	// Llena la informacion del buffer de veocidades iniciales
	glm::vec3 v(0.0f, 0.0f, 0.0f);
	float velocity, theta, phi;
	GLfloat* data = new GLfloat[(*numParticulas) * 3];
	for (unsigned int i = 0; i < (*numParticulas); i++) {
		theta = glm::mix(0.0f, glm::pi<float>() / 6.0f, ((float)rand() / RAND_MAX));
		phi = glm::mix(0.0f, glm::two_pi<float>(), ((float)rand() / RAND_MAX));

		// Ecuacion de una esfera, las velidades se generan de acuerdo a lo esferico
		/*v.x = sinf(theta) * cosf(phi);
		v.y = cos(theta);
		v.z = sinf(theta) * sinf(phi);*/
		v.x = (float)rand() / RAND_MAX;
		v.y = cosf(theta) * sinf(phi);
		v.z = (float)rand() / RAND_MAX;

		velocity = glm::mix(0.6f, 0.8f, ((float)rand() / RAND_MAX));
		v = glm::normalize(v) * velocity;
		data[3 * i] = v.x;
		data[3 * i + 1] = v.y;
		data[3 * i + 2] = v.z;
	}
	glBindBuffer(GL_ARRAY_BUFFER, *initVel);
	glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);

	// Llenar la infomación del tiempo de inicio
	delete[] data;
	data = new GLfloat[*numParticulas];
	float time = 0.0;
	float rate = 0.00075f;
	for (unsigned int i = 0; i < (*numParticulas); i++) {
		data[i] = time;//time es el tiempo de "nacido" que tiene una partícula
		time += rate;
	}
	glBindBuffer(GL_ARRAY_BUFFER, *startTime);
	glBufferSubData(GL_ARRAY_BUFFER, 0, (*numParticulas) * sizeof(float), data);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	delete[] data;

	glGenVertexArrays(1, VAO);//Se pasa como referencia para poder modificarlo
	glBindVertexArray(*VAO);
	glBindBuffer(GL_ARRAY_BUFFER, *initVel);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);//DESPUES DE CUANTOS DATOS ESTÁ EL SIGUIENTE->penúltimo argumento
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, *startTime);
	glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
}

void renderPaticulas(bool pBool, int contador, int maxContador, double curr, double last, float tAnim, float tPar, float altura,
	std::map<float, std::pair<std::string, glm::vec3> >::reverse_iterator itera, GLuint* nPar, GLuint* textureID, Shader shader) {
	contador++;
	if (contador >= maxContador) {
		pBool = false;
		contador = 0;
	}
	//Render de particulas de sangre
	glm::mat4 modelMatrixParticulas = glm::mat4(1.0);
	modelMatrixParticulas = glm::translate(modelMatrixParticulas, itera->second.second);
	modelMatrixParticulas[3][1] = terrain.getHeightTerrain(modelMatrixParticulas[3][0],
		modelMatrixParticulas[3][2]) + altura;//+5.0f es para la altura
	curr = TimeManager::Instance().GetTime();//currTimeParticulasZombieBlood, lastTimeParticulasZombieBlood
	if (curr - last > tAnim) {//Tiempo de la animacion
		last = curr;
	}
	glDepthMask(GL_FALSE);
	glPointSize(10.0f);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, *textureID);
	shader.turnOn();
	// Llamando al shader
	shader.setFloat("Time", float(curr - last));
	shader.setFloat("ParticleLifetime", tPar);//Este valor es para que duren mas las particulas
	shader.setInt("ParticleTex", 0);
	shader.setVectorFloat3("Gravity", glm::value_ptr(glm::vec3(0.0f, 0.1f, 0.0f)));//Este valor es para que vayan hacia arriba las particulas
	shader.setMatrix4("model", 1, false, glm::value_ptr(modelMatrixParticulas));
	glBindVertexArray(VAOZB);
	glDrawArrays(GL_POINTS, 0, *nPar);
	glDepthMask(GL_TRUE);
	shader.turnOff();
}


void prepareScene() {

	modelRock.setShader(&shaderMulLighting);
	terrain.setShader(&shaderTerrain);
	jugadorGameObject->SetShader(&shaderMulLighting);
	zombieGameObject->SetShader(&shaderMulLighting);

	for (size_t i = 0; i < enemyCollection.size(); i++)
	{
		enemyCollection[i]->SetShader(&shaderMulLighting);
	}

}

void prepareDepthScene() {

	modelRock.setShader(&shaderDepth);
	terrain.setShader(&shaderDepth);
	jugadorGameObject->SetShader(&shaderDepth);
	zombieGameObject->SetShader(&shaderDepth);

	for (size_t i = 0; i < enemyCollection.size(); i++)
	{
		enemyCollection[i]->SetShader(&shaderDepth);
	}

}

void drawGUIElement(GLuint textureID, glm::vec3 scale, glm::vec3 pos) {

	// Dibujado de una textura sola en pantalla.
	shaderViewTexture.setMatrix4("view", 1, false, glm::value_ptr(glm::mat4(1.0f)));
	shaderViewTexture.setMatrix4("projection", 1, false, glm::value_ptr(glm::mat4(1.0f)));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureID);
	boxRenderImagen.setScale(scale);
	boxRenderImagen.setPosition(pos);
	boxRenderImagen.render();
}


void renderScene(bool renderParticles) {
	/*******************************************
	* Terrain Cesped
	*******************************************/
	glm::mat4 modelCesped = glm::mat4(1.0);
	modelCesped = glm::translate(modelCesped, glm::vec3(0.0, 0.0, 0.0));
	modelCesped = glm::scale(modelCesped, glm::vec3(200.0, 0.001, 200.0));
	// Se activa la textura del background
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureTerrainBackgroundID);
	shaderTerrain.setInt("backgroundTexture", 0);
	// Se activa la textura de tierra
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, textureTerrainRID);
	shaderTerrain.setInt("rTexture", 1);
	// Se activa la textura de hierba
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, textureTerrainGID);
	shaderTerrain.setInt("gTexture", 2);
	// Se activa la textura del camino
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, textureTerrainBID);
	shaderTerrain.setInt("bTexture", 3);
	// Se activa la textura del blend map
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, textureTerrainBlendMapID);
	shaderTerrain.setInt("blendMapTexture", 4);
	shaderTerrain.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(40, 40)));
	terrain.render();
	shaderTerrain.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(0, 0)));
	glBindTexture(GL_TEXTURE_2D, 0);

	/*******************************************
	 * Custom objects obj
	 *******************************************/


	 //Rock render
	matrixModelRock[3][1] = terrain.getHeightTerrain(matrixModelRock[3][0], matrixModelRock[3][2]);
	modelRock.render(matrixModelRock);
	// Forze to enable the unit texture to 0 always ----------------- IMPORTANT
	glActiveTexture(GL_TEXTURE0);



	/*******************************************
	* RENDER & ACTUALIZACI�N DE NUESTROS OBJETOS.
	*******************************************/
	// Jugador
	jugadorGameObject->ModelMatrix[3][1] = terrain.getHeightTerrain(jugadorGameObject->ModelMatrix[3][0], jugadorGameObject->ModelMatrix[3][2]);
	jugadorGameObject->animationIndex = animationIndexPlayer;
	jugadorGameObject->Transform = glm::mat4(jugadorGameObject->ModelMatrix);
	jugadorGameObject->SetScale(glm::vec3(0.00015, 0.00015, 0.00015));
	jugadorGameObject->Draw();

	/*
	*  COMPORTAMIENTO DEL ZOMBIE:
	* 
	* 1. GENERAR UN ZOMBIE EN UN LUGAR ALEATORIO.
	* 2. ASIGNAR LA DIRECCI�N AL JUGADOR.
	* 3. DESPLAZARLO CADA FRAME.
	* 4. VERIFICAR EL COLLIDER, SI MUERE REPRODUCIR ANIMACI�N, LUEGO DESTRUIR OBJETO.
	* 5. SI NO MUERE, SEGUIR DESPLAZANDO.
	
	*/

	
	for (size_t i = 0; i < enemyCollection.size(); i++)
	{


		//Verificar si el zombie esta vivo o no.
		if (enemyCollection[i]->isDeath)
		{
			enemyCollection[i]->animationIndex = 0;
			enemyCollection[i]->ModelMatrix = glm::translate(enemyCollection[i]->ModelMatrix, glm::vec3(0, -100, 0));
		}
		else {

			
			//Obtenemos la posici�n del jugador y del zombie
			glm::vec3 targetPos = glm::vec3(jugadorGameObject->ModelMatrix[3][0], jugadorGameObject->ModelMatrix[3][1], jugadorGameObject->ModelMatrix[3][2]);
			glm::vec3 currPos = glm::vec3(enemyCollection[i]->ModelMatrix[3][0], enemyCollection[i]->ModelMatrix[3][1], enemyCollection[i]->ModelMatrix[3][2]);

			//Obtenemos la direcci�n hacia el jugador
			glm::vec3 direction = glm::normalize(targetPos - currPos);
			glm::vec3 currDirection = glm::normalize(enemyCollection[i]->ModelMatrix[2]);


			glm::mat4 currentTransform = enemyCollection[i]->ModelMatrix;	//Almacenamos temporalmente la matriz de transformaci�n del zombie para poder rotarla.


			float dot = glm::dot(direction, currDirection);			//producto punto para encontrar despues el angulo entre las direcciones.
			glm::vec3 cross = glm::cross(direction, currDirection); //producto cruz para encontrar la direcci�n de rotaci�n. El vector perdendicular resultante es el eje Y. (UP)

			glm::clamp(dot, -1.0f, 1.0f);
			float angulo = acos(dot);

			if (direction != currDirection)
			{
				if (cross.y < 0)
					enemyCollection[i]->ModelMatrix = glm::rotate(currentTransform, glm::radians(angulo), glm::vec3(0, 1, 0));
				if (cross.y > 0)
					enemyCollection[i]->ModelMatrix = glm::rotate(currentTransform, glm::radians(-angulo), glm::vec3(0, 1, 0));
			}
			else
			{
				enemyCollection[i]->ModelMatrix = glm::rotate(currentTransform, glm::radians(0.0f), glm::vec3(0, 1, 0));
			}


			glm::vec3 goal = (enemyCollection[i]->ModelMatrix[2]);
			enemyCollection[i]->ModelMatrix = glm::translate(enemyCollection[i]->ModelMatrix, -currDirection * VELOCIDAD_MOVIMIENTO_ZOMBIE);
			enemyCollection[i]->animationIndex = 1;


			//Actualizar altura a la del terreno.

			try
			{
				enemyCollection[i]->ModelMatrix[3][1] = terrain.getHeightTerrain(enemyCollection[i]->ModelMatrix[3][0], enemyCollection[i]->ModelMatrix[3][2]);

			}
			catch (const std::exception&)
			{
				enemyCollection[i]->ModelMatrix[3][1] = 0;
				continue;
			}

			

			int seconds = (int)diffTime % 60;
			//Revisar cada 4 segundos si la animacion de muerte termino para borrar al zombi
			if (enemyCollection[i]->isDeath && enemyCollection[i]->animationIndex == 0)
			{
				if (seconds % 4 == 0)
					enemyCollection.erase(enemyCollection.begin() + i);
			}



			enemyCollection[i]->Transform = glm::mat4(enemyCollection[i]->ModelMatrix);
			enemyCollection[i]->SetScale(glm::vec3(0.00015, 0.00015, 0.00015));
			enemyCollection[i]->Draw();
		}


	}

	// LOGICA DE LA BALA.
	float maxDistanceBala = 27;

	if(!bulletCollection.empty() )
		for (size_t i = 0; i < bulletCollection.size(); i++)
		{

			//Calcular distancia entre bala y jugador
			glm::vec3 targetPos = glm::vec3(jugadorGameObject->ModelMatrix[3][0], jugadorGameObject->ModelMatrix[3][1], jugadorGameObject->ModelMatrix[3][2]);
			glm::vec3 currPos = glm::vec3(bulletCollection[i]->ModelMatrix[3][0], bulletCollection[i]->ModelMatrix[3][1], bulletCollection[i]->ModelMatrix[3][2]);
			float distance = glm::distance(targetPos, currPos);


			if (distance > maxDistanceBala)
			{
				bulletCollection[i]->ModelMatrix = glm::translate(bulletCollection[i]->ModelMatrix, glm::vec3(1000.0f, -1000.0f, -1000.0f)); //Mover al piso para no dejar el collider.
				bulletCollection[i]->UpdateColliderOBB(0.0f, glm::vec3(1, 0, 0), glm::vec3(0.05, 0.05, 0.05), glm::vec3(1.0));
				addOrUpdateColliders(collidersOBB, "bullet" + std::to_string(i), bulletCollection[i]->GetOBB(), glm::mat4(0.0f));
				collidersOBB.erase("bullet"+ std::to_string(i)); //Eliminar el collider de la bala.


				bulletCollection.erase(bulletCollection.begin() + i);
			}
			else {
				glm::vec3 currDirection = glm::normalize(bulletCollection[i]->ModelMatrix[2]);
				bulletCollection[i]->ModelMatrix = glm::translate(bulletCollection[i]->ModelMatrix, -currDirection * VELOCIDAD_BALA);
				bulletCollection[i]->Transform = glm::mat4(bulletCollection[i]->ModelMatrix);
				bulletCollection[i]->Draw();
			}
		}

	

}


int main(int argc, char** argv) {
	init(1350, 768, "Proyecto Final", false);

	//startScene(menu);
	applicationLoop();
	destroy();
	return 1;
}

