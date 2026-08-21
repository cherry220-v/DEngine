#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include "widgets/ObjHierarchyView.h"
#include "widgets/CustomDock.h"

#include "widgets/hierarchy/view/HierarchyTreeWidget.hpp"
#include "widgets/hierarchy/model/SceneHierarchyModel.hpp"

#include <core/platform/render/qt6/QtRenderPlatform.hpp>
#include <core/platform/input/qt6/Qt6Input.hpp>
#include <core/platform/window/qt6/Qt6Window.hpp>

#include <memory>
#include <render/renderer/gl/OpenGLRenderDevice.hpp>
#include <render/renderer/Renderer.hpp>
#include <assets/AssetManager.hpp>
#include <core/Application.h>
#include <core/platform/render/glfw/GLFWRenderPlatform.hpp>
#include "engine/EditorApp.hpp"
#include <core/Engine.hpp>

// https://github.com/paceholder/nodeeditor

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow()
    , ui(new Ui::MainWindow)
{
    QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts);

    ui->setupUi(this);

	setupUI();

	setupEngine();

    connect(glWidget, &GLWidget::initialized, this, &MainWindow::loadScene);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupUI()
{
    auto* hierarchyDock = new CustomDock(this);
    hierarchyDock->setWindowTitle("Hierarchy");

    addDockWidget(Qt::LeftDockWidgetArea, hierarchyDock);

    hierarchyWidget = new HierarchyTreeWidget(hierarchyDock);

    ObjData data;
    data.text = "TEXT";
    glWidget = new GLWidget(renderer.get(), app.get(), this);
    //glWidget->hide();

    imguicontainer = new Widget();
    ui->mainLayout->addWidget(imguicontainer);

    hierarchyDock->setWidget(hierarchyWidget);
}

void MainWindow::setupEngine()
{
    WindowProps props;
    props.title = "Qt Engine Window";

    AssetManager& assetManager = AssetManager::get();
    assetManager.setAssetsPath("C:/Users/Trash/DEngine/assets");
    AssetLoaderRegistration::registerAll();
    assetManager.scanAssets();

    app = std::make_unique<Application>();
	EditorApp::init(app.get());

    window = std::make_unique<QT6Window>(props, this);

    auto renderDevice = std::make_unique<OpenGLRenderDevice>();
    auto renderer = std::make_unique<Renderer>();

    auto platform = std::make_unique<QtRenderPlatform>(imguicontainer);

    input = std::make_unique<QT6Input>(window.get());

    renderer->setDevice(std::move(renderDevice));
    renderer->setPlatform(std::move(platform));

    app->setInput(std::move(input));

    app->engine()->setRenderer(std::move(renderer));
    app->engine()->setupRenderer();

    std::ifstream file("C:/Users/Trash/DEngine/assets/scenes/Main.scene");

    if (!file.is_open())
        std::cout << "FILE NOT OPEN\n";

    std::string text((std::istreambuf_iterator<char>(file)),
        std::istreambuf_iterator<char>());

    auto data = nlohmann::json::parse(text);

    auto scene = Scene::deserialize(data);

	auto model = new SceneHierarchyModel;
	model->buildFromScene(scene.get());
    this->hierarchyWidget->setModel(model);

    app->setWindow(
        std::move(window)
    );

    glWidget->setApplication(app.get());
    qDebug() << "RUN";
    app->engine()->setScene(
        std::move(scene)
    );
    imguicontainer->engine = app->engine();

    //app->engine()->setActiveCamera(
    //    &camera
    //);

    //app->engine()->setPlayerEntity(
    //    &player
    //);

    //app->run();
}