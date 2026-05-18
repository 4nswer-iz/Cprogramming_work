
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <windows.h>
#include <conio.h>

#define NUM 6
#define MAX_WATER 100
#define WATER_AMOUNT 3

typedef struct
{
    int water;
    int alive;
    int fishSize; //물고기 크기, 레벨이 올라갈수록 커짐
}
FishTank;

int level;
FishTank tanks[NUM];
FishTank * cursor;
long prevElapsedTime;
long totalElapsedTime;
long startTime = 0;
int position = 0;

void initData();
void decreaseWater(long elapsedTime);
void printfFishesStatus();
int checkFishAlive();
void update();
void render();
int checkWin();
void gotoxy(int x, int y);
int nonBlockingGetch();

void processInput();
void moveCursorLeft();
void moveCursorRight();
void giveWater();
void growFish(FishTank * tank);

int main(void)
{
    int gameOver = 0;
    initData();
    startTime = clock();
    cursor = tanks;
    while (!gameOver)
    {
        position = cursor - tanks;
        processInput();
        update();
        render();
        if (totalElapsedTime / 20 > level - 1)
        {
            level++;
            gotoxy(10, 3);
            printf("축하합니다. 레벨업이 되었습니다.");
        }
        if (checkWin())
        {
            gameOver = 1;
        }
        printf("\n");
        if (checkFishAlive() == 0)
        {
            gotoxy(2, 3);
            printf("모든 물고기가 죽었습니다. 게임을 종료합니다.");
            gameOver = 1;
        }
        prevElapsedTime = totalElapsedTime;
        Sleep(100);
    }

    getchar();
    system("cls");
    return 0;
}

void initData()
{
    level = 1;
    prevElapsedTime = 0;
    totalElapsedTime = 0;
    for (int i = 0; i < NUM; i++)
    {
        tanks[i].water = 100;
        tanks[i].alive = 1;
        tanks[i].fishSize = 1;
    }
}

void processInput()
{
    int input;
    input = nonBlockingGetch();
    if (input == -1)
    {
        return;
    }
    switch (input)
    {
    case 'j':
        moveCursorLeft();
        break;
    case 'l':
        moveCursorRight();
        break;
    case 'k':
        giveWater();
        break;
    default:
        gotoxy(2, 12);
        printf("잘못된 입력입니다.");
        break;
    }
}

void moveCursorLeft()
{
    if (cursor > tanks)
    {
        cursor--;
    }
}

void moveCursorRight()
{
    if (cursor < tanks + NUM - 1)
    {
        cursor++;
    }
}

void giveWater()
{
    if (cursor->alive == 0)
    {
        return;
    }

    cursor->water += WATER_AMOUNT;
    if (cursor->water > MAX_WATER)
    {
        cursor->water = MAX_WATER;
    }
    growFish(cursor);
}

void growFish(FishTank * tank)
{
    if (tank->fishSize < 10)
    {
        tank->fishSize++;
    }
}

void update()
{
    totalElapsedTime = (clock() - startTime) / CLOCKS_PER_SEC;
    prevElapsedTime = totalElapsedTime - prevElapsedTime;
    decreaseWater(prevElapsedTime);
}

void render()
{
    system("cls");

    gotoxy(2, 2);
    printf("@물고기 키우기 프로젝트@");

    gotoxy(2, 3);
    printf("Level:%2d", level);

    gotoxy(2, 4);
    printf("총 경과 시간: %ld초", totalElapsedTime);

    gotoxy(2, 5);
    printf("최근 경과 시간: %ld초", prevElapsedTime);

    gotoxy(2, 6);
    for (int i = 0; i < NUM; i++)
    {
        printf(" %3d", i + 1);
    }

    gotoxy(2, 7);
    for (int i = 0; i < NUM; i++)
    {
        printf(" %3d", tanks[i].water);
    }

    gotoxy(2, 8); //물고기의 크기 표현
    for (int i = 0; i < NUM; i++)
    {
        printf(" %3d", tanks[i].fishSize);
    }

    gotoxy(2, 9);
    printf("   ");
    for (int i = 0; i < position; i++)
    {
        printf("    ");
    }
    printf("*");

    gotoxy(2, 11);
    printf("왼쪽(j)  물주기(k)  오른쪽(l)");
}

void decreaseWater(long elapsedTime)
{
    for (int i = 0; i < NUM; i++)
    {
        tanks[i].water -= (level * tanks[i].fishSize * (int)elapsedTime); //물고기가 성장한만큼 물 소비량이 증가
        if (tanks[i].water <= 0)
        {
            tanks[i].water = 0;
            tanks[i].alive = 0;
        }
    }
}

int checkWin()
{
    if (level == 5)
    {
        gotoxy(2, 3);
        printf("최고 레벨을 달성했습니다. 게임을 종료합니다.");
        return 1;
    }
    return 0;
}

int checkFishAlive()
{
    for (int i = 0; i < NUM; i++)
    {
        if (tanks[i].alive > 0)
        {
            return 1;
        }
    }
    return 0;
}

void gotoxy(int x, int y)
{
    COORD CursorPosition = { x, y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), CursorPosition);
}

int nonBlockingGetch()
{
    if (_kbhit())
    {
        return _getch();
    }
    return -1;
}

// 2658020 임은찬 11주차 과제 구조체와 구조체 포인터를 활용한 물고기 기르기 게임 시나리오 확장
