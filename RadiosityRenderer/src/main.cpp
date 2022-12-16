#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "graphics/shader.h"
#include "graphics/camera.h"

#include <iostream>

#include "loaders/modelLoader.h"
#define STB_IMAGE_IMPLEMENTATION
#include "loaders/stb_image.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 0.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
int isMouseEnabled = GLFW_CURSOR_DISABLED;

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

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    stbi_set_flip_vertically_on_load(true);

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile shaders
    // -------------------------
    Shader ourShader("src/resources/shaders/simpleShader.vert", "src/resources/shaders/simpleShader.frag");
    Shader lightCubeShader("src/resources/shaders/lightCubeShader.vert", "src/resources/shaders/lightCubeShader.frag");

    // load models
    // -----------
    Model ourModel(std::string("src/resources/models/cube/cube.obj"));

    // draw in wireframe
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);


    glm::vec3 cubePositions[7] =
    {
        glm::vec3(0.0f, -2.0f, -5.0f),  // Ground plane
        glm::vec3(0.0f, 2.0f, -5.0f),   // Ceiling plane
        glm::vec3(-2.0f, 0.0f, -5.0f),  // Left wall
        glm::vec3(2.0f, 0.0f, -5.0f),   // Right wall
        glm::vec3(0.0f, 0.0f, -7.0f),   // Back wall
        glm::vec3(0.4f, -0.7f, -5.5f), // Tall cube
        glm::vec3(0.4f, -0.7f, -4.0f), // Small cube
    };

    glm::vec3 cubeScales[7] =
    {
        glm::vec3(10.0f, 1.0f, 10.0f),    // Ground plane
        glm::vec3(10.0f, 1.0f, 10.0f),    // Ceiling plane
        glm::vec3(1.0f, 10.0f, 10.0f),    // Left wall
        glm::vec3(1.0f, 10.0f, 10.0f),    // Right wall
        glm::vec3(10.0f, 10.0f, 1.0f),    // Back wall
        glm::vec3(0.2f, 0.7f, 0.2f),    // Tall cube
        glm::vec3(0.1f, 0.1f, 0.1f),    // Small cube
    };

    std::pair<float, glm::vec3> cubeRotations[7] =
    {
        std::pair<float, glm::vec3>(0.0f, glm::vec3(0,1,0)),    // Ground plane
        std::pair<float, glm::vec3>(0.0f, glm::vec3(0,1,0)),    // Ceiling plane
        std::pair<float, glm::vec3>(0.0f, glm::vec3(0,1,0)),    // Left wall
        std::pair<float, glm::vec3>(0.0f, glm::vec3(0,1,0)),    // Right wall
        std::pair<float, glm::vec3>(0.0f, glm::vec3(0,1,0)),    // Back wall
        std::pair<float, glm::vec3>(0.8f, glm::vec3(0,1,0)),    // Tall cube
        std::pair<float, glm::vec3>(0.0f, glm::vec3(0,1,0)),    // Small cube
    };

    float grey = 211.0f / 255.0f;
    glm::vec3 darkishBlue = glm::vec3(0, 255.0f / 148.0f, 255 / 255);
    glm::vec3 lightishBlue = glm::vec3(0, 255.0f / 255.0f, 255.0f / 255.0f);

    glm::vec3 cubeColors[8] =
    {
        glm::vec3(grey, grey, grey),    // Ground plane
        glm::vec3(grey, grey, grey),    // Ceiling plane
        glm::vec3(1.0f, 0.0f, 0.0f),    // Left wall
        glm::vec3(0.0f, 1.0f, 0.0f),    // Right wall
        glm::vec3(grey, grey, grey),    // Back wall
        darkishBlue,                            // Tall cube
        lightishBlue,                           // Small cube
    };

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
        glClearColor(0.05f, 0.05f, 0.55f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // don't forget to enable shader before setting uniforms
        ourShader.use();

        for(int i = 0; i < 7; i++)
        {
            // view/projection transformations
            glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
            glm::mat4 view = camera.GetViewMatrix();
            ourShader.setMat4("projection", projection);
            ourShader.setMat4("view", view);

            // render the loaded model
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]); // translate it down so it's at the center of the scene
            model = glm::rotate(model, cubeRotations[i].first, cubeRotations[i].second);
            model = glm::scale(model, cubeScales[i]);	// it's a bit too big for our scene, so scale it down
            ourShader.setMat4("model", model);

            ourShader.setVec3("ObjectColor", cubeColors[i]);

            //Light properties
            ourShader.setVec3("LightPos", glm::vec3(0.0f, 0.7f, -5.0f));
            ourShader.setVec3("LightColor", glm::vec3(1.0f, 1.0f, 1.0f));

            ourModel.Draw(ourShader);
        }

        //Draw the light on the top
        lightCubeShader.use();

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        lightCubeShader.setMat4("projection", projection);
        lightCubeShader.setMat4("view", view);

        // render the loaded model
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 1.0f, -5.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(0.3f, 0.1f, 0.3f));	// it's a bit too big for our scene, so scale it down
        lightCubeShader.setMat4("model", model);

        ourModel.Draw(ourShader);

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

    if(glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)
    {
        if (isMouseEnabled == GLFW_CURSOR_DISABLED)
            isMouseEnabled = GLFW_CURSOR_NORMAL;
        else
            isMouseEnabled = GLFW_CURSOR_DISABLED;
    }

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, isMouseEnabled);
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