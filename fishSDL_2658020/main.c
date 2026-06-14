
#include <SDL.h>
#include <SDL_ttf.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>
#include "engine.h"

// 전역 변수
FishTank fishTanks[NUM];    // 물고기 배열
int level = 1;
int position = 0;
bool running = true;
bool gameStarted = false;   /// 게임 시작 전 메뉴가 표시되도록 확인
bool gameOver = false;
bool gameWin = false;
long startTime = 0;
long lastUpdateTime = 0;

// SDL
SDL_Window * window = NULL;          
SDL_Renderer * renderer = NULL;      
TTF_Font * font = NULL;              
SDL_Texture * fishTextures[6] = { NULL };   /// 물고기별 이미지
SDL_Texture * deadFishTexture = NULL;   ///죽은 물고기 이미지

// 오디오
SDL_AudioDeviceID audioDevice = 0; 
SDL_AudioSpec wavSpec;
Uint8 * wavBuffer = NULL;
Uint32 wavLength = 0;               

// 메인 함수
int main(int argc, char* argv[])
{
    srand((unsigned int)time(NULL));
    // 엔진 초기화
    if (!engine_init())
    {
        printf("loading fish images...\n");

        fishTextures[normal_fi] = loadTexture("normal_fi.bmp");

        if (fishTextures[normal_fi] == NULL)
        {
            printf("normal_fi.bmp 로드 실패\n");
        }

        printf("image load done\n");
        return 1;                   // 초기화 실패 시 프로그램 종료
    }

    initGame();                     // 게임 초기화

    while (running) // 게임 루프
    {               
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
                running = false;
            handleInput(&event);
        }

        /// 게임이 바로 시작되지 않도록 시작 메뉴 설정
        if (gameStarted)
        {
            updateGame();
            renderGame();
        }
        else
        {
            renderMenu();
        }

        // 윈도우 업데이트
        SDL_Delay(100);             // 게임 루프 간격 조절, 100ms마다 업데이트 및 렌더링
    }
    // 게임 종료 및 자원 해제
    cleanupGame();

    return 0;
}
