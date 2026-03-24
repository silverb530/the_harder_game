#include "Render.h"
#include <iostream>

using namespace std;


RenderManager::RenderManager() {
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    // 커서 숨기기 (화면을 깔끔하게 유지하기 위함)
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(hConsole, &cursorInfo);
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(hConsole, &cursorInfo);
}

void RenderManager::SetCursor(int x, int y) {
    COORD pos = { (SHORT)x, (SHORT)y };
    SetConsoleCursorPosition(hConsole, pos);
}

void RenderManager::Draw(const vector<vector<int>>& map, const Player& player, const vector<Obstacle>& obstacles) {
    SetCursor(0, 0); // 매번 화면을 지우지 않고 맨 위에서부터 덮어씁니다.

    for (size_t y = 0; y < map.size(); ++y) {
        for (size_t x = 0; x < map[y].size(); ++x) {

            // 1. 플레이어 그리기 (소수점 좌표를 int로 캐스팅하여 칸에 맞춤)
            if ((int)player.pos.x == x && (int)player.pos.y == y) {
                cout << "P ";
                continue;
            }

            // 2. 장애물 그리기
            bool isObstaclePrinted = false;
            for (const auto& obs : obstacles) {
                if ((int)obs.pos.x == x && (int)obs.pos.y == y) {
                    cout << "O "; // 장애물 표현
                    isObstaclePrinted = true;
                    break;
                }
            }
            if (isObstaclePrinted) continue;

            // 3. 맵 그리기 (규태님이 정의한 숫자에 따라 출력)
            if (map[y][x] == 1) cout << "■"; // 벽
            else if (map[y][x] == 2) cout << "S "; // 시작점
            else if (map[y][x] == 3) cout << "E "; // 끝점
            else cout << "  "; // 길
        }
        cout << "\n";
    }
}