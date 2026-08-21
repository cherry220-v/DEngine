#pragma once

#include <QApplication>
#include <QCommandLineParser>
#include <QRhiWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QCheckBox>
#include <QScrollArea>
#include <QFont>
#include <QFile>

#include <thirdparties/qrhiimgui2/imgui/qrhiimgui.h>

#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif

#include <thirdparties/imgui/imgui.h>

class Engine;

static float vertexData[] = {
     0.0f,   0.5f,   1.0f, 0.0f, 0.0f,
    -0.5f,  -0.5f,   0.0f, 1.0f, 0.0f,
     0.5f,  -0.5f,   0.0f, 0.0f, 1.0f,
};

static QShader getShader(const QString& name)
{
    QFile f(name);
    return f.open(QIODevice::ReadOnly) ? QShader::fromSerialized(f.readAll()) : QShader();
}

class Widget : public QRhiWidget
{
public:
    Widget(QWidget* parent = nullptr);

    void initialize(QRhiCommandBuffer* cb) override;
    void render(QRhiCommandBuffer* cb) override;

    bool event(QEvent* e) override;
    Engine* engine = nullptr;

private:
    void gui();

    QRhi* m_rhi = nullptr;
    QMatrix4x4 m_proj;
    std::unique_ptr<QRhiBuffer> m_vbuf;
    std::unique_ptr<QRhiBuffer> m_ubuf;
    std::unique_ptr<QRhiShaderResourceBindings> m_srb;
    std::unique_ptr<QRhiGraphicsPipeline> m_ps;

    std::unique_ptr<QRhiImguiRenderer> m_imguiRenderer;
    QRhiImgui m_imgui;
    QMatrix4x4 m_guiMvp;

    float m_rotation = 0;
    float m_opacity = 1;
    int m_opacityDir = -1;

    bool m_showDemoWindow = false;
};

struct ScrollAreaKeyFilter : public QObject
{
    ScrollAreaKeyFilter(QObject* target) : m_target(target) {}
    bool eventFilter(QObject* obj, QEvent* e) override {
        switch (e->type()) {
        case QEvent::KeyPress:
        case QEvent::KeyRelease:
            QCoreApplication::sendEvent(m_target, e);
            return true;
        default:
            break;
        }
        return QObject::eventFilter(obj, e);
    }
    QObject* m_target;
};

inline int exmain(int argc, char** argv)
{
    QApplication app(argc, argv);

    QRhiWidget::Api graphicsApi;
//#if defined(Q_OS_WIN)
//    graphicsApi = QRhiWidget::Api::Direct3D11;
//#elif defined(Q_OS_MACOS) || defined(Q_OS_IOS)
//    graphicsApi = QRhiWidget::Api::Metal;
//#elif QT_CONFIG(vulkan)
//    graphicsApi = QRhiWidget::Api::Vulkan;
//#else
//    graphicsApi = QRhiWidget::Api::OpenGL;
//#endif
    graphicsApi = QRhiWidget::Api::OpenGL;
    QCommandLineParser cmdLineParser;
    cmdLineParser.addHelpOption();
    QCommandLineOption nullOption({ "n", "null" }, QLatin1String("Null"));
    cmdLineParser.addOption(nullOption);
    QCommandLineOption glOption({ "g", "opengl" }, QLatin1String("OpenGL"));
    cmdLineParser.addOption(glOption);
    QCommandLineOption vkOption({ "v", "vulkan" }, QLatin1String("Vulkan"));
    cmdLineParser.addOption(vkOption);
    QCommandLineOption d3d11Option({ "d", "d3d11" }, QLatin1String("Direct3D 11"));
    cmdLineParser.addOption(d3d11Option);
    QCommandLineOption d3d12Option({ "D", "d3d12" }, QLatin1String("Direct3D 12"));
    cmdLineParser.addOption(d3d12Option);
    QCommandLineOption mtlOption({ "m", "metal" }, QLatin1String("Metal"));
    cmdLineParser.addOption(mtlOption);

    cmdLineParser.process(app);
    if (cmdLineParser.isSet(nullOption))
        graphicsApi = QRhiWidget::Api::Null;
    if (cmdLineParser.isSet(glOption))
        graphicsApi = QRhiWidget::Api::OpenGL;
    if (cmdLineParser.isSet(vkOption))
        graphicsApi = QRhiWidget::Api::Vulkan;
    if (cmdLineParser.isSet(d3d11Option))
        graphicsApi = QRhiWidget::Api::Direct3D11;
    if (cmdLineParser.isSet(d3d12Option))
        graphicsApi = QRhiWidget::Api::Direct3D12;
    if (cmdLineParser.isSet(mtlOption))
        graphicsApi = QRhiWidget::Api::Metal;

    QVBoxLayout* layout = new QVBoxLayout;

    QScrollArea* scrollArea = new QScrollArea;
    Widget* rhiWidget = new Widget;
    rhiWidget->resize(1920, 1080);
    rhiWidget->setApi(graphicsApi);
    qDebug() << rhiWidget->api();
    scrollArea->setWidget(rhiWidget);
    layout->addWidget(scrollArea);

    ScrollAreaKeyFilter scrollAreaKeyFilter(rhiWidget);
    scrollArea->installEventFilter(&scrollAreaKeyFilter);

    QVBoxLayout* otherContent = new QVBoxLayout;
    otherContent->addWidget(new QLabel(QLatin1String("A label")));
    otherContent->addWidget(new QPushButton(QLatin1String("A button")));
    QCheckBox* cb = new QCheckBox(QLatin1String("Show widget a widget on top"));
    cb->setChecked(true);
    otherContent->addWidget(cb);
    layout->addLayout(otherContent);

    QWidget topLevel;
    topLevel.setLayout(layout);
    topLevel.resize(1280, 720);

    QLabel* overlayLabel = new QLabel(&topLevel);
    overlayLabel->setText(QLatin1String("Some QWidget on top\n(to prove stacking and translucency works)"));
    overlayLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    overlayLabel->setAutoFillBackground(true);
    QPalette semiTransparent(QColor(255, 0, 0, 64));
    semiTransparent.setBrush(QPalette::Text, Qt::white);
    semiTransparent.setBrush(QPalette::WindowText, Qt::white);
    overlayLabel->setPalette(semiTransparent);
    QFont f = overlayLabel->font();
    f.setPixelSize(QFontInfo(f).pixelSize() * 2);
    f.setWeight(QFont::Bold);
    overlayLabel->setFont(f);
    overlayLabel->resize(600, 150);
    overlayLabel->move(200, 500);

    QObject::connect(cb, &QCheckBox::stateChanged, overlayLabel, [overlayLabel, cb] {
        overlayLabel->setVisible(cb->isChecked());
        });

    topLevel.show();
    return app.exec();
}