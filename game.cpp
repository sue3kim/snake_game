#include "game.hpp"
#include <cstdlib>
#include <ctime>
#include <algorithm>

Windmill windmill;

// Game ������: ���� �ʱ�ȭ �� �⺻ ����
Game::Game(int width, int height)
    : snake(width / 2, height / 2), width(width), height(height),
    scoreGrowth(0), scorePoison(0), scoreGate(0), maxLength(0),
    currentStage(1), missionB(4), missionGrowth(1), missionPoison(1), missionGate(1), missionMaxLength(5),
    missionBComplete(false), missionGrowthComplete(false), missionPoisonComplete(false), missionGateComplete(false), missionMaxLengthComplete(false),
    gateCreated(false), gateCreationTime(time(0)), startTime(time(0)), gameSpeed(200) {

    initscr();  // ncurses �ʱ�ȭ
    cbreak();
    noecho();
    curs_set(0);  // Ŀ�� ����
    keypad(stdscr, TRUE);  // Ű�е� �Է� Ȱ��ȭ
    timeout(gameSpeed);  // �ʱ� ���� �ӵ� ����

    win = newwin(height, width, 0, 0);  // ���� â ����
    scoreWin = newwin(7, 30, 4, width + 2); // ���ھ�� â ����
    missionWin = newwin(9, 30, 11, width + 2); // �̼� â ����
    timeWin = newwin(3, 30, 1, width + 2); // �ð� â ����

    srand(static_cast<unsigned int>(time(0)));  // ���� ������ �õ� ����

    initializeMap();  // �� �ʱ�ȭ
    initializeStage();  // �������� �ʱ�ȭ
    placeItems();  // ������ ��ġ
}

// Game �Ҹ���: ncurses ����
Game::~Game() {
    delwin(win);
    delwin(scoreWin);
    endwin();
}

// ���� ���� ����
void Game::run() {
    while (true) {
        draw();  // ȭ�� �׸���
        processInput();  // �Է� ó��
        update();  // ���� ������Ʈ
    }
}

// ���� ���� �Լ�
void Game::gameOver() {
    delwin(win);
    delwin(scoreWin);
    endwin();  // ncurses ����
    exit(0);  // ���α׷� ����
}

// ����� �Է� ó��
void Game::processInput() {
    int ch = getch();
    switch (ch) {
    case KEY_UP:
        snake.changeDirection(UP);
        break;
    case KEY_DOWN:
        snake.changeDirection(DOWN);
        break;
    case KEY_LEFT:
        snake.changeDirection(LEFT);
        break;
    case KEY_RIGHT:
        snake.changeDirection(RIGHT);
        break;
    }
}

// ����Ʈ ��� �Լ�
void Game::moveThroughGate(const std::pair<int, int>& exitGate) {
    auto exitX = exitGate.first;
    auto exitY = exitGate.second;
    std::pair<int, int> newHead = { -1, -1 };
    Direction newDirection;
    Direction entryDirection = snake.getDirection();
    std::vector<Direction> directions;

    // ���� ���⿡ ���� ���� ���� �켱���� ����
    switch (entryDirection) {
    case UP:
        directions = { UP, RIGHT, LEFT, DOWN };
        break;
    case DOWN:
        directions = { DOWN, RIGHT, LEFT, UP };
        break;
    case LEFT:
        directions = { LEFT, UP, DOWN, RIGHT };
        break;
    case RIGHT:
        directions = { RIGHT, DOWN, UP, LEFT };
        break;
    }

    // �켱���� ���⿡ ���� ���ο� �Ӹ� ��ġ�� ���� ����
    for (const auto& direction : directions) {
        switch (direction) {
        case UP:
            if (exitY > 0 && map[exitY - 1][exitX] == 0 && !snake.isOccupying(exitX, exitY - 1)) {
                newHead = { exitX, exitY - 1 };
                newDirection = UP;
            }
            break;
        case DOWN:
            if (exitY < height - 1 && map[exitY + 1][exitX] == 0 && !snake.isOccupying(exitX, exitY + 1)) {
                newHead = { exitX, exitY + 1 };
                newDirection = DOWN;
            }
            break;
        case LEFT:
            if (exitX > 0 && map[exitY][exitX - 1] == 0 && !snake.isOccupying(exitX - 1, exitY)) {
                newHead = { exitX - 1, exitY };
                newDirection = LEFT;
            }
            break;
        case RIGHT:
            if (exitX < width - 1 && map[exitY][exitX + 1] == 0 && !snake.isOccupying(exitX + 1, exitY)) {
                newHead = { exitX + 1, exitY };
                newDirection = RIGHT;
            }
            break;
        }
        if (newHead != std::pair<int, int>{-1, -1}) {
            break;
        }
    }

    // ��ȿ�� ���ο� �Ӹ� ��ġ�� ������ ���� ����
    if (newHead == std::pair<int, int>{-1, -1}) {
        gameOver();
        return;
    }

    snake.setHead(newHead, newDirection);

    // ����Ʈ�� �ٶ����� ���� �ִ� ��� �ٶ����� ȸ�� ����
    if (isGateInWindmill(exitGate)) {
        windmillPaused = true;
        pauseStart = time(0);
    }
}

// ���� ���� ������Ʈ
void Game::update() {
    static int updateCounter = 0;
    snake.update();
    timeout(static_cast<int>(snake.getMoveInterval() * 1000));
    time_t currentTime = time(0);
    double elapsedSeconds = difftime(currentTime, startTime);

    // ���� �ð� �ʰ� �� ���� ����
    if (elapsedSeconds > 120) {
        gameOver();
    }

    // �ִ� ���� ������Ʈ
    if (snake.getBody().size() > static_cast<size_t>(maxLength)) {
        maxLength = snake.getBody().size();
    }

    removeOldItems();  // ������ ������ ����

    auto head = snake.getHead();

    // ���̳� ��ֹ� �Ǵ� �ڽŰ� �浹 �� ���� ����
    if (map[head.second][head.first] == 1 || map[head.second][head.first] == 2 || snake.checkCollision()) {
        gameOver();
    }

    // �ٶ����� �Ȱ��� �浹 �˻�
    for (int i = 1; i <= windmill.length; ++i) {
        if ((map[windmill.center.second + i][windmill.center.first] == 1 && snake.isOccupying(windmill.center.first, windmill.center.second + i)) ||
            (map[windmill.center.second - i][windmill.center.first] == 1 && snake.isOccupying(windmill.center.first, windmill.center.second - i)) ||
            (map[windmill.center.second][windmill.center.first + i] == 1 && snake.isOccupying(windmill.center.first + i, windmill.center.second)) ||
            (map[windmill.center.second][windmill.center.first - i] == 1 && snake.isOccupying(windmill.center.first - i, windmill.center.second)) ||
            (map[windmill.center.second + i][windmill.center.first + i] == 1 && snake.isOccupying(windmill.center.first + i, windmill.center.second + i)) ||
            (map[windmill.center.second - i][windmill.center.first - i] == 1 && snake.isOccupying(windmill.center.first - i, windmill.center.second - i)) ||
            (map[windmill.center.second + i][windmill.center.first - i] == 1 && snake.isOccupying(windmill.center.first - i, windmill.center.second + i)) ||
            (map[windmill.center.second - i][windmill.center.first + i] == 1 && snake.isOccupying(windmill.center.first + i, windmill.center.second - i))) {
            gameOver();
        }
    }

    // ���� ������ �Ա�
    if (map[head.second][head.first] == 5) {
        snake.grow();
        map[head.second][head.first] = 0;
        scoreGrowth++;
    }
    // �� ������ �Ա�
    else if (map[head.second][head.first] == 6) {
        snake.shrink();
        map[head.second][head.first] = 0;
        scorePoison++;
        if (snake.getBody().size() < 3) {
            gameOver();
        }
    }
    // �ӵ� ���� ������ �Ա�
    else if (map[head.second][head.first] == 9) {
        snake.increaseSpeed();
        map[head.second][head.first] = 0;
    }
    // �ӵ� ���� ������ �Ա�
    else if (map[head.second][head.first] == 10) {
        snake.decreaseSpeed();
        map[head.second][head.first] = 0;
    }

    // ����Ʈ ���
    if (head == gateA) {
        moveThroughGate(gateB);
        scoreGate++;
    }
    else if (head == gateB) {
        moveThroughGate(gateA);
        scoreGate++;
    }

    updateMissionStatus();  // �̼� ���� ������Ʈ

    // ��� �̼� �Ϸ� �� ���� ���������� ����
    if (missionBComplete && missionGrowthComplete && missionPoisonComplete && missionGateComplete) {
        nextStage();
    }

    placeItems();  // ������ ��ġ

    // �ٶ����� ȸ�� ó�� (10�� ������Ʈ���� �� ���� ȸ��)
    if (currentStage == 4) {
        if (++updateCounter >= 10) {
            rotateWindmill();
            updateCounter = 0;
        }
    }

    timeout(static_cast<int>(snake.getMoveInterval() * 1000));  // ���� �̵� ���ݿ� ���� timeout ����
}

// �̼� ���� ������Ʈ
void Game::updateMissionStatus() {
    missionBComplete = (snake.getBody().size() >= static_cast<size_t>(missionB));
    missionGrowthComplete = (scoreGrowth >= missionGrowth);
    missionPoisonComplete = (scorePoison >= missionPoison);
    missionGateComplete = (scoreGate >= missionGate);
    missionMaxLengthComplete = (maxLength >= missionMaxLength);  // �ִ� ���� ���� �̼� �Ϸ� ���� �߰�
}

// ���� ���������� ����
void Game::nextStage() {
    currentStage++;

    if (currentStage > 4) {
        gameOver();
    }
    else {
        reset();
        snake.setMoveInterval(snake.getMoveInterval() * 0.5f);
    }
}

// �ʱ� �� ����
void Game::initializeMap() {
    map = std::vector<std::vector<int>>(height, std::vector<int>(width, 0));
    for (int i = 0; i < width; ++i) {
        map[0][i] = 1;
        map[height - 1][i] = 1;
    }
    for (int i = 0; i < height; ++i) {
        map[i][0] = 1;
        map[i][width - 1] = 1;
    }

    map[0][0] = 2;
    map[0][width - 1] = 2;
    map[height - 1][0] = 2;
    map[height - 1][width - 1] = 2;
}

// �������� �ʱ�ȭ
void Game::initializeStage() {
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            if (map[y][x] != 1 && map[y][x] != 2) {
                map[y][x] = 0;
            }
        }
    }

    if (currentStage == 2) {
        int startX = 15;  // ���� ���μ� ���� ����
        int startY = height - 15; // ���� ���μ� ���� ����
        for (int i = startY; i < height - 5; ++i) {
            map[i][startX] = 1;  // ���� ���μ�
        }
        for (int i = 5; i < startX; ++i) {
            map[startY][i] = 1;  // ���� ���μ�
        }
    }
    else if (currentStage == 3) {
        for (int i = 5; i < width - 5; ++i) {
            map[height / 3][i] = 1;
            map[2 * height / 3][i] = 1;
        }
    }
    else if (currentStage == 4) {
        initializeWindmill();
        snake = Snake(width - 5, 1);  // ���� ������ ��ܿ��� �����ϵ��� ����
        snake.changeDirection(DOWN);  // ���� ������ �Ʒ��� ����
    }
}

// ȭ�� �׸���
void Game::draw() {
    werase(win);
    werase(scoreWin);
    werase(missionWin);
    werase(timeWin);

    // �������� ��ȣ ǥ��
    mvprintw(0, width + 5, "Stage %d", currentStage);

    time_t currentTime = time(0);
    double elapsedSeconds = difftime(currentTime, startTime);
    int minutes = static_cast<int>(elapsedSeconds) / 60;
    int seconds = static_cast<int>(elapsedSeconds) % 60;

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            if (map[y][x] == 1) {
                mvwaddch(win, y, x, '#');
            }
            else if (map[y][x] == 2) {
                mvwaddch(win, y, x, '*');
            }
            else if (map[y][x] == 5) {
                mvwaddch(win, y, x, '+');
            }
            else if (map[y][x] == 6) {
                mvwaddch(win, y, x, '-');
            }
            else if (map[y][x] == 7) {
                mvwaddch(win, y, x, 'G');
            }
            else if (map[y][x] == 9) {  // �ӵ� ���� ������
                mvwaddch(win, y, x, '>');
            }
            else if (map[y][x] == 10) {  // �ӵ� ���� ������
                mvwaddch(win, y, x, '<');
            }
        }
    }

    auto body = snake.getBody();
    for (auto& segment : body) {
        mvwaddch(win, segment.second, segment.first, 'O');
    }

    box(scoreWin, 0, 0);
    mvwprintw(scoreWin, 1, 1, "Score Board");
    mvwprintw(scoreWin, 2, 1, "B: %lu / %d", snake.getBody().size(), maxLength);
    mvwprintw(scoreWin, 3, 1, "+: %d", scoreGrowth);
    mvwprintw(scoreWin, 4, 1, "-: %d", scorePoison);
    mvwprintw(scoreWin, 5, 1, "G: %d", scoreGate);

    // �̼� �׵θ� �׸���
    box(timeWin, 0, 0);
    mvwprintw(timeWin, 1, 1, "Time: %02d:%02d", minutes, seconds);

    //draw �κ�
        box(missionWin, 0, 0);
    mvwprintw(missionWin, 1, 1, "Mission");
    mvwprintw(missionWin, 2, 1, "Pass the stage in 2 minutes");
    mvwprintw(missionWin, 3, 1, "B: %d (%c)", missionB, missionBComplete ? 'v' : ' ');
    mvwprintw(missionWin, 4, 1, "Max B: %d (%c)", missionMaxLength, missionMaxLengthComplete ? 'v' : ' ');
    mvwprintw(missionWin, 5, 1, "+: %d (%c)", missionGrowth, missionGrowthComplete ? 'v' : ' ');
    mvwprintw(missionWin, 6, 1, "-: %d (%c)", missionPoison, missionPoisonComplete ? 'v' : ' ');
    mvwprintw(missionWin, 7, 1, "G: %d (%c)", missionGate, missionGateComplete ? 'v' : ' ');


    wrefresh(win);
    wrefresh(scoreWin);
    wrefresh(missionWin);
    wrefresh(timeWin);
}

// ������ ��ġ
void Game::placeItems() {
    time_t now = time(0);

    // ���� ������ ��ġ
    if (growthItems.size() < 3) {
        int x, y;
        do {
            x = rand() % width;
            y = rand() % height;
        } while (map[y][x] != 0 || snake.isOccupying(x, y));
        map[y][x] = 5;
        growthItems.push_back({ x, y });
        growthItemTimes.push_back(now);
    }

    // �� ������ ��ġ
    if (poisonItems.size() < 3) {
        int x, y;
        do {
            x = rand() % width;
            y = rand() % height;
        } while (map[y][x] != 0 || snake.isOccupying(x, y));
        map[y][x] = 6;
        poisonItems.push_back({ x, y });
        poisonItemTimes.push_back(now);
    }

    // �ӵ� ���� ������ ��ġ
    if (speedUpItems.size() < 1) {
        int x, y;
        do {
            x = rand() % width;
            y = rand() % height;
        } while (map[y][x] != 0 || snake.isOccupying(x, y));
        map[y][x] = 9;
        speedUpItems.push_back({ x, y });
        speedUpItemTimes.push_back(now);
    }

    // �ӵ� ���� ������ ��ġ
    if (slowDownItems.size() < 1) {
        int x, y;
        do {
            x = rand() % width;
            y = rand() % height;
        } while (map[y][x] != 0 || snake.isOccupying(x, y));
        map[y][x] = 10;
        slowDownItems.push_back({ x, y });
        slowDownItemTimes.push_back(now);
    }

    // ����Ʈ ���� ���� üũ
    if (snake.getBody().size() >= 4 && !gateCreated) {
        do {
            gateA = { rand() % width, rand() % height };
            gateB = { rand() % width, rand() % height };
        } while (map[gateA.second][gateA.first] != 1 || map[gateB.second][gateB.first] != 1 || gateA == gateB);

        map[gateA.second][gateA.first] = 7;
        map[gateB.second][gateB.first] = 7;

        gateCreationTime = now;
        gateCreated = true;
    }
    // ����Ʈ ����� ���� üũ
    else if (gateCreated && difftime(now, gateCreationTime) >= 20) {
        if (!snake.isOccupying(gateA.first, gateA.second) && !snake.isOccupying(gateB.first, gateB.second)) {
            if (gateA.first != -1 && gateA.second != -1) {
                map[gateA.second][gateA.first] = 1; // ������ �ǵ���
            }
            if (gateB.first != -1 && gateB.second != -1) {
                map[gateB.second][gateB.first] = 1; // ������ �ǵ���
            }

            gateA = { -1, -1 };
            gateB = { -1, -1 };

            do {
                gateA = { rand() % width, rand() % height };
                gateB = { rand() % width, rand() % height };
            } while (map[gateA.second][gateA.first] != 1 || map[gateB.second][gateB.first] != 1 || gateA == gateB);

            map[gateA.second][gateA.first] = 7;
            map[gateB.second][gateB.first] = 7;

            gateCreationTime = now;
        }
    }
}

// ������ ������ ����
void Game::removeOldItems() {
    time_t now = time(0);

    // ������ ���� ������ ����
    for (size_t i = 0; i < growthItems.size(); ++i) {
        if (difftime(now, growthItemTimes[i]) >= 10) {
            map[growthItems[i].second][growthItems[i].first] = 0;
            growthItems.erase(growthItems.begin() + i);
            growthItemTimes.erase(growthItemTimes.begin() + i);
            --i;
        }
    }

    // ������ �� ������ ����
    for (size_t i = 0; i < poisonItems.size(); ++i) {
        if (difftime(now, poisonItemTimes[i]) >= 10) {
            map[poisonItems[i].second][poisonItems[i].first] = 0;
            poisonItems.erase(poisonItems.begin() + i);
            poisonItemTimes.erase(poisonItemTimes.begin() + i);
            --i;
        }
    }

    // ������ �ӵ� ���� ������ ����
    for (size_t i = 0; i < speedUpItems.size(); ++i) {
        if (difftime(now, speedUpItemTimes[i]) > 10) {
            map[speedUpItems[i].second][speedUpItems[i].first] = 0;
            speedUpItems.erase(speedUpItems.begin() + i);
            speedUpItemTimes.erase(speedUpItemTimes.begin() + i);
            --i;
        }
    }

    // ������ �ӵ� ���� ������ ����
    for (size_t i = 0; i < slowDownItems.size(); ++i) {
        if (difftime(now, slowDownItemTimes[i]) > 10) {
            map[slowDownItems[i].second][slowDownItems[i].first] = 0;
            slowDownItems.erase(slowDownItems.begin() + i);
            slowDownItemTimes.erase(slowDownItemTimes.begin() + i);
            --i;
        }
    }
}

// ���� ���� �ʱ�ȭ
void Game::reset() {
    if (currentStage == 4) {
        snake = Snake(width - 5, 1);  // ���� ������ ��ܿ��� �����ϵ��� ����
        snake.changeDirection(DOWN);  // ���� ������ �Ʒ��� ����
    }
    else {
        snake = Snake(width / 2, height / 2);  // �⺻ �� ���ġ
    }

    scoreGrowth = 0;
    maxLength = 0;
    scorePoison = 0;
    scoreGate = 0;
    missionBComplete = false;
    missionMaxLengthComplete = false;
    missionGrowthComplete = false;
    missionPoisonComplete = false;
    missionGateComplete = false;

    gateCreated = false;
    gateA = { -1, -1 };
    gateB = { -1, -1 };

    startTime = time(0);  // �������� ���� �ð� �ʱ�ȭ

    initializeMap();
    initializeStage();
    placeItems();
}

// �ٶ����� �ʱ�ȭ
void Game::initializeWindmill() {
    windmill.center = { width / 2, height / 2 };
    windmill.length = 5;
    windmill.state = 0;

    for (int i = 1; i <= windmill.length; ++i) {
        map[windmill.center.second + i][windmill.center.first] = 1;
        map[windmill.center.second - i][windmill.center.first] = 1;
    }
}

// �ٶ����� ȸ��
void Game::rotateWindmill() {
    static bool windmillPaused = false;
    static time_t pauseStart;

    if (windmillPaused) {
        if (difftime(time(0), pauseStart) >= 1) {  // 1�� ��� �� ȸ�� �簳
            windmillPaused = false;
        }
        else {
            return;  // ���� ��� ���̸� ȸ������ ����
        }
    }

    windmill.state = (windmill.state + 1) % 8;  // 8�� ���·� ��ȯ (45���� ȸ��)

    for (int i = 1; i <= windmill.length; ++i) {
        map[windmill.center.second + i][windmill.center.first] = 0;
        map[windmill.center.second - i][windmill.center.first] = 0;
        map[windmill.center.second][windmill.center.first + i] = 0;
        map[windmill.center.second][windmill.center.first - i] = 0;
        map[windmill.center.second + i][windmill.center.first + i] = 0;
        map[windmill.center.second - i][windmill.center.first - i] = 0;
        map[windmill.center.second + i][windmill.center.first - i] = 0;
        map[windmill.center.second - i][windmill.center.first + i] = 0;
    }

    switch (windmill.state) {
    case 0:  // 0��
        for (int i = 1; i <= windmill.length; ++i) {
            map[windmill.center.second + i][windmill.center.first] = 1;
            map[windmill.center.second - i][windmill.center.first] = 1;
        }
        break;
    case 1:  // 45��
        for (int i = 1; i <= windmill.length; ++i) {
            map[windmill.center.second + i][windmill.center.first + i] = 1;
            map[windmill.center.second - i][windmill.center.first - i] = 1;
        }
        break;
    case 2:  // 90��
        for (int i = 1; i <= windmill.length; ++i) {
            map[windmill.center.second][windmill.center.first + i] = 1;
            map[windmill.center.second][windmill.center.first - i] = 1;
        }
        break;
    case 3:  // 135��
        for (int i = 1; i <= windmill.length; ++i) {
            map[windmill.center.second + i][windmill.center.first - i] = 1;
            map[windmill.center.second - i][windmill.center.first + i] = 1;
        }
        break;
    case 4:  // 180��
        for (int i = 1; i <= windmill.length; ++i) {
            map[windmill.center.second - i][windmill.center.first] = 1;
            map[windmill.center.second + i][windmill.center.first] = 1;
        }
        break;
    case 5:  // 225��
        for (int i = 1; i <= windmill.length; ++i) {
            map[windmill.center.second - i][windmill.center.first - i] = 1;
            map[windmill.center.second + i][windmill.center.first + i] = 1;
        }
        break;
    case 6:  // 270��
        for (int i = 1; i <= windmill.length; ++i) {
            map[windmill.center.second][windmill.center.first - i] = 1;
            map[windmill.center.second][windmill.center.first + i] = 1;
        }
        break;
    case 7:  // 315��
        for (int i = 1; i <= windmill.length; ++i) {
            map[windmill.center.second - i][windmill.center.first + i] = 1;
            map[windmill.center.second + i][windmill.center.first - i] = 1;
        }
        break;
    }
}


// �ٶ������ ����Ʈ�� ��ġ���� Ȯ��
bool Game::isGateInWindmill(const std::pair<int, int>& gate) {
    int x = gate.first;
    int y = gate.second;
    int cx = windmill.center.first;
    int cy = windmill.center.second;
    int length = windmill.length;

    // �ٶ������� �� ��ġ�� ��ġ���� Ȯ��
    if ((x == cx && abs(y - cy) <= length) ||
        (y == cy && abs(x - cx) <= length) ||
        (abs(x - cx) <= length && abs(y - cy) <= length)) {
        return true;
    }
    return false;
}



