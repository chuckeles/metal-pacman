#pragma once

#include "Component.hpp"
#include "Events.hpp"


/**
 * Allows the player to control an entity.
 */


class Entity;

class PlayerMoveComponent : public Component {

public:

    virtual void OnAttach(std::weak_ptr<Entity> entity) override;
    virtual void OnDetach() override;
    virtual std::string GetType() const override;

    void Move(float delta);


private:

    std::weak_ptr<Entity> mEntity;
    Events::ListenerId listenerId;

    bool mPressed = false;
    float mRotationBuffer = 0;

};


