#include "board.h"
#include <QRandomGenerator>
#include <QTimer>
#include <QVector>
#include <QtMath>
#include <QPushButton>
#include <QGraphicsSceneMouseEvent>
#include "resources.h"

Board::Board(QGraphicsView* graphicsView):MainScene(graphicsView) {
    initBoard();
    initShooter();

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    timer->start(10);
}

Board::~Board() {
    disconnect(timer, SIGNAL(timeout()), this, SLOT(update()));
    timer->stop();
}

void Board::initBoard() {
    this->bubbles = new Bubbles **[rows];
    for (int row = 0; row < rows; row++) {
        bubbles[row] = new Bubbles *[cols];
        for (int col = 0; col < cols; col++) {
            Color randomColor = static_cast<Color>(QRandomGenerator::global()->generate() % 7);
            if (row >= 9) {
                randomColor = TRANSPARENT;
            }
            if (randomColor != TRANSPARENT) {
                bubbleCount++;
            }
            bubbles[row][col] = new Bubbles(row, col, randomColor);
        }
    }
    for (int row = 0; row < rows; row++) {
        for (int col = 0; col < cols; col++) {
            bool isEven = row % 2 == 0;

            if (row != 0 && (!isEven || col != 0)) {
                bubbles[row][col]->setNeighbour(TOP_LEFT, bubbles[row - 1][col - isEven]);
            }
            if (row != 0 && (isEven || col != cols - 1)) {
                bubbles[row][col]->setNeighbour(TOP_RIGHT, bubbles[row - 1][col + !isEven]);
            }
            if (col != 0) {
                bubbles[row][col]->setNeighbour(LEFT, bubbles[row][col - 1]);
            }
            if (col != 8) {
                bubbles[row][col]->setNeighbour(RIGHT, bubbles[row][col + 1]);
            }
            if (row != rows - 1 && (!isEven || col != 0)) {
                bubbles[row][col]->setNeighbour(BOTTOM_LEFT, bubbles[row + 1][col - isEven]);
            }
            if (row != rows - 1 && (isEven || col != cols - 1)) {
                bubbles[row][col]->setNeighbour(BOTTOM_RIGHT, bubbles[row + 1][col + !isEven]);
            }
            int centerX = sceneRect().center().x();
            int x = centerX + (col - 4) * (Bubbles::width + 10);
            int offsetX = row % 2 == 1 ? (Bubbles::width+10) * 0.25 : (Bubbles::width+10) * -0.25;
            bubbles[row][col]->setPos(x + offsetX, Bubbles::height/2 + row * Bubbles::height);
            addItem(bubbles[row][col]);
        }
    }
    deletingDepth = 0;
}

void Board::initShooter() {
    QPointF p(sceneRect().center().x(), 0);
    aimer = new Aimer(p, this);
    int size = 5;
    for (int i = 0; i < size; i++) {
        Color randomColor = static_cast<Color>(QRandomGenerator::global()->generate() % 6);
        Bubbles * b = new Bubbles(randomColor);

        int centerX = sceneRect().center().x();
        int x = centerX - i * (Bubbles::width + 10); // 10 is the margin
        b->setPos(x, sceneRect().bottom() - Bubbles::height/2 - 10);
        addItem(b);

        bubbleList.append(b);
    }
}

void Board::paintBg() {
    QLinearGradient bg;
    bg.setCoordinateMode(QLinearGradient::ObjectBoundingMode);
    bg.setColorAt(0.0, QColor::fromRgb(27, 50, 104));
    bg.setColorAt(0.45, QColor::fromRgb(58, 92, 163));
    bg.setColorAt(1.0, QColor::fromRgb(139, 60, 162));
    setBackgroundBrush(bg);

    if (area == nullptr) {
        area = new QGraphicsRectItem(sceneRect());
       // area->setPen(Qt::NoPen);
        QBrush brush(QColor(10,5,10,50));
        area->setBrush(brush);
        addItem(area);
    }
}

void Board::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    double mouseX = mouseEvent->scenePos().x();
    double mouseY = mouseEvent->scenePos().y();
    mousePos.setX(mouseX);
    mousePos.setY(mouseY);
}

void Board::mousePressEvent(QGraphicsSceneMouseEvent * mouseEvent) {
    if (gameState == NORMAL) {
        if (mouseEvent->buttons() & Qt::LeftButton)
        {
            gameState = PAUSE;
            shootingAngle = aimer->getAngle();
            shootingBubble = new Bubbles(-1, -1, bubbleList.first()->getColor());
            shootingBubble->setPos(bubbleList.first()->pos());
            shootingBubble->setZValue(89);
            addItem(shootingBubble);
            bubbleCount++;


            for(auto iter = bubbleList.begin(); iter != bubbleList.end(); ++iter) {
                Bubbles * curr = *iter;
                if (iter == bubbleList.end() - 1) {
                    Color randomColor = static_cast<Color>(QRandomGenerator::global()->generate() % 6);
                    curr->setColor(randomColor);
                }  else {
                    Bubbles * next = *(iter + 1);
                    curr->setColor(next->getColor());
                }
            }
        }

    } else if (gameState == VICTORY || gameState == GAMEOVER) {
        if (restartButton != nullptr && restartButton->isUnderMouse()) {
            emit restart();
        }
    }
}

Bubbles* stickedBubble = nullptr;

void Board::update()
{
    if (shootingBubble != nullptr) {
        if (shootingBubble->x() - shootingBubble->boundingRect().width() / 2 < sceneRect().left() || shootingBubble->x() + shootingBubble->boundingRect().width() / 2 > sceneRect().right()) {
            shootingAngle = 180 - shootingAngle;
        }
        shootingBubble->moveBy(qCos(qDegreesToRadians(shootingAngle)) *15, -qSin(qDegreesToRadians(shootingAngle)) *15);
        QList<QGraphicsItem*> collidingItems = shootingBubble->collidingItems();

        if (shootingBubble->y() < sceneRect().top()) {
            collidingItems.push_back(stickedBubble);
        }
        for (const auto item : collidingItems) {
            if (Bubbles* bubble = dynamic_cast<Bubbles*>(item)) {
                if (bubble->row != -1) {
                    if (bubble->getColor() != TRANSPARENT || shootingBubble->y() < sceneRect().top()) {
                        if (stickedBubble != nullptr) {
                            stickedBubble->setColor(shootingBubble->getColor());
                            QVector<Bubbles*> checked;
                            stickedBubble->tryPop(checked);
                            if (checked.size() >= 3) {
                                stickedBubble->pop(deletingBubbles, fallingBubbles);
                            }
                        } else {
                            if (gameState != GAMEOVER) {
                                showRestartButton = true;
                            }
                            gameState = GAMEOVER;
                        }
                        delete shootingBubble;
                        shootingBubble = nullptr;
                        stickedBubble = nullptr;
                        break;
                    } else {
                        stickedBubble = bubble;
                    }
                }
            }
        }
    }

    if (gameState == GAMEOVER) {
        if (popUp == nullptr) {
            popUp = new QGraphicsPixmapItem(*Resources::getInstance()->getResource("gameover"));
            popUp->setOpacity(0.1);
            popUp->setPos(QPointF(sceneRect().center().x() - popUp->boundingRect().width() / 2, sceneRect().top() - 100));
            popUp->setZValue(91);
            addItem(popUp);
        }
        if (popUp->y() <= sceneRect().center().y() - popUp->boundingRect().height() / 2) {
            popUp->setPos(popUp->pos().x(), popUp->pos().y() + 10);
        }
        if (popUp->opacity() < 1) {
            popUp->setOpacity(popUp->opacity() * 1.1);
        }
    }
    if (gameState == NORMAL || gameState == PAUSE) {
        aimer->updateMousePos(mousePos);
    }



    paintBg();


    if (!deletingBubbles[deletingDepth].empty()) {
        const QVector<Bubbles*> &bubbles = deletingBubbles[deletingDepth];
        bool nextLevel = true;
        for (auto& b : bubbles) {
            b->setScale(b->scale() * 0.85);
            if (b->scale() < 0.1) {
                // Reset back to its original state
                b->setColor(TRANSPARENT);
                b->setScale(1);
            } else {
                // Still have bubbles animating
                nextLevel = false;
            }
        }
        if (nextLevel) {
            bubbleCount -= deletingBubbles[deletingDepth].count();
            deletingBubbles[deletingDepth].clear();
            deletingDepth++;
        }
    } else {
        deletingDepth = 0;
        deletingBubbles.clear();
        // Animate the bubbles to be fallen down
        if (fallingBubbles.size() > 0) {
            bool finish = true;
            for (auto& b: fallingBubbles) {
                QTransform t = b->transform();
                t.translate(0, t.m32() + 1);
                b->setTransform(t);
                b->setOpacity(b->opacity() * 0.99);
                if (b->transform().m32() + b->pos().y() > sceneRect().bottom()) {
                    // Reset back to its original state
                    QTransform t = b->transform();
                    t.reset();
                    b->setTransform(t);
                    b->setOpacity(1);
                    b->setColor(TRANSPARENT);
                } else {
                    finish = false;
                }
            }
            if (finish) {
                // All bubbles were animated
                bubbleCount -= fallingBubbles.count();
                fallingBubbles.clear();
            }
        } else {
            if (gameState == PAUSE && shootingBubble == nullptr)
                gameState = NORMAL;
        }
    }

    if (bubbleCount == 0) {
        gameState = VICTORY;
        showRestartButton = true;
    }

    if (gameState == VICTORY) {
        if (popUp == nullptr) {
            popUp = new QGraphicsPixmapItem(*Resources::getInstance()->getResource("victory"));
            popUp->setOpacity(0.1);
            popUp->setPos(QPointF(sceneRect().center().x() - popUp->boundingRect().width() / 2, sceneRect().top() - 100));
            popUp->setZValue(91);
            addItem(popUp);
        }
        if (popUp->y() <= sceneRect().center().y() - popUp->boundingRect().height() / 2) {
            popUp->setPos(popUp->pos().x(), popUp->pos().y() + 10);
        }
        if (popUp->opacity() < 1) {
            popUp->setOpacity(popUp->opacity() * 1.1);
        }
    }

    if (showRestartButton) {
        if (restartButton == nullptr) {
            restartButton = new QGraphicsPixmapItem(*Resources::getInstance()->getResource("restart"));
            restartButton->setPos(QPointF(sceneRect().center().x() - restartButton->boundingRect().width() / 2, sceneRect().center().y() + 180));
            restartButton->setZValue(101);
            addItem(restartButton);
        }
    }
}

