#include <iostream>
#include <cassert>
#include <random>
#include <algorithm>
#include <Windows.h>
#include <queue>

// 상수들
enum EConstant
{
    WIDTH = 64,
    HEIGHT = 32,

    MAX_COIN_COUNT = 10,
    MAX_OBSTACLES_COUNT = 256,
    DIRECTIONS = 4
};

// 출력 심볼
enum ESymbol
{
    AI = 'A',
    OBSTACLE = 'X'
};

// 좌표 지정용 구조체
class Point
{
public:
    Point()
        : X(0)
        , Y(0)
    {
    }

    Point(const int x, const int y)
        : X(x)
        , Y(y)
    {
    }

    inline Point operator+(const Point rhs) const
    {
        return { X + rhs.X, Y + rhs.Y };
    }

    inline bool operator==(const Point& rhs) const
    {
        return X == rhs.X && Y == rhs.Y;
    }

public:
    int X;
    int Y;
};

struct Node
{
    Point pos;
    int weight;

    bool operator()(const Node& n0, const Node& n1) const
    {
        return n0.weight > n1.weight;
    }
};

// 미리 생성해둔 장애물 좌표들
static const Point OBSTACLE_POINTS[] = {
    { 10, 0 }, { 16, 0 }, { 23, 0 }, { 26, 0 }, { 32, 0 }, { 46, 0 }, { 48, 0 }, { 51, 0 }, { 53, 0 }, { 57, 0 },
    { 58, 0 }, { 8, 1 }, { 23, 1 }, { 28, 1 }, { 30, 1 }, { 34, 1 }, { 45, 1 }, { 0, 2 }, { 3, 2 }, { 8, 2 },
    { 23, 2 }, { 34, 2 }, { 39, 2 }, { 42, 2 }, { 48, 2 }, { 54, 2 }, { 55, 2 }, { 63, 2 }, { 0, 3 }, { 9, 3 },
    { 17, 3 }, { 36, 3 }, { 39, 3 }, { 43, 3 }, { 51, 3 }, { 52, 3 }, { 12, 4 }, { 13, 4 }, { 16, 4 }, { 30, 4 },
    { 32, 4 }, { 45, 4 }, { 4, 5 }, { 14, 5 }, { 40, 5 }, { 43, 5 }, { 51, 5 }, { 53, 5 }, { 58, 5 }, { 5, 6 },
    { 8, 6 }, { 18, 6 }, { 20, 6 }, { 21, 6 }, { 29, 6 }, { 40, 6 }, { 14, 7 }, { 18, 7 }, { 20, 7 }, { 37, 7 },
    { 54, 7 }, { 11, 8 }, { 31, 8 }, { 35, 8 }, { 38, 8 }, { 58, 8 }, { 0, 9 }, { 6, 9 }, { 19, 9 }, { 22, 9 },
    { 25, 9 }, { 27, 9 }, { 30, 9 }, { 33, 9 }, { 34, 9 }, { 37, 9 }, { 49, 9 }, { 52, 9 }, { 56, 9 }, { 62, 9 },
    { 0, 10 }, { 20, 10 }, { 21, 10 }, { 40, 10 }, { 9, 11 }, { 17, 11 }, { 19, 11 }, { 23, 11 }, { 24, 11 }, { 26, 11 },
    { 30, 11 }, { 48, 11 }, { 2, 12 }, { 10, 12 }, { 11, 12 }, { 27, 12 }, { 30, 12 }, { 44, 12 }, { 54, 12 }, { 61, 12 },
    { 27, 13 }, { 49, 13 }, { 50, 13 }, { 54, 13 }, { 62, 13 }, { 17, 14 }, { 20, 14 }, { 50, 14 }, { 53, 14 }, { 55, 14 },
    { 58, 14 }, { 6, 15 }, { 15, 15 }, { 23, 15 }, { 25, 15 }, { 45, 15 }, { 50, 15 }, { 51, 15 }, { 52, 15 }, { 54, 15 },
    { 0, 16 }, { 22, 16 }, { 26, 16 }, { 30, 16 }, { 37, 16 }, { 41, 16 }, { 2, 17 }, { 12, 17 }, { 13, 17 }, { 21, 17 },
    { 31, 17 }, { 33, 17 }, { 45, 17 }, { 47, 17 }, { 49, 17 }, { 3, 18 }, { 19, 18 }, { 20, 18 }, { 41, 18 }, { 46, 18 },
    { 54, 18 }, { 4, 19 }, { 17, 19 }, { 21, 19 }, { 27, 19 }, { 44, 19 }, { 54, 19 }, { 62, 19 }, { 11, 20 }, { 15, 20 },
    { 25, 20 }, { 27, 20 }, { 28, 20 }, { 35, 20 }, { 41, 20 }, { 51, 20 }, { 53, 20 }, { 55, 20 }, { 60, 20 }, { 8, 21 },
    { 19, 21 }, { 20, 21 }, { 29, 21 }, { 54, 21 }, { 3, 22 }, { 12, 22 }, { 24, 22 }, { 25, 22 }, { 34, 22 }, { 35, 22 },
    { 47, 22 }, { 51, 22 }, { 63, 22 }, { 16, 23 }, { 18, 23 }, { 22, 23 }, { 28, 23 }, { 41, 23 }, { 46, 23 }, { 47, 23 },
    { 55, 23 }, { 1, 24 }, { 6, 24 }, { 11, 24 }, { 17, 24 }, { 19, 24 }, { 23, 24 }, { 28, 24 }, { 36, 24 }, { 37, 24 },
    { 43, 24 }, { 44, 24 }, { 47, 24 }, { 53, 24 }, { 56, 24 }, { 57, 24 }, { 59, 24 }, { 5, 25 }, { 7, 25 }, { 9, 25 },
    { 12, 25 }, { 19, 25 }, { 24, 25 }, { 47, 25 }, { 49, 25 }, { 53, 25 }, { 56, 25 }, { 0, 26 }, { 7, 26 }, { 12, 26 },
    { 28, 26 }, { 29, 26 }, { 30, 26 }, { 39, 26 }, { 61, 26 }, { 2, 27 }, { 11, 27 }, { 15, 27 }, { 39, 27 }, { 41, 27 },
    { 46, 27 }, { 62, 27 }, { 2, 28 }, { 10, 28 }, { 15, 28 }, { 24, 28 }, { 39, 28 }, { 46, 28 }, { 48, 28 }, { 55, 28 },
    { 57, 28 }, { 2, 29 }, { 3, 29 }, { 11, 29 }, { 12, 29 }, { 13, 29 }, { 16, 29 }, { 17, 29 }, { 34, 29 }, { 35, 29 },
    { 52, 29 }, { 59, 29 }, { 61, 29 }, { 4, 30 }, { 14, 30 }, { 15, 30 }, { 44, 30 }, { 50, 30 }, { 58, 30 }, { 59, 30 },
    { 22, 31 }, { 26, 31 }, { 37, 31 }, { 39, 31 }, { 52, 31 }, { 56, 31 },
};

static const Point DIRS[DIRECTIONS] = {
    { -1, 0 }, { 1, 0 }, { 0, -1 }, { 0, 1 }
};

static char sScreenBuffer[HEIGHT][WIDTH + 1];

// 더블 버퍼링용
static HANDLE shPrimary = GetStdHandle(STD_OUTPUT_HANDLE);
static HANDLE shSecondary = CreateConsoleScreenBuffer(
    GENERIC_READ | GENERIC_WRITE,
    FILE_SHARE_READ | FILE_SHARE_WRITE,
    nullptr,
    CONSOLE_TEXTMODE_BUFFER,
    nullptr
);

static HANDLE shFrontBuffer = shPrimary;
static HANDLE shBackBuffer = shSecondary;

int main()
{
    COORD size;
    size.X = WIDTH + 1;
    size.Y = HEIGHT;

    SetConsoleScreenBufferSize(shPrimary, size);
    SetConsoleScreenBufferSize(shSecondary, size);

    std::random_device rd;
    std::mt19937_64 gen(rd());

    std::uniform_int_distribution<int> xRandom(0, WIDTH - 1);
    std::uniform_int_distribution<int> yRandom(0, HEIGHT - 1);

    // 동전 좌표 및 획득 확인용
    Node coins[MAX_COIN_COUNT];
    bool bAlreadyPicked[MAX_COIN_COUNT];

    bool bNewStage = true;

    // AI의 위치 및 남은 동전 수
    Point aiPos = { 0, 0 };
    int coinCount = -1;

    // 실제 거리 버퍼
    int distanceMatrix[HEIGHT][WIDTH];

    std::priority_queue<Node, std::vector<Node>, Node> pq;

    while (true)
    {
        // 맵 초기화
        for (int y = 0; y < HEIGHT; ++y)
        {
            int x;
            for (x = 0; x < WIDTH; ++x)
            {
                sScreenBuffer[y][x] = ' ';
            }
            sScreenBuffer[y][x] = '\n';
        }

        // 장애물 그리기
        for (const Point p : OBSTACLE_POINTS)
        {
            sScreenBuffer[p.Y][p.X] = ESymbol::OBSTACLE;
        }

        if (bNewStage)
        {
            memset(reinterpret_cast<int*>(distanceMatrix), 0, sizeof(int) * WIDTH * HEIGHT);

            coinCount = MAX_COIN_COUNT;

            for (int i = 0; i < MAX_COIN_COUNT; ++i)
            {
                bAlreadyPicked[i] = false;

                while (true)
                {
                    const Point p = { xRandom(gen), yRandom(gen) };

                    const char levelItem = sScreenBuffer[p.Y][p.X];
                    if (levelItem == ' ')
                    {
                        coins[i] = { p, i };

                        break;
                    }
                }
            }

            bNewStage = false;
        }
        else
        {
            for (int i = 0; i < MAX_COIN_COUNT; ++i)
            {
                if (bAlreadyPicked[i])
                {
                    continue;
                }

                const Node& coin = coins[i];
                for (const Point next : DIRS)
                {
                    const Point nextPos = aiPos + next;
                    if (nextPos.X < 0 || nextPos.X >= WIDTH
                        || nextPos.Y < 0 || nextPos.Y >= HEIGHT
                        || sScreenBuffer[nextPos.Y][nextPos.X] == 'X')
                    {
                        continue;
                    }

                    const int estimatedDist = std::abs(nextPos.X - coin.pos.X) + std::abs(nextPos.Y - coin.pos.Y);

                    pq.push({ nextPos, distanceMatrix[nextPos.Y][nextPos.X] + estimatedDist });
                }
            }

            const Node next = pq.top();
            ++distanceMatrix[next.pos.Y][next.pos.X];

            while (!pq.empty())
            {
                pq.pop();
            }

            aiPos = next.pos;

            for (const Node& coin : coins)
            {
                if (bAlreadyPicked[coin.weight])
                {
                    continue;
                }

                if (next.pos == coin.pos)
                {
                    memset(reinterpret_cast<int*>(distanceMatrix), 0, sizeof(int) * WIDTH * HEIGHT);

                    bAlreadyPicked[coin.weight] = true;
                    --coinCount;

                    break;
                }
            }

            bNewStage = coinCount == 0;
        }

        // 동전 그리기
        for (const Node coin : coins)
        {
            if (!bAlreadyPicked[coin.weight])
            {
                sScreenBuffer[coin.pos.Y][coin.pos.X] = static_cast<char>(coin.weight + '0');
            }
        }

        sScreenBuffer[aiPos.Y][aiPos.X] = ESymbol::AI;

        SetConsoleCursorPosition(shBackBuffer, { 0, 0 });
        WriteConsoleA(shBackBuffer, sScreenBuffer, sizeof(sScreenBuffer), nullptr, nullptr);

        std::swap(shFrontBuffer, shBackBuffer);

        SetConsoleActiveScreenBuffer(shFrontBuffer);

        Sleep(67); // 약 15fps
    }
}
