#include"Scene.h"
#include"Objects.h"
#include"Global.h"
#include"Plane.h"
#include"Sphere.h"

Camera camera(glm ::vec3(0.0f, 0.0f, 3.0f));

float deltaTime = 0.0f;
float lastFrame = 0.0f;

float mouse_left = 0;
float mouse_right = 0;
float lastx = 0, lasty = 0;

void processInput(GLFWwindow* window);
void mouse_call_back(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void renderQuad();
void renderCube();

int main() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "RTR", NULL, NULL);

	if (window == NULL)
	{
		std::cout << "error to create window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window); 
	glfwSetCursorPosCallback(window, mouse_call_back);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetScrollCallback(window, scroll_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "error to load glad" << std::endl;
		glfwTerminate();
		return -1;
	}

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	LoadShader("shader/cube/cube.vs", "shader/cube/cube.fs", nullptr, "cubeshader");
	LoadShader("shader/plane/plane.vs", "shader/plane/plane.fs", nullptr, "planeshader");
	LoadShader("shader/csm/csm.vs", "shader/csm/csm.fs", "shader/csm/csm.gs", "csm");
	LoadShader("shader/hdr/hdr.vs", "shader/hdr/hdr.fs", nullptr, "hdr");
	LoadShader("shader/light/light.vs", "shader/light/light.fs", nullptr, "light");
	LoadShader("shader/Deferred/gbuffer2.vs", "shader/Deferred/gbuffer2.fs", nullptr, "gbuffer2");
	LoadShader("shader/ssao/ssao.vs", "shader/ssao/ssao.fs", nullptr, "ssao");
	LoadShader("shader/ssao/ssaoblur.vs", "shader/ssao/ssaoblur.fs", nullptr, "ssaoblur");
	LoadShader("shader/sphere/sphere.vs", "shader/sphere/sphere.fs", nullptr, "sphere");
	LoadShader("shader/skybox/hdr-cube.vs", "shader/skybox/hdr-cube.fs",nullptr,"hdr-cube");
	LoadShader("shader/skybox/sky.vs", "shader/skybox/sky.fs", nullptr, "sky");
	LoadShader("shader/pbr/irradiance.vs", "shader/pbr/irradiance.fs", nullptr, "irradiance");
	LoadShader("shader/pbr/prefilter.vs", "shader/pbr/prefilter.fs", nullptr, "prefilter");
	LoadShader("shader/pbr/LUT.vs", "shader/pbr/LUT.fs", nullptr, "LUT");
	LoadShader("shader/ssr/ssr.vs", "shader/ssr/ssr.fs", nullptr, "SSR");
	LoadShader("shader/Deferred/Defer.vs", "shader/Deferred/Defer.fs", nullptr, "Deferred");

	LoadTexture("image/texture/brickwall.jpg", "brickwall");
	LoadTexture("image/texture/brickwall_normal.jpg", "brickwall_normal");

	LoadTexture("image/pbr/wall/albedo.png", "wall_albedo");
	LoadTexture("image/pbr/wall/normal.png", "wall_normal");
	LoadTexture("image/pbr/wall/metallic.png", "wall_metallic");
	LoadTexture("image/pbr/wall/roughness.png", "wall_roughness");
	LoadTexture("image/pbr/wall/ao.png", "wall_ao");

	LoadTexture("image/pbr/rusted_iron/albedo.png","rusted_iron_albedo");
	LoadTexture("image/pbr/rusted_iron/normal.png", "rusted_iron_normal");
	LoadTexture("image/pbr/rusted_iron/metallic.png", "rusted_iron_metallic");
	LoadTexture("image/pbr/rusted_iron/roughness.png", "rusted_iron_roughness");
	LoadTexture("image/pbr/rusted_iron/ao.png", "rusted_iron_ao");

	LoadTexture("image/pbr/gold/albedo.png", "gold_albedo");
	LoadTexture("image/pbr/gold/normal.png", "gold_normal");
	LoadTexture("image/pbr/gold/metallic.png", "gold_metallic");
	LoadTexture("image/pbr/gold/roughness.png", "gold_roughness");
	LoadTexture("image/pbr/gold/ao.png", "gold_ao");

	LoadTexture("image/pbr/plastic/albedo.png", "plastic_albedo");
	LoadTexture("image/pbr/plastic/normal.png", "plastic_normal");
	LoadTexture("image/pbr/plastic/metallic.png", "plastic_metallic");
	LoadTexture("image/pbr/plastic/roughness.png", "plastic_roughness");
	LoadTexture("image/pbr/plastic/ao.png", "plastic_ao");

	Light light(std::make_shared<Shader>(GetShader("light")), glm::mat4(1.0f), glm::vec3(500.0f), glm::vec3(10.0f),1000.0f);

	std::shared_ptr<Objects> objs = std::make_shared<Objects>();
	std::shared_ptr<Object> cube = std::make_shared<Cube>(std::make_shared<Shader>(GetShader("cubeshader")), glm::mat4(1.0), "cube", GetTexture("brickwall"), GetTexture("brickwall_normal"));
	std::shared_ptr<Object> plane = std::make_shared<Plane>(std::make_shared<Shader>(GetShader("planeshader")), glm::mat4(1.0), "plane", GetTexture("wall_albedo"), GetTexture("wall_normal"), GetTexture("wall_metallic"), GetTexture("wall_roughness"), GetTexture("wall_ao"));
	std::shared_ptr<Object> sphere = std::make_shared<Sphere>(std::make_shared<Shader>(GetShader("sphere")), glm::mat4(1.0), "sphere", GetTexture("gold_albedo"), GetTexture("gold_normal"), GetTexture("gold_metallic"), GetTexture("gold_roughness"), GetTexture("gold_ao"));
	objs->add(plane);
	objs->add(sphere);
	//objs->add(cube);
	std::shared_ptr<Draw> draw = std::make_shared<Draw>(objs);
	Scene scene(window,draw);
	scene.set_objs(objs->get_objs());
	scene.init();

// CSM shadow MAP
//----------------------------------------------------------------------------------------------------------
	unsigned int lightFBO;
	glGenFramebuffers(1, &lightFBO);

	glGenTextures(1, &shadowMap);
	glBindTexture(GL_TEXTURE_2D_ARRAY, shadowMap);
	glTexImage3D( 
		GL_TEXTURE_2D_ARRAY, 0, GL_DEPTH_COMPONENT32F, depthmapresolution, depthmapresolution, int(shadowlayer.size()) + 1,
		0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	constexpr float bordercolor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BORDER_COLOR, bordercolor);

	glBindFramebuffer(GL_FRAMEBUFFER, lightFBO);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadowMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	int status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!";
		throw 0;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	unsigned int  matUBO;
	glGenBuffers(1, &matUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, matUBO);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4x4) * 16, NULL, GL_STATIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, matUBO);  //  设置索引
	glBindBuffer(GL_UNIFORM_BUFFER, 0); //  个人理解 uniform 缓冲空状态  （不再对当前 target 使用任何缓存对象）

// HDR MAP
//----------------------------------------------------------------------------------------------------------
	unsigned int hdr;
	glGenTextures(1, &hdr);
	glBindTexture(GL_TEXTURE_2D, hdr);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	unsigned int rboDepth;
	glGenRenderbuffers(1, &rboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);  // 存储深度信息，在渲染时交换信息
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT);

	unsigned int hdrFBO;
	glGenFramebuffers(1, &hdrFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, hdr, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	GetShader("hdr").use();
	GetShader("hdr").setInt("hdr",0);
	GetShader("hdr").setInt("ssaoblur", 1);
	GetShader("hdr").setInt("ssr", 2);

// G-buffer 
//----------------------------------------------------------------------------------------------------------
	unsigned int gbuffer2;
	glGenFramebuffers(1, &gbuffer2);
	glBindFramebuffer(GL_FRAMEBUFFER, gbuffer2);
	// 在绑定到GL_FRAMEBUFFER目标之后，所有的读取和写入帧缓冲的操作将会影响当前绑定的帧缓冲。

	glGenTextures(1, &gPosition);
	glBindTexture(GL_TEXTURE_2D, gPosition);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition,0);

	glGenTextures(1, &gNormal);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);

	glGenTextures(1, &gAlbedo);
	glBindTexture(GL_TEXTURE_2D, gAlbedo);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedo, 0);

	glGenTextures(1, &gParameter);
	glBindTexture(GL_TEXTURE_2D, gParameter);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, gParameter, 0);

	unsigned int attachment2[4] = { GL_COLOR_ATTACHMENT0,GL_COLOR_ATTACHMENT1,GL_COLOR_ATTACHMENT2,GL_COLOR_ATTACHMENT3  };
	glDrawBuffers(4, attachment2);

	unsigned int rbodepth;
	glGenRenderbuffers(1, &rbodepth);
	glBindRenderbuffer(GL_RENDERBUFFER, rbodepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT); // 指定存储在 renderbuffer 中图像的宽高以及颜色格式
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbodepth);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "error to compile gbuffer2" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	GetShader("Deferred").use();
	GetShader("Deferred").setInt("gPosition", 0);
	GetShader("Deferred").setInt("gNormal", 1);
	GetShader("Deferred").setInt("gAlbedo", 2);
	GetShader("Deferred").setInt("gParameter", 3);
	GetShader("Deferred").setInt("irradianceMap", 4);
	GetShader("Deferred").setInt("prefilterMap", 5);
	GetShader("Deferred").setInt("LUTMap", 6);
	GetShader("Deferred").setInt("shadowMap", 7);

// ssao
//----------------------------------------------------------------------------------------------------------
	std::uniform_real_distribution<GLfloat> randomFloat(0.0f, 1.0f);
	std::default_random_engine generator;
	std::vector<glm::vec3> ssaoKernel; //  采样核心
	for (size_t i = 0; i < 64; i++)
	{
		glm::vec3 sample(  // 半球向量
			randomFloat(generator) * 2.0 - 1.0,
			randomFloat(generator) * 2.0 - 1.0,
			randomFloat(generator)
		);
		sample = glm::normalize(sample);
		sample *= randomFloat(generator);
		float scale = float(i) / 64.0f;
		scale = lerp(0.1f, 1.0f, scale * scale);
		sample *= scale;
		ssaoKernel.push_back(sample);
	} 

	std::vector<glm::vec3> ssaonoise;  // 随机偏转
	for (size_t i = 0; i < 16; i++)
	{
		glm::vec3 noise(
			randomFloat(generator) * 2.0 - 1.0,
			randomFloat(generator) * 2.0 - 1.0,
			0.0
		);
		ssaonoise.push_back(noise);
	}

	glGenTextures(1, &noiseTexture);
	glBindTexture(GL_TEXTURE_2D, noiseTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaonoise);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	unsigned int ssaoFBO;
	glGenFramebuffers(1, &ssaoFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);

	glGenTextures(1, &ssao);
	glBindTexture(GL_TEXTURE_2D, ssao);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, SCR_WIDTH, SCR_HEIGHT, 0, GL_RED, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D , ssao, 0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "error to compile ssao";

	GetShader("ssao").use();
	GetShader("ssao").setInt("gPosition", 0);
	GetShader("ssao").setInt("gNormal", 1);
	GetShader("ssao").setInt("noiseTexture", 2);

	unsigned int ssaoBlurFBO;
	glGenFramebuffers(1, &ssaoBlurFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);

	glGenTextures(1, &ssaoblur);
	glBindTexture(GL_TEXTURE_2D, ssaoblur);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, SCR_WIDTH, SCR_HEIGHT, 0, GL_RED, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoblur,0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "error to compile ssaoblur" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	GetShader("ssaoblur").use();
	GetShader("ssaoblur").setInt("ssao", 0);

// 获取hdr环境贴图
//----------------------------------------------------------------------------------------------------------
	unsigned int captureFBO, captureRBO;
	glGenFramebuffers(1, &captureFBO);
	glGenRenderbuffers(1, &captureRBO);
		
	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "error to compile captureFBO" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	unsigned int cubeMap;
	glGenTextures(1, &cubeMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);
	for (unsigned int i = 0; i < 6; i++)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 512, 512, 0, GL_RGB, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); 
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);//  纹理放大和缩小时的过滤方式，当纹理坐标和图片不匹配时采用的采样方式

	glm::mat4 capturePro = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
	glm::mat4 captureViews[] = {
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
	};

	unsigned int hdrtexture = loadHDRTexture("image/hdr/newport_loft.hdr");
	GetShader("hdr-cube").use();
	GetShader("hdr-cube").setInt("hdrskybox", 0);
	GetShader("hdr-cube").setMat4("projection", capturePro);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, hdrtexture);

	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glViewport(0, 0, 512, 512);
	for (unsigned int i = 0; i < 6; i++)
	{
		GetShader("hdr-cube").setMat4("view", captureViews[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, cubeMap, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		renderCube();
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	GetShader("sky").use();
	GetShader("sky").setInt("cubemap", 0);

// 计算间接光（漫反射）
//----------------------------------------------------------------------------------------------------------
	glGenTextures(1, &irradianceMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
	for (unsigned int i = 0; i < 6; i++)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 32, 32, 0, GL_RGB, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);

	GetShader("irradiance").use();
	GetShader("irradiance").setInt("cubeMap", 0);
	GetShader("irradiance").setMat4("projection", capturePro);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);

	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glViewport(0, 0, 32, 32);
	for (unsigned int  i = 0; i < 6; i++)
	{
		GetShader("irradiance").setMat4("view", captureViews[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradianceMap, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		renderCube();
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

// 计算间接光（镜面反射，环境光）
//----------------------------------------------------------------------------------------------------------
	glGenTextures(1, &prefilterMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
	for (unsigned int  i = 0; i < 6; i++)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 128, 128, 0,GL_RGB, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP); // 多级渐远纹理

	GetShader("prefilter").use();
	GetShader("prefilter").setInt("cubeMap", 0);
	GetShader("prefilter").setMat4("projection", capturePro);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);
	
	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	unsigned int maxMipLevels = 5;  //  对于每一个mip级别我们设置一种粗糙度
	for (unsigned int  mip = 0; mip < maxMipLevels; ++mip)
	{
		unsigned int mipWidth = 128 * std::pow(0.5, mip);
		unsigned int mipHeight = 128 * std::pow(0.5, mip);
		glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
		glViewport(0, 0, mipWidth, mipHeight);

		float roughness = (float)mip / (float)(maxMipLevels - 1);
		GetShader("prefilter").setFloat("roughness", roughness);
		for (unsigned int  i = 0; i < 6; i++)
		{
			GetShader("prefilter").setMat4("view", captureViews[i]);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, prefilterMap,mip);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			renderCube();
		}
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

// 计算间接光（镜面反射，BRDF）
//----------------------------------------------------------------------------------------------------------
	glGenTextures(1, &LUTMap);
	glBindTexture(GL_TEXTURE_2D, LUTMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, 512, 512, 0, GL_RG, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, LUTMap, 0);
	glViewport(0, 0, 512, 512);
	GetShader("LUT").use();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	renderQuad();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

//  全局反射 SSR
//----------------------------------------------------------------------------------------------------------
	unsigned int SSRFBO;
	glGenFramebuffers(1, &SSRFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, SSRFBO);

	unsigned int SSR;
	glGenTextures(1, &SSR);
	glBindTexture(GL_TEXTURE_2D, SSR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, SSR, 0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "SSR Blur Framebuffer not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	GetShader("SSR").use();
	GetShader("SSR").setInt("gPosition", 0);
	GetShader("SSR").setInt("gNormal", 1);
	GetShader("SSR").setInt("gAlbedo", 2);
	GetShader("SSR").setInt("gParameter", 3);

	int scrWidth, scrHeight;
	glfwGetFramebufferSize(window, &scrWidth, &scrHeight);
	glViewport(0, 0, scrWidth, scrHeight);

	while (!glfwWindowShouldClose(window))
	{
		globalcamera = camera;
		globallight = light;

		globalview = camera.GetViewMatrix();
		globalprojection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, nearplane, farplane);

		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window);
		
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// csm
		const auto lightMat = getLightSpaceMatrices();
		glBindBuffer(GL_UNIFORM_BUFFER, matUBO);
		for (size_t i = 0; i < lightMat.size(); i++)
			glBufferSubData(GL_UNIFORM_BUFFER, i * sizeof(glm::mat4x4), sizeof(glm::mat4x4), &lightMat[i]);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		glBindFramebuffer(GL_FRAMEBUFFER, lightFBO);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_TEXTURE_2D_ARRAY, shadowMap, 0);
		glViewport(0, 0, depthmapresolution, depthmapresolution);
		glClear(GL_DEPTH_BUFFER_BIT);
		glCullFace(GL_FRONT);
		auto temp = GetShader("csm");
		temp.use();
		draw->temp_render(temp);
		glCullFace(GL_BACK);

		//G-buffer2
		glBindFramebuffer(GL_FRAMEBUFFER, gbuffer2);
		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		temp = GetShader("gbuffer2");
		temp.use();
		temp.setMat4("view", globalview);
		temp.setMat4("projection", globalprojection);
		draw->temp_render(temp);

		// ssao
		glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		temp = GetShader("ssao");
		temp.use();
		for (size_t i = 0; i < 64; i++)
			temp.setVec3("sampler[" + std::to_string(i) + "]", ssaoKernel[i]);
		temp.setMat4("projection", globalprojection);
		temp.setMat4("view", globalview);
		temp.setMat4("projection", globalprojection);
		temp.setFloat("SCR_WIDTH", (float)SCR_WIDTH);
		temp.setFloat("SCR_HEIGHT", (float)SCR_HEIGHT);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, gPosition);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, gNormal);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, noiseTexture);
		renderQuad();

		//ssaoblur
		glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		temp = GetShader("ssaoblur");
		temp.use();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, ssao);
		renderQuad();

		// SSR
		glBindFramebuffer(GL_FRAMEBUFFER, SSRFBO);
		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		temp = GetShader("SSR");
		temp.use();
		temp.setMat4("projection", globalprojection);
		temp.setMat4("view", globalview);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, gPosition);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, gNormal);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, gAlbedo);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, gParameter);
		renderQuad();

		// 场景  延迟渲染
		glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);					
		temp = GetShader("Deferred");
		temp.use();
		temp.setMat4("fs_view", globalview);
		temp.setVec3("viewPos", globalcamera.Position);
		temp.setVec3("light.position", globallight.get_light_p());
		temp.setVec3("light.color", globallight.get_light_c());
		temp.setFloat("light.radius", globallight.get_light_r());
		temp.setFloat("far_plane", farplane);
		temp.setInt("countLayer", shadowlayer.size());
		for (size_t i = 0; i < shadowlayer.size(); i++)
			temp.setFloat("planeDistances[" + std::to_string(i) + "]", shadowlayer[i]);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, gPosition);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, gNormal);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, gAlbedo);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, gParameter);
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
		glActiveTexture(GL_TEXTURE6);
		glBindTexture(GL_TEXTURE_2D, LUTMap);
		glActiveTexture(GL_TEXTURE7);
		glBindTexture(GL_TEXTURE_2D_ARRAY, shadowMap);
		renderQuad();

		//  hdr to ldr
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		temp = GetShader("hdr");
		temp.use();
		temp.setFloat("exposure", 1.0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D,  hdr);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, ssaoblur);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, SSR);
		renderQuad();

		// 天空盒
		glBindFramebuffer(GL_READ_FRAMEBUFFER, gbuffer2);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // 写入到默认帧缓冲
		glBlitFramebuffer(0, 0, SCR_WIDTH, SCR_HEIGHT, 0, 0, SCR_WIDTH, SCR_HEIGHT, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		temp = GetShader("sky");
		temp.use();
		temp.setMat4("view", globalview);
		temp.setMat4("projection", globalprojection);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);
		renderCube();
		
		// operate
		scene.render();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	return 0;
}
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
}
void mouse_call_back(GLFWwindow* window,double xpos,double ypos)  {

	currentx = static_cast<float>(xpos);
	currenty = SCR_HEIGHT - static_cast<float>(ypos);

	if (mouse_left == 0)
	{
		lastx = currentx;
		lasty = currenty;
		camera.ProcessMouseMovement((lastx - currentx) * 0.1f, (lasty - currenty) * 0.1f);
	}
	else if (mouse_left == 1) {
		camera.ProcessMouseMovement((lastx - currentx) * 0.1f, (lasty - currenty) * 0.1f);
	}
	else if (mouse_left == 2) {
		mouse_left = 0;
	}

}
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && currentx >= 300.0f && currentx <= 1300.0f)
		mouse_left = 1;
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE && currentx >= 300.0f && currentx <= 1300.0f)
		mouse_left = 2;
}
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
}
	
unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad()
{
	if (quadVAO == 0)
	{
		float quadVertices[] = {
			// positions        // texture Coords
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		// setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	}
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}

unsigned int cubeVAO = 0;
unsigned int cubeVBO;
void renderCube() {
	if (cubeVAO == 0)
	{
		float cubevertices[] = {
			-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
			 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
			 1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
			 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
			-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
			-1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
			// front face
			-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
			 1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
			 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
			 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
			-1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
			-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
			// left face
			-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
			-1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
			-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
			-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
			-1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
			-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
			// right face
			 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
			 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
			 1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
			 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
			 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
			 1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
			// bottom face
			-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
			 1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
			 1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
			 1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
			-1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
			-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
			// top face
			-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
			 1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
			 1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
			 1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
			-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
			-1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
		};

		unsigned int VBO;
		glGenBuffers(1, &VBO);
		glGenVertexArrays(1, &cubeVAO);

		glBindVertexArray(cubeVAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(cubevertices), &cubevertices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));
		glEnableVertexAttribArray(2);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

	}

	glBindVertexArray(cubeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
}


//默认的帧缓冲默认一个颜色分量只占用8位(bits) 只有整数位，（0，255）
//pbr 为了遵守能量守恒定律，我们需要对漫反射光和镜面反射光之间做出明确的区分。当一束光线碰撞到一个表面的时候，
// 它就会分离成一个折射部分和一个反射部分。反射部分就是会直接反射开来而不会进入平面的那部分光线，
//这就是我们所说的镜面光照。而折射部分就是余下的会进入表面并被吸收的那部分光线，这也就是我们所说的漫反射光照。

//  brdf 计算的是入射光和反射光的关系