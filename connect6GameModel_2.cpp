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
void GameModel::updateGameMap(int row, int col)
{
    if (playerFlag)
        ChessBoard[row][col] = 1;
    else
        ChessBoard[row][col] = -1;

    // 换手
    playerFlag = !playerFlag;
}

void GameModel::PVP(int row, int col)
{
    updateGameMap(row, col);
}

void GameModel::PVAI(int &clickRow, int &clickCol)
{
    // 计算评分
    calculateScore();

    // 从评分中找出最大分数的位置
    int maxScore = 0;
    std::vector<std::pair<int, int>> maxPoints;

    for (int row = 1; row < BoardSize; row++)
        for (int col = 1; col < BoardSize; col++)
        {
            // 前提是这个坐标是空的
            if (ChessBoard[row][col] == 0)
            {
                if (Score[row][col] > maxScore)          // 找最大的数和坐标
                {
                    maxPoints.clear();
                    maxScore = Score[row][col];
                    maxPoints.push_back(std::make_pair(row, col));
                }
                else if (Score[row][col] == maxScore)     // 如果有多个最大的数，都存起来
                    maxPoints.push_back(std::make_pair(row, col));
            }
        }

    // 随机落子，如果有多个点的话
    srand((unsigned)time(0));
    int index = rand() % maxPoints.size();

    std::pair<int, int> pointPair = maxPoints.at(index);
    clickRow = pointPair.first; // 记录落子点
    clickCol = pointPair.second;
    updateGameMap(clickRow, clickCol);
}

 //评分函数
void GameModel::calculateScore()
{
    // 统计玩家或者电脑连成的子
    int PN = 0; // 玩家连成子的个数
    int RN = 0; // AI连成子的个数
    int emptyNum = 0; // 各方向空白位的个数

    // 清空评分数组
    Score.clear();
    for (int i = 0; i < BoardSize; i++)
    {
        std::vector<int> lineScores;
        for (int j = 0; j < BoardSize; j++)
            lineScores.push_back(0);
        Score.push_back(lineScores);
    }

    // 计分
    for (int row = 0; row < BoardSize; row++)
        for (int col = 0; col < BoardSize; col++)
        {
            // 空白点就算
            if (row > 0 && col > 0 &&
                ChessBoard[row][col] == 0)
            {
                // 遍历周围八个方向
                for (int y = -1; y <= 1; y++)
                    for (int x = -1; x <= 1; x++)
                    {
                        // 重置
                        PN = 0;
                        RN = 0;
                        emptyNum = 0;

                        // 原坐标不算
                        if (!(y == 0 && x == 0))
                        {
                            // 每个方向延伸5个子

                            // 对玩家白子评分（正反两个方向）
                            for (int i = 1; i <= 5; i++)
                            {
                                if (row + i * y > 0 && row + i * y < BoardSize &&
                                    col + i * x > 0 && col + i * x < BoardSize &&
                                    ChessBoard[row + i * y][col + i * x] == 1) // 玩家的子
                                {
                                    PN++;
                                }
                                else if (row + i * y > 0 && row + i * y < BoardSize &&
                                         col + i * x > 0 && col + i * x < BoardSize &&
                                         ChessBoard[row + i * y][col + i * x] == 0) // 空白位
                                {
                                    emptyNum++;
                                    break;
                                }
                                else            // 出边界
                                    break;
                            }

                            for (int i = 1; i <= 5; i++)
                            {
                                if (row - i * y > 0 && row - i * y < BoardSize &&
                                    col - i * x > 0 && col - i * x < BoardSize &&
                                    ChessBoard[row - i * y][col - i * x] == 1) // 玩家的子
                                {
                                    PN++;
                                }
                                else if (row - i * y > 0 && row - i * y < BoardSize &&
                                         col - i * x > 0 && col - i * x < BoardSize &&
                                         ChessBoard[row - i * y][col - i * x] == 0) // 空白位
                                {
                                    emptyNum++;
                                    break;
                                }
                                else            // 出边界
                                    break;
                            }

                            if (PN == 1)                      // 杀二
                                Score[row][col] += 10;
                            else if (PN == 2)                 // 杀三
                            {
                                if (emptyNum == 1)
                                    Score[row][col] += 20;
                                else if (emptyNum == 2)
                                    Score[row][col] += 40;
                            }
                            else if (PN == 3)                 // 杀四
                            {
                                if (emptyNum == 1)
                                    Score[row][col] += 50;
                                else if (emptyNum == 2)
                                    Score[row][col] += 100;
                            }
                            else if (PN == 4)   // 杀五
                            {
                                if (emptyNum == 1)
                                    Score[row][col] +=1000;
                                else if (emptyNum == 2)
                                    Score[row][col] += 2000;
                            }
                            else if (PN == 5)    // 杀六
                            {
                                if (emptyNum == 1)
                                    Score[row][col] +=10000;
                                else if (emptyNum == 2)
                                    Score[row][col] += 20000;
                            }


                            // 进行一次清空
                            emptyNum = 0;

                            // 对AI黑子评分
                            for (int i = 1; i <= 5; i++)
                            {
                                if (row + i * y > 0 && row + i * y < BoardSize &&
                                    col + i * x > 0 && col + i * x < BoardSize &&
                                    ChessBoard[row + i * y][col + i * x] == -1) // AI的子
                                {
                                    RN++;
                                }
                                else if (row + i * y > 0 && row + i * y < BoardSize &&
                                         col + i * x > 0 && col + i * x < BoardSize &&
                                         ChessBoard[row +i * y][col + i * x] == 0) // 空白位
                                {
                                    emptyNum++;
                                    break;
                                }
                                else            // 出边界
                                    break;
                            }

                            for (int i = 1; i <= 5; i++)
                            {
                                if (row - i * y > 0 && row - i * y < BoardSize &&
                                    col - i * x > 0 && col - i * x < BoardSize &&
                                    ChessBoard[row - i * y][col - i * x] == -1) // AI的子
                                {
                                    RN++;
                                }
                                else if (row - i * y > 0 && row - i * y < BoardSize &&
                                         col - i * x > 0 && col - i * x < BoardSize &&
                                         ChessBoard[row - i * y][col - i * x] == 0) // 空白位
                                {
                                    emptyNum++;
                                    break;
                                }
                                else            // 出边界
                                    break;
                            }

                            if (RN == 0)                      // 普通下子
                                Score[row][col] += 5;
                            else if (RN == 1)                 // 活二
                                Score[row][col] += 10;
                            else if (RN == 2)
                            {
                                if (emptyNum == 1)                // 眠三
                                    Score[row][col] += 25;
                                else if (emptyNum == 2)
                                    Score[row][col] += 50;  // 活三
                            }
                            else if (RN == 3)
                            {
                                if (emptyNum == 1)                // 眠四
                                    Score[row][col] += 55;
                                else if (emptyNum == 2)
                                    Score[row][col] += 100; // 活四
                            }
                            else if (RN == 4)

                            {
                                if (emptyNum == 1)                // 眠五
                                      Score[row][col] += 250;
                                 else if (emptyNum == 2)
                                      Score[row][col] += 500; // 活五
                            }

                            else if (RN == 5)
                            {
                                Score[row][col] += 1000;
                            }
                        }
                    }

            }
        }
}
