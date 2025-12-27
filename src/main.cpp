#include <GLFW/glfw3.h>
#include <ctime>
#include <iostream>
#include <vector>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "core/JavaRandom.hpp"
#include "core/SeedHelper.hpp"
#include "renderer/Camera.hpp"
#include "renderer/RenderBackend.hpp"
#include "tools/WorldVerifier.hpp"
#include "world/Chunk.hpp"
#include "world/Noise.hpp"
#include "world/WorldGen.hpp"
#include <random>

Camera *g_Camera = nullptr;

void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
  if (g_Camera)
    g_Camera->processMouse(xpos, ypos);
}

int main() {
  std::cout << "[ProjectBeta] Starting..." << std::endl;
  MathHelper::Init();

  if (!glfwInit())
    return -1;
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  GLFWwindow *window =
      glfwCreateWindow(1280, 720, "Project Beta (Vulkan)", nullptr, nullptr);

  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glfwSetCursorPosCallback(window, mouse_callback);

  Camera camera(glm::vec3(0.0f, 250.0f, 200.0f));
  camera.yaw = -90.0f;
  camera.pitch = -60.0f;
  camera.updateVectors();
  g_Camera = &camera;

  RenderBackend renderer;
  if (!renderer.Init(window))
    return -1;

  std::cout << "[WorldGen] Starting World Generation..." << std::endl;

  std::string seedStr = "";
  int64_t seed;

  if (seedStr.empty()) {
    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<int64_t> dis;
    seed = dis(gen);
  } else {
    seed = parseSeed(seedStr);
  }

  // Override for testing functionality
  seed = -100;

  // VERIFICATION
  std::string regionPath =
      "/home/rockeywerewolf/DevProjects/ProjectBeta/decomp/minecraft_any/game/"
      "saves/checking/region/r.0.0.mcr";
  // We check region 0,0 (Chunks 0..31, 0..31)
  ParityVerifier::Verifier::verify(regionPath, seed, 0, 0);

  std::cout << "[WorldGen] Random Seed: " << seed << std::endl;
  WorldGen worldGen(seed);

  std::vector<float> rawVertices;
  std::vector<Chunk> chunks;

  int radius = 8;
  int width = radius * 2;

  for (int x = -radius; x < radius; ++x) {
    for (int z = -radius; z < radius; ++z) {
      Chunk chunk(x, z);
      worldGen.generateChunk(chunk);
      chunks.push_back(chunk);
    }
  }

  std::cout << "[WorldGen] Populating Chunks..." << std::endl;
  for (int x = -radius; x < radius; ++x) {
    for (int z = -radius; z < radius; ++z) {
      worldGen.populate(chunks, radius, x, z);
    }
  }

  size_t totalVerts = 0;
  for (size_t i = 0; i < chunks.size(); ++i) {
    int xIndex = i / width;
    int zIndex = i % width;

    const Chunk *nX = nullptr;
    const Chunk *pX = nullptr;
    const Chunk *nZ = nullptr;
    const Chunk *pZ = nullptr;

    if (xIndex > 0)
      nX = &chunks[i - width];
    if (xIndex < width - 1)
      pX = &chunks[i + width];
    if (zIndex > 0)
      nZ = &chunks[i - 1];
    if (zIndex < width - 1)
      pZ = &chunks[i + 1];

    std::vector<Vertex> mesh;
    chunks[i].generateMesh(mesh, nX, pX, nZ, pZ);

    for (const auto &v : mesh) {
      rawVertices.push_back(v.x);
      rawVertices.push_back(v.y);
      rawVertices.push_back(v.z);
      rawVertices.push_back(v.u);
      rawVertices.push_back(v.v);
      rawVertices.push_back(v.r);
      rawVertices.push_back(v.g);
      rawVertices.push_back(v.b);
    }
    totalVerts += mesh.size();
  }
  std::cout << "[Meshing] Generated " << totalVerts << " vertices across "
            << chunks.size() << " chunks." << std::endl;

  auto pChunkBuffer = renderer.CreateVertexBuffer(rawVertices);

  float lastFrame = 0.0f;

  while (!glfwWindowShouldClose(window)) {
    float lastTitleUpdate = 0.0f;
    float currentFrame = (float)glfwGetTime();
    float deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    // Process Keyboard
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
      glfwSetWindowShouldClose(window, true);
    camera.processInput(window, deltaTime);

    if (currentFrame - lastTitleUpdate > 0.1f) {
      lastTitleUpdate = currentFrame;
      std::string title =
          "Project Beta | Pos: " + std::to_string((int)camera.position.x) +
          ", " + std::to_string((int)camera.position.y) + ", " +
          std::to_string((int)camera.position.z);
      glfwSetWindowTitle(window, title.c_str());
    }

    glfwPollEvents();

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    float aspect =
        (width > 0 && height > 0) ? (float)width / (float)height : 1.0f;

    static int frameCount = 0;
    if (frameCount++ % 1000 == 0) {
      std::cout << "[Debug] Window: " << width << "x" << height
                << " Aspect: " << aspect << std::endl;
    }

    glm::mat4 projection =
        glm::perspective(glm::radians(60.0f), aspect, 0.1f, 1000.0f);
    glm::mat4 view = camera.getViewMatrix();
    glm::mat4 vp = projection * view;

    glm::mat4 transposedVP = glm::transpose(vp);

    renderer.UpdateConstants(&transposedVP[0][0], sizeof(glm::mat4));

    renderer.Clear();

    renderer.m_pImmediateContext->SetPipelineState(renderer.m_pPSO);
    renderer.m_pImmediateContext->CommitShaderResources(
        renderer.m_pSRB, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

    Uint64 offset = 0;
    IBuffer *pBuffs[] = {pChunkBuffer};
    renderer.m_pImmediateContext->SetVertexBuffers(
        0, 1, pBuffs, &offset, RESOURCE_STATE_TRANSITION_MODE_TRANSITION,
        SET_VERTEX_BUFFERS_FLAG_RESET);

    DrawAttribs drawAttrs;
    drawAttrs.NumVertices = (Uint32)(rawVertices.size() / 8);
    renderer.m_pImmediateContext->Draw(drawAttrs);

    renderer.Present();
  }

  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}
