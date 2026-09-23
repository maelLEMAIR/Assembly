#include <iostream>
#include <vector>
#include <conio.h>

static int failures = 0;
#define CHECK(expr) do { if (!(expr)) { std::printf("ECHEC: %s\n", #expr); ++failures; } } while (0)

#define GREENBG "\x1B[42m"
#define BLUEBG "\x1B[104m"
#define REDBG "\x1B[41m"
#define YELLOWBG "\x1B[43m"
#define BLACKBG "\x1B[40m"
#define BLACK "\x1B[30m"
#define DEFAULT "\x1B[39m"
#define DEFAULTBG "\x1B[49m"

struct Entity
{
    int64_t x, y, vx, vy;
};

extern "C" {
    int64_t asm_add(int64_t a, int64_t b);
    int64_t asm_maxInAnArray(int64_t* a, int64_t b);
    int64_t* asm_sortAnArray(int64_t* a, int64_t b);
    int64_t asm_CompCharsArrays(const char* a, int64_t b, const char* c, int64_t d);

    float_t asm_sumFloat(float_t a, float_t b);
    void asm_move(Entity* a);
} 

std::vector<std::vector<char>> GRID =
{
    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},  
    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},  
    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},  
    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},  
    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},  
    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},  
    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},  
    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},  
    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},  
    {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '}
};

enum MOVEMENT
{
    LEFT = 'q',
    RIGHT = 'd',
    UP = 'z',
    DOWN = 's',
    ENTER = 13,
    EXIT = 27
};

void PrintGrid(Entity* pPlayer)
{
    for (int i = 0; i < GRID.size(); i++)
    {
        for (int j = 0; j < GRID[i].size(); j++)
        {
            std::cout << "[";
            if (pPlayer->x == j && pPlayer->y == i) std::cout << GREENBG << "X" << DEFAULTBG;
            else std::cout << " " << DEFAULTBG;
            std::cout << "]";
        }
        std::cout << '\n';
    }
    std::cout << std::flush;   // <-- ajoute ça
}

bool HandleInput(Entity* pPlayer)
{
    pPlayer->vx = 0;
    pPlayer->vy = 0;
    std::cout << "Enter Movement: " << std::flush;
    char ch = _getch();
    switch (ch)
    {
    case LEFT:
        if (pPlayer->x > 0) pPlayer->vx = -1;
        break;
    case RIGHT:
        if (pPlayer->x < GRID[pPlayer->y].size() - 1) pPlayer->vx = 1;
        break;
    case UP:
        if (pPlayer->y > 0) pPlayer->vy = -1;
        break;
    case DOWN:
        if (pPlayer->y < GRID.size() - 1) pPlayer->vy = 1;
        break;
    case EXIT:
        return false;
    default:
        return true;
    }
    asm_move(pPlayer);
    PrintGrid(pPlayer);
    return true;
}

int main() 
{
    std::ios::sync_with_stdio(false);
    setvbuf(stdout, nullptr, _IONBF, 0);
    
    bool isOpen = true;
    
    Entity* vivienSauvage = new Entity{0, 0, 0, 0};

    while (isOpen)
    {
        PrintGrid(vivienSauvage);
        isOpen = HandleInput(vivienSauvage);
        system("cls");
    }
    return failures;
}
