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


    // 开启鼠标追踪功能
    setMouseTracking(true);

    // 添加菜单
    QMenu *gameMenu = menuBar()->addMenu(tr("Game")); // menuBar默认是存在的，直接加菜单就可以了
    QAction *actionPVP = new QAction("Person VS Person", this);
    connect(actionPVP, SIGNAL(triggered()), this, SLOT(PVP()));
    gameMenu->addAction(actionPVP);

    QAction *actionPVE = new QAction("Person VS Computer", this);
    connect(actionPVE, SIGNAL(triggered()), this, SLOT(PVAI()));
    gameMenu->addAction(actionPVE);

    // 开始游戏
    BeginGame();
}

MainWindow::~MainWindow()
{
    if (game)
    {
        delete game;
        game = nullptr;
    }
}

void MainWindow::BeginGame()
{   
    // 初始化游戏模型
    game = new GameModel;
    PVP();
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

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{   
    // 监听鼠标，定位
    int x = event->x();
    int y = event->y();

    // 棋盘边缘不能落子
    if (x >= Margin + BlockSize / 2 &&
            x < size().width() - Margin &&
            y >= Margin + BlockSize / 2 &&
            y < size().height()- Margin)
    {
        // 获取最近的左上角的点
        int col = x / BlockSize;
        int row = y / BlockSize;

        int LeftPosX = Margin + BlockSize * col;
        int LeftPosY = Margin + BlockSize * row;

        // 根据距离算出合适的点击位置,一共四个点，根据半径距离选最近的
        clickPosRow = -1; // 初始化最终的值
        clickPosCol = -1;
        int len = 0; // 计算完后取整就可以了

        // 确定一个误差在范围内的点，且只可能确定一个出来
        len = sqrt((x - LeftPosX) * (x - LeftPosX) + (y - LeftPosY) * (y - LeftPosY));
        if (len < OffPos)
        {
            clickPosRow = row;
            clickPosCol = col;
        }
        len = sqrt((x - LeftPosX - BlockSize) * (x - LeftPosX - BlockSize) + (y - LeftPosY) * (y - LeftPosY));
        if (len < OffPos)
        {
            clickPosRow = row;
            clickPosCol = col + 1;
        }
        len = sqrt((x - LeftPosX) * (x - LeftPosX) + (y - LeftPosY - BlockSize) * (y - LeftPosY - BlockSize));
        if (len < OffPos)
        {
            clickPosRow = row + 1;
            clickPosCol = col;
        }
        len = sqrt((x - LeftPosX - BlockSize) * (x - LeftPosX - BlockSize) + (y - LeftPosY - BlockSize) * (y - LeftPosY - BlockSize));
        if (len < OffPos)
        {
            clickPosRow = row + 1;
            clickPosCol = col + 1;
        }
    }

    // 存了坐标后也要重绘
    update();
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    // 人下棋，并且不能抢机器的棋
    if (!(game_type == Robet && !game->playerFlag))
    {
        chessOneByPerson();
        // 如果是人机模式，需要调用AI下棋
        if (game->gameType == Robet && !game->playerFlag)
        {
            // 用定时器做一个延迟
            QTimer::singleShot(AIWaiting, this, SLOT(chessOneByAI()));
        }
    }

}
