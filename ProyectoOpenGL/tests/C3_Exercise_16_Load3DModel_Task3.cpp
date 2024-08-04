#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/shader_m.h>
#include <learnopengl/camera.h>
#include <game_object.h>

#include <iostream>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION 
#include <learnopengl/stb_image.h>
#include <irrklang/irrKlang.h>

// Variables estáticas para mantener el estado entre llamadas
irrklang::ISoundEngine* engine = nullptr;
irrklang::ISoundSource* stepSound = nullptr;
irrklang::ISoundSource* deer = nullptr;
//
irrklang::ISound* fireSound = nullptr;
//anadir la opsicion del la fogata
glm::vec3 firePosition(2.0f, 0.0f, 0.0f);


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(1.8f, 4.0f, 2.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

float direction = -camera.Yaw + 90.0f;



GameObject* jugador = nullptr;
std::vector<GameObject> objects;

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
    Shader ourShaderScene("shaders/tests/shader_exercise16_mloading.vs", "shaders/tests/shader_exercise16_mloading.fs");

    // load models
    // -----------
    //Model ourModel(FileSystem::getPath("resources/objects/backpack/backpack.obj"));
    Model ourModel("C:/Users/stive/Documents/GitHub/ProyectoOpenGL/models/EscenaCiudad/EscenaCiudad.obj");
    //Model ourModel("model/backpack/backpack.obj");
    
    jugador = new GameObject("C:/Users/stive/Documents/GitHub/ProyectoOpenGL/models/caja/caja.obj",
        glm::vec3(0.0f, 0.0f, 0.0f),
        0.0f,
        glm::vec3(0.0f, 1.0f, 0.0f),
        glm::vec3(0.5f, 0.5f, 0.5f),
        true);
    jugador->AddBoundingBox(glm::vec3(1.0f), glm::vec3(0.0f));

    Shader ourShader("shaders/tests/colisionesModelos1.vs", "shaders/tests/colisionesModelos1.fs");
    Shader bbShader("shaders/tests/shaderCollision.vs", "shaders/tests/shaderCollision.fs");

    glm::vec3 cubePositions[] = {
        glm::vec3(-4.0f,  4.0f,  0.0f),
        glm::vec3(-3.0f,  4.0f,  0.0f),
        glm::vec3(-2.0f,  4.0f,  0.0f),
        glm::vec3(-1.0f,  4.0f,  0.0f),
        glm::vec3( 0.0f,  4.0f,  0.0f),
        glm::vec3( 1.0f,  4.0f,  0.0f),
        glm::vec3( 2.0f,  4.0f,  0.0f),
        glm::vec3( 3.0f,  4.0f,  0.0f),
        glm::vec3( 4.0f,  4.0f,  0.0f)
    };

    for (const auto& pos : cubePositions)
    {
        GameObject aux = GameObject("C:/Users/stive/Documents/GitHub/ProyectoOpenGL/models/caja/caja.obj",
            pos,
            0.0f,
            glm::vec3(0.0f, 1.0f, 0.0f),
            glm::vec3(0.5f, 0.5f, 0.5f));
        aux.AddBoundingBox(glm::vec3(1.0f), glm::vec3(0.0f));
        objects.push_back(aux);
    }

    // draw in wireframe
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    camera.MovementSpeed = 15; //Optional. Modify the speed of the camera


     ///////////////////////////////////////////MUSICA/////////////////////////////////////////////////////////////////

  // Inicializar el motor de sonido
    engine = irrklang::createIrrKlangDevice();
    if (!engine) {
        std::cerr << "No se pudo inicializar el motor de sonido." << std::endl;
        return -1;
    }

    // Reproducir música de fondo
    engine->play2D("audio/fondo/musica_de_fondo.wav", true);

    // Cargar el sonido de pasos
    stepSound = engine->addSoundSourceFromFile("audio/efectos/pasos.wav");

    if (!stepSound) {
        std::cerr << "No se pudo cargar el sonido de pasos." << std::endl;
    }
    deer = engine->addSoundSourceFromFile("audio/efectos/deer.wav");

    if (!deer) {
        std::cerr << "No se pudo cargar el sonido de pasos." << std::endl;
    }

    fireSound = engine->play3D("audio/efectos/fuego.wav", irrklang::vec3df(firePosition.x, firePosition.y, firePosition.z), true, false, true);
    if (!fireSound) {
        std::cerr << "No se pudo cargar el sonido de fuego." << std::endl;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////


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
        jugador->Position = camera.Position;
        jugador->AngleRotation = direction;
        //std::cout << jugador->Position.x << " " << jugador->Position.y << " " << jugador->Position.z << std::endl;
        processInput(window);

        ///////////////////////////////////////////MUSICA/////////////////////////////////////////////////////////////////

        irrklang::vec3df listenerPos(camera.Position.x, camera.Position.y, camera.Position.z);
        irrklang::vec3df listenerLookDir(camera.Front.x, camera.Front.y, camera.Front.z);
        irrklang::vec3df listenerUpVector(camera.Up.x, camera.Up.y, camera.Up.z);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        
        
        engine->setListenerPosition(listenerPos, listenerLookDir, irrklang::vec3df(0, 0, 0), listenerUpVector);

        // render
        // ------
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // don't forget to enable shader before setting uniforms
        ourShaderScene.use();

         //view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -5.0f)) * camera.GetViewMatrix();
        ourShaderScene.setMat4("projection", projection);
        ourShaderScene.setMat4("view", view);

        // render the loaded model
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));	// it's a bit too big for our scene, so scale it down
        ourShaderScene.setMat4("model", model);
        ourModel.Draw(ourShaderScene);
  
        /////////////////////////////////////////////////renderización de jugador y zombie////////////////////////////////////////////////////////////////
       
        // activate shader
        ourShader.use();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);

        // render player
        model = jugador->GetModelMatrix();
        ourShader.setMat4("model", model);
        jugador->Render(ourShader);

        // Render player's Bounding Boxes
        bbShader.use();
        bbShader.setMat4("model", model);
        bbShader.setMat4("view", view);
        bbShader.setMat4("projection", projection);
        jugador->RenderBoundingBoxes(bbShader);


        // render cubes
        for (auto& object : objects)
        {
            ourShader.use();
            model = object.GetModelMatrix();
            ourShader.setMat4("model", model);
            object.Render(ourShader);

            // Render cube's bounding boxes
            bbShader.use();
            bbShader.setMat4("model", model);
            object.RenderBoundingBoxes(bbShader);
        }

        /////////////////////////////////////////////////fin renderización de jugador y zombie////////////////////////////////////////////////////////////////

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    delete jugador;

  

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
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
    // Se verifica si existe una colision
    // Se permite o deniega el bloqueo de movimiento
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS ||
        glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS ||
        glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS ||
        glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        direction = -camera.Yaw + 90.0f;
        if (!engine->isCurrentlyPlaying(stepSound)) {
            engine->play2D(stepSound, false);
        }
        DoCollisions(objects, jugador, &camera);
    }

    //If I want to stay in ground level (xz plane)
    camera.Position.y = 4.0f;
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