#include "imguiii.h"
#include <core/Engine.hpp>

Widget::Widget(QWidget* parent)
    : QRhiWidget(parent)
{
    setMouseTracking(true);
    this->setApi(QRhiWidget::Api::OpenGL);
}

void Widget::initialize(QRhiCommandBuffer* cb)
{
    if (!m_imguiRenderer) {
        ImGuiIO& io(ImGui::GetIO());
        io.IniFilename = nullptr;
        m_imgui.rebuildFontAtlasWithFont(QLatin1String(":/fonts/RobotoMono-Medium.ttf"));
        m_imguiRenderer.reset(new QRhiImguiRenderer);
    }
    if (m_rhi != rhi()) {
        m_ps.reset();
        m_rhi = rhi();
    }
    if (!m_ps) {
        m_vbuf.reset(m_rhi->newBuffer(QRhiBuffer::Immutable, QRhiBuffer::VertexBuffer, sizeof(vertexData)));
        m_vbuf->create();

        m_ubuf.reset(m_rhi->newBuffer(QRhiBuffer::Dynamic, QRhiBuffer::UniformBuffer, 68));
        m_ubuf->create();

        m_srb.reset(m_rhi->newShaderResourceBindings());
        m_srb->setBindings({
                QRhiShaderResourceBinding::uniformBuffer(0, QRhiShaderResourceBinding::VertexStage | QRhiShaderResourceBinding::FragmentStage,
                                                        m_ubuf.get())
            });
        m_srb->create();

        m_ps.reset(m_rhi->newGraphicsPipeline());

        QRhiGraphicsPipeline::TargetBlend premulAlphaBlend;
        premulAlphaBlend.enable = true;
        m_ps->setTargetBlends({ premulAlphaBlend });

        m_ps->setShaderStages({
            { QRhiShaderStage::Vertex, getShader(QLatin1String(":/shaders/color.vert.qsb")) },
            { QRhiShaderStage::Fragment, getShader(QLatin1String(":/shaders/color.frag.qsb")) }
            });

        QRhiVertexInputLayout inputLayout;
        inputLayout.setBindings({
            { 5 * sizeof(float) }
            });
        inputLayout.setAttributes({
            { 0, 0, QRhiVertexInputAttribute::Float2, 0 },
            { 0, 1, QRhiVertexInputAttribute::Float3, 2 * sizeof(float) }
            });

        m_ps->setVertexInputLayout(inputLayout);
        m_ps->setShaderResourceBindings(m_srb.get());
        m_ps->setRenderPassDescriptor(renderTarget()->renderPassDescriptor());

        m_ps->create();

        QRhiResourceUpdateBatch* resourceUpdates = m_rhi->nextResourceUpdateBatch();
        resourceUpdates->uploadStaticBuffer(m_vbuf.get(), vertexData);
        cb->resourceUpdate(resourceUpdates);
    }

    const QSize outputSize = renderTarget()->pixelSize();
    m_proj = m_rhi->clipSpaceCorrMatrix();
    m_proj.perspective(45.0f, outputSize.width() / (float)outputSize.height(), 0.01f, 1000.0f);
    m_proj.translate(0, 0, -4);

    m_guiMvp = m_rhi->clipSpaceCorrMatrix();
    const float dpr = devicePixelRatio();
    m_guiMvp.ortho(0, outputSize.width() / dpr, outputSize.height() / dpr, 0, 1, -1);
}

void Widget::render(QRhiCommandBuffer* cb)
{
    m_imgui.nextFrame(size(), devicePixelRatio(), QPointF(0, 0), std::bind(&Widget::gui, this));
    m_imgui.syncRenderer(m_imguiRenderer.get());

    m_imguiRenderer->prepare(m_rhi, renderTarget(), cb, m_guiMvp, 1.0f);

    QRhiResourceUpdateBatch* u = m_rhi->nextResourceUpdateBatch();
    m_rotation += 1.0f;
    QMatrix4x4 mvp = m_proj;
    mvp.rotate(m_rotation, 0, 1, 0);
    u->updateDynamicBuffer(m_ubuf.get(), 0, 64, mvp.constData());
    m_opacity += m_opacityDir * 0.005f;
    if (m_opacity < 0.0f || m_opacity > 1.0f) {
        m_opacityDir *= -1;
        m_opacity = qBound(0.0f, m_opacity, 1.0f);
    }
    u->updateDynamicBuffer(m_ubuf.get(), 64, 4, &m_opacity);

    const QSize outputSizeInPixels = renderTarget()->pixelSize();
    cb->beginPass(renderTarget(), QColor::fromRgbF(0.4f, 0.7f, 0.0f, 1.0f), { 1.0f, 0 }, u);

    cb->setGraphicsPipeline(m_ps.get());
    cb->setViewport({ 0, 0, float(outputSizeInPixels.width()), float(outputSizeInPixels.height()) });
    cb->setShaderResources();

    const QRhiCommandBuffer::VertexInput vbufBinding(m_vbuf.get(), 0);
    cb->setVertexInput(0, 1, &vbufBinding);

    m_imguiRenderer->render();

    cb->endPass();

    update();
}

void Widget::gui()
{
    qDebug()
        << "CurrentContext:"
        << ImGui::GetCurrentContext();

    if (!engine)
        return;

    //ImGui::ShowDemoWindow(&m_showDemoWindow);

    ImVec2 p = ImGui::GetCursorScreenPos();
    ImVec2 size = ImGui::GetContentRegionAvail();

    auto* drawList = ImGui::GetWindowDrawList();

    //drawList->AddImage(
    //    engine->getRenderer()->texture(),
    //    p,
    //    ImVec2(p.x + size.x, p.y + size.y)
    //);
    qDebug() << engine->getRenderer()->texture();

}

bool Widget::event(QEvent* e)
{
    if (m_imgui.processEvent(e))
        return true;

    return QRhiWidget::event(e);
}