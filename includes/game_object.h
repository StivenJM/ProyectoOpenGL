#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/shader_m.h>
#include <learnopengl/model.h>

#include <vector>

class BoundingBox {
    // Bounding Box - Box to detect collisions
    /*
    * Las colisiones se realizan en el plano XYZ 
    */
public:
    glm::vec3 dimensions; 
    glm::vec3 position;
    glm::vec3 front;
    std::vector<glm::vec3> vertices;
    std::vector<unsigned int> indices;

    BoundingBox(glm::vec3 dim, glm::vec3 pos) : dimensions(dim), position(pos), front(glm::vec3(1.0f, 0.0f, 0.0f))
    {
        // Create vertices
        vertices.push_back(position + glm::vec3(-dimensions.x, -dimensions.y, -dimensions.z));
        vertices.push_back(position + glm::vec3( dimensions.x, -dimensions.y, -dimensions.z));
        vertices.push_back(position + glm::vec3( dimensions.x,  dimensions.y, -dimensions.z));
        vertices.push_back(position + glm::vec3(-dimensions.x,  dimensions.y, -dimensions.z));
        vertices.push_back(position + glm::vec3(-dimensions.x, -dimensions.y,  dimensions.z));
        vertices.push_back(position + glm::vec3( dimensions.x, -dimensions.y,  dimensions.z));
        vertices.push_back(position + glm::vec3( dimensions.x,  dimensions.y,  dimensions.z));
        vertices.push_back(position + glm::vec3(-dimensions.x,  dimensions.y,  dimensions.z));

        unsigned int auxIndices[] = {
            // Up
            3, 7, 6,
            3, 6, 2,
            // Bottom
            0, 4, 5,
            0, 5, 1,
            // Back
            0, 1, 2,
            0, 2, 3,
            // Front
            4, 5, 6,
            4, 6, 7,
            // Left
            0, 4, 7, 
            0, 7, 3,
            // Right
            1, 5, 6, 
            1, 6, 2,
        };

        // Create indices
        for (int i : auxIndices)
        {
            indices.push_back(i);
        }

        setupBB();
    }

    void Draw(Shader& shader)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        // Draw cube
        shader.use();
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    void DrawAxis(Shader& shader, float scale)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        // Draw cube
        shader.use();
        shader.setFloat("scale", scale);
        glBindVertexArray(VAO_AXIS);
        glDrawArrays(GL_LINES, 0, 2);
        glBindVertexArray(0);
        shader.setFloat("scale", 1.0f);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

private:
    // render data 
    unsigned int VAO, VBO, EBO;
    unsigned int VAO_AXIS, VBO_AXIS;

    // initializes all the buffer objects/arrays
    void setupBB()
    {
        // create buffers/arrays
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);
        // load data into vertex buffers
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        // A great thing about structs is that their memory layout is sequential for all its items.
        // The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
        // again translates to 3/2 floats which translates to a byte array.
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

        // set the vertex attribute pointers
        // vertex Positions
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
        glEnableVertexAttribArray(0);
        
        glBindVertexArray(0);


        // create buffers/arrays
        //----------------------
        float axisVertices[] = {
            0.0f, 0.0f, 0.0f,
            this->front.x, this->front.y, this->front.z 
        };

        glGenVertexArrays(1, &VAO_AXIS);
        glGenBuffers(1, &VBO_AXIS);

        glBindVertexArray(VAO_AXIS);
        // load data into vertex buffers
        glBindBuffer(GL_ARRAY_BUFFER, VBO_AXIS);
        // A great thing about structs is that their memory layout is sequential for all its items.
        // The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
        // again translates to 3/2 floats which translates to a byte array.
        glBufferData(GL_ARRAY_BUFFER, sizeof(axisVertices), axisVertices, GL_STATIC_DRAW);

        // set the vertex attribute pointers
        // vertex Positions
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glEnableVertexAttribArray(0);

        glBindVertexArray(0);
    }
};

// GAME OBJECT CLASS
// --------------------------------------

// Default GameObject values
const float ANGLE_ROTATION = 0.0f;

class GameObject {
private:
    Model ModelComplete;
    std::vector<BoundingBox> BoundingBoxes; // Bounding boxes
    bool DynamicObject;
public:
    std::string ModelStr;
    glm::vec3 Position; // Posicion central del objeto
    float AngleRotation;
    glm::vec3 AxisRotation;
    glm::vec3 Scale;

    GameObject(std::string modelStr, glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), float angleRotation = ANGLE_ROTATION, glm::vec3 axisRotation = glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f), bool dynamic = false) : ModelComplete(modelStr)
    {
        // El angulo de rotacion es en grados
        this->ModelStr = modelStr;
        this->Position = position;
        this->AngleRotation = angleRotation;
        this->AxisRotation = axisRotation;
        this->Scale = scale;
        this->DynamicObject = dynamic;
    }

    void AddBoundingBox(glm::vec3 dimensions, glm::vec3 position) {
        BoundingBox aux(dimensions, position);
        BoundingBoxes.push_back(aux);
    }

    std::vector<BoundingBox> GetBoundingBoxes()
    {
        std::vector<BoundingBox> result;
        for (BoundingBox& bb : this->BoundingBoxes)
        {
            glm::mat4 model = this->GetModelMatrix();

            glm::mat4 rotation = glm::mat4(1.0f);
            rotation = glm::rotate(rotation, glm::radians(this->AngleRotation), this->AxisRotation);

            BoundingBox auxBB(bb);
            for (auto& v : auxBB.vertices)
            {
                glm::vec4 v4f = glm::vec4(v, 1.0f);
                v4f = model * v4f;

                v.x = v4f.x;
                v.y = v4f.y;
                v.z = v4f.z;
            }

            glm::vec4 v4f = glm::vec4(auxBB.front, 1.0f);
            v4f = rotation * v4f;
            auxBB.front.x = v4f.x;
            auxBB.front.y = v4f.y;
            auxBB.front.z = v4f.z;

            result.push_back(auxBB);
        }
        return result;
    }

    void RenderBoundingBoxes(Shader& shader)
    {
        for (auto& bb : this->BoundingBoxes)
        {
            // render
            bb.Draw(shader);
            bb.DrawAxis(shader, 5.0f);
        }
        /*this->BoundingBoxes[1].Draw(shader);*/
    }

    glm::mat4 GetModelMatrix() const 
    {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, Position);
        model = glm::rotate(model, glm::radians(AngleRotation), AxisRotation);
        model = glm::scale(model, Scale);
        return model;
    }

    void Render(Shader& shader)
    {
        ModelComplete.Draw(shader);
    }
};

glm::vec3 CheckCollision(GameObject& one, GameObject& two) // AABB - AABB
{
    // La funcion retorna un vector 3d que indica cuanto debe moverse el objeto one para no transpasar el objeto two
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
                glm::vec3 direction = one.Position - (two.Position + b2.position);
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

                // La sensitividad debe ir disminuyendo a medida que los objetos con los que se colisiona crecen
                if (magnitudePenetration < 2.0f) return 0.5f * penetration;
                else return 0.01f * penetration;
            }
        }
    }
    return glm::vec3(0.0f);
}

void DoCollisions(std::vector<GameObject>& objects, GameObject* jugador, Camera* camera = NULL)
{
    for (auto& obj : objects)
    {
        glm::vec3 move = CheckCollision(*jugador, obj);
        if (camera != NULL)
            camera->Position += move;
        else 
            jugador->Position += move;
    }
}
#endif