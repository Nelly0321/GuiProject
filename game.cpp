#include "game.h"
#include "board.h"

Game::Game(QGraphicsView * graphicsView)
{
    this->graphicsView = graphicsView;
    this->graphicsView->setMouseTracking(true);
    this->init();
}

Game::~Game()
{
    delete scene;
}

void Game::resetGame() {
    disconnect(scene, SIGNAL(restart()),
            this, SLOT(resetGame()));
    Board * newScene = new Board(graphicsView);
    newScene->setSceneRect(0, 0, 746, 1500);
    connect(newScene, SIGNAL(restart()),
            this, SLOT(resetGame()));
    Board * oldScene = qobject_cast<Board*>(this->scene);
    scene = newScene;
    oldScene->deleteLater();
}

void Game::init() {
    scene = new Board(graphicsView);
    scene->setSceneRect(0, 0, 746, 1500);
    connect(scene, SIGNAL(restart()),
            this, SLOT(resetGame()));
}

QGraphicsScene* Game::getScene() {
    return scene;
}
