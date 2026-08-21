#pragma once

#include <QWidget>
#include <QTreeWidgetItem>
#include <QPushButton>
#include <QHBoxLayout>
#include <QLabel>

class HierarchyObject : public QWidget
{
	Q_OBJECT

signals:
	void requestDelete();
public:
	explicit HierarchyObject(const QString& title, QWidget* parent = nullptr);
	~HierarchyObject() = default;

	bool visible() { return isVisible; };
	void visible(bool v) { isVisible = v; }

	QString title() { return titleLabel->text(); }
	void title(QString text) { return titleLabel->setText(text); }

private:
	bool isVisible = true;

	QWidget* parent = nullptr;
	QLabel* titleLabel = nullptr;
};