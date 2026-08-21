#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <glad/glad.h>

#include <QMainWindow>
#include <core/Application.h>
#include <geometry/Cube.hpp>
#include <render/texture/Texture.hpp>
#include "glwidget.h"
#include <core/platform/window/qt6/QT6Window.hpp>
#include <core/platform/input/IInput.hpp>
#include <render/renderer/Renderer.hpp>
#include <assets/AssetManager.hpp>

#include <private/qrhigles2_p.h>

#include <thirdparties/qrhiimgui2/imgui/qrhiimgui.h>
#include <widgets/sceneview/imguiii.h>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class HierarchyTreeWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

	void setupUI();
	void setupEngine();

    void loadScene() {};

private:
    Widget* imguicontainer = nullptr;

    std::unique_ptr<Renderer> renderer;
    std::unique_ptr<Application> app;
    std::unique_ptr<QT6Window> window;
    std::unique_ptr<Scene> scene;
    std::unique_ptr<CameraComponent> camera;
    std::unique_ptr<IInput> input;

    GLWidget* glWidget = nullptr;
    Ui::MainWindow *ui;
    RenderContext context;

    HierarchyTreeWidget* hierarchyWidget = nullptr;

protected:
    //void keyPressEvent(QKeyEvent* event) override;
    //void keyReleaseEvent(QKeyEvent* event) override;
    //void mousePressEvent(QMouseEvent* event) override;
    //void mouseReleaseEvent(QMouseEvent* event) override;
};

#endif // MAINWINDOW_H
