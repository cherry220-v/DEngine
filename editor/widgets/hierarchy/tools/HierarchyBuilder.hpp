#pragma once

class Scene;
class SceneHierarchyModel;

class HierarchyBuilder
{
public:
    static void build(Scene* scene, SceneHierarchyModel& model);
};