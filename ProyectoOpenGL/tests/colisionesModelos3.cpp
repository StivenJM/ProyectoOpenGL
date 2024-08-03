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

struct CollisionPair {
    BoundingBox* B1;
    BoundingBox* B2;

    CollisionPair(BoundingBox& b1, BoundingBox& b2) : B1(&b1), B2(&b2) {}
};

void processInput(GLFWwindow* window);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
CollisionPair* CheckCollision(GameObject& one, GameObject& two); // AABB - AABB
void DoCollisions();

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 5.0f));

float lastX = 0.0f;
float lastY = 0.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

float direction = -camera.Yaw + 90.0f;



GameObject* jugador = nullptr;
std::vector<GameObject> objects;

// bloqueo de movimiento GENERAL
bool blockNZ = false;
bool blockX = false;
bool blockZ = false;
bool blockNX = false;

// bloqueo de movimiento player
bool blockFordward = false;
bool blockRight = false;
bool blockBackward = false;
bool blockLeft = false;

std::vector<GameObject> objetosEnColision;

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

    jugador = new GameObject("D:/stive/Documentos/ProgramasVisualStudio/ProyectoOpenGL/ProyectoOpenGL/models/tests/caja/caja.obj",
        glm::vec3(0.0f, 0.0f, 0.0f),
        0.0f,
        glm::vec3(0.0f, 1.0f, 0.0f),
        glm::vec3(0.5f, 0.5f, 0.5f),
        true);
    jugador->AddBoundingBox(glm::vec3(1.0f), glm::vec3(0.0f));

    // build and compile our shader zprogram
    // ------------------------------------
    Shader ourShader("shaders/tests/colisionesModelos1.vs", "shaders/tests/colisionesModelos1.fs");
    Shader bbShader("shaders/tests/shaderCollision.vs", "shaders/tests/shaderCollision.fs");

    glm::vec3 cubePositions[] = {
        glm::vec3(-8.0f,  0.0f,  0.0f),
        glm::vec3(-6.0f,  0.0f,  0.0f),
        glm::vec3(-4.0f,  0.0f,  0.0f),
        glm::vec3(-2.0f,  0.0f,  0.0f),
        glm::vec3( 0.0f,  0.0f,  0.0f),
        glm::vec3( 2.0f,  0.0f,  0.0f),
        glm::vec3( 4.0f,  0.0f,  0.0f),
        glm::vec3( 6.0f,  0.0f,  0.0f),
        glm::vec3( 8.0f,  0.0f,  0.0f)
    };

    for (const auto& pos : cubePositions)
    {
        GameObject aux = GameObject("D:/stive/Documentos/ProgramasVisualStudio/ProyectoOpenGL/ProyectoOpenGL/models/tests/caja/caja.obj", 
            pos,
            0.0f,
            glm::vec3(0.0f, 1.0f, 0.0f),
            glm::vec3(1.0f, 1.0f, 1.0f));
        aux.AddBoundingBox(glm::vec3(1.0f), glm::vec3(0.0f));
        objects.push_back(aux);
    }

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        float timeValue = glfwGetTime();
        deltaTime = timeValue - lastFrame;
        lastFrame = timeValue;

        // input
        jugador->Position = camera.Position;
        jugador->AngleRotation = direction;
        //std::cout << jugador->Position.x << " " << jugador->Position.y << " " << jugador->Position.z << std::endl;
        processInput(window);

        // render
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // activate shader
        ourShader.use();

        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -5.0f)) * camera.GetViewMatrix();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);

        // render player
        glm::mat4 model = jugador->GetModelMatrix();
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

        // swap buffers and poll IO events
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

    /*if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS && !blockFordward)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS && !blockBackward)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS && !blockLeft)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS && !blockRight)
        camera.ProcessKeyboard(RIGHT, deltaTime);*/

    // Si se pulsa cualquier tecla de direccion se actualiza el angulo del personaje
    // Se verifica si existe una colision
    // Se permite o deniega el bloqueo de movimiento
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS ||
        glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS ||
        glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS ||
        glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        direction = -camera.Yaw + 90.0f;

        DoCollisions();

        // Actualiza la perspectiva del personaje para el bloqueo de movimiento
        int anguloVista = (int)camera.Yaw % 360;
        //std::cout << anguloVista << std::endl;

        if (anguloVista < 0)
        {
            anguloVista = 360 + anguloVista;
        }

        if (anguloVista < 45)
        {
            blockFordward = blockX;
            blockRight = blockZ;
            blockBackward = blockNX;
            blockLeft = blockNZ;
        }
        else if (anguloVista < 135)
        {
            blockFordward = blockZ;
            blockRight = blockNX;
            blockBackward = blockNZ;
            blockLeft = blockX;
        }
        else if (anguloVista < 225)
        {
            blockFordward = blockNX;
            blockRight = blockNZ;
            blockBackward = blockX;
            blockLeft = blockZ;
        }
        else if (anguloVista < 315)
        {
            blockFordward = blockNZ;
            blockRight = blockX;
            blockBackward = blockZ;
            blockLeft = blockNX;
        }
        else if (anguloVista < 360)
        {
            blockFordward = blockX;
            blockRight = blockZ;
            blockBackward = blockNX;
            blockLeft = blockNZ;
        }
    }

    //If I want to stay in ground level (xz plane)
    camera.Position.y = 0.0f;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
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

CollisionPair* CheckCollision(GameObject& one, GameObject& two) // AABB - AABB
{
    std::vector<BoundingBox> boundingBoxesOne = one.GetBoundingBoxes();
    std::vector<BoundingBox> boundingBoxesTwo = two.GetBoundingBoxes();

    for (auto& b1 : boundingBoxesOne)
    {
        for (auto& b2 : boundingBoxesTwo)
        {
            // Get separating axis
            std::vector<glm::vec2> sa;
            sa.push_back(glm::vec2(b1.front.x, b1.front.z));
            sa.push_back(glm::vec2(-b1.front.z, b1.front.x));
            sa.push_back(glm::vec2(b2.front.x, b2.front.z));
            sa.push_back(glm::vec2(-b2.front.z, b2.front.x));

            // Get plane XZ of bounding boxes
            std::vector<glm::vec2> verticesB1;
            verticesB1.push_back(glm::vec2(b1.vertices[3].x, b1.vertices[3].z));
            verticesB1.push_back(glm::vec2(b1.vertices[2].x, b1.vertices[2].z));
            verticesB1.push_back(glm::vec2(b1.vertices[6].x, b1.vertices[6].z));
            verticesB1.push_back(glm::vec2(b1.vertices[7].x, b1.vertices[7].z));

            std::vector<glm::vec2> verticesB2;
            verticesB2.push_back(glm::vec2(b2.vertices[3].x, b2.vertices[3].z));
            verticesB2.push_back(glm::vec2(b2.vertices[2].x, b2.vertices[2].z));
            verticesB2.push_back(glm::vec2(b2.vertices[6].x, b2.vertices[6].z));
            verticesB2.push_back(glm::vec2(b2.vertices[7].x, b2.vertices[7].z));

            bool allTrue = true;

            // Check collisions in each axis
            for (glm::vec2 a : sa)
            {
                // Get limits in boxes
                float intervalAStart = std::numeric_limits<float>::max();
                float intervalAEnd = -std::numeric_limits<float>::max();
                float intervalBStart = std::numeric_limits<float>::max();
                float intervalBEnd = -std::numeric_limits<float>::max();

                for (glm::vec2 v : verticesB1)
                {
                    float projection = glm::dot(v, a);
                    intervalAStart = std::min(intervalAStart, projection);
                    intervalAEnd = std::max(intervalAEnd, projection);
                }

                for (glm::vec2 v : verticesB2)
                {
                    float projection = glm::dot(v, a);
                    intervalBStart = std::min(intervalBStart, projection);
                    intervalBEnd = std::max(intervalBEnd, projection);
                }

                // collision a-axis?
                allTrue &= intervalAEnd >= intervalBStart && 
                    intervalBEnd >= intervalAStart;
            }

            // Return if there's a collision
            if (allTrue)
            {
                // Reaccionar a las colisiones
                glm::vec3 penetration = glm::vec3(0.0f);
                glm::vec3 direction = one.Position - two.Position;
                glm::vec2 a = glm::vec2(direction.x, direction.z);

                float intervalAStart = std::numeric_limits<float>::max();
                float intervalAEnd = -std::numeric_limits<float>::max();
                float intervalBStart = std::numeric_limits<float>::max();
                float intervalBEnd = -std::numeric_limits<float>::max();

                for (glm::vec2 v : verticesB1)
                {
                    float projection = glm::dot(v, a);
                    intervalAStart = std::min(intervalAStart, projection);
                    intervalAEnd = std::max(intervalAEnd, projection);
                }

                for (glm::vec2 v : verticesB2)
                {
                    float projection = glm::dot(v, a);
                    intervalBStart = std::min(intervalBStart, projection);
                    intervalBEnd = std::max(intervalBEnd, projection);
                }

                float magnitudePenetration = std::min(abs(intervalAEnd - intervalBStart), abs(intervalBEnd - intervalAStart));
                penetration = magnitudePenetration * glm::normalize(direction);

                camera.Position += 0.01f * penetration;

                CollisionPair* aux = new CollisionPair(b1, b2);
                return aux;
            }
        }
    }
    return nullptr;
}

void DoCollisions()
{
    for (auto& obj : objects)
    {
        CollisionPair* collision = CheckCollision(*jugador, obj);
        if (collision)
        {
            //BlockMovement(*jugador, obj, collision);
            std::cout << "Bloqueando Movimiento" << std::endl;
            /*std::cout << jugador->Position.x << " " << jugador->Position.y << " " << jugador->Position.z << std::endl;
            std::cout << obj.Position.x << " " << obj.Position.y << " " << obj.Position.z << std::endl;*/
            delete collision;
        }
    }
}