#include "skybox.h"

#include <fstream>
#include <iostream>
#include <sstream>

#include "../core/dayTimer.h"
#include "../core/entity.h"
#include "stb_image.h"

void Skybox::setupGeometry() {
    // This is ugly but better than having to deal with
    // importing a cube that will always be the same anyway.
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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), static_cast<void*>(nullptr));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
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

Skybox::Skybox(const std::string& vertPath, const std::string& fragPath) {
    skyboxShader = new Shader(vertPath, fragPath);
    skyboxShader->upload();

    setupGeometry();

    const std::vector<std::string> dayFaces = {
        "sh_rt.png", "sh_lf.png", "sh_up.png",
        "sh_dn.png", "sh_bk.png", "sh_ft.png"
    };

    const std::vector<std::string> nightFaces = {
        "n_rt.png", "n_lf.png", "n_up.png",
        "n_dn.png", "n_bk.png", "n_ft.png"
    };

    const std::vector<std::string> sunsetFaces = {
        "d_rt.png", "d_lf.png", "d_up.png",
        "d_dn.png", "d_ft.png", "d_bk.png"
    };

    dayTexture = loadCubemap(dayFaces);
    nightTexture = loadCubemap(nightFaces);
    sunsetTexture = loadCubemap(sunsetFaces);
}

Skybox::~Skybox() {
    delete skyboxShader;
}

void Skybox::update() {
    currentTime = static_cast<float>(getEntity()->getComponent<DayTimer>()->getCurrentTime());
}

Skybox::TimeOfDaySettings Skybox::calculateTimeSettings(const float timeOfDay) {
    TimeOfDaySettings settings{};

    const double offset_time = timeOfDay - DayTimer::DAWN_START;
    constexpr double sun_in_sky = DayTimer::DUSK_END - DayTimer::DAWN_START;
    const float sun_angle = static_cast<float>(offset_time / sun_in_sky) * glm::pi<float>();

    settings.sunPosition = -glm::vec3(
        cos(sun_angle),
        -sin(sun_angle),
        0.2f
    );

    settings.starBrightness = glm::smoothstep((float)DayTimer::DUSK_START, (float)DayTimer::DUSK_END, timeOfDay) -
        glm::smoothstep((float)DayTimer::DAWN_START, (float)DayTimer::DAWN_END, timeOfDay);

    const float daylight = glm::smoothstep((float)DayTimer::DAWN_START, (float)DayTimer::DAWN_END, timeOfDay) -
        glm::smoothstep((float)DayTimer::DUSK_START, (float)DayTimer::DUSK_END, timeOfDay);
    settings.ambientLight = glm::vec3(0.6f) * daylight +
                           glm::vec3(0.2f) * (1.0f - daylight);

    settings.fogDensity = 0.1f + 0.2f * (1.0f - daylight);

    return settings;
}

void Skybox::render() {
    glDepthFunc(GL_LEQUAL);
    skyboxShader->bind();

    // Calculate time-based settings
    TimeOfDaySettings settings = calculateTimeSettings(currentTime);

    auto camera = Renderer::getActive()->getCamera();

    // Set uniforms
    skyboxShader->setUniform("projection", camera->getProjection());
    skyboxShader->setUniform("view", glm::mat4(glm::mat3(camera->getView())));
    skyboxShader->setUniform("timeOfDay", currentTime);
    skyboxShader->setUniform("sunPosition", settings.sunPosition);
    skyboxShader->setUniform("starBrightness", settings.starBrightness);
    skyboxShader->setUniform("ambientLight", settings.ambientLight);

    // Bind textures
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, dayTexture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, nightTexture);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_CUBE_MAP, sunsetTexture);

    skyboxShader->setUniform("skyboxDay", 0);
    skyboxShader->setUniform("skyboxNight", 1);
    skyboxShader->setUniform("skyboxSunset", 2);

    // Render skybox cube
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    skyboxShader->unbind();
    glDepthFunc(GL_LESS);
}

void Skybox::init() {
    Renderer::getActive()->registerRenderable(this);
}