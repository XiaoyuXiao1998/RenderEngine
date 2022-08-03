#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../include/shader/shader.h"
#include "../include/scene/scene.h"
#include "../include/camera/camera.h"
#include"../include/light/dir_light.h"
#include"../include/EnvGenerator/EnvGenerator.h"

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.f, 4.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;


int main()
{


    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }



    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile shaders
    // -------------------------
    Shader* BallShader = new Shader("../../../shader/KullaConty.vert", "../../../shader/KullaConty.frag");
    Shader* SkyboxShader = new Shader("../../../shader/skyBox.vert", "../../../shader/skyBox.frag");


    // load models
    // -----------


    ModelTransform  transform1(0, 0.f, 0.f);

    Model* ourModel = new Model("../../../assets/chrome_ball/scene.gltf", transform1, MAT::Material_type::MATALIC);
    ModelTransform  transform2(0, 0, 0, 1, 1, 1);
    Model* Skybox = new Model("../../../assets/testObj/testObj.obj", transform2);

    //set prt parameters
  //  ourModel->loadPRTparameters("../../../assets//cubemap/CornellBox", *BallShader);


    vector<std::string> faces
    {
       "../../../assets/cubemap/GraceCathedral/posx.jpg",
       "../../../assets/cubemap/GraceCathedral/negx.jpg",
       "../../../assets/cubemap/GraceCathedral/posy.jpg",
       "../../../assets/cubemap/GraceCathedral/negy.jpg",
       "../../../assets/cubemap/GraceCathedral/posz.jpg",
       "../../../assets/cubemap/GraceCathedral/negz.jpg"
    };
  //  SkyboxShader->use();
   //SkyboxShader->setInt("skybox", 0);
   // Skybox->setCubeMapTextures(faces);
    Skybox->setCubeMapTextures("../../../assets/winter_sky_1k.hdr");

    Scene scene;
    scene.addModels(ourModel, BallShader);
    scene.addModels(Skybox, SkyboxShader);

    scene.setCamera(&camera);

    //reference :  3.594112
    scene.setTestAreaLight(glm::vec3(1, 1, 1), glm::vec3(1, 1, 1), true, glm::vec3(0, 32.94, 0.f), 1, 1);


    //**************set IBL************************
    //initialize ibl generator
    EnvGenerator* envGenerator;
    envGenerator = new EnvGenerator(128);

    //TODO:generate diffuse irradiance map
    envGenerator->setupGL();
    envGenerator->genIrradiance(glm::vec3(transform1.tx, transform1.ty, transform1.tz), Skybox);

    envGenerator->setupGL(preFilteredMap,true);
    envGenerator->genPrefiltedMap(glm::vec3(transform1.tx, transform1.ty, transform1.tz), Skybox);
    envGenerator->loadBRDFLUTMap();





    //set scene diffuse irradiacne map
    scene.setDiffuseIrradianceMap(envGenerator->getDiffuseIrradianceMapId());
    scene.setPrefilterMap(envGenerator->getPrefilterMapID());
    scene.setBRDFLUT(envGenerator->getBRDFLUTID());


    // scene.setDiffuseIrradianceMap(Skybox->mesh_materials[0]->getCubeMapId());





     // draw in wireframe
     //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    scene.setDepthMap();

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //scene.viewDepthMap();
        float time = (float)glfwGetTime() / 2;
        scene.renderScene(test_IBL_PBR);



        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
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

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}