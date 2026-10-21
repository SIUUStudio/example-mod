#include <Geode/Geode.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>

using namespace geode::prelude;

float g_maxCorrectionDistance = 30.0f; 

struct SnappedInput {
    PlayerButton button;
    bool isPush;
    bool isPlayer2;
    float targetXPosition;
};

std::vector<SnappedInput> g_snappedQueue;

class $modify(AutocorrectLayer, GJBaseGameLayer) {
    void pushButton(PlayerButton button, bool isPlayer2) {
        auto player = isPlayer2 ? m_player2 : m_player1;
        if (!player) {
            GJBaseGameLayer::pushButton(button, isPlayer2);
            return;
        }

        float currentX = player->getPositionX();
        float snapX = -1.0f;
        float closestDist = g_maxCorrectionDistance;

        auto objects = m_objects;
        if (objects) {
            for (int i = 0; i < objects->count(); ++i) {
                auto obj = static_cast<GameObject*>(objects->objectAtIndex(i));
                if (!obj) continue;

                float objX = obj->getPositionX();
                float dist = objX - currentX;

                if (dist > 0 && dist < closestDist) {
                    if (obj->m_objectType == GameObjectType::Modifier || 
                        obj->m_objectType == GameObjectType::Hazard) {
                        closestDist = dist;
                        snapX = objX;
                    }
                }
            }
        }

        if (snapX > 0.0f) {
            log::info("Early click detected! Snapping input to X: {}", snapX);
            SnappedInput snapped = { button, true, isPlayer2, snapX };
            g_snappedQueue.push_back(snapped);
        } else {
            GJBaseGameLayer::pushButton(button, isPlayer2);
        }
    }

    void releaseButton(PlayerButton button, bool isPlayer2) {
        GJBaseGameLayer::releaseButton(button, isPlayer2);
    }

    void update(float dt) {
        auto player1 = m_player1;
        if (player1 && !g_snappedQueue.empty()) {
            float currentX = player1->getPositionX();
            auto it = g_snappedQueue.begin();
            while (it != g_snappedQueue.end()) {
                if (currentX >= it->targetXPosition) {
                    log::info("Executing autocorrected click at ideal position.");
                    GJBaseGameLayer::pushButton(it->button, it->isPlayer2);
                    it = g_snappedQueue.erase(it);
                } else {
                    ++it;
                }
            }
        }
        GJBaseGameLayer::update(dt);
    }
};
