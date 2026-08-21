#include <glad/glad.h>
#include "mainwindow.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>

int main(int argc, char* argv[])
{

    QApplication a(argc, argv);

    MainWindow w;

    w.resize(1280, 720);
    w.show();

    return a.exec();
}