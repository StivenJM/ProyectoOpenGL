#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/shader_m.h>
#include <learnopengl/camera.h>
#include <learnopengl/model.h>

#include <iostream>

#define STB_IMAGE_IMPLEMENTATION 
#include <learnopengl/stb_image.h>


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
unsigned int loadTexture(char const* path, bool vertically = false);
void setupPointLight(Shader& lightingShader, std::vector<glm::vec3> pointLightPositions, unsigned int limite);
// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

//lighting
//glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
glm::vec3 lightPos(1.0f, 4.0f, 0.0f);

float baseAmbient = 0.3f;
float baseDiffuse = 1.0f;
float baseSpecular = 0.5f;
float fluctuationFrequency = 2.0f;
float fluctuationIntensity = 0.1f;
float umbralProximidad = 2.0f;
glm::vec3 colorFogata(1.0f, 0.63f, 0.23f); //color falso

// Ambient, diffuse and specular colors
glm::vec3 moonColors[] = {
    glm::vec3(0.05f, 0.05f, 0.05f),
    glm::vec3(0.44f, 0.44f, 0.83f),
    glm::vec3(0.0f, 0.0f, 0.0f)
};

glm::vec3 fireColors[] = {
    glm::vec3(0.1f, 0.1f, 0.1f),
    glm::vec3(1.0f, 0.63f, 0.23f),
    glm::vec3(1.0f, 0.63f, 0.0f)
};

glm::vec3 spotColors[] = {
    glm::vec3(0.0f, 0.0f, 0.0f),
    glm::vec3(0.2f, 0.2f, 0.2f),
    glm::vec3(0.2f, 0.2f, 0.2f)
};

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
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Exercise 16 Task 3", NULL, NULL);
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

    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    //stbi_set_flip_vertically_on_load(true);

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile shaders
    // -------------------------
    //Shader ourShader("shaders/tests/shader_exercise16_mloading.vs", "shaders/tests/shader_exercise16_mloading.fs");
    Shader lightingShader("shaders/tests/objetoLuzLuna.vs", "shaders/tests/objetoLuzLuna.fs");
    Shader lightCubeShader("shaders/tests/cuboLuz-LuzLuna.vs", "shaders/tests/cuboLuz-LuzLuna.fs");

    // load models
    // -----------
    //Model ourModel(FileSystem::getPath("resources/objects/backpack/backpack.obj"));
    Model modeloEscena("C:/Users/stive/Documents/GitHub/ProyectoOpenGL/models/EscenaCiudad/EscenaCiudad.obj");
    Model objetoCaja("C:/Users/stive/Documents/GitHub/ProyectoOpenGL/models/caja/caja.obj");
    //Model ourModel("C:/Users/Mishel Ramirez/ComputacionGrafica/Proyecto/ProyectoOpenGL/ProyectoOpenGL/models/fuego/Fuego.obj");

    //Model ourModel("C:/Users/Mishel Ramirez/ComputacionGrafica/Proyecto/ProyectoOpenGL/ProyectoOpenGL/models/cuidadPequena/Mishel.obj");

    // draw in wireframe
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    camera.MovementSpeed = 10; //Optional. Modify the speed of the camera 

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


    std::vector<glm::vec3> pointLightPositions = {
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(1.0f, 0.0f, 0.0f),
        glm::vec3(2.0f, 0.0f, 0.0f),
        glm::vec3(3.0f, 0.0f, 0.0f)
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

    /*lightingShader.use(); 
    lightingShader.setInt("nrPointLights", (int)pointLightPositions.size());*/
    
    // render loop
    // -----------
    while (!glfwWindowShouldClose(window)) 
    {
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // don't forget to enable shader before setting uniforms

        lightingShader.use();
        lightingShader.setVec3("viewPos", camera.Position);
        lightingShader.setFloat("shininess",4.0f);

        // directional light
        lightingShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
        lightingShader.setVec3("dirLight.ambient", moonColors[0]);
        lightingShader.setVec3("dirLight.diffuse", moonColors[1]);
        lightingShader.setVec3("dirLight.specular", moonColors[2]);
        // point light 1
        setupPointLight(lightingShader, pointLightPositions, (unsigned int)pointLightPositions.size());

        // spotLight
        lightingShader.setVec3("spotLight.position", camera.Position); 
        lightingShader.setVec3("spotLight.direction", camera.Front); 
        lightingShader.setVec3("spotLight.ambient", spotColors[0]);
        lightingShader.setVec3("spotLight.diffuse", spotColors[1]);
        lightingShader.setVec3("spotLight.specular", spotColors[2]);
        lightingShader.setFloat("spotLight.constant", 1.0f); 
        lightingShader.setFloat("spotLight.linear", 0.09); 
        lightingShader.setFloat("spotLight.quadratic", 0.032); 
        lightingShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(10.0f))); 
        lightingShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(18.0f))); 

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        lightingShader.setMat4("projection", projection);
        lightingShader.setMat4("view", view);

        // world transformation - CAJA
        glm::mat4 model = glm::mat4(1.0f);
        lightingShader.setMat4("model", model);
        modeloEscena.Draw(lightCubeShader);

        // world transformation - CAJA
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, glm::vec3(0.5f));
        lightingShader.setMat4("model", model);
        objetoCaja.Draw(lightingShader); 

        lightCubeShader.use();
        lightCubeShader.setMat4("projection", projection);
        lightCubeShader.setMat4("view", view);
        lightCubeShader.setVec3("lightColor", colorFogata); 

        // we now draw as many light bulbs as we have point lights.
        glBindVertexArray(lightCubeVAO);
        for (unsigned int i = 0; i < 4; i++)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, pointLightPositions[i]);
            model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
            lightCubeShader.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

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
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
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
    camera.ProcessMouseScroll(yoffset);
}

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

void setupPointLight(Shader& lightingShader, std::vector<glm::vec3> pointLightPositions, unsigned int limite)
{

    // Point lights
    for (unsigned int i = 0; i < limite; i++)
    {
        lightingShader.setVec3( "pointLights[" + std::to_string(i) + "].position", pointLightPositions[i]); 
        lightingShader.setVec3( "pointLights[" + std::to_string(i) + "].ambient",  fireColors[0]);
        lightingShader.setVec3( "pointLights[" + std::to_string(i) + "].diffuse",  fireColors[1]);
        lightingShader.setVec3( "pointLights[" + std::to_string(i) + "].specular", fireColors[2]);
        lightingShader.setFloat("pointLights[" + std::to_string(i) + "].constant", 1.0f); 
        lightingShader.setFloat("pointLights[" + std::to_string(i) + "].linear", 0.14f); 
        lightingShader.setFloat("pointLights[" + std::to_string(i) + "].quadratic", 0.07f); 
    }
}