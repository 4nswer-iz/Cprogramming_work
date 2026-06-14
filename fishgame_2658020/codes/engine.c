
#include <stdio.h>
#include <SDL.h>
#include <SDL_ttf.h>    
#include <stdbool.h>			
#include "engine.h"

// 함수 정의

bool engine_init()
{

    // SDL 초기화, 비디오와 오디오 서브시스템 초기화
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0)
        return 0;

    // 창 생성 및 렌더러 초기화
    window = SDL_CreateWindow("Raising Fishes", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!window)
        return 0;
    
    /// 물고기별 이미지
    fishTextures[normal_fi] = loadTexture("normal_fi.bmp");
    fishTextures[lazy_fi] = loadTexture("lazy_fi.bmp");
    fishTextures[growth_fi] = loadTexture("growth_fi.bmp");
    fishTextures[baby_fi] = loadTexture("baby_fi.bmp");
    fishTextures[stupid_fi] = loadTexture("stupid_fi.bmp");
    fishTextures[big_fi] = loadTexture("big_fi.bmp");
    deadFishTexture = loadTexture("dead_fi.bmp");
    
    // TTF 초기화 및 폰트 로드
    if (TTF_Init() != 0)
    {
        printf("TTF Fail\n");
            return 0;
    }
    font = TTF_OpenFont("C:\\Windows\\Fonts\\arial.ttf", 20);       // 폰트 파일 경로(절대 혹은 상대) 필요
    if (!font)
    {
        printf("폰트 로드 실패: %s\n", TTF_GetError());
        SDL_Quit();
        return 0;
    }
    
    if (!fishTextures[normal_fi] ||
        !fishTextures[lazy_fi] ||
        !fishTextures[growth_fi] ||
        !fishTextures[baby_fi] ||
        !fishTextures[stupid_fi] ||
        !fishTextures[big_fi] ||
        !deadFishTexture)
    {
        return 0;
    }
    
    if (fishTextures[normal_fi] == NULL)
    {
        printf("normal fish texture load fail\n");
    }
    
    // 오디오 초기화
    if (!initAudio())
    {                            // 오디오 초기화 실패 시 에러 메시지 출력
        SDL_Quit();
        return 0;
    }
    
    return 1;
}

// 오디오 초기화 함수 정의
bool initAudio()
{
    // WAV 파일 로드, water.wav 파일을 메모리에 로드하여 wavSpec, wavBuffer, wavLength에 정보 저장
    if (SDL_LoadWAV("water.wav", &wavSpec, &wavBuffer, &wavLength) == NULL) {
        printf("WAV 파일 로드 실패: %s\n", SDL_GetError());
        return false;
    }
    // 오디오 출력 장치 열기
    audioDevice = SDL_OpenAudioDevice(
        NULL,       // 기본 오디오 장치 사용
        0,          // 0: 출력 장치
        &wavSpec,   // wav 파일의 오디오 형식
        NULL,       // 원하는 형식 그대로 사용
        0
    );
    if (audioDevice == 0)
    {
        printf("오디오 장치 열기 실패: %s\n", SDL_GetError());
        SDL_FreeWAV(wavBuffer);
        wavBuffer = NULL;
        return false;
    }
    // 오디오 장치를 재생가능한 상태로 만든다.
    SDL_PauseAudioDevice(audioDevice, 0); // 오디오 장치 재생 시작, 0: 재생, 1: 일시정지

    return true;
}
// 물 주는 소리 재생 함수 정의
void playWaterSound()
{
    if (audioDevice != 0 && wavBuffer != NULL)
    {
        SDL_ClearQueuedAudio(audioDevice);                  // 기존에 큐에 남아있는 소리 데이터 제거
        SDL_QueueAudio(audioDevice, wavBuffer, wavLength);  // wavBuffer에 저장된 소리 데이터를 오디오 장치에 보내서 재생
    }
}

/// 시작 메뉴 표시
void renderMenu()
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    renderText("RASING FISH GAME", 310, 150);

    renderText("How to play", 343, 200);
    renderText("J : Move Left", 343, 230);
    renderText("L : Move Right", 343, 260);
    renderText("K : Give Water", 343, 290);

    renderText("Press ENTER or SPACE to Start", 262, 380);

    SDL_RenderPresent(renderer);
}

void initGame()
{
    for (int i = 0; i < NUM; i++)
    {
        fishTanks[i].fish = 10;
        fishTanks[i].water = 100;
        fishTanks[i].isAlive = 1;
        fishTanks[i].growthStage = 0;   /// 물고기 성장 단계

    }

    /// 각 어항에 물고기 배치
    fishTanks[0].type = normal_fi;
    fishTanks[1].type = lazy_fi;
    fishTanks[2].type = growth_fi;
    fishTanks[3].type = baby_fi;
    fishTanks[4].type = stupid_fi;
    fishTanks[5].type = big_fi;

    startTime = SDL_GetTicks();             // 게임 시작 시간 기록
    lastUpdateTime = startTime;             // 마지막 업데이트 시간 초기화
}

void renderGame()
{                         // 게임 화면 렌더링
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);     // 배경을 검은색으로 설정
    SDL_RenderClear(renderer);                          // 화면 초기화
    renderFishTanks();                      // 어항과 물고기 렌더링

    char levelText[64];                     // 레벨 텍스트 렌더링
    sprintf_s(levelText, sizeof(levelText), "Level %d", level);     // 현재 레벨 표시

    long elapsedTime =
        (SDL_GetTicks() - startTime) / 1000;    /// 게임 경과 시간 표시
    
    char timeText[64];

    sprintf_s(timeText, sizeof(timeText), "Time : %ld", elapsedTime);
    
    renderText(levelText, 10, 10);          // 게임 상태 텍스트 렌더링

    renderText(timeText, 650, 10);  /// 시간 표시되도록 업데이트 전 순서로


    SDL_RenderPresent(renderer);            // 렌더링 업데이트

}

void cleanupGame()
{
    // 종료 메시지 화면
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);             // 배경을 검은색으로 설정
    SDL_RenderClear(renderer);                                  // 화면 초기화
    if (gameWin)
    {
        renderText("You Win! All levels completed!", 200, 200); // 승리 메시지 렌더링
    }
    else if (gameOver)
    {
        renderText("Game Over! All fish are dead!", 200, 200); // 게임 오버 메시지 렌더링
    }
    else
    {
        renderText("Game Over", 200, 200);      // 일반 게임 오버 메시지 렌더링
    }
    SDL_RenderPresent(renderer);                // 렌더링 업데이트   
    SDL_Delay(3000);                            // 메시지 표시 후 3초 대기

    for (int i = 0; i < NUM; i++)
    {
        if (fishTextures[i] != NULL)
        {
            SDL_DestroyTexture(fishTextures[i]);
        }
    }

    if (deadFishTexture != NULL)
    {
        SDL_DestroyTexture(deadFishTexture);
    }

    if (audioDevice != 0)
    {
        SDL_CloseAudioDevice(audioDevice);      // 오디오 장치 닫기
    }

    if (wavBuffer != NULL)
    {
        SDL_FreeWAV(wavBuffer);                 // WAV 버퍼 메모리 해제
    }
    TTF_CloseFont(font);                        // 폰트 메모리 해제
    SDL_DestroyRenderer(renderer);              // 렌더러 메모리 해제
    SDL_DestroyWindow(window);                  // 창 메모리 해제
    TTF_Quit();                                 // TTF 종료
    SDL_Quit();                                 // SDL 종료   
}

const char* getFishTypeName(FishType type)
{
    switch (type)
    {
    case normal_fi:
        return "normal";

    case lazy_fi:
        return "lazy";

    case growth_fi:
        return "growth";

    case baby_fi:
        return "baby";

    case stupid_fi:
        return "stupid";

    case big_fi:
        return "big";

    default:
        return "Unknown";   /// 뭔가 잘못된 값이나 초기화가 안됐을 때 경고나 작동이 안될 수 있어서 넣어두는 값(이라고 ai가)
    }
}

void renderText(const char* text, int x, int y)
{           // 텍스트 렌더링 함수
    SDL_Color color = { 255, 255, 255 };                    // 흰색 텍스트 색상
    SDL_Surface* surface = TTF_RenderText_Solid(font, text, color);         // 텍스트를 표면으로 렌더링
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface); // 표면에서 텍스처 생성
    SDL_Rect dest = { x, y, surface->w, surface->h };       // 텍스처를 화면에 렌더링, 위치와 크기 설정
    SDL_RenderCopy(renderer, texture, NULL, &dest);         // 텍스처 렌더링
    SDL_FreeSurface(surface);                               // 표면 메모리 해제
    SDL_DestroyTexture(texture);                            // 텍스처 메모리 해제
}

void renderFishTanks()
{                        // 어항과 물고기 렌더링 함수
    for (int i = 0; i < NUM; i++)
    {
        int x = 50 + i * (FISHTANK_WIDTH + 10);
        SDL_Rect bowl = { x, 300, FISHTANK_WIDTH, FISHTANK_HEIGHT };
        SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255); // 파란색 테두리
        SDL_RenderDrawRect(renderer, &bowl);

        // 물 높이 표시
        int waterHeight = fishTanks[i].water * FISHTANK_HEIGHT / 100;
        SDL_Rect water = { x, 300 + FISHTANK_HEIGHT - waterHeight, FISHTANK_WIDTH, waterHeight };
        SDL_SetRenderDrawColor(renderer, 0, 128, 255, 255);
        SDL_RenderFillRect(renderer, &water);

        // 물고기 이미지 표시
        {
            SDL_Rect fishRect =
            {
                x + 20, 300 + FISHTANK_HEIGHT - waterHeight - 30, 60, 30
            };

            if (fishTanks[i].isAlive)
            {
                SDL_RenderCopy(renderer, fishTextures[fishTanks[i].type], NULL, &fishRect);
            }
            else   /// 죽은 물고기 이미지 표시
            {
                SDL_RenderCopy(renderer, deadFishTexture, NULL, &fishRect);
            }
        }

        // 커서 표시
        if (i == position)
        {
            SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255); // 노란색
            SDL_RenderDrawRect(renderer, &bowl);
        }

        /// 물고기 종류 표시
        char text[64];
        
        renderText(getFishTypeName(fishTanks[i].type), x + 5, 510);

        sprintf_s(text, sizeof(text), "Water:%d", fishTanks[i].water);

        renderText(text, x + 5, 532);

        sprintf_s(text, sizeof(text), "Growth: %d", fishTanks[i].growthStage);

        renderText(text, x + 5, 551);

        renderText(fishTanks[i].isAlive ? "Alive" : "Dead", x + 5, 569);
    }
}

void updateGame()
{
    long currentTime = SDL_GetTicks();
    long elapsed = (currentTime - lastUpdateTime) / 1000; // 초 단위
    if (elapsed > 0)
    {
        int aliveCount = 0;
        for (int i = 0; i < NUM; i++)
        {
            if (fishTanks[i].isAlive == 1)
            {
                int consumeRate = 1;
                int growthRate = 1;

                switch (fishTanks[i].type)
                {
                case normal_fi:
                    consumeRate = 1;
                    growthRate = 1;
                    break;

                case lazy_fi:
                    consumeRate = (rand() % 2) + 1;
                    growthRate = 1;
                    break;

                case growth_fi:
                    consumeRate = 1;
                    growthRate = 2;
                    break;

                case baby_fi:   /// 아기 물고기는 성장 속도가 다른 물고기보다 느리기 때문에 계산을 이와 같이 설정(하면 된다고 ai가)
                    consumeRate = 1;
                    if (rand() % 2 == 0)
                        growthRate = 0;
                    else
                        growthRate = 1;
                    break;

                case stupid_fi:
                    consumeRate = (rand() % 2) * 2;
                    growthRate = 1;
                    break;

                case big_fi:
                    consumeRate = 2;
                    growthRate = 1;
                    break;
                }


                // 물 증발 및 소비
                fishTanks[i].water -= (consumeRate * level * (fishTanks[i].fish / 20 + 1) * elapsed);
                if (fishTanks[i].water < 0)
                {
                    fishTanks[i].water = 0;
                    fishTanks[i].isAlive = 0;
                }

                // 물고기 성장
                if (fishTanks[i].water > 0)
                {
                    fishTanks[i].fish += growthRate * ((fishTanks[i].water / 100 + 1) * elapsed);
                }
                if (fishTanks[i].fish > 100) fishTanks[i].fish = 100;

                aliveCount++;
            }

            /// 물고기 성장 정도
            if (fishTanks[i].fish >= 30)
                fishTanks[i].growthStage = 1;

            if (fishTanks[i].fish >= 60)
                fishTanks[i].growthStage = 2;

            if (fishTanks[i].fish >= 90)
                fishTanks[i].growthStage = 3;
        }

        if (aliveCount == 0)
        {
            gameOver = true;
            running = false;
        }

        // 레벨 업 조건: 시간 경과
        long totalElapsed = (currentTime - startTime) / 1000;
        if (totalElapsed / 20 > level - 1)
        {
            level++;
            if (level > 5)
            {
                level = 5;
                gameWin = true;
                running = false;
            }
        }

        lastUpdateTime = currentTime;
    }
}

void handleInput(SDL_Event* e)
{
    if (!gameStarted)   /// 게임 메뉴 표시 상태에서 게임 시작 입력 키
    {
        if (e->type == SDL_KEYDOWN)
        {
            if (e->key.keysym.sym == SDLK_RETURN ||
                e->key.keysym.sym == SDLK_SPACE)
            {
                gameStarted = true;
            }
        }
        return;
    }


    if (e->type == SDL_KEYDOWN)
    {
        switch (e->key.keysym.sym)
        {
        case SDLK_j:
            if (position > 0) position--;
            break;
        case SDLK_l:
            if (position < NUM - 1) position++;
            break;
        case SDLK_k:
            if (fishTanks[position].water >= 0 && fishTanks[position].water < 100)  // 물이 0 이상 100 미만일 때만 물을 줄 수 있도록 조건 추가
                fishTanks[position].water += 5;
            playWaterSound(); // 물 주는 소리 재생 
            if (fishTanks[position].water > 100) fishTanks[position].water = 100; // 물이 100을 초과하지 않도록 제한
            break;
        case SDLK_ESCAPE:
            running = false;
            break;
        }
    }
}

SDL_Texture* loadTexture(const char* path) {
    SDL_Surface* surface = SDL_LoadBMP(path);
    if (!surface) {
        printf("Load fail: %s\n", SDL_GetError());
        return NULL;
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}
