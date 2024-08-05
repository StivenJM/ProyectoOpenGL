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
void configureLightShader(Shader& shader);
void setupPointLight(Shader& shader);


// settings
const unsigned int SCR_WIDTH = 1800;
const unsigned int SCR_HEIGHT = 950;


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

// Variable para acercar o alejarse cuando se presiona el click derecho
bool estaApuntando = false;

//----------------------------FLASH LIGHT --------------------------------//
glm::vec3 lightPos(1.0f, 4.0f, 0.0f);

float baseAmbient = 0.3f;
float baseDiffuse = 1.0f;
float baseSpecular = 0.5f;
float fluctuationFrequency = 2.0f;
float fluctuationIntensity = 0.1f;
float umbralProximidad = 2.0f;
glm::vec3 colorFogata(1.0f, 0.63f, 0.23f); //color falso

// Ambient, diffuse and specular colors
std::vector<glm::vec3> pointLightPositions = {
    glm::vec3(  5.22f, 3.5f,    0.96f), 
    glm::vec3(110.60f, 2.5f, -  7.91f),
    glm::vec3(225.50f, 3.0f,   23.65f),
    glm::vec3(271.89f, 3.5f, - 90.05f),
    glm::vec3(248.27f, 3.0f, -101.40f),
    glm::vec3(284.23f, 2.5f, -214.85f)
};

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

//----------------------------END FLASH LIGHT --------------------------------//

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

    // Obtener el monitor principal
    GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
    if (primaryMonitor == nullptr) {
        return -1;
    }

    // Obtener el modo del video del monitor principal
    const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);
    if (mode == nullptr) {
        return -1;
    }

    // Obtener el tamaño de la ventana
    int windowWidth, windowHeight;
    glfwGetWindowSize(window, &windowWidth, &windowHeight);

    // Calcular las coordenadas para centrar la ventana
    int xpos = (mode->width - windowWidth) / 2;
    int ypos = (mode->height - windowHeight) / 2;

    // Posicionar la ventana en las coordenadas calculadas
    glfwSetWindowPos(window, xpos, ypos);

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


   
    //----------------------------------SHADERS----------------------------------------//

    Shader shaderScene("shaders/tests/objetoLuzLuna.vs", "shaders/tests/objetoLuzLuna.fs");
    Shader shaderObject("shaders/tests/objetoLuzLuna.vs", "shaders/tests/objetoLuzLuna.fs");
    Shader bbShader("shaders/tests/shaderCollision.vs", "shaders/tests/shaderCollision.fs");
    Shader lightCubeShader("shaders/tests/cuboLuz-LuzLuna.vs", "shaders/tests/cuboLuz-LuzLuna.fs");

    // --------------------------------------------------------------------------------//



    // --------------------------------MODELOS ----------------------------------------//
     
    //Model ourModel(FileSystem::getPath("resources/objects/backpack/backpack.obj"));
    //Model modeloEscena("D:/Daniel/Documentos/Visual Studio 2022/proyectoCompGrafica/ProyectoOpenGL/ProyectoOpenGL/models/EscenaCiudad/EscenaCiudad.obj");
    Model modeloFuego("D:/Daniel/Documentos/Visual Studio 2022/proyectoCompGrafica/ProyectoOpenGL/ProyectoOpenGL/models/fuego/Fuego.obj");
    
    // --------------------------------ENEMIGOS ----------------------------------------//
    GameObject zombie("D:/Daniel/Documentos/Visual Studio 2022/proyectoCompGrafica/ProyectoOpenGL/ProyectoOpenGL/models/zombie/zombie.obj",
        glm::vec3(159.0f, 1.0f, 11.0f),
        0.0f,
        glm::vec3(0.0f, 1.0f, 0.0f),
        glm::vec3(0.5f, 0.5f, 0.5f),
        true);
    //zombi.AddBoundingBox(glm::vec3(0.3f, 0.7f, 0.3f), glm::vec3(0.0f));
    zombie.Scale = glm::vec3(2.5f);
    zombie.AngleRotation = -90.0f;
    zombie.AddBoundingBox(glm::vec3(0.3f, 0.7f, 0.3f), glm::vec3(0.0f));

    GameObject nemesis("D:/Daniel/Documentos/Visual Studio 2022/proyectoCompGrafica/ProyectoOpenGL/ProyectoOpenGL/models/nemesis/nemesis.obj",
        glm::vec3(307.0f, -4.0f, -239.0f),
        0.0f,
        glm::vec3(0.0f, 1.0f, 0.0f),
        glm::vec3(0.5f, 0.5f, 0.5f),
        true);
    nemesis.Scale = glm::vec3(40.0f);
    nemesis.AngleRotation = -45.0;
    nemesis.AddBoundingBox(glm::vec3(0.2f, 0.2f, 0.2f), glm::vec3(0.0f));
    objects.push_back(nemesis);


    // --------------------------------COLISIONES ----------------------------------------//
    // CUADROS PARA COLISIONES BASADOS EN CAJA
    GameObject colisiones("D:/Daniel/Documentos/Visual Studio 2022/proyectoCompGrafica/ProyectoOpenGL/ProyectoOpenGL/models/caja/caja.obj");
    colisiones.AddBoundingBox(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(-33.0, 0.0f, 0.0f));
    colisiones.AddBoundingBox(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(-33.0, 0.0f, 2.0f));
    colisiones.AddBoundingBox(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(-33.0, 0.0f, 4.0f));
    colisiones.AddBoundingBox(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(-33.0, 0.0f, 6.0f));
    colisiones.AddBoundingBox(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(-33.0, 0.0f, 8.0f));
    colisiones.AddBoundingBox(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(-33.0, 0.0f, 10.0f));
    colisiones.AddBoundingBox(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(-33.0, 0.0f, 12.0f));
    colisiones.AddBoundingBox(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(-33.0, 0.0f, 14.0f));
    colisiones.AddBoundingBox(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(-33.0, 0.0f, 16.0f));

    //colisiones.AddBoundingBox(glm::vec3(7.0f, 20.0f, 21.0f), glm::vec3(-36.5, 0.0f, 10.5f));        // AB
    //colisiones.AddBoundingBox(glm::vec3(53.0f, 20.0f, 5.0f), glm::vec3(-6.5f, 0.0f, -2.5f));        // AC
    //colisiones.AddBoundingBox(glm::vec3(10.0f, 20.0f, 82.0f), glm::vec3(15.5f, 0.0f, -41.5f));      // CD
    //colisiones.AddBoundingBox(glm::vec3(34.0f, 20.0f, 6.0f), glm::vec3(37.0f, 0.0f, -85.0f));       // DE
    //colisiones.AddBoundingBox(glm::vec3(6.0f, 20.0f, 62.0f), glm::vec3(57.0f, 0.0f, -50.0f));       // EF
    //colisiones.AddBoundingBox(glm::vec3(106.0f, 20.0f, 2.0f), glm::vec3(107.0f, 0.0f, -19.0f));     // FG
    //colisiones.AddBoundingBox(glm::vec3(4.0f, 20.0f, 18.0f), glm::vec3(162.0f, 0.0f, -9.0f));       // GH
    //colisiones.AddBoundingBox(glm::vec3(122.0f, 20.0f, 6.0f), glm::vec3(196.5f, 0.0f, -3.0f));      // HI
    //colisiones.AddBoundingBox(glm::vec3(5.0f, 20.0f, 257.0f), glm::vec3(229.5f, 0.0f, -128.5f));    // IJ
    //colisiones.AddBoundingBox(glm::vec3(5.0f, 20.0f, 113.0f), glm::vec3(288.5f, 0.0f, -259.5f));    // JK
    //colisiones.AddBoundingBox(glm::vec3(5.0f, 20.0f, 53.0f), glm::vec3(347.5f, 0.0f, -230.5f));     // KL
    //colisiones.AddBoundingBox(glm::vec3(77.0f, 20.0f, 5.0f), glm::vec3(306.5f, 0.0f, -201.5f));     // LM
    //colisiones.AddBoundingBox(glm::vec3(5.0f, 20.0f, 203.0f), glm::vec3(270.5f, 0.0f, -102.5f));    // MP
    //colisiones.AddBoundingBox(glm::vec3(111.0f, 20.0f, 5.0f), glm::vec3(323.5f, 0.0f, -3.5f));      // PN
    //colisiones.AddBoundingBox(glm::vec3(5.0f, 20.0f, 22.0f), glm::vec3(381.5f, 0.0f, 10.5f));       // NO
    //colisiones.AddBoundingBox(glm::vec3(412.0f, 20.0f, 4.0f), glm::vec3(173.0f, 0.0f, 23.0f));      // OB
    objects.push_back(colisiones);


     // --------------------------------------------------------------------------------//
    
    jugador = new GameObject("D:/Daniel/Documentos/Visual Studio 2022/proyectoCompGrafica/ProyectoOpenGL/ProyectoOpenGL/models/JillValentine/JillValentine.obj",
        glm::vec3(0.0f, 0.0f, 0.0f),
        0.0f,
        glm::vec3(0.0f, 1.0f, 0.0f),
        glm::vec3(0.5f, 0.5f, 0.5f),
        true);
    jugador->AddBoundingBox(glm::vec3(0.3f, 0.7f, 0.3f), glm::vec3(0.0f));
    jugador->Scale = glm::vec3(1.5f, 1.5f, 1.5f);

    

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


    camera.MovementSpeed = 20;

    // render loop
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        jugador->Position = camera.Position;
        jugador->AngleRotation = direction + 90.0f;

        processInput(window);

        ///////////////////////////////////////////MUSICA/////////////////////////////////////////////////////////////////

        irrklang::vec3df listenerPos(camera.Position.x, camera.Position.y, camera.Position.z);
        irrklang::vec3df listenerLookDir(camera.Front.x, camera.Front.y, camera.Front.z);
        irrklang::vec3df listenerUpVector(camera.Up.x, camera.Up.y, camera.Up.z);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        
        engine->setListenerPosition(listenerPos, listenerLookDir, irrklang::vec3df(0, 0, 0), listenerUpVector);

        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //********************************AGREGANDO PROPIEDADES DE LUZ A SHADERS*******************************************//
        configureLightShader(shaderScene);
        configureLightShader(shaderObject);
        //*****************************FIN AGREGANDO PROPIEDADES DE LUZ A SHADERS*******************************************//


        //**************************CONFIGURANDO VIEWPORT Y PROJECTION***************************************************//
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 500.0f);
        glm::mat4 view = glm::mat4(1.0f);
        if (!estaApuntando) view = glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f, -1.0f, -4.0f)) * camera.GetViewMatrix();
        else view = glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f, -1.0f, 15.0f)) * camera.GetViewMatrix();
        //************************FIN CONFIGURANDO VIEWPORT Y PROJECTION***************************************************//

        
        
        // world transformation - ESCENA
        shaderScene.setMat4("projection", projection);
        shaderScene.setMat4("view", view);
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
        shaderScene.setMat4("model", model);
        //modeloEscena.Draw(shaderScene);

        // world transformation - JUGADOR
        shaderObject.use();
        shaderObject.setMat4("projection", projection);
        shaderObject.setMat4("view", view);
        model = jugador->GetModelMatrix();
        shaderObject.setMat4("model", model);
        jugador->Render(shaderObject);


        //Render player's Bounding Boxes
        bbShader.use();
        bbShader.setMat4("model", model);
        bbShader.setMat4("view", view);
        bbShader.setMat4("projection", projection);
        jugador->RenderBoundingBoxes(bbShader);

        // world transformation - ENEMIGOS

        // para que se mueva al rededor del tiempo
        float time = glfwGetTime();
        float translado = 10.0f * (sin(time * 0.5f) + 1.0f);

        glm::mat4 tZombi = glm::translate(glm::mat4(1.0f), glm::vec3(translado, 0.0f, 0.0f));

        shaderObject.use();
        shaderObject.setMat4("projection", projection);
        shaderObject.setMat4("view", view);
        model = tZombi * zombie.GetModelMatrix();
        shaderObject.setMat4("model", model);
        zombie.Render(shaderObject);

        shaderObject.use();
        model = nemesis.GetModelMatrix();
        shaderObject.setMat4("model", model);
        nemesis.Render(shaderObject);
         
         
        // world transformation - Obstaculos transparentes
        
        for (auto& object : objects)
        {
            shaderObject.use();
            model = object.GetModelMatrix();
            shaderObject.setMat4("model", model);
            object.Render(shaderObject);

            // Render cube's bounding boxes
            bbShader.use();
            bbShader.setMat4("model", model);
            object.RenderBoundingBoxes(bbShader);
        }


        // world transformation - RENDERIZADO DE CUBOS DE LUZ
        lightCubeShader.use();
        lightCubeShader.setMat4("projection", projection);
        lightCubeShader.setMat4("view", view);
        //lightCubeShader.setVec3("lightColor", colorFogata);


        //// draw lights.
        //glBindVertexArray(lightCubeVAO);
        for (unsigned int i = 0; i < 6; i++)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, pointLightPositions[i]);
            model = glm::scale(model, glm::vec3(1.0f)); // Make it a smaller cube
            lightCubeShader.setMat4("model", model);
            modeloFuego.Draw(lightCubeShader);
        }

        //----------------------------------------------------------FIN RENDERIZADO CUBOS DE LUZ------------------------------------------------------------//


        /////////////////////////////////////////////////fin renderización de jugador y zombie////////////////////////////////////////////////////////////////

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    delete jugador;

    glfwTerminate();
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

    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) camera.MovementSpeed = 40;
    else camera.MovementSpeed = 20;

    // Si se pulsa cualquier tecla de direccion se actualiza el angulo del personaje
    // Se verifica si existe una colision
    // Se permite o deniega el bloqueo de movimiento
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS ||
        glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS ||
        glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS ||
        glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {

        DoCollisions(objects, jugador, &camera);
        direction = -camera.Yaw + 90.0f;
        if (!engine->isCurrentlyPlaying(stepSound)) {
            engine->play2D(stepSound, false);
        }

    }

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) estaApuntando = true;
    else estaApuntando = false;

    camera.Position.y = 0.5f;
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}


void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; 

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}


void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}

void configureLightShader(Shader& shader)
{
    shader.use();
    shader.setVec3("viewPos", camera.Position);
    shader.setFloat("shininess", 4.0f);

    // directional light
    shader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
    shader.setVec3("dirLight.ambient", moonColors[0]);
    shader.setVec3("dirLight.diffuse", moonColors[1]);
    shader.setVec3("dirLight.specular", moonColors[2]);

    // point light 1
    setupPointLight(shader);

    // spotLight
    shader.setVec3("spotLight.position", camera.Position);
    shader.setVec3("spotLight.direction", camera.Front);
    shader.setVec3("spotLight.ambient", spotColors[0]);
    shader.setVec3("spotLight.diffuse", spotColors[1]);
    shader.setVec3("spotLight.specular", spotColors[2]);
    shader.setFloat("spotLight.constant", 1.0f);
    shader.setFloat("spotLight.linear", 0.09);
    shader.setFloat("spotLight.quadratic", 0.032);
    shader.setFloat("spotLight.cutOff", glm::cos(glm::radians(10.0f)));
    shader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(18.0f)));
}

void setupPointLight(Shader& shader)
{
    unsigned int limite = (unsigned int)pointLightPositions.size();
    // Point lights
    for (unsigned int i = 0; i < limite; i++)
    {
        shader.setVec3("pointLights[" + std::to_string(i) + "].position", pointLightPositions[i]);
        shader.setVec3("pointLights[" + std::to_string(i) + "].ambient", fireColors[0]);
        shader.setVec3("pointLights[" + std::to_string(i) + "].diffuse", fireColors[1]);
        shader.setVec3("pointLights[" + std::to_string(i) + "].specular", fireColors[2]);
        shader.setFloat("pointLights[" + std::to_string(i) + "].constant", 1.0f);
        shader.setFloat("pointLights[" + std::to_string(i) + "].linear", 0.001f);
        shader.setFloat("pointLights[" + std::to_string(i) + "].quadratic", 0.002f);
    }
}