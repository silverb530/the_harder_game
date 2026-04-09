// 푸시 테스트용 파일
#include <iostream>
#include <Windows.h>
#include <conio.h>
#include "Types.h"
#include "GameSystem.h"
#include "MapManager.h"

int main() {
    system("mode con cols=270 lines=80");

    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);

    // 2. [레이아웃 설정 대신] 화면 버퍼 크기를 창 크기와 1:1로 맞춤
    COORD bufferSize = { 270, 55 };
    SetConsoleScreenBufferSize(hOut, bufferSize);

    // 3. ANSI 활성화
    DWORD dwMode = 0;
    if (GetConsoleMode(hOut, &dwMode)) {
        dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        SetConsoleMode(hOut, dwMode);
    }

    // 4. 커서 숨기기
    CONSOLE_CURSOR_INFO ci = { 1, FALSE };
    SetConsoleCursorInfo(hOut, &ci);

    // 5.  마우스 클릭 시 화면 멈춤(빠른 편집 모드) 방지
    HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);
    DWORD dwInMode;
    if (GetConsoleMode(hIn, &dwInMode)) {
        // 빠른 편집 모드 해제 및 확장 플래그 유지
        dwInMode &= ~ENABLE_QUICK_EDIT_MODE;
        dwInMode |= ENABLE_EXTENDED_FLAGS;
        SetConsoleMode(hIn, dwInMode);
    }

    MapManager mapMgr;
    mapMgr.LoadStage(6);

    // 초기 화면 렌더링
    mapMgr.RenderMap();

    while (true) {
        if (_kbhit()) {
            int key = _getch();
            if (key == 27) break;
            if (key == 224) {
                key = _getch();
                if (key == 72) mapMgr.MovePlayer(-1, 0); // ↑
                if (key == 80) mapMgr.MovePlayer(1, 0); // ↓
                if (key == 75) mapMgr.MovePlayer(0, -1); // ←
                if (key == 77) mapMgr.MovePlayer(0, 1); // →
            }
        }
        mapMgr.UpdateObstacles();
        mapMgr.RenderMap();
        Sleep(40);
    }

}
