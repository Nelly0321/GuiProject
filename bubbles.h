#ifndef BUBBLES_H
#define BUBBLES_H

#include <QGraphicsItem>
#include <QPainter>

enum Direction {
    TOP_LEFT, TOP_RIGHT, LEFT, RIGHT, BOTTOM_LEFT, BOTTOM_RIGHT, NONE
};

enum Color {
    BLACK, RED, YELLOW, GREEN, BLUE, PURPLE, TRANSPARENT
};

class Bubbles : public QGraphicsItem
{
public:
    Bubbles(Color color, QGraphicsItem* parent = nullptr);
    Bubbles(int row, int col, Color color, QGraphicsItem* parent = nullptr);
    ~Bubbles() override;
    void setNeighbour(Direction dir, Bubbles* n);
    bool hasNeighbour();
    Bubbles** getNeighbours();
    Bubbles* getNeighbour(Direction dir);
    Color getColor();
    void setColor(Color color);
    void tryPop(QVector<Bubbles*>& bubbles);
    void pop(QMap<int, QVector<Bubbles*>>& bubbles, QVector<Bubbles*>& fallingBubbles, int depth = 0);
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    QPainterPath shape() const override;


    bool reachTop(QVector<Bubbles*>& traversed);
    const static int width = 72;
    const static int height = 72;

    int row = 0;
    int col = 0;
private:
    Bubbles ** neighbours;
    Color color = Color::BLACK;
    bool poped = false;
    bool fallen = false;
    void unlink();
};

#endif // BUBBLES_H
