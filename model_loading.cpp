#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/shader_m.h>
#include <learnopengl/camera.h>
#include <learnopengl/model.h>

#include <iostream>
#include <vector>

// ---------------- SETTINGS ----------------
const unsigned int SCR_WIDTH = 1200;
const unsigned int SCR_HEIGHT = 800;

// ---------------- CAMERA ----------------
Camera camera(glm::vec3(0.0f, 5.0f, 8.0f));

// ---------------- TIME ----------------
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// ---------------- GAME STATE ----------------
bool gameOver = false;
float startTime = 0.0f;
int score = 0;

// ---------------- PLAYER (Spider) ----------------
glm::vec3 playerPos(0.0f);
float speed = 5.0f;

// ---------------- ENEMY (Rock) ----------------
glm::vec3 enemyPos(-10.0f, 0.0f, -10.0f);

// ---------------- COINS ----------------
glm::vec3 coins[3] = {
    {5, 0, 5}, {-5, 0, 2}, {2, 0, -6}};
bool collected[3] = {false, false, false};

// ---------------- COLLISION ----------------
bool collide(glm::vec3 a, glm::vec3 b, float d)
{
    return glm::length(a - b) < d;
}

// ---------------- INPUT ----------------
void input(GLFWwindow *window)
{
    if (gameOver)
    {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE))
            glfwSetWindowShouldClose(window, true);
        return;
    }

    float v = speed * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W))
        playerPos.z -= v;
    if (glfwGetKey(window, GLFW_KEY_S))
        playerPos.z += v;
    if (glfwGetKey(window, GLFW_KEY_A))
        playerPos.x -= v;
    if (glfwGetKey(window, GLFW_KEY_D))
        playerPos.x += v;

    if (glfwGetKey(window, GLFW_KEY_ESCAPE))
        glfwSetWindowShouldClose(window, true);
}

// ---------------- MAIN ----------------
int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Spider vs Rocks", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to init GLAD\n";
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    // 1. Load Shader
    Shader ourShader(
        "C:/Users/PREDATOR/Source/Repos/LearnOpenGLs/src/3.model_loading/1.model_loading/1.model_loading.vs",
        "C:/Users/PREDATOR/Source/Repos/LearnOpenGLs/src/3.model_loading/1.model_loading/1.model_loading.fs");

    // 2. Load Models
    Model spiderModel("C:/Users/PREDATOR/Source/Repos/LearnOpenGLs/resources/objects/model/obj/Only_Spider_with_Animations_Export.obj");
    Model rockModel("C:/Users/PREDATOR/Source/Repos/LearnOpenGLs/resources/objects/rock/rock.obj");
    Model coinModel("C:/Users/PREDATOR/Source/Repos/LearnOpenGLs/resources/objects/rock/rock.obj");

    // 3. Setup Ground (Simple Cube VAO)
    float vertices[] = {
        -0.5f, -0.5f, -0.5f, 0.5f, -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f, 0.5f, -0.5f, -0.5f, 0.5f, -0.5f, -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, -0.5f, 0.5f, 0.5f, -0.5f, -0.5f, 0.5f,
        -0.5f, 0.5f, 0.5f, -0.5f, 0.5f, -0.5f, -0.5f, -0.5f, -0.5f, -0.5f, -0.5f, -0.5f, -0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f,
        0.5f, 0.5f, 0.5f, 0.5f, 0.5f, -0.5f, 0.5f, -0.5f, -0.5f, 0.5f, -0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f, 0.5f, 0.5f,
        -0.5f, -0.5f, -0.5f, 0.5f, -0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f, -0.5f, -0.5f, 0.5f, -0.5f, -0.5f, -0.5f,
        -0.5f, 0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f, -0.5f};
    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    startTime = (float)glfwGetTime();

    // ---------------- LOOP ----------------
    while (!glfwWindowShouldClose(window))
    {
        float current = (float)glfwGetTime();
        deltaTime = current - lastFrame;
        lastFrame = current;

        input(window);

        // Enemy AI: Rock follows spider
        if (!gameOver && current - startTime > 1.0f)
        {
            glm::vec3 dir = playerPos - enemyPos;
            if (glm::length(dir) > 0.1f)
                enemyPos += glm::normalize(dir) * deltaTime * 1.5f;
        }

        // Collision: Coins
        for (int i = 0; i < 3; i++)
        {
            if (!collected[i] && collide(playerPos, coins[i], 1.0f))
            {
                collected[i] = true;
                score++;
                std::cout << "Score: " << score << " / 3\n";
            }
        }

        // Win/Lose Condition
        if (score == 3 && !gameOver)
        {
            std::cout << "WINNER! COLLECTED ALL ITEMS!\n";
            gameOver = true;
        }
        if (!gameOver && collide(playerPos, enemyPos, 1.0f))
        {
            std::cout << "GAME OVER! THE ROCK CRUSHED YOU!\n";
            gameOver = true;
        }

        // Render Setup
        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ourShader.use();

        // Camera Logic
        camera.Position = playerPos + glm::vec3(0.0f, 8.0f, 12.0f);
        camera.Front = glm::normalize(playerPos - camera.Position);

        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);

        // --- 🕷️ DRAW PLAYER (Spider) ---
        ourShader.setBool("hasTexture", true);
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, playerPos);
        model = glm::scale(model, glm::vec3(0.015f));
        ourShader.setMat4("model", model);
        spiderModel.Draw(ourShader);

        // --- 🗿 DRAW ENEMY (Rock Model) ---
        model = glm::mat4(1.0f);
        model = glm::translate(model, enemyPos);
        model = glm::scale(model, glm::vec3(0.5f)); // ปรับสเกลหินตามความเหมาะสม
        ourShader.setMat4("model", model);
        rockModel.Draw(ourShader);

        // --- 🟡 DRAW COINS (Model instead of Cube) ---
        for (int i = 0; i < 3; i++)
        {
            if (!collected[i])
            {
                model = glm::mat4(1.0f);
                model = glm::translate(model, coins[i]);
                model = glm::scale(model, glm::vec3(0.2f)); // เหรียญเล็กๆ
                model = glm::rotate(model, (float)glfwGetTime() * 2.0f, glm::vec3(0, 1, 0));
                ourShader.setMat4("model", model);
                // เราสามารถเซ็ตสีเหลืองทับไปใน Shader ถ้าต้องการ
                ourShader.setVec3("color", glm::vec3(1.0f, 0.9f, 0.0f));
                coinModel.Draw(ourShader);
            }
        }

        // --- 🟩 DRAW GROUND (Manual Cube) ---
        ourShader.setBool("hasTexture", false);
        glBindVertexArray(VAO);
        model = glm::translate(glm::mat4(1.0f), glm::vec3(0, -0.5f, 0));
        model = glm::scale(model, glm::vec3(40.0f, 0.1f, 40.0f));
        ourShader.setMat4("model", model);
        ourShader.setVec3("color", glm::vec3(0.2f, 0.2f, 0.2f));
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glfwTerminate();
    return 0;
}