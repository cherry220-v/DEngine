#include "CustomDock.h"

CustomDock::CustomDock(QWidget* parent) : parent(parent)
{
	auto* container = new QWidget(this);

	container->setContentsMargins(1, 0, 1, 1);

	auto* layout = new QHBoxLayout();
	layout->setContentsMargins(5, 0, 5, 0);

	titleLabel = new QLabel(this);
	auto* optionsButton = new QPushButton(this);
	auto* sizeButton = new QPushButton(this);
	auto* closeButton = new QPushButton(this);

	//optionsButton->setIcon();
	optionsButton->setFixedSize(QSize(20, 20));
	optionsButton->setIconSize(QSize(16, 16));
	optionsButton->setAutoFillBackground(false);

	sizeButton->setFixedSize(QSize(20, 20));
	sizeButton->setIconSize(QSize(16, 16));
	sizeButton->setAutoFillBackground(false);

	closeButton->setFixedSize(QSize(20, 20));
	closeButton->setIconSize(QSize(16, 16));
	closeButton->setAutoFillBackground(false);

	layout->addWidget(titleLabel);
	layout->addStretch();
	layout->addWidget(optionsButton);
	layout->addWidget(sizeButton);
	layout->addWidget(closeButton);

	container->setLayout(layout);

	closeButton->setIcon(style()->standardIcon(QStyle::SP_TitleBarCloseButton));

	sizeButton->setIcon(style()->standardIcon(QStyle::SP_TitleBarNormalButton));

	optionsButton->setIcon(style()->standardIcon(QStyle::SP_MessageBoxQuestion));


	this->setTitleBarWidget(container);

	connect(closeButton, &QPushButton::clicked, this, &QDockWidget::close);

	connect(sizeButton, &QPushButton::clicked, [this]() {
		this->setFloating(!this->isFloating());
		});
}

void CustomDock::setWindowTitle(const QString& title)
{
	QDockWidget::setWindowTitle(title);
	this->titleLabel->setText(title);
}
