#pragma once

#include <QObject>
#include <QAction>

#include <ecs/entity/Entity.hpp>

class SceneCommands : public QObject
{
	Q_OBJECT

public:
    explicit SceneCommands(QObject* parent = nullptr);

    QAction* deleteAction() const { return m_deleteAction; }
    QAction* renameAction() const { return m_renameAction; }
    QAction* createChildAction() const { return m_createChildAction; }

signals:
    void requestDelete(Entity e);
    void requestRename(Entity e);
    void requestCreateChild(Entity parent);

public slots:
    void setCurrentEntity(Entity e);

private:
    Entity m_currentEntity = INVALID_ENTITY;

    QAction* m_deleteAction = nullptr;
    QAction* m_renameAction = nullptr;
    QAction* m_createChildAction = nullptr;
};