#pragma once
#ifndef MAPMANAGER_H
#define MAPMANAGER_H

#define LH 50
#define LW 120
#define OBS_COUNT 100
#define ROT_OBS_COUNT 400

#include "Types.h"
#include <cmath>

enum TileType {
    _W = 0, _L = 1, _S = 2, _E = 3,
    _K1 = 4, _K2 = 5, _K3 = 6, _K4 = 7, _K5 = 8,
    _D1 = 9, _D2 = 10, _D3 = 11, _D4 = 12, _D5 = 13
};

struct Obs {
    int r, c, dr, dc;
    int minR, maxR, minC, maxC;
    int speed; // ★ 속도 (1: 제일 빠름, 2: 2배 느림, 3: 3배 느림...)
    int tick;  // ★ 내부 타이머 (이동 타이밍 계산용)
};

struct RotObs {
    float cx, cy;
    float radius;
    float angle;
    float speed;
    int r, c;
};

struct StageInfo {
    const char* name;
    const char* worldName;
    int level;
};

class MapManager {
private:
    int currentStageNum;
    int (*mapData)[LW];

    bool hasKey[6];
    // UI 출력을 위해 필요한 변수들
    int playerR, playerC;
    bool hasKey1, hasKey2;

    static int stage1[LH][LW];
    static int stage2[LH][LW];
    static int stage3[LH][LW];
    static int stage4[LH][LW];
    static int stage5[LH][LW];
    int deaths;
    Obs obsList[OBS_COUNT];
    RotObs rotObsList[ROT_OBS_COUNT];
    int rotObsCount;
public:
    MapManager();
    void LoadStage(int stageNum);
    void RenderMap(); // ★ 합쳐질 렌더링 함수
    void UpdateObstacles();
    void MovePlayer(int dr, int dc);
    bool CanMove(int x, int y);
    int GetTile(int x, int y) const;
    void OpenDoorAnimation(int doorType);
    // 플레이어 정보를 외부에서 받아오기 위한 함수
    void UpdatePlayerState(int r, int c, bool k1, bool k2) {
        playerR = r; playerC = c; hasKey1 = k1; hasKey2 = k2;
    }
};

#endif