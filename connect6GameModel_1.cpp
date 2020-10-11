#include <utility>
#include <stdlib.h>
#include <time.h>
#include "GameModel.h"

GameModel::GameModel()
{

}

void GameModel::Start(GameType type)
{
    gameType = type;
    // 初始棋盘
    ChessBoard.clear();
    for (int i = 0; i < BoardSize; i++)
    {
        std::vector<int> lineBoard;
        for (int j = 0; j < BoardSize; j++)
            lineBoard.push_back(0);
        ChessBoard.push_back(lineBoard);
    }

    // 如果是AI模式，需要初始化评分数组
    if (gameType == Robet)
    {
        Score.clear();
        for (int i = 0; i < BoardSize; i++)
        {
            std::vector<int> lineScores;
            for (int j = 0; j < BoardSize; j++)
                lineScores.push_back(0);
            Score.push_back(lineScores);
        }
    }

    // 己方下为true,对方下位false
    playerFlag = true;


}
