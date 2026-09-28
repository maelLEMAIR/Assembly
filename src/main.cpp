#include <iostream>
#include <vector>
#include <conio.h>
#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <Windows.h>

#include "vec4.hpp"

static int failures = 0;
#define CHECK(expr) do { if (!(expr)) { std::printf("ECHEC: %s\n", #expr); ++failures; } } while (0)

#define GREENBG "\x1B[42m"
#define GREEN "\x1B[32m"
#define BLUEBG "\x1B[104m"
#define REDBG "\x1B[41m"
#define RED "\x1B[31m"
#define YELLOWBG "\x1B[43m"
#define YELLOW "\x1B[33m"
#define BLACKBG "\x1B[40m"
#define BLACK "\x1B[30m"
#define DEFAULT "\x1B[0m"
#define DEFAULTBG "\x1B[49m"

static int64_t neighbours[4][2] = { {-1, 0}, {1, 0}, {0, -1}, {0, 1} };



struct Entity
{
    vec2 pos = vec2(0.0f, 0.0f);             
    vec2 vel = vec2(0.0f, 0.0f);       
    int64_t hpMax, hp;
    int64_t attack;
};

struct Door;

struct Grid
{
    std::vector<std::vector<char>> grid;
    std::vector<Door*> doors;   
    std::vector<Entity*> entities;
};

struct Door
{
    vec2 pos = vec2(0.0f, 0.0f);             
    int32_t spawnX, spawnY;
    Door* next;            
    Grid* grid;           
};

extern "C" {
    int64_t asm_add(int64_t a, int64_t b);
    int64_t asm_maxInAnArray(int64_t* a, int64_t b);
    int64_t* asm_sortAnArray(int64_t* a, int64_t b);
    int64_t asm_CompCharsArrays(const char* a, int64_t b, const char* c, int64_t d);
    int64_t asm_checkCell(std::vector<std::vector<char>>* grid, int64_t* pos, char c, int64_t stride);
    float_t asm_sumFloat(float_t a, float_t b);
    int64_t asm_getEntity(Entity* entity, int64_t* pos);
    int64_t asm_isDead(Entity* entity);
    int64_t asm_checkDoor(Door** doors, int64_t count, Entity* pPlayer, int64_t stride);
    void asm_move(Entity* a);
    void asm_movef(Entity* a);
    void asm_attack(Entity* player, Entity* monster);
    void asm_heal(Entity* player, int64_t numberOfHeal);
}

enum MOVEMENT
{
    LEFT    = 'q',
    RIGHT   = 'd',
    UP      = 'z',
    DOWN    = 's',
    ENTER   = 'a',
    EXIT    = 27
};

Grid* currentGrid;

void PrintInfo(Entity* pPlayer)
{
    std::cout << DEFAULTBG << "[";
    for (int i = 0; i < pPlayer->hpMax; i++)
    {
        if (pPlayer->hp >= i)
            std::cout << GREENBG << "  ";
        else
            std::cout << DEFAULTBG << "  ";
    }
    std::cout << DEFAULTBG << "]   " << pPlayer->hp << " / " << pPlayer->hpMax << "\n\n";
}

void PrintGrid(Entity* pPlayer)
{
    for (int i = 0; i < (int64_t)currentGrid->grid.size(); i++)
    {
        for (int j = 0; j < (int64_t)currentGrid->grid[i].size(); j++)
        {
            if (pPlayer->pos.x() == j && pPlayer->pos.y() == i) std::cout << " " << GREEN << "X" << DEFAULT << " ";
            else if (currentGrid->grid[i][j] == '-') std::cout << BLUEBG << "   " << DEFAULTBG;
            else if (currentGrid->grid[i][j] == 'M') std::cout << " " << RED << "M" << DEFAULT << " ";
            else if (currentGrid->grid[i][j] == 'D') std::cout << YELLOWBG << "   " << DEFAULTBG;
            else if (currentGrid->grid[i][j] == '+') std::cout << GREEN << " + " << DEFAULT;
            else std::cout << "   " << DEFAULTBG;
        }
        std::cout << '\n';
    }
}

void AttackNeighbours(Entity* pPlayer)
{
    bool attackEnemy = false;

    for (int i = 0; i < 4; i++)
    {
        int64_t pos[] = { (int64_t)pPlayer->pos.x(), (int64_t)pPlayer->pos.y() };
        pos[0] += neighbours[i][0];
        pos[1] += neighbours[i][1];
        for (Entity* ent : currentGrid->entities)
        {
            if (ent == pPlayer)
                continue;

            if (asm_getEntity(ent, pos))
            {
                asm_attack(pPlayer, ent);
                attackEnemy = true;
            }
        }
    }
    if (attackEnemy)
        std::cout << RED << "ATTACK AN ENEMY !" << DEFAULT << '\n';
    else
        std::cout << YELLOW << "NO ENEMY FOUND !" << DEFAULT << '\n';

    Sleep(250);
}

void CheckHealthPoint()
{
    for (auto it = currentGrid->entities.begin(); it != currentGrid->entities.end(); )
    {
        Entity* ent = *it;
        if (asm_isDead(ent) == 1)
        {
            currentGrid->grid[(int64_t)ent->pos.y()][(int64_t)ent->pos.x()] = ' ';
            it = currentGrid->entities.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

void CheckDoor(Entity* pPlayer)
{
    int64_t indexDoor = asm_checkDoor(
        currentGrid->doors.data(),
        (int64_t)currentGrid->doors.size(),
        pPlayer,
        sizeof(Door*));

    if (indexDoor != -1)
    {
        Door* door = currentGrid->doors[indexDoor];
        if (door->next != nullptr)
        {
            currentGrid = door->next->grid;
            pPlayer->pos = door->next->pos;
        }
    }
}

void CheckHealBoost(Entity* pPlayer)
{
    int64_t pos[] = { pPlayer->x, pPlayer->y };
    if ( asm_checkCell(&currentGrid->grid, pos, '+', sizeof(std::vector<char>)) )
    {
        asm_heal(pPlayer, 3);
        currentGrid->grid[pos[1]][pos[0]] = ' ';
    }
}

bool HandleInput(Entity* pPlayer)
{
    pPlayer->vel = vec2(0, 0);
    std::cout << YELLOW << "ZQSD " << DEFAULT << " TO MOV & " << YELLOW << "A " << DEFAULT << "TO ATTACK" << '\n';
    char ch = _getch();
    int64_t pos[] = { (int64_t)pPlayer->pos.x(), (int64_t)pPlayer->pos.y() };
    switch (ch)
    {
    case LEFT:
        pos[0] = pPlayer->pos.x() - 1;
        if (pPlayer->pos.x() > 0 && asm_checkCell(&currentGrid->grid, pos, '-', sizeof(std::vector<char>)) == 0 &&
            asm_checkCell(&currentGrid->grid, pos, 'M', sizeof(std::vector<char>)) == 0) pPlayer->vel = vec2(-1.0f, 0.0f);
        
        break;
    case RIGHT:
        pos[0] = pPlayer->pos.x() + 1;
        if (pPlayer->pos.x() < (int64_t)currentGrid->grid[(int64_t)pPlayer->pos.y()].size() - 1 && asm_checkCell(&currentGrid->grid, pos, '-', sizeof(std::vector<char>)) == 0 &&
            asm_checkCell(&currentGrid->grid, pos, 'M', sizeof(std::vector<char>)) == 0) pPlayer->vel = vec2(1, 0);
        break;
    case UP:
        pos[1] = pPlayer->pos.y() - 1;
        if (pPlayer->pos.y() > 0 && asm_checkCell(&currentGrid->grid, pos, '-', sizeof(std::vector<char>)) == 0 &&
            asm_checkCell(&currentGrid->grid, pos, 'M', sizeof(std::vector<char>)) == 0) pPlayer->vel = vec2(0, -1);
        break;
    case DOWN:
        pos[1] = pPlayer->pos.y() + 1;
        if (pPlayer->pos.y() < (int64_t)currentGrid->grid.size() - 1 && asm_checkCell(&currentGrid->grid, pos, '-', sizeof(std::vector<char>)) == 0 &&
            asm_checkCell(&currentGrid->grid, pos, 'M', sizeof(std::vector<char>)) == 0) pPlayer->vel = vec2(0, 1);
        break;
    case ENTER:
        AttackNeighbours(pPlayer);
        break;
    case EXIT:
        return false;
    default:
        return true;
    }
    asm_move(pPlayer);
    CheckHealBoost(pPlayer);
    CheckDoor(pPlayer);
    PrintGrid(pPlayer);
    return true;
}

Grid* CreateGrid(std::vector<std::vector<char>> layout, Entity* pPlayer)
{
    Grid* g = new Grid();
    g->grid = layout;

    for (int i = 0; i < (int64_t)g->grid.size(); i++)
    {
        int32_t width = (int32_t)g->grid[i].size();
        for (int j = 0; j < width; j++)
        {
            char c = g->grid[i][j];
            if (c == 'D')
            {
                int32_t spawnX = (j == 0) ? 1 : (j == width - 1 ? width - 2 : j);
                Door* door = new Door{ vec2(j, i), spawnX, i, nullptr, g };
                g->doors.push_back(door);
            }
            else if (c == 'M')
            {
                Entity* monster = new Entity{ vec2(j, i), vec2(0, 0),5, 5, 1 };
                g->entities.push_back(monster);
            }
            else if (c == 'X' && pPlayer != nullptr)
            {
                pPlayer->pos = vec2(j, i);
                g->grid[i][j] = ' ';
            }
        }
    }
    return g;
}

int main()
{
    bool isOpen = true;

    Entity* vivienSavage = new Entity{ vec2(0, 0), vec2(0, 0), 10, 8, 2 };

    std::vector<std::vector<char>> layout1 = {
        {'-', '-', '-', '-', '-', '-', '-', '-', '-', '-'},
        {'-', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'M', '-'},
        {'-', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '-'},
        {'-', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '-'},
        {'D', 'X', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'D'},
        {'-', ' ', ' ', ' ', '-', ' ', ' ', ' ', ' ', '-'},
        {'-', ' ', ' ', ' ', '-', ' ', ' ', ' ', ' ', '-'},
        {'-', ' ', ' ', ' ', '-', '-', '+', ' ', ' ', '-'},
        {'-', ' ', ' ', '-', '-', '-', '-', ' ', '-', '-'},
        {'-', '-', '-', '-', '-', '-', '-', '-', '-', '-'}
    };

    std::vector<std::vector<char>> layout2 = {
        {'-', '-', '-', '-', '-', '-', '-', '-', '-', '-'},
        {'-', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '+', '-'},
        {'-', ' ', ' ', 'M', ' ', ' ', ' ', ' ', ' ', '-'},
        {'-', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '-'},
        {'D', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'D'},
        {'-', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '-', '-'},
        {'-', ' ', '-', ' ', ' ', ' ', ' ', ' ', '-', '-'},
        {'-', ' ', '-', ' ', ' ', ' ', '-', '-', '-', '-'},
        {'-', ' ', '-', ' ', 'M', '-', '-', '-', '-', '-'},
        {'-', '-', '-', '-', '-', '-', '-', '-', '-', '-'}
    };

    std::vector<std::vector<char>> layout3 = {
        {'-', '-', '-', '-', '-', '-', '-', '-', '-', '-'},
        {'-', '-', ' ', ' ', ' ', ' ', ' ', ' ', '+', '-'},
        {'-', '-', 'M', ' ', ' ', ' ', 'M', ' ', ' ', '-'},
        {'-', '-', '-', '-', '-', '-', ' ', ' ', ' ', '-'},
        {'D', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'D'},
        {'-', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '-'},
        {'-', ' ', ' ', ' ', ' ', '-', ' ', ' ', ' ', '-'},
        {'-', ' ', '+', '-', '-', '-', '-', ' ', ' ', '-'},
        {'-', '-', '-', '-', '-', '-', '-', ' ', ' ', '-'},
        {'-', '-', '-', '-', '-', '-', '-', '-', '-', '-'}
    };

    Grid* grid_1 = CreateGrid(layout1, vivienSavage);
    Grid* grid_2 = CreateGrid(layout2, nullptr);
    Grid* grid_3 = CreateGrid(layout3, nullptr);

    grid_1->doors[1]->next = grid_2->doors[0];
    grid_2->doors[0]->next = grid_1->doors[1];

    grid_2->doors[1]->next = grid_3->doors[0];
    grid_3->doors[0]->next = grid_2->doors[1];

    grid_3->doors[1]->next = grid_1->doors[0];
    grid_1->doors[0]->next = grid_3->doors[1];

    currentGrid = grid_1;

    while (isOpen)
    {
        CheckHealthPoint();
        PrintInfo(vivienSavage);
        PrintGrid(vivienSavage);
        isOpen = HandleInput(vivienSavage);
        system("cls");
    }
    return failures;
}