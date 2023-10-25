#include "bubbles.h"
#include "resources.h"
#include <QVector>
#include <QQueue>

Bubbles::Bubbles(int row, int col, Color color, QGraphicsItem * parent):QGraphicsItem(parent)
{
    this->row = row;
    this->col = col;
    neighbours = new Bubbles*[6];
    for (int i = 0; i < 6; i++) {
        neighbours[i] = nullptr;
    }
    this->color = color;
}

Bubbles::Bubbles(Color color, QGraphicsItem * parent):Bubbles(-1, -1, color, parent)
{
}

Bubbles::~Bubbles()
{
    unlink();
    delete [] neighbours;
}

void Bubbles::unlink() {
    this->color = TRANSPARENT;
}

bool Bubbles::hasNeighbour() {
    for (int dir = TOP_LEFT; dir <= BOTTOM_RIGHT; dir++) {
        if (neighbours[dir] != nullptr && neighbours[dir]->color != TRANSPARENT) {
            return true;
        }
    }
    return false;
}

void Bubbles::setNeighbour(Direction direction, Bubbles* neighbour) {
    neighbours[direction] = neighbour;
}

Bubbles** Bubbles::getNeighbours() {
    return neighbours;
}

Bubbles * Bubbles::getNeighbour(Direction direction) {
    return neighbours[direction];
}

Color Bubbles::getColor() {
    return color;
}

void Bubbles::setColor(Color color) {
    this->color = color;
    this->poped = false;
}

QRectF Bubbles::boundingRect() const {
    return QRect(-width/2.0, -height/2.0, width, height);
}


QPainterPath Bubbles::shape() const
{
    QPainterPath path;
    if (row == -1) {
        path.addEllipse(QRect(-width*0.25, -height*0.25, width*0.5, height*0.5));
    } else {
        path.addEllipse(QRect(-width/2.0, -height/2.0, width, height));
    }
    return path;
}

void Bubbles::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QPixmap * bubbleImage = nullptr;
    switch (color) {
    case BLACK:
        bubbleImage = Resources::getInstance()->getResource("bubble_black");
    break;
    case RED:
        bubbleImage = Resources::getInstance()->getResource("bubble_red");
    break;
    case YELLOW:
        bubbleImage = Resources::getInstance()->getResource("bubble_yellow");
    break;
    case GREEN:
        bubbleImage = Resources::getInstance()->getResource("bubble_green");
    break;
    case BLUE:
        bubbleImage = Resources::getInstance()->getResource("bubble_blue");
    break;
    case PURPLE:
        bubbleImage = Resources::getInstance()->getResource("bubble_purple");
    break;
    case TRANSPARENT:
        return;
    }
    if (bubbleImage != nullptr)
        painter->drawPixmap(-width/2.0, -height/2.0, width, height, *bubbleImage);
}

bool Bubbles::reachTop(QVector<Bubbles*>& traversed) {
    if (this->poped) {
        return false;
    }
    if (row == 0) {
        return true;
    }
    if (traversed.contains(this)) {
        return false;
    }
    traversed.push_back(this);
    for (int dir = TOP_LEFT; dir <= BOTTOM_RIGHT; dir++) {
        if (neighbours[dir] != nullptr && neighbours[dir]->color != TRANSPARENT) {
            if (neighbours[dir]->reachTop(traversed)) {
                return true;
            }
        }
    }
    return false;
}

void Bubbles::tryPop(QVector<Bubbles*>& traversed) {
    if (traversed.contains(this)) {
        return;
    }
    traversed.push_back(this);
    for (int dir = TOP_LEFT; dir <= BOTTOM_RIGHT; dir++) {
        if (neighbours[dir] != nullptr && neighbours[dir]->color == this->color) {
            neighbours[dir]->tryPop(traversed);
        }
    }
}


void Bubbles::pop(QMap<int, QVector<Bubbles*>> &eliminatedBubbles, QVector<Bubbles*>& fallingBubbles, int level) {
    QQueue<Bubbles*> queue;
    queue.enqueue(this);
    queue.enqueue(nullptr);
    QVector<Bubbles*> surroundings;
    this->poped = true;
    while (!queue.empty()) {
        Bubbles * b = queue.dequeue();
        if (b == nullptr) {
            level++;
            queue.enqueue(nullptr);
            if (queue.head() == nullptr) break;
            else continue;
        }
        eliminatedBubbles[level].push_back(b);

        for (int dir = TOP_LEFT; dir <= BOTTOM_RIGHT; dir++) {
            if (b->neighbours[dir] != nullptr && b->neighbours[dir]->getColor() != TRANSPARENT && !b->neighbours[dir]->poped) {
                if (b->neighbours[dir]->getColor() == color) {
                    b->neighbours[dir]->poped = true;
                    queue.enqueue(b->neighbours[dir]);
                } else {
                    if (!surroundings.contains(b->neighbours[dir])) {
                        surroundings.push_back(b->neighbours[dir]);
                    }
                }
            }
        }
    }

    for (auto & s : surroundings) {
        QVector<Bubbles*> temp;
        bool rt = s->reachTop(temp);
        if (!rt) {
            for (auto & t : temp) {
                t->poped = true;
            }
            fallingBubbles.append(temp);
        }
    }
}
