#include "HierarchyObject.h"

HierarchyObject::HierarchyObject(const QString& title, QWidget* parent) : parent(parent)
{
    auto* layout = new QHBoxLayout(this);
    auto* label = new QLabel(title);

    auto* btnView = new QPushButton("V");
    auto* btnEdit = new QPushButton("E");
    auto* btnDel = new QPushButton("X");

    btnView->setFixedSize(QSize(20, 20));
    btnView->setIconSize(QSize(16, 16));

    btnEdit->setFixedSize(QSize(20, 20));
    btnEdit->setIconSize(QSize(16, 16));

    btnDel->setFixedSize(QSize(20, 20));
    btnDel->setIconSize(QSize(16, 16));

    layout->setContentsMargins(5, 2, 5, 2);
    layout->addWidget(btnView);
    layout->addWidget(label);
    layout->addStretch();
    layout->addWidget(btnEdit);
    layout->addWidget(btnDel);

    connect(btnDel, &QPushButton::clicked, this, &HierarchyObject::requestDelete);
}

