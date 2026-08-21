#pragma once

#include <QDockWidget>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QStyle>

class CustomDock : public QDockWidget
{
	Q_OBJECT

public:
	CustomDock(QWidget* parent = nullptr);
	~CustomDock() = default;

	void setWindowTitle(const QString& title);

private:
	QLabel* titleLabel = nullptr;
	QWidget* parent;
};