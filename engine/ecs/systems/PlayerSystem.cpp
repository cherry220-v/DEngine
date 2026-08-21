#include "PlayerSystem.hpp"

#include <core/EngineContext.hpp>
#include <core/platform/window/IWindow.hpp>
#include "../components/Transform.hpp"
#include "../components/PlayerComponent.hpp"
#include "../components/CameraComponent.hpp"
#include <ecs/Scene.hpp>
#include <glm/vec3.hpp>

#include <iostream>
#include "PhysicsSystem.hpp"

void PlayerSystem::init(EngineContext* ctx)
{
    m_input = ctx->input;
}

void PlayerSystem::setInput(IInput* input)
{
    m_input = input;
}

void PlayerSystem::update(Scene& scene, float dt)
{
    if (m_input == nullptr)
    {
        return;
    }

    auto v = scene.view<PlayerComponent, Transform>();

    if (v.empty()) return;
    m_input->update();

    for (auto e : v)
    {
        auto* player = scene.getComponent<PlayerComponent>(e);
        auto* cam = scene.getComponent<CameraComponent>(player->camera);
        auto* camTransform = scene.getComponent<Transform>(player->camera);
        auto* transform = scene.getComponent<Transform>(e);
        cam->aspect = (float)m_input->window()->getWidth() / (float)m_input->window()->getHeight();

        if (m_input->getKey(Key::Escape)) {
            m_input->window()->setCursorMode(CursorMode::Normal);
            player->setActive(0);
        }

        if (m_input->getKey(Key::L)) {
            m_input->window()->setCursorMode(CursorMode::Locked);
            player->setActive(1);
        }

        if (!player->isActive()) continue;

        double xpos, ypos;
        m_input->getMousePosition(xpos, ypos);

        static double lastX = xpos;
        static double lastY = ypos;
        static bool first = true;

        if (first)
        {
            lastX = xpos;
            lastY = ypos;
            first = false;
        }

        float xoffset = float(xpos - lastX);
        float yoffset = float(lastY - ypos);

        lastX = xpos;
        lastY = ypos;

        xoffset *= player->sensitivity;
        yoffset *= player->sensitivity;

        player->yaw += xoffset;
        player->pitch += yoffset;

        if (player->pitch > 89.0f) player->pitch = 89.0f;
        if (player->pitch < -89.0f) player->pitch = -89.0f;

        camTransform->position = transform->position;
        glm::quat qPitch =
            glm::angleAxis(
                glm::radians(player->pitch),
                glm::vec3(1, 0, 0)
			);
        glm::quat qYaw =
            glm::angleAxis(
                glm::radians(-player->yaw),
                glm::vec3(0, 1, 0)
            );
		camTransform->rotation = qYaw * qPitch;

        player->front =
            glm::normalize(
                camTransform->rotation *
                glm::vec3(0, 0, -1)
            );

        player->right =
            glm::normalize(
                camTransform->rotation *
                glm::vec3(1, 0, 0)
            );

        player->up =
            glm::normalize(
                camTransform->rotation *
                glm::vec3(0, 1, 0)
            );

        const glm::vec3 worldUp(0.0f, 1.0f, 0.0f);

        float speed =
            m_input->getKey(Key::LeftShift)
            ? player->shiftSpeed
            : player->moveSpeed;

        glm::vec3 moveDir(0.0f);

        if (m_input->getKey(Key::W))
            moveDir += player->front;

        if (m_input->getKey(Key::S))
            moveDir -= player->front;

        if (m_input->getKey(Key::A))
            moveDir -= player->right;

        if (m_input->getKey(Key::D))
            moveDir += player->right;

        moveDir.y = 0.0f;

        if (glm::length(moveDir) > 0.0f)
            moveDir = glm::normalize(moveDir);
        auto physicsSystem = scene.getSystem<PhysicsSystem>();
        physicsSystem->moveCharacter(
            scene,
            e,
            moveDir* speed * dt,
            dt
        );

        if (!player->isActive()) return;

    }
}