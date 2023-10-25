#include "mainscene.h"
#include "bubbles.h"

MainScene::MainScene(QGraphicsView * graphicsView)
{
    this->graphicsView = graphicsView;
    graphicsView->setScene(this);
    QRectF rect(0, 0, 768, 1024);
    setSceneRect(rect);
    QBrush bg;
    bg.setColor(QColor(30, 40, 40 ));
    bg.setStyle(Qt::SolidPattern);
    setBackgroundBrush(bg);
}
