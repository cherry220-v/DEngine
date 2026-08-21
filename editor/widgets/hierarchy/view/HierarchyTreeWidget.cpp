#include "HierarchyTreeWidget.hpp"
#include <QHeaderView>
#include <QMenu>

HierarchyTreeWidget::HierarchyTreeWidget(QWidget* parent) : parent(parent)
{
    this->setHeaderHidden(true);
    this->setContextMenuPolicy(Qt::CustomContextMenu);

    SceneHierarchyModel* model = new SceneHierarchyModel();
    m_commands = new SceneCommands(this);

    this->setModel(model);

    QHeaderView* header = this->header();

    header->setStretchLastSection(false);

    header->setSectionResizeMode(0, QHeaderView::Stretch);

    header->setSectionResizeMode(1, QHeaderView::Fixed);
    header->setSectionResizeMode(2, QHeaderView::Fixed);
    header->setSectionResizeMode(3, QHeaderView::Fixed);

    this->setColumnWidth(1, 25);
    this->setColumnWidth(2, 25);
    this->setColumnWidth(3, 25);

    connect(this, &QTreeView::clicked, this, &HierarchyTreeWidget::onCellClicked);
    connect(this, &QTreeView::customContextMenuRequested, this, &HierarchyTreeWidget::showContextMenu);
}

void HierarchyTreeWidget::onCellClicked(const QModelIndex& index)
{
    if (!index.isValid())
        return;

    auto& model = m_model;
    auto* node = static_cast<HierarchyNode*>(index.internalPointer());
    if (!node)
        return;

    Entity entity = node->entity;
    int column = index.column();

    switch (column)
    {
    case 0:
        emit entitySelected(entity);
        break;

    case 1:
        node->visible = !node->visible;
        emit model->dataChanged(index, index, { Qt::DecorationRole });
        this->update(index);

        emit entityToggledVisibility(entity, node->visible);
        break;

    case 2:
        node->locked = !node->locked;
        emit model->dataChanged(index, index, { Qt::DecorationRole });
        this->update(index);

        emit entityToggledLock(entity, node->locked);
        break;

    case 3:
        emit entityDeleted(entity);
        break;

    default:
        break;
    }
}

void HierarchyTreeWidget::setScene(Scene* scene)
{
	m_scene = scene;
}

void HierarchyTreeWidget::setContext(EditorContext* ctx)
{
	m_ctx = ctx;
}

void HierarchyTreeWidget::setModel(SceneHierarchyModel* model)
{
    m_model = model;
    QTreeView::setModel(model);
}

void HierarchyTreeWidget::showContextMenu(const QPoint& pos)
{
    QModelIndex index = this->indexAt(pos);

    QMenu contextMenu(this);

    if (index.isValid())
    {
        auto* node = static_cast<HierarchyNode*>(index.internalPointer());
        if (!node) return;

        m_commands->setCurrentEntity(node->entity);

        contextMenu.addAction(m_commands->createChildAction());
        contextMenu.addAction(m_commands->renameAction());
        contextMenu.addSeparator();
        contextMenu.addAction(m_commands->deleteAction());
    }
    else
    {
        m_commands->setCurrentEntity(INVALID_ENTITY);

        QAction* createRootAction = new QAction(tr("Create Empty Entity"), &contextMenu);
        connect(createRootAction, &QAction::triggered, this, [this]() {
            emit m_commands->requestCreateChild(INVALID_ENTITY);
            });
        contextMenu.addAction(createRootAction);
    }

    contextMenu.exec(this->viewport()->mapToGlobal(pos));
}
