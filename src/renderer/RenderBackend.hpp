#pragma once

#include "BasicMath.hpp"
#include "DeviceContext.h"
#include "EngineFactoryVk.h"
#include "RefCntAutoPtr.hpp"
#include "RenderDevice.h"
#include "SwapChain.h"

// Linux Includes
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_X11
#include <GLFW/glfw3native.h>

#include <cstring>
#include <vector>

using namespace Diligent;

class RenderBackend {
public:
  RefCntAutoPtr<IRenderDevice> m_pDevice;
  RefCntAutoPtr<IDeviceContext> m_pImmediateContext;
  RefCntAutoPtr<ISwapChain> m_pSwapChain;
  RefCntAutoPtr<IPipelineState> m_pPSO;
  RefCntAutoPtr<IBuffer> m_pVSConstants;
  RefCntAutoPtr<IShaderResourceBinding> m_pSRB;

  bool Init(GLFWwindow *window) {
    if (!window)
      return false;

    SwapChainDesc SCDesc;
    SCDesc.ColorBufferFormat = TEX_FORMAT_RGBA8_UNORM;
    SCDesc.DepthBufferFormat = TEX_FORMAT_D32_FLOAT;

    EngineVkCreateInfo EngineCI;
#ifndef NDEBUG
    EngineCI.EnableValidation = true;
#endif

    auto *m_pFactoryVk = GetEngineFactoryVk();
    m_pFactoryVk->CreateDeviceAndContextsVk(EngineCI, &m_pDevice,
                                            &m_pImmediateContext);

    if (!m_pDevice)
      return false;

    LinuxNativeWindow WindowAttribs;
    WindowAttribs.WindowId = glfwGetX11Window(window);
    WindowAttribs.pDisplay = glfwGetX11Display();

    m_pFactoryVk->CreateSwapChainVk(m_pDevice, m_pImmediateContext, SCDesc,
                                    WindowAttribs, &m_pSwapChain);

    CreatePipeline();
    return true;
  }

  void CreatePipeline() {
    GraphicsPipelineStateCreateInfo PSOCreateInfo;
    PSOCreateInfo.PSODesc.Name = "Chunk PSO";
    PSOCreateInfo.PSODesc.PipelineType = PIPELINE_TYPE_GRAPHICS;

    std::vector<LayoutElement> LayoutElems = {
        LayoutElement(0, 0, 3, VT_FLOAT32, false, 0, 32),
        LayoutElement(1, 0, 2, VT_FLOAT32, false, 12, 32),
        LayoutElement(2, 0, 3, VT_FLOAT32, false, 20, 32)};

    PSOCreateInfo.GraphicsPipeline.InputLayout.LayoutElements =
        LayoutElems.data();
    PSOCreateInfo.GraphicsPipeline.InputLayout.NumElements =
        (Uint32)LayoutElems.size();

    PSOCreateInfo.GraphicsPipeline.NumRenderTargets = 1;
    PSOCreateInfo.GraphicsPipeline.RTVFormats[0] =
        m_pSwapChain->GetDesc().ColorBufferFormat;
    PSOCreateInfo.GraphicsPipeline.DSVFormat =
        m_pSwapChain->GetDesc().DepthBufferFormat;
    PSOCreateInfo.GraphicsPipeline.PrimitiveTopology =
        PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

    PSOCreateInfo.GraphicsPipeline.RasterizerDesc.CullMode = CULL_MODE_NONE;
    PSOCreateInfo.GraphicsPipeline.DepthStencilDesc.DepthEnable = true;

    ShaderCreateInfo ShaderCI;
    ShaderCI.SourceLanguage = SHADER_SOURCE_LANGUAGE_HLSL;

    RefCntAutoPtr<IShaderSourceInputStreamFactory> pShaderSourceFactory;
    m_pDevice->GetEngineFactory()->CreateDefaultShaderSourceStreamFactory(
        "assets/shaders", &pShaderSourceFactory);
    ShaderCI.pShaderSourceStreamFactory = pShaderSourceFactory;

    RefCntAutoPtr<IShader> pVS;
    ShaderCI.Desc.ShaderType = SHADER_TYPE_VERTEX;
    ShaderCI.Desc.Name = "Chunk VS";
    ShaderCI.FilePath = "chunk.vsh";
    m_pDevice->CreateShader(ShaderCI, &pVS);

    RefCntAutoPtr<IShader> pPS;
    ShaderCI.Desc.ShaderType = SHADER_TYPE_PIXEL;
    ShaderCI.Desc.Name = "Chunk PS";
    ShaderCI.FilePath = "chunk.psh";
    m_pDevice->CreateShader(ShaderCI, &pPS);

    PSOCreateInfo.pVS = pVS;
    PSOCreateInfo.pPS = pPS;

    BufferDesc CBDesc;
    CBDesc.Name = "VS Constants";
    CBDesc.Size = sizeof(float) * 16;
    CBDesc.Usage = USAGE_DYNAMIC;
    CBDesc.BindFlags = BIND_UNIFORM_BUFFER;
    CBDesc.CPUAccessFlags = CPU_ACCESS_WRITE;
    m_pDevice->CreateBuffer(CBDesc, nullptr, &m_pVSConstants);

    PSOCreateInfo.PSODesc.ResourceLayout.DefaultVariableType =
        SHADER_RESOURCE_VARIABLE_TYPE_STATIC;
    m_pDevice->CreateGraphicsPipelineState(PSOCreateInfo, &m_pPSO);

    m_pPSO->GetStaticVariableByName(SHADER_TYPE_VERTEX, "Constants")
        ->Set(m_pVSConstants);
    m_pPSO->CreateShaderResourceBinding(&m_pSRB, true);
  }

  RefCntAutoPtr<IBuffer>
  CreateVertexBuffer(const std::vector<float> &vertices) {
    BufferDesc VertBuffDesc;
    VertBuffDesc.Name = "Chunk Vertex Buffer";
    VertBuffDesc.Usage = USAGE_IMMUTABLE;
    VertBuffDesc.BindFlags = BIND_VERTEX_BUFFER;
    VertBuffDesc.Size = vertices.size() * sizeof(float);

    BufferData VBData;
    VBData.pData = vertices.data();
    VBData.DataSize = vertices.size() * sizeof(float);

    RefCntAutoPtr<IBuffer> pBuffer;
    m_pDevice->CreateBuffer(VertBuffDesc, &VBData, &pBuffer);
    return pBuffer;
  }

  void UpdateConstants(const void *data, size_t size) {
    void *pMappedData = nullptr;
    m_pImmediateContext->MapBuffer(m_pVSConstants, MAP_WRITE, MAP_FLAG_DISCARD,
                                   pMappedData);

    if (pMappedData) {
      memcpy(pMappedData, data, size);
      m_pImmediateContext->UnmapBuffer(m_pVSConstants, MAP_WRITE);
    }
  }

  void Clear() {
    const float ClearColor[] = {0.5f, 0.7f, 1.0f, 1.0f};
    auto *pRTV = m_pSwapChain->GetCurrentBackBufferRTV();
    auto *pDSV = m_pSwapChain->GetDepthBufferDSV();
    m_pImmediateContext->SetRenderTargets(
        1, &pRTV, pDSV, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
    m_pImmediateContext->ClearRenderTarget(
        pRTV, ClearColor, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
    m_pImmediateContext->ClearDepthStencil(
        pDSV, CLEAR_DEPTH_FLAG, 1.0f, 0,
        RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
  }

  void Present() { m_pSwapChain->Present(); }
};
