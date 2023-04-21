#include <vector>
#include <thread>
#include <array>
#include "AI.h"
#include "constants.h"
#include<math.h>
#define PI 3.1415926532123915

// 为假则play()期间确保游戏状态不更新，为真则只保证游戏状态在调用相关方法时不更新
extern const bool asynchronous = false;

// 选手需要依次将player0到player4的职业在这里定义

extern const std::array<THUAI6::StudentType, 4> studentType = {
    THUAI6::StudentType::Athlete,
    THUAI6::StudentType::Teacher,
    THUAI6::StudentType::StraightAStudent,
    THUAI6::StudentType::Sunshine};

extern const THUAI6::TrickerType trickerType = THUAI6::TrickerType::Assassin;

//可以在AI.cpp内部声明变量与函数

void AI::play(IStudentAPI& api)
{
    // 公共操作
    if (this->playerID == 0)
    {
        // 玩家0执行操作
    }
    else if (this->playerID == 1)
    {
        // 玩家1执行操作
    }
    else if (this->playerID == 2)
    {
        // 玩家2执行操作
    }
    else if (this->playerID == 3)
    {
        // 玩家3执行操作
    }
    //当然可以写成if (this->playerID == 2||this->playerID == 3)之类的操作
    // 公共操作
}


//初始化map(原图)
static std::vector<std::vector<THUAI6::PlaceType>> map;
bool initialized=0;
void initialmap(ITrickerAPI& api) {
    if (initialized == 0) {
        map = api.GetFullMap();
        initialized = 1;
    }
    else {
        ;
    }
}
//初始化map1（路径图）
int map1[50][50] = {  };
void refresh() {
    int i, j;
    for (i = 0; i < 50; i++)
        for (j = 0; j < 50; j++)
            map1[i][j] = 0;
}
struct mapnode {
    int x, y;
};
//栈储存路径
std::vector<mapnode> path;
int timestep =1000*1000/ Constants::Assassin::moveSpeed;
//方块通过判断
bool Wallma(THUAI6::PlaceType block) {
    if (block == THUAI6::PlaceType::Land || block == THUAI6::PlaceType::Grass || block == THUAI6::PlaceType::Gate||block== THUAI6::PlaceType::Window)
        return true;
    else
        return false;
}
//小人走路:捣蛋鬼
//走格子，自动翻窗
void walkAB(ITrickerAPI& api, mapnode node1, mapnode node2) {
    int x0, y0, x1, y1,time;
    x0 = api.CellToGrid(node1.x);
    y0 = api.CellToGrid(node1.y);
    x1 = api.CellToGrid(node2.x);
    y1 = api.CellToGrid(node2.y);
    if (api.GetPlaceType(x1, y1) == THUAI6::PlaceType::Window) {
        api.SkipWindow();
        std::this_thread::sleep_for(std::chrono::milliseconds(800));
        api.Wait();
    }
    else {
        api.Move(timestep, atan2(y1 - y0, x1 - x0));
        std::this_thread::sleep_for(std::chrono::milliseconds(252));
        api.Wait();
    }
    
}
//位置补偿
void walkcomp(ITrickerAPI& api, mapnode node1){
    int x=api.GetSelfInfo()->x, y= api.GetSelfInfo()->y,x_=api.CellToGrid(node1.x),y_=api.CellToGrid(node1.y);
    if (sqrt((x - x_) * (x - x_) + (y - y_) * (y - y_)) >= 396) {
        api.Move(sqrt((x - x_) * (x - x_) + (y - y_) * (y - y_))*1000/3960, atan2((y_ - y), (x_ - x)));
        int time = sqrt((x - x_) * (x - x_) + (y - y_) * (y - y_)) * 1000 / 3960;
        std::this_thread::sleep_for(std::chrono::milliseconds(time));
    }
 }
//沿路走
void walkpath(ITrickerAPI& api, std::vector<mapnode> &path1) {
    int i = 0,j;
    j = path1.size();
    mapnode nows = {25,25};
    while (i<j-1) {
        walkAB(api, nows, path1[i]);
        if (api.GetPlaceType(path[i].x, path[i].y) == THUAI6::PlaceType::Window) {
            nows = path1[i+1];
            i++;
        }
        else{ nows = path1[i]; }
        walkcomp(api, nows);
        i++;
    }

}
//BFS寻路，最后一个节点是目标点

void BFS(int x0,int y0,int x1,int y1) {

    int xstep,ystep,xt=x0,yt=y0;
    mapnode temp = {};
    if (x0 >= x1)
        xstep = -1;
    else
        xstep = 1;
    if (y0 >= y1)
        ystep = -1;
    else
        ystep = 1;

    while (xt != x1 || yt != y1) {
        if (map1[xt + xstep][yt] == 0 && Wallma(map[xt + xstep][yt]))
        {
            temp.x = (xt + xstep);
            temp.y = (yt);
            path.emplace_back(temp);
            xt += xstep;
            map1[xt][yt] = 1;

        }
        else if (map1[xt][yt + ystep] == 0 && Wallma(map[xt][yt + ystep] ))
        {
            temp.x = (xt);
            temp.y = (yt + ystep);
            path.emplace_back(temp);
            yt += ystep;
            map1[xt][yt] = 1;

        }
        else if (map1[xt - xstep][yt] == 0 && Wallma(map[xt - xstep][yt])) {
            temp.x = (xt - xstep);
            temp.y = (yt);
            path.emplace_back(temp);
            xt -= xstep;
            map1[xt][yt] = 1;

        }
        else if (map1[xt][yt - ystep] == 0 && Wallma(map[xt][yt - ystep])) {
            temp.x = (xt);
            temp.y = (yt - ystep);
            path.emplace_back(temp);
            yt -= ystep;
            map1[xt][yt] = 1;

        }
        else {
            if (!path.empty())
            path.pop_back();
            if(!path.empty())
            temp = path.back();
            xt = temp.x;
            yt = temp.y;
        }

    }
}

//路径凸优化

void AI::play(ITrickerAPI& api)
{
    auto self = api.GetSelfInfo();
    api.PrintSelfInfo();
    api.GetFullMap();
    refresh();
    initialmap(api);
    //int x = api.GetSelfInfo()->x/1000, y = api.GetSelfInfo()->y/1000;
    BFS(21, 25, 18,5 );
    std::this_thread::sleep_for(std::chrono::seconds(10));
    walkpath(api, path);
    std::this_thread::sleep_for(std::chrono::seconds(252));
}
