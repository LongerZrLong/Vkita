#pragma once

#include <array>

#include "Core/Base.h"

#include "GraphicsContext.h"
#include "GraphicsPipeline.h"
#include "Buffer.h"
#include "DescriptorSetManager.h"

namespace VKT {

    class Renderer
    {
    public:
        static void Init(GraphicsContext *graphicsContext);

        static void OnWindowResize(uint32_t width, uint32_t height);

        static void BeginScene();
        static void EndScene();

        static void Draw(const Ref<GraphicsPipeline> &graphicsPipeline,
                         const Ref<VertexBuffer> &vertexBuffer,
                         const Ref<IndexBuffer> &indexBuffer,
                         const Ref<DescriptorSetManager> &descriptorSetManager);

        static GraphicsContext &GetGraphicsContext() { return *s_GraphicsContext; }

    private:
        Renderer() = default;
        ~Renderer() = default;

        static GraphicsContext *s_GraphicsContext;

    };
}