#ifndef GAMESYSTEM_H
#define GAMESYSTEM_H


#include "Types.h" // 공통 구조체(Player, Obstacle) 포함 [cite: 31, 32]
#include <vector>
#include <cmath>   // distance 계산을 위한 sqrt 사용 [cite: 22]

class GameSystem {
public:
    // 게임의 현재 상태 정의 [cite: 36]
    enum class GameState {
        MENU,
        PLAYING,
        PLAYER_DEAD,
        STAGE_CLEAR,
        GAME_OVER
    };

private:
    GameState currentState;
    int currentStage;
    const float collisionThreshold = 0.8f; // 충돌 판정 기준 거리 [cite: 21]

public:
    GameSystem() : currentState(GameState::MENU), currentStage(1) {}

    // 1. 충돌 판정 로직 (핵심)
    // 플레이어와 장애물 사이의 거리를 계산하여 충돌 여부 반환 [cite: 21, 40]
    bool CheckCollision(const Player& player, const std::vector<Obstacle>& obstacles) {
        for (const auto& obs : obstacles) {
            // 거리 공식: sqrt((x2-x1)^2 + (y2-y1)^2) [cite: 22]
            float dist = std::sqrt(std::pow(obs.pos.x - player.pos.x, 2) +
                std::pow(obs.pos.y - player.pos.y, 2));

            if (dist < collisionThreshold) {
                return true; // 충돌 발생 [cite: 21]
            }
        }
        return false;
    }

    // 2. 플레이어 사망 처리 [cite: 22, 36]
    void HandlePlayerDeath(Player& player, Point startPos) {
        player.deaths++;        // 데스 카운트 증가 [cite: 22]
        player.isAlive = false; // 사망 상태로 변경 [cite: 5, 22]
        player.pos = startPos;  // 시작 지점으로 강제 이동 [cite: 22]
        currentState = GameState::PLAYER_DEAD;
    }

    // 3. 게임 상태 관리 함수들 [cite: 36]
    void SetState(GameState state) { currentState = state; }
    GameState GetState() const { return currentState; }

    int GetCurrentStage() const { return currentStage; }
    void NextStage() { currentStage++; }
};

#endif