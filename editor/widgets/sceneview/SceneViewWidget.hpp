#pragma once

#include <QWidget>
#include <QObject>

class SceneViewWidget : public QWidget
{
	Q_OBJECT
public:
	explicit SceneViewWidget(QWidget* parent = nullptr);

private:
	QWidget* parent = nullptr;

	QWindow* m_renderWindow;
};