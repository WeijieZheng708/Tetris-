#include <QPainter>
#include <QTimer>
#include <QMouseEvent>
#include <QMessageBox>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QDebug>
#include <math.h>
#include "mainwindow.h"
const int Margin = 30; // 棋盘边缘空隙
const int Radius = 20; // 棋子半径
const int MarkSize = 6; // 落子标记边长
const int BlockSize = 50; // 格子的大小
const int OffPos = 20; // 可将鼠标定位至格点的最大范围
const int AIWaiting = 500; // AI下棋的延迟
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // 设置棋盘大小
    setFixedSize(Margin * 2 + BlockSize * BoardSize, Margin * 2 + BlockSize * BoardSize);
}
MainWindow::~MainWindow()
{
    if (game)
    {
        delete game;
        game = nullptr;
    }
}
void MainWindow::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    // 绘制棋盘
   for (int i = 0; i < BoardSize + 1; i++)
    {
        painter.drawLine(Margin + BlockSize * i, Margin, Margin + BlockSize * i, size().height() - Margin);
        painter.drawLine(Margin, Margin + BlockSize * i, size().width() - Margin, Margin + BlockSize * i);
    }

    QBrush brush;
    brush.setStyle(Qt::SolidPattern);
    // 绘制落子标记(防止鼠标出框越界)
    if (clickPosRow > 0 && clickPosRow < BoardSize &&
        clickPosCol > 0 && clickPosCol < BoardSize &&
        game->ChessBoard[clickPosRow][clickPosCol] == 0)
    {
        if (game->playerFlag)
            brush.setColor(Qt::white);
        else
            brush.setColor(Qt::black);
        painter.setBrush(brush);
        painter.drawRect(Margin + BlockSize * clickPosCol - MarkSize / 2, Margin + BlockSize * clickPosRow - MarkSize / 2, MarkSize, MarkSize);
    }

    // 绘制棋子 
    for (int i = 0; i < BoardSize; i++)
        for (int j = 0; j < BoardSize; j++)
        {
            if (game->ChessBoard[i][j] == 1)
            {
                brush.setColor(Qt::white);
                painter.setBrush(brush);
                painter.drawEllipse(Margin + BlockSize * j - Radius, Margin + BlockSize * i - Radius, Radius * 2, Radius * 2);
            }
            else if (game->ChessBoard[i][j] == -1)
            {
                brush.setColor(Qt::black);
                painter.setBrush(brush);
                painter.drawEllipse(Margin + BlockSize * j - Radius, Margin + BlockSize * i - Radius, Radius * 2, Radius * 2);
            }
        }

    // 判断输赢
    if (clickPosRow > 0 && clickPosRow < BoardSize &&
        clickPosCol > 0 && clickPosCol < BoardSize &&
        (game->ChessBoard[clickPosRow][clickPosCol] == 1 ||
            game->ChessBoard[clickPosRow][clickPosCol] == -1))
    {
        if (game->Win(clickPosRow, clickPosCol) && game->gameStatus == PLAYING)
        {
            qDebug() << "win";
            game->gameStatus = WIN;

            QString str;
            if (game->ChessBoard[clickPosRow][clickPosCol] == 1)
                str = "白棋";
            else if (game->ChessBoard[clickPosRow][clickPosCol] == -1)
                str = "黑棋";
            QMessageBox::StandardButton btnValue = QMessageBox::information(this, "good", str + " 胜利");

            // 重置游戏状态，否则容易死循环
            if (btnValue == QMessageBox::Ok)
            {
                game->Start(game_type);
                game->gameStatus = PLAYING;
            }
        }
    }


    // 判断死局
    if (game->Dead())
    {

        QMessageBox::StandardButton btnValue = QMessageBox::information(this, "gg", "dead game!");
        if (btnValue == QMessageBox::Ok)
        {
            game->Start(game_type);
            game->gameStatus = PLAYING;
        }

    }
}
