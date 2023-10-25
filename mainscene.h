#ifndef MAINSCENE_H
#define MAINSCENE_H

#include <QGraphicsScene>
#include <QGraphicsView>

class MainScene : public QGraphicsScene
{
public:
    MainScene(QGraphicsView * graphicsView);
protected:
    QGraphicsView * graphicsView = nullptr;
};

#endif // MAINSCENE_H
