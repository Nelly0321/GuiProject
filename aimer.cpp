#include "aimer.h"
#include <QtMath>

#include "bubbles.h"

Aimer::Aimer(QPointF& Pos, QGraphicsScene * scene): scene(scene)
{
    QPen pen(QColor::fromRgb(255,255,255,128), 8, Qt::DashLine);
    line = scene->addLine(scene->sceneRect().center().x(), scene->sceneRect().bottom() - Bubbles::height/2-10, mousePos.x(), mousePos.y(), pen);
    this->updateMousePos(Pos);
}

Aimer::~Aimer()
{
    delete line;
}

qreal Aimer::getAngle() {
    return angle;
}

void Aimer::updateMousePos(QPointF &mousePos) {
    this->mousePos.setX(mousePos.x());
    this->mousePos.setY(mousePos.y());
    QGraphicsLineItem * m_line = line;
    QLineF tmp(scene->sceneRect().center().x(), scene->sceneRect().bottom() - Bubbles::height/2 - 10, mousePos.x(), mousePos.y());
    qreal angle = tmp.angle();
    this->angle = angle;
    QLineF l1 = m_line->line();
    l1.setAngle(angle);
    l1.setLength(200);
    m_line->setLine(l1);
}
