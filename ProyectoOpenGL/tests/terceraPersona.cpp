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
void setupLighting(Shader& shader, glm::vec3 pointLightPositions[], glm::vec3 pointLightColors[], float distance, float fluctuationFrequency, float fluctuationIntensity);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
glm::vec3 cameraPosition(0.0f, 0.0f, 5.0f);
Camera camera(cameraPosition);

float lastX = 0.0f;
float lastY = 0.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

float xoffset = 0.0f;
float yoffset = 0.0f;

//float sensitivity = 0.01;

//lighting
//glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
glm::vec3 lightPos(1.0f, 4.0f, 0.0f);

float baseAmbient = 0.3f;
float baseDiffuse = 1.0f;
float baseSpecular = 0.5f;
float fluctuationFrequency = 2.0f;
float fluctuationIntensity = 0.1f;
float umbralProximidad = 2.0f;


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
    Shader lightCubeShader("shaders/tests/shader_B2T4_lightcube.vs", "shaders/tests/shader_B2T4_lightcube.fs");

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------

    float verticesTextura1[] = {
        // positions                // normals          // texture coords
                    //atras
        -0.5f, -0.5f, -0.5f,    0.0f,  0.0f, -1.0f,     0.1427f, 0.5f,
         0.5f, -0.5f, -0.5f,    0.0f,  0.0f, -1.0f,     0.3385f, 0.5f,
         0.5f,  0.5f, -0.5f,    0.0f,  0.0f, -1.0f,     0.3385f, 0.66f,
         0.5f,  0.5f, -0.5f,    0.0f,  0.0f, -1.0f,     0.3385f, 0.66f,
        -0.5f,  0.5f, -0.5f,    0.0f,  0.0f, -1.0f,     0.1427f, 0.66f,
        -0.5f, -0.5f, -0.5f,    0.0f,  0.0f, -1.0f,     0.1427f, 0.5f,

        //delante
        -0.5f, -0.5f,  0.5f,    0.0f,  0.0f,  1.0f,     0.5333f, 0.5f,
         0.5f, -0.5f,  0.5f,    0.0f,  0.0f,  1.0f,     0.73f, 0.5f,
         0.5f,  0.5f,  0.5f,    0.0f,  0.0f,  1.0f,     0.73f, 0.67f,
         0.5f,  0.5f,  0.5f,    0.0f,  0.0f,  1.0f,     0.73f, 0.67f,
        -0.5f,  0.5f,  0.5f,    0.0f,  0.0f,  1.0f,     0.5333f, 0.67f,
        -0.5f, -0.5f,  0.5f,    0.0f,  0.0f,  1.0f,     0.5333f, 0.5f,

        //izquierda
        -0.5f,  0.5f,  0.5f,    -1.0f,  0.0f,  0.0f,    0.5333f, 0.67f,
        -0.5f,  0.5f, -0.5f,    -1.0f,  0.0f,  0.0f,    0.3385f, 0.66f,
        -0.5f, -0.5f, -0.5f,    -1.0f,  0.0f,  0.0f,    0.3385f, 0.5f,
        -0.5f, -0.5f, -0.5f,    -1.0f,  0.0f,  0.0f,    0.3385f, 0.5f,
        -0.5f, -0.5f,  0.5f,    -1.0f,  0.0f,  0.0f,    0.5333f, 0.5f,
        -0.5f,  0.5f,  0.5f,    -1.0f,  0.0f,  0.0f,    0.5333f, 0.67f,

        //derecha
        0.5f,  0.5f,  0.5f,    1.0f,  0.0f,  0.0f,     0.73f, 0.67f,
        0.5f,  0.5f, -0.5f,    1.0f,  0.0f,  0.0f,     0.925f, 0.67f,
        0.5f, -0.5f, -0.5f,    1.0f,  0.0f,  0.0f,     0.925f, 0.5f,
        0.5f, -0.5f, -0.5f,    1.0f,  0.0f,  0.0f,     0.925f, 0.5f,
        0.5f, -0.5f,  0.5f,    1.0f,  0.0f,  0.0f,     0.73f, 0.5f,
        0.5f,  0.5f,  0.5f,    1.0f,  0.0f,  0.0f,     0.73f, 0.67f,

        //abajo
        -0.5f, -0.5f, -0.5f,    0.0f, -1.0f,  0.0f,     0.5333f, 0.26f,
         0.5f, -0.5f, -0.5f,    0.0f, -1.0f,  0.0f,     0.73f, 0.26f,
         0.5f, -0.5f,  0.5f,    0.0f, -1.0f,  0.0f,     0.73f, 0.5f,
         0.5f, -0.5f,  0.5f,    0.0f, -1.0f,  0.0f,     0.73f, 0.5f,
        -0.5f, -0.5f,  0.5f,    0.0f, -1.0f,  0.0f,     0.5333f, 0.5f,
        -0.5f, -0.5f, -0.5f,    0.0f, -1.0f,  0.0f,     0.5333f, 0.26f,

        //arriba
        -0.5f,  0.5f, -0.5f,    0.0f,  1.0f,  0.0f,     0.5333f, 0.67f,
         0.5f,  0.5f, -0.5f,    0.0f,  1.0f,  0.0f,     0.73f, 0.67f,
         0.5f,  0.5f,  0.5f,    0.0f,  1.0f,  0.0f,     0.73f, 0.92f,
         0.5f,  0.5f,  0.5f,    0.0f,  1.0f,  0.0f,     0.73f, 0.92f,
        -0.5f,  0.5f,  0.5f,    0.0f,  1.0f,  0.0f,     0.5333f, 0.92f,
        -0.5f,  0.5f, -0.5f,    0.0f,  1.0f,  0.0f,     0.5333f, 0.67f
    };

    // world space positions of our cubes
    glm::vec3 cubePositions[] = {
        //Cubo frente de la camara 
        glm::vec3(0.0f,  0.0f,  0.0f),
        glm::vec3(2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3(2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3(1.3f, -2.0f, -2.5f),
        glm::vec3(1.5f,  2.0f, -2.5f),
        glm::vec3(1.5f,  0.2f, -1.5f)
    };

    /*glm::vec3 cubePositions[] = {
        glm::vec3(-4.0f,  1.0f,  0.0f), // Estos datos son cambiados
        glm::vec3(-3.0f,  0.0f,  0.0f),
        glm::vec3(-2.0f,  0.0f,  0.0f),
        glm::vec3(-1.0f,  0.0f,  0.0f),
        glm::vec3(0.0f,  0.0f,  0.0f),
        glm::vec3(1.0f,  0.0f,  0.0f),
        glm::vec3(2.0f,  0.0f,  0.0f),
        glm::vec3(3.0f,  0.0f,  0.0f),
        glm::vec3(4.0f,  0.0f,  0.0f)
    };*/


    // positions of the point lights
    glm::vec3 pointLightPositions[] = {
        glm::vec3(0.3f, 0.3f, -2.0f),
        //glm::vec3(5.6f,  0.0f,  -5.0f),
        glm::vec3(15.3f, 0.0f, -3.0f),
        //glm::vec3(10.0f,  0.0f, -8.0f),
        //glm::vec3(0.0f,  0.0f, 3.0f)
    };

    glm::vec3 pointLightColors[] = {
        glm::vec3(1.0f, 0.5f, 0.0f), // Color cálido para la fogata
        glm::vec3(1.0f, 0.5f, 0.0f),
        glm::vec3(1.0f, 0.5f, 0.0f),
        glm::vec3(1.0f, 0.5f, 0.0f)
    };

    glm::vec3 firePositions[] = {
        glm::vec3(-15.3f, 0.0f, -3.0f),
        glm::vec3(0.3f, 0.3f, -2.0f),
        glm::vec3(15.3f, 0.0f, -3.0f)
    };

    unsigned int VAO, VBO1;
    glGenVertexArrays(1, &VAO);

    /*  Primer VBO  */
    glGenBuffers(1, &VBO1);
    glBindBuffer(GL_ARRAY_BUFFER, VBO1);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesTextura1), verticesTextura1, GL_STATIC_DRAW);
    glBindVertexArray(VAO);
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    //texture attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0); // Desenlazar VBO


    // configure el VAO de la luz (VBO permanece igual; los vértices son los mismos para el objeto de luz que también es un cubo 3D)
    unsigned int lightCubeVAO;
    glGenVertexArrays(1, &lightCubeVAO);
    glBindVertexArray(lightCubeVAO);
    // tenga en cuenta que actualizamos la zancada del atributo de posición de la lámpara para reflejar 
    // los datos actualizados del búfer
    glBindBuffer(GL_ARRAY_BUFFER, VBO1);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Desenlazar VAO
    glBindVertexArray(0);

    // Desenlazar los VBO y el VAO

    // load and create textures 
    // -------------------------
    unsigned int texture1 = loadTexture("textures/tests/Texture1.png", true);
    unsigned int texture2 = loadTexture("textures/tests/YuiTexture.png", true);
    unsigned int texture3 = loadTexture("textures/tests/AsuzaTexture.png", true);
    // shader configuration
    // --------------------
    ourShader.use();
    ourShader.setInt("material.diffuse", 0);

    // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
    // -------------------------------------------------------------------------------------------
    /*ourShader.use();
    ourShader.setInt("texture1", 0);*/

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

        // Posiciones de las fogatas
    

    for (int i = 0; i < 3; ++i) {
        float distance = glm::distance(camera.Position, firePositions[i]);
        setupLighting(ourShader, pointLightPositions, pointLightColors, distance, fluctuationFrequency, fluctuationIntensity);
    }

        // render
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ourShader.use();
        ourShader.setVec3("viewPos", camera.Position); 
        ourShader.setFloat("material.shininess", 32.0f);

        // Parámetros de la luz de la luna (luz direccional)
        glm::vec3 moonDirection = glm::vec3(-0.5f, -1.0f, -0.5f); // Dirección de la luz lunar 
        glm::vec3 moonAmbient = glm::vec3(0.1f, 0.1f, 0.2f);    // Color de luz tenue 
        glm::vec3 moonDiffuse = glm::vec3(0.2f, 0.2f, 0.4f);    // Color de luz más intensa 
        glm::vec3 moonSpecular = glm::vec3(0.3f, 0.3f, 0.5f);    // Color del brillo especular */

        // Enviar los parámetros de la luna al shader
        ourShader.setVec3("moonLight.direction", moonDirection); 
        ourShader.setVec3("moonLight.ambient", moonAmbient); 
        ourShader.setVec3("moonLight.diffuse", moonDiffuse); 
        ourShader.setVec3("moonLight.specular", moonSpecular);

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
        ourShader.setMat4("model", model);

        // bind vertex array
        glBindVertexArray(VAO);

        // render cubes
        for (unsigned int i = 0; i < 9; i++)
        {
            glm::mat4 model = glm::mat4(1.0f);
            glm::vec3 newPos = cubePositions[i];

            if (i == 0)
            {
                //Cubo frente de la camara
                // La textura se carga por defecto en el sampler 0
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, texture2);

                model = glm::translate(model, camera.Position);
                model = glm::rotate(model, glm::radians(camera.Yaw + 270.0f), glm::vec3(0.0f, 1.0f, 0.0f));

            }
            else if (i < 5)
            {
                // La textura se carga por defecto en el sampler 0
                glActiveTexture(GL_TEXTURE0);
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
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, texture3);

                // Scale the cube over time
                model = glm::translate(model, newPos);
                /*float scale = sin(timeValue) * 0.5f + 1.0f;
                model = glm::scale(model, glm::vec3(scale, scale, scale));*/
            }
            ourShader.setMat4("model", model);

            // Render the cube
            glBindVertexArray(VAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);

        }

        xoffset = 0.0f;
        yoffset = 0.0f;

        /*pointLightPositions[3].x = 12.5f * sin(timeValue);
        pointLightPositions[3].y = 9.5f * cos(timeValue);
        pointLightPositions[3].z = -20.5f * sin(timeValue);*/

        // dibuja también el objeto lámpara
        lightCubeShader.use();
        lightCubeShader.setMat4("projection", projection);
        lightCubeShader.setMat4("view", view);
        //Color de cubo
        lightCubeShader.setVec3("lightColor", glm::vec3(1.0f, 0.63f, 0.23f));

        glm::vec3 characterPosition = camera.Position;
        for (int i = 0; i < 4; ++i) {
            glm::vec3 lightPosition = pointLightPositions[i];
            float distance = glm::length(characterPosition - lightPosition);

            if (distance < umbralProximidad) {
                float fluctuation = sin(glfwGetTime() * fluctuationFrequency + i) * fluctuationIntensity;
                lightPosition.y += fluctuation;
            }

            lightCubeShader.setVec3("pointLights[" + std::to_string(i) + "].position", lightPosition);
        }

        for (int i = 0; i < 4; ++i) {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, pointLightPositions[i]);
            lightCubeShader.setMat4("model", model);

            // Render cube
            glBindVertexArray(VAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }


        /*lightCubeShader.use();
        lightCubeShader.setMat4("projection", projection);
        lightCubeShader.setMat4("view", view);
        lightCubeShader.setVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));

        // we now draw as many light bulbs as we have point lights.
        glBindVertexArray(lightCubeVAO);
        for (unsigned int i = 4; i < 5; i++)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, pointLightPositions[i]);
            model = glm::scale(model, glm::vec3(1.0f));
            lightCubeShader.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }*/

        // swap buffers and poll IO events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }



    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO1);
    glDeleteVertexArrays(1, &lightCubeVAO);

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

void setupLighting(Shader& shader, glm::vec3 pointLightPositions[], glm::vec3 pointLightColors[], float distance, float fluctuationFrequency, float fluctuationIntensity)
{
    float minDistance = 5.0f;  // Distancia mínima para aplicar fluctuación
    float maxDistance = 10.0f; // Distancia máxima para aplicar fluctuación
    float fluctuation = 0.0f;

    if (distance < minDistance) {
        fluctuation = sin(glfwGetTime() * fluctuationFrequency) * fluctuationIntensity;
    }
    else if (distance < maxDistance) {
        float t = (distance - minDistance) / (maxDistance - minDistance);
        fluctuation = sin(glfwGetTime() * fluctuationFrequency) * fluctuationIntensity * (1.0f - t);
    }

    shader.use();
    shader.setVec3("viewPos", camera.Position);

    // Directional light
    shader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
    shader.setVec3("dirLight.ambient", baseAmbient + fluctuation, baseAmbient * 0.5f + fluctuation, baseAmbient * 0.2f + fluctuation);
    shader.setVec3("dirLight.diffuse", baseDiffuse + fluctuation, baseDiffuse * 0.5f + fluctuation, baseDiffuse * 0.2f + fluctuation);
    shader.setVec3("dirLight.specular", baseSpecular + fluctuation, baseSpecular * 0.5f + fluctuation, baseSpecular * 0.2f + fluctuation);

    // Point lights
    for (unsigned int i = 0; i < 4; i++)
    {
        shader.setVec3("pointLights[" + std::to_string(i) + "].position", pointLightPositions[i]);
        shader.setVec3("pointLights[" + std::to_string(i) + "].ambient", pointLightColors[i] * 0.1f);
        shader.setVec3("pointLights[" + std::to_string(i) + "].diffuse", pointLightColors[i]);
        shader.setVec3("pointLights[" + std::to_string(i) + "].specular", pointLightColors[i]);
        shader.setFloat("pointLights[" + std::to_string(i) + "].constant", 1.0f);
        shader.setFloat("pointLights[" + std::to_string(i) + "].linear", 0.14f);
        shader.setFloat("pointLights[" + std::to_string(i) + "].quadratic", 0.07f);

    }

    shader.setVec3("spotLight.position", camera.Position.x, camera.Position.y, camera.Position.z);
    shader.setVec3("spotLight.direction", camera.Position.x, camera.Position.y, camera.Position.z);
    shader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
    shader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
    shader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
    shader.setFloat("spotLight.constant", 1.0f);
    shader.setFloat("spotLight.linear", 0.09f);
    shader.setFloat("spotLight.quadratic", 0.032f);
    shader.setFloat("spotLight.cutOff", glm::cos(glm::radians(10.0f)));
    shader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));

    
}