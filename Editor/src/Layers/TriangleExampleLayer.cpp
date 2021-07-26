#include "TriangleExampleLayer.h"

#include "../Renderer/Renderer.h"
#include "../Renderer/Shader.h"

void TriangleExampleLayer::OnInit() {
	ShaderLibrary::Instance().Load("assets/shaders/VertexPosColor.glsl");

    triangleVA.reset(new VertexArray());
    float triangleVertices[3 * 7] = {
        -0.5f, -0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f,
         0.5f, -0.5f, 0.0f, 0.2f, 0.3f, 0.8f, 1.0f,
         0.0f,  0.5f, 0.0f, 0.8f, 0.8f, 0.2f, 1.0f,
    };
    const auto triangleVB = std::make_shared<VertexBuffer>(triangleVertices, (uint32_t)sizeof(triangleVertices));
    BufferLayout layout = {
        { ShaderDataType::Float3, "a_Position" },
        { ShaderDataType::Float4, "a_Color" },
    };
    triangleVB->SetLayout(layout);
    triangleVA->AddVertexBuffer(triangleVB);

    uint32_t triangleIndices[3] = { 0, 1, 2 };
    const auto triangleIB = std::make_shared<IndexBuffer>(triangleIndices, (uint32_t)(sizeof(triangleIndices) / sizeof(uint32_t)));
    triangleVA->SetIndexBuffer(triangleIB);


    quadVertexArray.reset(new VertexArray());
    float squareVertices[(3 + 2) * 4] = {
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
         0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
         0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
        -0.5f,  0.5f, 0.0f, 0.0f, 1.0f,
    };

    const auto quadVertexBuffer = std::make_shared<VertexBuffer>(squareVertices, (uint32_t)sizeof(squareVertices));
    quadVertexBuffer->SetLayout({
        { ShaderDataType::Float3, "a_Position" },
        { ShaderDataType::Float2, "a_TexCoord" },
        });
    quadVertexArray->AddVertexBuffer(quadVertexBuffer);
    uint32_t squareIndices[3 * 2] = {
        0, 1, 2,
        2, 3, 0
    };
    const auto squareIB = std::make_shared<IndexBuffer>(squareIndices, (uint32_t)(sizeof(squareIndices) / sizeof(uint32_t)));
    quadVertexArray->SetIndexBuffer(squareIB);

    auto textureShader = ShaderLibrary::Instance().Load("assets/shaders/Texture.glsl");
    textureShader->Bind();
    textureShader->UploadUniformInt("u_Texture", diffuseTextureSlot);
    textureCheckerboard.reset(new Texture2D("assets/textures/Checkerboard.png"));
    textureWithAlpha.reset(new Texture2D("assets/textures/ChernoLogo.png"));
}

void TriangleExampleLayer::OnUpdate(Timestep ts) {
    auto triangleShader = ShaderLibrary::Instance().Get("VertexPosColor");
    Renderer::Submit(triangleShader, triangleVA, glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f, 0.0f, 0.0f)));

    auto textureShader = ShaderLibrary::Instance().Get("Texture");
    textureCheckerboard->Bind(diffuseTextureSlot);
    Renderer::Submit(textureShader, quadVertexArray, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));

    textureWithAlpha->Bind(diffuseTextureSlot);
    Renderer::Submit(textureShader, quadVertexArray, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)) * glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.1f)));
}