#include "SceneCommands.hpp"

#include <QIcon>

SceneCommands::SceneCommands(QObject* parent) : QObject(parent)
{
    m_renameAction = new QAction(QIcon(":/icons/edit.png"), tr("Rename"), this);
    m_renameAction->setShortcut(QKeySequence(Qt::Key_F2));
    connect(m_renameAction, &QAction::triggered, this, [this]() {
        if (m_currentEntity != INVALID_ENTITY) emit requestRename(m_currentEntity);
        });

    m_createChildAction = new QAction(QIcon(":/icons/add.png"), tr("Create Child Entity"), this);
    connect(m_createChildAction, &QAction::triggered, this, [this]() {
        if (m_currentEntity != INVALID_ENTITY) emit requestCreateChild(m_currentEntity);
        });

    m_deleteAction = new QAction(QIcon(":/icons/delete.png"), tr("Delete"), this);
    m_deleteAction->setShortcut(QKeySequence(Qt::Key_Delete));
    connect(m_deleteAction, &QAction::triggered, this, [this]() {
        if (m_currentEntity != INVALID_ENTITY) emit requestDelete(m_currentEntity);
        });
}

void SceneCommands::setCurrentEntity(Entity e)
{
    m_currentEntity = e;
    //m_deleteAction->setEnabled(e != ROOT_ENTITY);
}