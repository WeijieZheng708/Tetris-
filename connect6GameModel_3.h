#ifndef GAMEMODEL_H
#define GAMEMODEL_H
#include <vector>
enum GameType
{
    PERSON,
    Robet
};

// 游戏状态
enum GameStatus
{
    PLAYING,
    WIN,
    DEAD
};

// 棋盘尺寸
const int BoardSize = 20;

class GameModel
{
public:
    GameModel();

public:
    std::vector<std::vector<int>> ChessBoard; // 存储当前游戏棋盘和棋子的情况,空白为0，白子1，黑子-1
    std::vector<std::vector<int>> Score; // 存储各个点位的评分情况，作为AI下棋依据
    bool playerFlag; // 标示下棋方
    GameType gameType; // 游戏模式
    GameStatus gameStatus; // 游戏状态

    void Start(GameType type); // 开始游戏
    void calculateScore(); // 计算评分
    void PVP(int row, int col); // 人执行下棋
    void PVAI(int &clickRow, int &clickCol); // 机器执行下棋
    void updateGameMap(int row, int col); // 每次落子后更新游戏棋盘
    bool Win(int row, int col); // 判断游戏是否胜利
    bool Dead(); // 判断是否和棋
};

#endif // GAMEMODEL_H
