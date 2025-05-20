#include <iostream>
#include <vector>
#include <cstdlib> 
#include <ctime>   

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;
using namespace glm;

const GLuint WIDTH = 800, HEIGHT = 600;

struct Triangle {
    vec2 position;
    vec3 color;
};

vector<Triangle> triangles;

GLuint VAOtriangulo;
GLuint shaderProgram;

GLuint createTriangle(float x0, float y0, float x1, float y1, float x2, float y2) {
    float vertices[] = {
        x0, y0, 0.0f,
        x1, y1, 0.0f,
        x2, y2, 0.0f
    };

    GLuint VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return VAO;
}

const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 position;

uniform mat4 projection;
uniform mat4 model;

void main() {
    gl_Position = projection * model * vec4(position, 1.0);
}
)";

const char* fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;
uniform vec3 inputColor;
void main() {
    FragColor = vec4(inputColor, 1.0);
}
)";

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        float x = (float)xpos;
        float y = (float)(HEIGHT - ypos);

        float r = (float)(rand() % 256) / 255.0f;
        float g = (float)(rand() % 256) / 255.0f;
        float b = (float)(rand() % 256) / 255.0f;

        Triangle t;
        t.position = vec2(x, y);
        t.color = vec3(r, g, b);

        triangles.push_back(t);
    }
}

int compileShader(GLenum type, const char* source) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        cout << "ERROR::SHADER_COMPILATION_ERROR\n" << infoLog << endl;
    }

    return shader;
}

int main() {
    srand((unsigned)time(NULL));

    if (!glfwInit()) {
        cout << "Failed to initialize GLFW\n";
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Exercicio Triangulos", NULL, NULL);
    if (!window) {
        cout << "Failed to create window\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        cout << "Failed to initialize GLAD\n";
        return -1;
    }

    VAOtriangulo = createTriangle(-0.1f, -0.1f, 0.1f, -0.1f, 0.0f, 0.1f);

    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    int success;
    char infoLog[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        cout << "ERROR::PROGRAM_LINKING_ERROR\n" << infoLog << endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    glViewport(0, 0, WIDTH, HEIGHT);
    mat4 projection = ortho(0.0f, float(WIDTH), 0.0f, float(HEIGHT), -1.0f, 1.0f);

    triangles.push_back({vec2(100, 100), vec3(1.0f, 0.0f, 0.0f)});
    triangles.push_back({vec2(200, 200), vec3(0.0f, 1.0f, 0.0f)});
    triangles.push_back({vec2(300, 300), vec3(0.0f, 0.0f, 1.0f)});
    triangles.push_back({vec2(400, 400), vec3(1.0f, 1.0f, 0.0f)});
    triangles.push_back({vec2(500, 500), vec3(1.0f, 0.0f, 1.0f)});

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);

        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, value_ptr(projection));

        glBindVertexArray(VAOtriangulo);

        for (auto &t : triangles) {
            mat4 model = mat4(1.0f);
            model = translate(model, vec3(t.position, 0.0f));
            glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, value_ptr(model));

            glUniform3fv(glGetUniformLocation(shaderProgram, "inputColor"), 1, value_ptr(t.color));

            glDrawArrays(GL_TRIANGLES, 0, 3);
        }

        glBindVertexArray(0);
        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}