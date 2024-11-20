#include "skybox.h"

#include <iostream>
#include "skyLight.h"
#include "../core/entity.h"
#include "stb_image.h"

// TODO: Clean this up.
// Just wanted to protoype a PoC quickly.

const char* skyboxVertexShader = R"(
    #version 330 core
    layout (location = 0) in vec3 aPos;

    uniform mat4 projection;
    uniform mat4 view;

    out vec3 texCoords;

    void main() {
        texCoords = aPos;
        vec4 pos = projection * mat4(mat3(view)) * vec4(aPos, 1.0);
        gl_Position = pos.xyww;
    }
)";

// Fragment shader with time-based blending
const char* skyboxFragmentShader = R"(
    #version 330 core
    in vec3 texCoords;

    uniform vec3 ambientLight;
    uniform samplerCube skyboxDay;
    uniform samplerCube skyboxNight;
    uniform samplerCube skyboxSunset;
    uniform float timeOfDay;    // Range 0.0 to 24.0
    uniform vec3 sunPosition;   // Calculated based on time
    uniform float starBrightness;

    out vec4 FragColor;

    vec4 getSunGlow(vec3 direction, vec3 sunPos) {
        float sunDot = max(dot(direction, normalize(sunPos)), 0.0);
        vec4 sunColor = vec4(1.0, 0.9, 0.7, 1.0);
        return sunColor * pow(sunDot, 256.0);
    }

    void main() {
        // Calculate blend factors based on time of day
        float dayTime = smoothstep(6.0, 8.0, timeOfDay) - smoothstep(16.0, 18.0, timeOfDay);
        float nightTime = 1 + smoothstep(18.0, 20.0, timeOfDay) - smoothstep(4.0, 6.0, timeOfDay);
        float sunsetTime = 1.0 - dayTime - nightTime;

        // Sample from each cubemap
        vec4 dayColor = texture(skyboxDay, texCoords);
        vec4 nightColor = texture(skyboxNight, texCoords);
        vec4 sunsetColor = texture(skyboxSunset, texCoords);

        // Blend between the textures
        vec4 finalColor = (dayColor * dayTime +
                           nightColor * nightTime +
                           sunsetColor * sunsetTime);

        // Add sun glow
        finalColor += getSunGlow(normalize(texCoords), sunPosition) * (dayTime + sunsetTime * 0.5);

        FragColor = finalColor;
    }
)";

void Skybox::setupGeometry() {
    float skyboxVertices[] = {
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Skybox::initializeShaders() {
    // Vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &skyboxVertexShader, NULL);
    glCompileShader(vertexShader);

    // Fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &skyboxFragmentShader, NULL);
    glCompileShader(fragmentShader);

    // Shader program
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // Clean up individual shaders
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

static unsigned int loadCubemap(const std::vector<std::string>& faces) {
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    std::string lookup_path = R"(..\resources\textures\skybox\)";

    for(unsigned int i = 0; i < faces.size(); i++) {
        int width, height, nrChannels;
        auto path = lookup_path + faces[i];
        unsigned char *data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);

        if (data) {
            GLenum format = (nrChannels == 3) ? GL_RGB : GL_RGBA;
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                         0, format, width, height, 0, format,
                         GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        } else {
            std::cerr << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
        }
    }

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}

Skybox::Skybox() {
    setupGeometry();
    initializeShaders();

    std::vector<std::string> dayFaces = {
        "sh_rt.png", "sh_lf.png", "sh_up.png",
        "sh_dn.png", "sh_bk.png", "sh_ft.png"
    };

    std::vector<std::string> nightFaces = {
        "n_rt.png", "n_lf.png", "n_up.png",
        "n_dn.png", "n_bk.png", "n_ft.png"
    };

    std::vector<std::string> sunsetFaces = {
        "d_rt.png", "d_lf.png", "d_up.png",
        "d_dn.png", "d_ft.png", "d_bk.png"
    };

    dayTexture = loadCubemap(dayFaces);
    nightTexture = loadCubemap(nightFaces);
    sunsetTexture = loadCubemap(sunsetFaces);
}

void Skybox::update() {
    auto tick = getEntity()->getComponent<SkyLight>()->getTime();
    currentTime = 24.f * (tick / SkyLight::TICKS_PER_DAY);
    currentTime = fmod(currentTime, 24.f);
}

Skybox::TimeOfDaySettings Skybox::calculateTimeSettings(const float timeOfDay) {
    TimeOfDaySettings settings{};

    // Calculate sun position based on time (simplified model)
    const float sunAngle = (timeOfDay - 12.0f) * ((22.f / 7.f) / 12.0f);
    settings.sunPosition = glm::vec3(
        cos(sunAngle),
        -sin(sunAngle),
        0.2f
    );

    settings.starBrightness = glm::smoothstep(18.0f, 20.0f, timeOfDay) -
        glm::smoothstep(4.0f, 6.0f, timeOfDay);

    const float daylight = glm::smoothstep(6.0f, 8.0f, timeOfDay) -
        glm::smoothstep(16.0f, 18.0f, timeOfDay);
    settings.ambientLight = glm::vec3(0.6f) * daylight +  // Reduce from 0.8f
                           glm::vec3(0.2f) * (1.0f - daylight);  // Increase from 0.1f

    settings.fogDensity = 0.1f + 0.2f * (1.0f - daylight);

    return settings;
}

void Skybox::render() {
    glDepthFunc(GL_LEQUAL);
    glUseProgram(shaderProgram);

    // Calculate time-based settings
    TimeOfDaySettings settings = calculateTimeSettings(static_cast<float>(currentTime));

    auto camera = Renderer::getActive()->getCamera();
    std::cout << "time of day " << currentTime << '\n';
    std::cout << "starbrightness " << settings.starBrightness << '\n';

    // Set uniforms
    setMat4("projection", camera->getProjection());
    setMat4("view", glm::mat4(glm::mat3(camera->getView())));
    setFloat("timeOfDay", static_cast<float>(currentTime));
    setVec3("sunPosition", settings.sunPosition);
    setFloat("starBrightness", settings.starBrightness);
    setVec3("ambientLight", settings.ambientLight);

    // Bind textures
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, dayTexture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, nightTexture);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_CUBE_MAP, sunsetTexture);

    glUniform1i(glGetUniformLocation(shaderProgram, "skyboxDay"), 0);
    glUniform1i(glGetUniformLocation(shaderProgram, "skyboxNight"), 1);
    glUniform1i(glGetUniformLocation(shaderProgram, "skyboxSunset"), 2);

    // Render skybox cube
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    glDepthFunc(GL_LESS);
}

void Skybox::init() {
    Renderer::getActive()->registerRenderable(this);
}