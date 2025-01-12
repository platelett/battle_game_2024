#pragma once
#include "battle_game/core/unit.h"

namespace battle_game::unit {

class TwinViaPlatelet : public Unit {
 public:
    TwinViaPlatelet(GameCore *game_core, uint32_t id, uint32_t player_id);
    void Render() override;
    void Update() override;
    [[nodiscard]] bool IsHit(glm::vec2 position) const override;

 protected:
    void Fire();
    [[nodiscard]] const char *UnitName() const override;
    [[nodiscard]] const char *Author() const override;

 private:
    float turret_rotation_{0.0f};
    uint32_t fire_count_down_{0};
    static uint32_t twin_body_model_index;
    static uint32_t twin_turret_model_index;
    const glm::vec2 left_barrel_offset_{-0.2f, 1.2f};
    const glm::vec2 right_barrel_offset_{0.2f, 1.2f};
    glm::vec2 RotateVector(const glm::vec2& vec, float angle) const;
};

}  // namespace battle_game::unit