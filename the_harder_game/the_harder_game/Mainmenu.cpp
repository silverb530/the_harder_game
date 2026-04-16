#define _CRT_SECURE_NO_WARNINGS
#include "MainMenu.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cwchar>
#include <ctime>
#include <cmath>
#include <conio.h>

// ════════════════════════════════════════════════════════════════════
//  레이아웃 행 번호 (26pt 폰트 / 270x55 버퍼 최적화)
// ════════════════════════════════════════════════════════════════════
static const int P1_Y = 3;      // 타이틀 첫 번째 단락
static const int P2_Y = 10;     // 타이틀 두 번째 단락
static const int TAG_Y = 18;    // 태그라인 (상태 메시지)
static const int SEP_Y = 21;    // 중앙 구분선
static const int MENU_Y[4] = { 25, 29, 33, 37 }; // 메뉴 항목 간격 확대
static const int HNT_Y = 43;    // 하단 조작 가이드
static const int DTH_Y = 47;    // 데스 카운터

// ════════════════════════════════════════════════════════════════════
//  ASCII 타이틀 아트
// ════════════════════════════════════════════════════════════════════
static const wchar_t* P1[] = {
    L" _____ _             __        __         _     _  _      ",
    L"|_   _| |__   ___   \\ \\      / /__  _ __| | __| || |___  ",
    L"  | | | '_ \\ / _ \\   \\ \\ /\\ / / _ \\| '__| |/ _` || / __| ",
    L"  | | | | | |  __/    \\ V  V / (_) | |  | | (_| || \\__ \\ ",
    L"  |_| |_| |_|\\___|     \\_/\\_/ \\___/|_|  |_|\\__,_||_|___/ "
};
static const int P1_CNT = 5;

static const wchar_t* P2[] = {
    L" _   _               _           _       ____      _          _       _     ",
    L"| | | | __ _ _ __ __| | ___  ___| |_    / ___| ___| |_       | | ___ | |__  ",
    L"| |_| |/ _` | '__/ _` |/ _ \\/ __| __|  | |  _ / _ \\ __|   _  | |/ _ \\| '_ \\ ",
    L"|  _  | (_| | | | (_| |  __/\\__ \\ |_   | |_| |  __/ |_   | |_| | (_) | |_) |",
    L"|_| |_|\\__,_|_|  \\__,_|\\___||___/\\__|   \\____|\\___| \\__|  \\___/ \\___/|_.__/ ",
};
static const int P2_CNT = 5;
/// <summary>
/// 화장실은 쉬는시간에
/// </summary>
static const wchar_t* TAGS[] = {
    L"[ 취업 성공률 0.0001% - 연구 결과 (n=1, 본인) ]",
    L"[ 이력서 97번째 수정 중... 맞춤법은 나중에 ]",
    L"[ 면접관: '열정이 있나요?'   지원자: '...' ]",
    L"[ 자소서 1000자 제한인데 쓸 말이 없음 ]",
    L"[ 부모님: 언제 취업해?   나: 지금 연습 중 ]",
    L"[ 이 게임 클리어 = 포트폴리오 1줄 추가 ]"
};
static const int TAG_CNT = 6;

static const wchar_t* MENU_LABEL[] = { L"게임 시작", L"조작 방법", L"명예의 전당", L"현실 도피" };
static const wchar_t* MENU_KEY[] = { L"ENTER", L"H", L"R", L"ESC" };

MainMenu::MainMenu() : sel(0), deaths(0), tagIdx(0), frame(0), titleDone(false), titleLine(0), titleDelay(0) {
    srand((unsigned)time(nullptr));
}

void MainMenu::InitDrops() {
    for (int i = 0; i < DROP_CNT; ++i) {
        // 매트릭스 비가 중앙 UI를 가리지 않도록 좌우 끝에만 배치
        if (i < DROP_CNT / 2) drops[i].x = rand() % 40;
        else drops[i].x = (ScreenBuffer::W - 40) + rand() % 40;

        drops[i].y = rand() % ScreenBuffer::H;
        drops[i].spd = 1 + rand() % 3;
        drops[i].tick = 0;
        drops[i].ch = L"0123456789ABCDEF"[rand() % 16];
    }
}

void MainMenu::RenderMatrix() {
    for (int i = 0; i < DROP_CNT; ++i) {
        Drop& d = drops[i];
        if (d.y >= 0 && d.y < ScreenBuffer::H) {
            sb.PutW(d.x, d.y, d.ch, ATTR(CC_DKGREEN, CC_BLACK));
        }
        if (++d.tick >= d.spd) {
            d.tick = 0; d.y++;
            if (d.y >= ScreenBuffer::H) d.y = 0;
        }
    }
}

void MainMenu::RenderTitle() {
    int totalSteps = P1_CNT + 1 + P2_CNT;
    int currentLimit = titleDone ? totalSteps : titleLine;

    // ★ 블록 내 가장 긴 줄 기준 X로 고정 → 줄마다 삐뚤어지는 문제 해결
    int p1MaxW = 0;
    for (int i = 0; i < P1_CNT; ++i) {
        int w = ScreenBuffer::DisplayWidth(P1[i]);
        if (w > p1MaxW) p1MaxW = w;
    }
    int p2MaxW = 0;
    for (int i = 0; i < P2_CNT; ++i) {
        int w = ScreenBuffer::DisplayWidth(P2[i]);
        if (w > p2MaxW) p2MaxW = w;
    }
    int p1X = ((ScreenBuffer::W - p1MaxW) / 2) - 15;
    int p2X = ((ScreenBuffer::W - p2MaxW) / 2) - 15;
    if (p1X < 0) p1X = 0;
    if (p2X < 0) p2X = 0;

    for (int i = 0; i < currentLimit; ++i) {
        if (i < P1_CNT)
            sb.PutStrW(p1X, P1_Y + i, P1[i], ATTR(CC_YELLOW, CC_BLACK));
        else if (i > P1_CNT)
            sb.PutStrW(p2X, P2_Y + (i - P1_CNT - 1), P2[i - P1_CNT - 1], ATTR(CC_YELLOW, CC_BLACK));
    }

    if (!titleDone && --titleDelay <= 0) {
        titleDelay = 3;
        if (++titleLine >= totalSteps) titleDone = true;
    }
}

void MainMenu::RenderMenu() {
    if (!titleDone) return;
    sb.PutStrCtrW(SEP_Y, L"───────────────── SELECT ─────────────────", ATTR(CC_DKGRAY, CC_BLACK));

    for (int i = 0; i < MENU_CNT; ++i) {
        bool active = (i == sel);
        WORD attr = active ? ATTR(CC_WHITE, CC_BLACK) : ATTR(CC_DKGRAY, CC_BLACK);

        // 메뉴 텍스트를 고정 폭으로 만들어 중앙 정렬 시 흔들리지 않게 함
        wchar_t mBuf[128];
        if (active)
            swprintf(mBuf, 128, L"▶ %-12s [%s] ◀", MENU_LABEL[i], MENU_KEY[i]);
        else
            swprintf(mBuf, 128, L"  %-12s [%s]  ", MENU_LABEL[i], MENU_KEY[i]);

        sb.PutStrCtrW(MENU_Y[i], mBuf, attr);
    }
}

void MainMenu::RenderTagline() {
    if (titleDone) sb.PutStrCtrW(TAG_Y, TAGS[tagIdx], ATTR(CC_GRAY, CC_BLACK));
}

void MainMenu::RenderHints() {
    if (titleDone) sb.PutStrCtrW(HNT_Y, L"▲▼: 이동  ENTER: 선택  H: 도움말", ATTR(CC_DKGRAY, CC_BLACK));
}

void MainMenu::RenderDeathCounter() {
    if (titleDone) {
        wchar_t dBuf[64];
        swprintf(dBuf, 64, L"누적 탈락자: %d명", deaths);
        sb.PutStrCtrW(DTH_Y, dBuf, ATTR(CC_RED, CC_BLACK));
    }
}

void MainMenu::RenderPopup(int idx) {
    const int PW = 80, PH = 14;
    const int PX = ((ScreenBuffer::W - PW) / 2) - 15;
    const int PY = ((ScreenBuffer::H - PH) / 2);

    sb.FillRect(PX, PY, PW, PH, L' ', ATTR(CC_WHITE, CC_BLACK));
    for (int x = PX; x < PX + PW; ++x) { sb.PutW(x, PY, L'=', ATTR(CC_YELLOW, CC_BLACK)); sb.PutW(x, PY + PH - 1, L'=', ATTR(CC_YELLOW, CC_BLACK)); }
    for (int y = PY; y < PY + PH; ++y) { sb.PutW(PX, y, L'|', ATTR(CC_YELLOW, CC_BLACK)); sb.PutW(PX + PW - 1, y, L'|', ATTR(CC_YELLOW, CC_BLACK)); }

    // ★ 팝업 박스 내부 중앙 정렬 헬퍼 (테두리 1칸 안쪽 기준)
    auto putCtr = [&](int row, const wchar_t* s, WORD attr) {
        int dw = ScreenBuffer::DisplayWidth(s);
        int x = PX + 1 + ((PW - 2) - dw) / 2;
        if (x < PX + 1) x = PX + 1;
        sb.PutStrW(x, row, s, attr);
        };

    if (idx == 0) {
        putCtr(PY + 3, L"[ SYSTEM WARNING ]", ATTR(CC_RED, CC_BLACK));
        putCtr(PY + 6, L"정말로 이 지옥 같은 게임을 시작하시겠습니까?", ATTR(CC_BLACK, CC_WHITE));
        putCtr(PY + 10, L"시작(ENTER) / 취소(ESC)", ATTR(CC_DKGRAY, CC_BLACK));
    }
    else if (idx == 1) {
        putCtr(PY + 3, L"[ 조작법 가이드 ]", ATTR(CC_CYAN, CC_BLACK));
        putCtr(PY + 6, L"방향키 : 이동  /  ESC : 메뉴", ATTR(CC_BLACK, CC_WHITE));
        putCtr(PY + 10, L"아무 키나 누르면 닫힙니다.", ATTR(CC_DKGRAY, CC_BLACK));
    }
    else if (idx == 3) {
        putCtr(PY + 4, L"현실 세계(바탕화면)로 도망치시겠습니까?", ATTR(CC_BLACK, CC_WHITE));
        putCtr(PY + 10, L"탈출(ENTER) / 잔류(ESC)", ATTR(CC_DKGRAY, CC_BLACK));
    }
}

bool MainMenu::WaitPopupInput(int idx) {
    while (true) {
        RenderAll();
        RenderPopup(idx);
        sb.Flip();
        if (_kbhit()) {
            int k = _getch();
            if (idx == 0 || idx == 3) {
                if (k == 13) return true;
                if (k == 27) return false;
            }
            else return false;
        }
        Sleep(33);
    }
}

void MainMenu::RenderAll() {
    sb.Clear(ATTR(CC_DKGRAY, CC_BLACK));
    RenderMatrix();
    RenderTitle();
    RenderTagline();
    RenderMenu();
    RenderHints();
    RenderDeathCounter();
}

MainMenu::MenuResult MainMenu::Show(HANDLE hOut) {
    sb.Init(hOut);
    InitDrops();
    while (true) {
        RenderAll();
        sb.Flip();
        if (_kbhit()) {
            int k = _getch();
            if (k == 224) {
                k = _getch();
                if (k == 72) sel = (sel - 1 + MENU_CNT) % MENU_CNT;
                else if (k == 80) sel = (sel + 1) % MENU_CNT;
            }
            else if (k == 13) {
                if (sel == 0 || sel == 3) {
                    if (WaitPopupInput(sel)) return (sel == 0) ? MenuResult::START_GAME : MenuResult::EXIT;
                }
                else WaitPopupInput(sel);
            }
            else if (k == 27) {
                if (WaitPopupInput(3)) return MenuResult::EXIT;
            }
            else if (k == 'h' || k == 'H') WaitPopupInput(1);
        }
        if (frame % 180 == 0) tagIdx = (tagIdx + 1) % TAG_CNT;
        if (frame % 30 == 0) deaths += rand() % 3;
        frame++;
        Sleep(33);
    }
}