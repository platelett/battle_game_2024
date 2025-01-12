#include "twin_via_platelet.h"

#include "battle_game/core/bullets/bullets.h"
#include "battle_game/core/game_core.h"
#include "battle_game/graphics/graphics.h"

#include <vector>
#include <cmath>

namespace battle_game::unit {

uint32_t TwinViaPlatelet::twin_body_model_index = 0xffffffffu;
uint32_t TwinViaPlatelet::twin_turret_model_index = 0xffffffffu;

// 旋转向量的辅助函数
glm::vec2 TwinViaPlatelet::RotateVector(const glm::vec2& vec, float angle) const {
    float c = std::cos(angle);
    float s = std::sin(angle);
    return glm::vec2(c * vec.x - s * vec.y, s * vec.x + c * vec.y);
}

TwinViaPlatelet::TwinViaPlatelet(GameCore *game_core, uint32_t id, uint32_t player_id)
    : Unit(game_core, id, player_id) {
  if (twin_body_model_index == 0xffffffffu) {
    auto mgr = AssetsManager::GetInstance();
    {
      /* TwinViaPlatelet Body */
      twin_body_model_index = mgr->RegisterModel(
          {
              {{-0.8f, 0.8f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
              {{-0.8f, -1.0f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
              {{0.8f, 0.8f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
              {{0.8f, -1.0f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
              {{0.6f, 1.0f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
              {{-0.6f, 1.0f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
          },
          {0, 1, 2, 1, 2, 3, 0, 2, 5, 2, 4, 5});
    }

    {
      /* TwinTurret with Two Barrels */
      std::vector<ObjectVertex> turret_vertices;
      std::vector<uint32_t> turret_indices;
      const int precision = 60;
      const float inv_precision = 1.0f / float(precision);
      
      // 创建圆形基础
      for (int i = 0; i < precision; i++) {
        auto theta = (float(i) + 0.5f) * inv_precision;
        theta *= glm::pi<float>() * 2.0f;
        auto sin_theta = std::sin(theta);
        auto cos_theta = std::cos(theta);
        turret_vertices.emplace_back(
            ObjectVertex{{sin_theta * 0.5f, cos_theta * 0.5f},
                        {0.0f, 0.0f},
                        {0.7f, 0.7f, 0.7f, 1.0f}});
        turret_indices.push_back(i);
        turret_indices.push_back((i + 1) % precision);
        turret_indices.push_back(precision);
      }
      // 圆心顶点
      turret_vertices.emplace_back(
          ObjectVertex{{0.0f, 0.0f},
                      {0.0f, 0.0f},
                      {0.7f, 0.7f, 0.7f, 1.0f}});

      // 创建双炮管（两个矩形）
      float barrel_width = 0.1f;
      float barrel_length = 1.2f;
      float left_x_offset = -0.2f;  // 左炮管偏移
      float right_x_offset = 0.2f;  // 右炮管偏移

      // 创建左炮管（矩形）
      turret_vertices.push_back(
          {{-0.1f + left_x_offset, 0.0f}, {0.0f, 0.0f}, {0.7f, 0.7f, 0.7f, 1.0f}});
      turret_vertices.push_back(
          {{0.1f + left_x_offset, 0.0f}, {0.0f, 0.0f}, {0.7f, 0.7f, 0.7f, 1.0f}});
      turret_vertices.push_back(
          {{-0.1f + left_x_offset, 1.2f}, {0.0f, 0.0f}, {0.7f, 0.7f, 0.7f, 1.0f}});
      turret_vertices.push_back(
          {{0.1f + left_x_offset, 1.2f}, {0.0f, 0.0f}, {0.7f, 0.7f, 0.7f, 1.0f}});

      turret_indices.push_back(precision + 1 + 0);    
      turret_indices.push_back(precision + 1 + 1);
      turret_indices.push_back(precision + 1 + 2);
      turret_indices.push_back(precision + 1 + 1);
      turret_indices.push_back(precision + 1 + 2);
      turret_indices.push_back(precision + 1 + 3);

      // 创建右炮管（矩形）
      turret_vertices.push_back(
          {{-0.1f + right_x_offset, 0.0f}, {0.0f, 0.0f}, {0.7f, 0.7f, 0.7f, 1.0f}});
      turret_vertices.push_back(
          {{0.1f + right_x_offset, 0.0f}, {0.0f, 0.0f}, {0.7f, 0.7f, 0.7f, 1.0f}});
      turret_vertices.push_back(
          {{-0.1f + right_x_offset, 1.2f}, {0.0f, 0.0f}, {0.7f, 0.7f, 0.7f, 1.0f}});
      turret_vertices.push_back(
          {{0.1f + right_x_offset, 1.2f}, {0.0f, 0.0f}, {0.7f, 0.7f, 0.7f, 1.0f}});
          
      turret_indices.push_back(precision + 5 + 0);    
      turret_indices.push_back(precision + 5 + 1);
      turret_indices.push_back(precision + 5 + 2);
      turret_indices.push_back(precision + 5 + 1);
      turret_indices.push_back(precision + 5 + 2);
      turret_indices.push_back(precision + 5 + 3);

      // 注册炮塔模型
      twin_turret_model_index = mgr->RegisterModel(turret_vertices, turret_indices);
    }
  }
}

void TwinViaPlatelet::Render() {
  // 渲染主体
  battle_game::SetTransformation(position_, rotation_);
  battle_game::SetTexture(0);
  battle_game::SetColor(game_core_->GetPlayerColor(player_id_));
  battle_game::DrawModel(twin_body_model_index);
  
  // 渲染炮塔
  battle_game::SetRotation(turret_rotation_);
  battle_game::DrawModel(twin_turret_model_index);
  
  // 渲染生命条
  RenderLifeBar();
}

void TwinViaPlatelet::Update() {
  // 处理移动
  auto player = game_core_->GetPlayer(player_id_);
  if (player) {
    auto &input_data = player->GetInputData();
    glm::vec2 offset{0.0f};
    if (input_data.key_down[GLFW_KEY_W]) {
      offset.y += 1.0f;
    }
    if (input_data.key_down[GLFW_KEY_S]) {
      offset.y -= 1.0f;
    }
    float speed = 3.0f * GetSpeedScale();
    offset *= kSecondPerTick * speed;
    auto new_position =
        position_ + RotateVector(offset, rotation_);
    if (!game_core_->IsBlockedByObstacles(new_position)) {
      game_core_->PushEventMoveUnit(id_, new_position);
    }

    float rotation_offset = 0.0f;
    if (input_data.key_down[GLFW_KEY_A]) {
      rotation_offset += 1.0f;
    }
    if (input_data.key_down[GLFW_KEY_D]) {
      rotation_offset -= 1.0f;
    }
    rotation_offset *= kSecondPerTick * glm::radians(180.0f) * GetSpeedScale();
    game_core_->PushEventRotateUnit(id_, rotation_ + rotation_offset);
  }

  // 处理炮塔旋转
  if (player) {
    auto &input_data = player->GetInputData();
    auto diff = input_data.mouse_cursor_position - position_;
    if (glm::length(diff) < 1e-4) {
      turret_rotation_ = rotation_;
    } else {
      turret_rotation_ = std::atan2(diff.y, diff.x) - glm::radians(90.0f);
    }
  }

  // 处理发射子弹
  Fire();
}

void TwinViaPlatelet::Fire() {
  if (fire_count_down_ == 0) {
    auto player = game_core_->GetPlayer(player_id_);
    if (player) {
      auto &input_data = player->GetInputData();
      if (input_data.mouse_button_down[GLFW_MOUSE_BUTTON_LEFT]) {
        // 计算左炮口的位置
        glm::vec2 left_fire_position = position_ + RotateVector(left_barrel_offset_, turret_rotation_);
        // 计算右炮口的位置
        glm::vec2 right_fire_position = position_ + RotateVector(right_barrel_offset_, turret_rotation_);
        // 计算子弹的速度
        glm::vec2 velocity = RotateVector(glm::vec2{0.0f, 20.0f}, turret_rotation_);

        // 生成左侧子弹
        GenerateBullet<bullet::CannonBall>(
            left_fire_position,
            turret_rotation_,
            GetDamageScale(),
            velocity);

        // 生成右侧子弹
        GenerateBullet<bullet::CannonBall>(
            right_fire_position,
            turret_rotation_,
            GetDamageScale(),
            velocity);

        // 设置发射间隔为 0.1 秒
        fire_count_down_ = kTickPerSecond / 10;
      }
    }
  }
  if (fire_count_down_) {
    fire_count_down_--;
  }
}

bool TwinViaPlatelet::IsHit(glm::vec2 position) const {
  position = WorldToLocal(position);
  return position.x > -0.8f && position.x < 0.8f && position.y > -1.0f &&
         position.y < 1.0f && position.x + position.y < 1.6f &&
         position.y - position.x < 1.6f;
}

const char *TwinViaPlatelet::UnitName() const {
  return "Twin Via Platelet";
}

const char *TwinViaPlatelet::Author() const {
  return "platelet";
}

}  // namespace battle_game::unit