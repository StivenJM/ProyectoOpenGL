#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/shader_m.h>
#include <learnopengl/camera.h>

#define STB_IMAGE_IMPLEMENTATION 
#include <learnopengl/stb_image.h>

#include <iostream>

void processInput(GLFWwindow* window);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
unsigned int loadTexture(char const* path, bool vertically = false);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
glm::vec3 cameraPosition(0.0f, 0.0f, 5.0f);
Camera camera(cameraPosition);

// personaje
float direction = -camera.Yaw + 90.0f;

float lastX = 0.0f;
float lastY = 0.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

float xoffset = 0.0f;
float yoffset = 0.0f;

float sensitivity = 0.01;


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
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "B2T3 - Lorences Daniel - 1721155537", NULL, NULL);
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
    //Exercise 11 Task 3
    // -----------------------------
    glEnable(GL_DEPTH_TEST);


    // build and compile our shader zprogram
    // ------------------------------------
    Shader ourShader("shaders/tests/shader_terceraPersona.vs", "shaders/tests/shader_terceraPersona.fs");

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float verticesTextura1[] = {
        //atras
         -0.5f, -0.5f, -0.5f,        0.1427f, 0.5f,
          0.5f, -0.5f, -0.5f,        0.3385f, 0.5f,
          0.5f,  0.5f, -0.5f,        0.3385f, 0.66f,
          0.5f,  0.5f, -0.5f,        0.3385f, 0.66f,
         -0.5f,  0.5f, -0.5f,        0.1427f, 0.66f,
         -0.5f, -0.5f, -0.5f,        0.1427f, 0.5f,

         //adelante
        -0.5f, -0.5f,  0.5f,        0.5333f, 0.5f,
         0.5f, -0.5f,  0.5f,        0.73f, 0.5f,
         0.5f,  0.5f,  0.5f,        0.73f, 0.67f,
         0.5f,  0.5f,  0.5f,        0.73f, 0.67f,
        -0.5f,  0.5f,  0.5f,        0.5333f, 0.67f,
        -0.5f, -0.5f,  0.5f,        0.5333f, 0.5f,

        //izquierda
        -0.5f,  0.5f,  0.5f,        0.5333f, 0.67f,
        -0.5f,  0.5f, -0.5f,        0.3385f, 0.66f,
        -0.5f, -0.5f, -0.5f,        0.3385f, 0.5f,
        -0.5f, -0.5f, -0.5f,        0.3385f, 0.5f,
        -0.5f, -0.5f,  0.5f,        0.5333f, 0.5f,
        -0.5f,  0.5f,  0.5f,        0.5333f, 0.67f,

        //derecha
        0.5f,  0.5f,  0.5f,        0.73f, 0.67f,
        0.5f,  0.5f, -0.5f,        0.925f, 0.67f,
        0.5f, -0.5f, -0.5f,        0.925f, 0.5f,
        0.5f, -0.5f, -0.5f,        0.925f, 0.5f,
        0.5f, -0.5f,  0.5f,        0.73f, 0.5f,
        0.5f,  0.5f,  0.5f,        0.73f, 0.67f,

        //abajo
        -0.5f, -0.5f, -0.5f,        0.5333f, 0.26f,
         0.5f, -0.5f, -0.5f,        0.73f, 0.26,
         0.5f, -0.5f,  0.5f,        0.73f, 0.5f,
         0.5f, -0.5f,  0.5f,        0.73f, 0.5f,
        -0.5f, -0.5f,  0.5f,        0.5333f, 0.5f,
        -0.5f, -0.5f, -0.5f,        0.5333f, 0.26,

        //arriba
        -0.5f,  0.5f, -0.5f,        0.5333f, 0.67f,
         0.5f,  0.5f, -0.5f,        0.73f, 0.67f,
         0.5f,  0.5f,  0.5f,        0.73f, 0.92f,
         0.5f,  0.5f,  0.5f,        0.73f, 0.92f,
        -0.5f,  0.5f,  0.5f,        0.5333f, 0.92f,
        -0.5f,  0.5f, -0.5f,        0.5333f, 0.67f
            };

    // world space positions of our cubes
    /*glm::vec3 cubePositions[] = {
        glm::vec3(0.0f,  0.0f,  0.0f),
        glm::vec3(2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3(2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3(1.3f, -2.0f, -2.5f),
        glm::vec3(1.5f,  2.0f, -2.5f),
        glm::vec3(1.5f,  0.2f, -1.5f)
    };*/

    glm::vec3 cubePositions[] = {
        glm::vec3(-4.0f,  1.0f,  0.0f), // Estos datos son cambiados
        glm::vec3(-3.0f,  0.0f,  0.0f),
        glm::vec3(-2.0f,  0.0f,  0.0f),
        glm::vec3(-1.0f,  0.0f,  0.0f),
        glm::vec3(0.0f,  0.0f,  0.0f),
        glm::vec3(1.0f,  0.0f,  0.0f),
        glm::vec3(2.0f,  0.0f,  0.0f),
        glm::vec3(3.0f,  0.0f,  0.0f),
        glm::vec3(4.0f,  0.0f,  0.0f)
    };

    unsigned int VAO, VBO1;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    /*  Primer VBO  */
    glGenBuffers(1, &VBO1);
    glBindBuffer(GL_ARRAY_BUFFER, VBO1);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesTextura1), verticesTextura1, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0); // Desenlazar VBO

    // Desenlazar VAO
    glBindVertexArray(0);

    // Desenlazar los VBO y el VAO

    // load and create textures 
    // -------------------------
    unsigned int texture1 = loadTexture("textures/tests/Texture1.png", true);
    unsigned int texture2 = loadTexture("textures/tests/YuiTexture.png", true);
    unsigned int texture3 = loadTexture("textures/tests/AsuzaTexture.png", true);

    // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
    // -------------------------------------------------------------------------------------------
    ourShader.use();
    ourShader.setInt("texture1", 0);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        float timeValue = glfwGetTime();
        deltaTime = timeValue - lastFrame;
        lastFrame = timeValue;

        // input
        processInput(window);

        // render
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // activate shader
        ourShader.use();

        // set projection matrix
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        ourShader.setMat4("projection", projection);

        // set view matrix
        glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -5.0f)) * camera.GetViewMatrix();
        ourShader.setMat4("view", view);

        // set model matrix
        glm::mat4 model = glm::mat4(1.0f);

        // bind vertex array
        glBindVertexArray(VAO);

        // render cubes
        for (unsigned int i = 0; i < 9; i++)
        {
            model = glm::mat4(1.0f);
            glm::vec3 newPos = cubePositions[i];

            if (i == 0)
            {
                // La textura se carga por defecto en el sampler 0
                glBindTexture(GL_TEXTURE_2D, texture2);

                model = glm::translate(model, camera.Position);
                model = glm::rotate(model, glm::radians(direction), glm::vec3(0.0f, 1.0f, 0.0f));

            }
            else if (i < 5)
            {
                // La textura se carga por defecto en el sampler 0
                glBindTexture(GL_TEXTURE_2D, texture1);
                /*
                // Move the cube over time
                newPos.x += sin(timeValue + i) * 2.0f;
                newPos.y += cos(timeValue + i) * 2.0f;
                newPos.z += sin(timeValue + i) * 2.0f;*/
                model = glm::translate(model, newPos);
            }
            else if (i < 9)
            {
                // La textura se carga por defecto en el sampler 0
                glBindTexture(GL_TEXTURE_2D, texture3);

                // Scale the cube over time
                model = glm::translate(model, newPos);
                /*float scale = sin(timeValue) * 0.5f + 1.0f;
                model = glm::scale(model, glm::vec3(scale, scale, scale));*/
            }
            ourShader.setMat4("model", model);

            // Render the cube
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        xoffset = 0.0f;
        yoffset = 0.0f;

        // swap buffers and poll IO events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }



    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO1);

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

    // Si se pulsa cualquier tecla de direccion se actualiza el angulo del personaje
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS || 
        glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS ||
        glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS ||
        glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        direction = -camera.Yaw + 90.0f;

    //If I want to stay in ground level (xz plane)
    camera.Position.y = 0.0f;
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
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    xoffset = xpos - lastX;
    yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}


// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}

// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int loadTexture(char const* path, bool vertically)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    stbi_set_flip_vertically_on_load(vertically);
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}
