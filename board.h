#ifndef BOARD_H
#define BOARD_H


#include <QGraphicsScene>
#include <QGraphicsView>
#include "mainscene.h"
#include "bubbles.h"
#include "aimer.h"

enum GameState {
    NORMAL, GAMEOVER, VICTORY, PAUSE
};

class Board : public MainScene
{
    Q_OBJECT
public:
    Board(QGraphicsView* graphicsView);
    ~Board();
signals:
    void restart();
private:
    const int rows = 13;
    const int cols = 9;
    int bubbleCount = 0;
    Bubbles*** bubbles;
    QVector<Bubbles*> bubbleList;
    QMap<int, QVector<Bubbles*>> deletingBubbles;
    QVector<Bubbles*> fallingBubbles;
    int deletingDepth = 0;
    void initBoard();
    void initShooter();
    void paintBg();
    QTimer *timer;
    Aimer *aimer;
    Bubbles* shootingBubble = nullptr;
    qreal shootingAngle;
    QPointF mousePos;
    QGraphicsRectItem *area = nullptr;
    QGraphicsPixmapItem *popUp = nullptr;
    QGraphicsRectItem *filter = nullptr;
    GameState gameState = NORMAL;
    bool showRestartButton = false;
    QGraphicsPixmapItem *restartButton = nullptr;
protected slots:
    void update();
    void mouseMoveEvent(QGraphicsSceneMouseEvent * mouseEvent);
    void mousePressEvent(QGraphicsSceneMouseEvent * mouseEvent);
};

#endif // BOARD_H
