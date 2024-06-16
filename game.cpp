#include "game.hpp"
#include <cstdlib>
#include <ctime>
#include <algorithm>

Windmill windmill;

// Game 생성자: 게임 초기화 및 기본 설정
Game::Game(int width, int height)
    : snake(width / 2, height / 2), width(width), height(height),
    scoreGrowth(0), scorePoison(0), scoreGate(0), maxLength(0),
    currentStage(1), missionB(4), missionGrowth(1), missionPoison(1), missionGate(1), missionMaxLength(5),
    missionBComplete(false), missionGrowthComplete(false), missionPoisonComplete(false), missionGateComplete(false), missionMaxLengthComplete(false),
    gateCreated(false), gateCreationTime(time(0)), startTime(time(0)), gameSpeed(200) {

    initscr();  // ncurses 초기화
    cbreak();
    noecho();
    curs_set(0);  // 커서 숨김
    keypad(stdscr, TRUE);  // 키패드 입력 활성화
    timeout(gameSpeed);  // 초기 게임 속도 설정

    win = newwin(height, width, 0, 0);  // 게임 창 생성
    scoreWin = newwin(7, 30, 4, width + 2); // 스코어보드 창 생성
    missionWin = newwin(9, 30, 11, width + 2); // 미션 창 생성
    timeWin = newwin(3, 30, 1, width + 2); // 시간 창 생성

    srand(static_cast<unsigned int>(time(0)));  // 난수 생성기 시드 설정

    initializeMap();  // 맵 초기화
    initializeStage();  // 스테이지 초기화
    placeItems();  // 아이템 배치
}

// Game 소멸자: ncurses 종료
Game::~Game() {
    delwin(win);
    delwin(scoreWin);
    endwin();
}

// 게임 메인 루프
void Game::run() {
    while (true) {
        draw();  // 화면 그리기
        processInput();  // 입력 처리
        update();  // 상태 업데이트
    }
}

// 게임 오버 함수
void Game::gameOver() {
    delwin(win);
    delwin(scoreWin);
    endwin();  // ncurses 종료
    exit(0);  // 프로그램 종료
}

// 사용자 입력 처리
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

// 게이트 통과 함수
void Game::moveThroughGate(const std::pair<int, int>& exitGate) {
    auto exitX = exitGate.first;
    auto exitY = exitGate.second;
    std::pair<int, int> newHead = { -1, -1 };
    Direction newDirection;
    Direction entryDirection = snake.getDirection();
    std::vector<Direction> directions;

    // 진입 방향에 따른 진출 방향 우선순위 설정
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

    // 우선순위 방향에 따라 새로운 머리 위치와 방향 설정
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

    // 유효한 새로운 머리 위치가 없으면 게임 오버
    if (newHead == std::pair<int, int>{-1, -1}) {
        gameOver();
        return;
    }

    snake.setHead(newHead, newDirection);

    // 게이트가 바람개비 내에 있는 경우 바람개비 회전 멈춤
    if (isGateInWindmill(exitGate)) {
        windmillPaused = true;
        pauseStart = time(0);
    }
}

// 게임 상태 업데이트
void Game::update() {
    static int updateCounter = 0;
    snake.update();
    timeout(static_cast<int>(snake.getMoveInterval() * 1000));
    time_t currentTime = time(0);
    double elapsedSeconds = difftime(currentTime, startTime);

    // 게임 시간 초과 시 게임 오버
    if (elapsedSeconds > 120) {
        gameOver();
    }

    // 최대 길이 업데이트
    if (snake.getBody().size() > static_cast<size_t>(maxLength)) {
        maxLength = snake.getBody().size();
    }

    removeOldItems();  // 오래된 아이템 제거

    auto head = snake.getHead();

    // 벽이나 장애물 또는 자신과 충돌 시 게임 오버
    if (map[head.second][head.first] == 1 || map[head.second][head.first] == 2 || snake.checkCollision()) {
        gameOver();
    }

    // 바람개비 팔과의 충돌 검사
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

    // 성장 아이템 먹기
    if (map[head.second][head.first] == 5) {
        snake.grow();
        map[head.second][head.first] = 0;
        scoreGrowth++;
    }
    // 독 아이템 먹기
    else if (map[head.second][head.first] == 6) {
        snake.shrink();
        map[head.second][head.first] = 0;
        scorePoison++;
        if (snake.getBody().size() < 3) {
            gameOver();
        }
    }
    // 속도 증가 아이템 먹기
    else if (map[head.second][head.first] == 9) {
        snake.increaseSpeed();
        map[head.second][head.first] = 0;
    }
    // 속도 감소 아이템 먹기
    else if (map[head.second][head.first] == 10) {
        snake.decreaseSpeed();
        map[head.second][head.first] = 0;
    }

    // 게이트 통과
    if (head == gateA) {
        moveThroughGate(gateB);
        scoreGate++;
    }
    else if (head == gateB) {
        moveThroughGate(gateA);
        scoreGate++;
    }

    updateMissionStatus();  // 미션 상태 업데이트

    // 모든 미션 완료 시 다음 스테이지로 진행
    if (missionBComplete && missionGrowthComplete && missionPoisonComplete && missionGateComplete) {
        nextStage();
    }

    placeItems();  // 아이템 배치

    // 바람개비 회전 처리 (10번 업데이트마다 한 번씩 회전)
    if (currentStage == 4) {
        if (++updateCounter >= 10) {
            rotateWindmill();
            updateCounter = 0;
        }
    }

    timeout(static_cast<int>(snake.getMoveInterval() * 1000));  // 뱀의 이동 간격에 따라 timeout 설정
}

// 미션 상태 업데이트
void Game::updateMissionStatus() {
    missionBComplete = (snake.getBody().size() >= static_cast<size_t>(missionB));
    missionGrowthComplete = (scoreGrowth >= missionGrowth);
    missionPoisonComplete = (scorePoison >= missionPoison);
    missionGateComplete = (scoreGate >= missionGate);
    missionMaxLengthComplete = (maxLength >= missionMaxLength);  // 최대 도달 길이 미션 완료 조건 추가
}

// 다음 스테이지로 진행
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

// 초기 맵 설정
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

// 스테이지 초기화
void Game::initializeStage() {
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            if (map[y][x] != 1 && map[y][x] != 2) {
                map[y][x] = 0;
            }
        }
    }

    if (currentStage == 2) {
        int startX = 15;  // ㄱ자 가로선 시작 지점
        int startY = height - 15; // ㄱ자 세로선 시작 지점
        for (int i = startY; i < height - 5; ++i) {
            map[i][startX] = 1;  // ㄱ자 세로선
        }
        for (int i = 5; i < startX; ++i) {
            map[startY][i] = 1;  // ㄱ자 가로선
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
        snake = Snake(width - 5, 1);  // 뱀을 오른쪽 상단에서 시작하도록 설정
        snake.changeDirection(DOWN);  // 시작 방향을 아래로 설정
    }

    // 미션 난이도 조정
       missionB += 3;
       missionGrowth += 2;
       missionPoison += 1;
       missionGate += 1;
}

// 화면 그리기
void Game::draw() {
    werase(win);
    werase(scoreWin);
    werase(missionWin);
    werase(timeWin);

    // 스테이지 번호 표시
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
            else if (map[y][x] == 9) {  // 속도 증가 아이템
                mvwaddch(win, y, x, '>');
            }
            else if (map[y][x] == 10) {  // 속도 감소 아이템
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

    // 미션 테두리 그리기
    box(timeWin, 0, 0);
    mvwprintw(timeWin, 1, 1, "Time: %02d:%02d", minutes, seconds);

    //draw 부분
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

// 아이템 배치
void Game::placeItems() {
    time_t now = time(0);

    // 성장 아이템 배치
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

    // 독 아이템 배치
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

    // 속도 증가 아이템 배치
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

    // 속도 감소 아이템 배치
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

    // 게이트 생성 조건 체크
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
    // 게이트 재생성 조건 체크
    else if (gateCreated && difftime(now, gateCreationTime) >= 20) {
        if (!snake.isOccupying(gateA.first, gateA.second) && !snake.isOccupying(gateB.first, gateB.second)) {
            if (gateA.first != -1 && gateA.second != -1) {
                map[gateA.second][gateA.first] = 1; // 벽으로 되돌림
            }
            if (gateB.first != -1 && gateB.second != -1) {
                map[gateB.second][gateB.first] = 1; // 벽으로 되돌림
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

// 오래된 아이템 제거
void Game::removeOldItems() {
    time_t now = time(0);

    // 오래된 성장 아이템 제거
    for (size_t i = 0; i < growthItems.size(); ++i) {
        if (difftime(now, growthItemTimes[i]) >= 10) {
            map[growthItems[i].second][growthItems[i].first] = 0;
            growthItems.erase(growthItems.begin() + i);
            growthItemTimes.erase(growthItemTimes.begin() + i);
            --i;
        }
    }

    // 오래된 독 아이템 제거
    for (size_t i = 0; i < poisonItems.size(); ++i) {
        if (difftime(now, poisonItemTimes[i]) >= 10) {
            map[poisonItems[i].second][poisonItems[i].first] = 0;
            poisonItems.erase(poisonItems.begin() + i);
            poisonItemTimes.erase(poisonItemTimes.begin() + i);
            --i;
        }
    }

    // 오래된 속도 증가 아이템 제거
    for (size_t i = 0; i < speedUpItems.size(); ++i) {
        if (difftime(now, speedUpItemTimes[i]) > 10) {
            map[speedUpItems[i].second][speedUpItems[i].first] = 0;
            speedUpItems.erase(speedUpItems.begin() + i);
            speedUpItemTimes.erase(speedUpItemTimes.begin() + i);
            --i;
        }
    }

    // 오래된 속도 감소 아이템 제거
    for (size_t i = 0; i < slowDownItems.size(); ++i) {
        if (difftime(now, slowDownItemTimes[i]) > 10) {
            map[slowDownItems[i].second][slowDownItems[i].first] = 0;
            slowDownItems.erase(slowDownItems.begin() + i);
            slowDownItemTimes.erase(slowDownItemTimes.begin() + i);
            --i;
        }
    }
}

// 게임 상태 초기화
void Game::reset() {
    if (currentStage == 4) {
        snake = Snake(width - 5, 1);  // 뱀을 오른쪽 상단에서 시작하도록 설정
        snake.changeDirection(DOWN);  // 시작 방향을 아래로 설정
    }
    else {
        snake = Snake(width / 2, height / 2);  // 기본 뱀 재배치
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

    startTime = time(0);  // 스테이지 시작 시간 초기화

    initializeMap();
    initializeStage();
    placeItems();
}

// 바람개비 초기화
void Game::initializeWindmill() {
    windmill.center = { width / 2, height / 2 };
    windmill.length = 5;
    windmill.state = 0;

    for (int i = 1; i <= windmill.length; ++i) {
        map[windmill.center.second + i][windmill.center.first] = 1;
        map[windmill.center.second - i][windmill.center.first] = 1;
    }
}

// 바람개비 회전
void Game::rotateWindmill() {
    static bool windmillPaused = false;
    static time_t pauseStart;

    if (windmillPaused) {
        if (difftime(time(0), pauseStart) >= 1) {  // 1초 대기 후 회전 재개
            windmillPaused = false;
        }
        else {
            return;  // 아직 대기 중이면 회전하지 않음
        }
    }

    windmill.state = (windmill.state + 1) % 8;  // 8개 상태로 순환 (45도씩 회전)

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
    case 0:  // 0도
        for (int i = 1; i <= windmill.length; ++i) {
            map[windmill.center.second + i][windmill.center.first] = 1;
            map[windmill.center.second - i][windmill.center.first] = 1;
        }
        break;
    case 1:  // 45도
        for (int i = 1; i <= windmill.length; ++i) {
            map[windmill.center.second + i][windmill.center.first + i] = 1;
            map[windmill.center.second - i][windmill.center.first - i] = 1;
        }
        break;
    case 2:  // 90도
        for (int i = 1; i <= windmill.length; ++i) {
            map[windmill.center.second][windmill.center.first + i] = 1;
            map[windmill.center.second][windmill.center.first - i] = 1;
        }
        break;
    case 3:  // 135도
        for (int i = 1; i <= windmill.length; ++i) {
            map[windmill.center.second + i][windmill.center.first - i] = 1;
            map[windmill.center.second - i][windmill.center.first + i] = 1;
        }
        break;
    case 4:  // 180도
        for (int i = 1; i <= windmill.length; ++i) {
            map[windmill.center.second - i][windmill.center.first] = 1;
            map[windmill.center.second + i][windmill.center.first] = 1;
        }
        break;
    case 5:  // 225도
        for (int i = 1; i <= windmill.length; ++i) {
            map[windmill.center.second - i][windmill.center.first - i] = 1;
            map[windmill.center.second + i][windmill.center.first + i] = 1;
        }
        break;
    case 6:  // 270도
        for (int i = 1; i <= windmill.length; ++i) {
            map[windmill.center.second][windmill.center.first - i] = 1;
            map[windmill.center.second][windmill.center.first + i] = 1;
        }
        break;
    case 7:  // 315도
        for (int i = 1; i <= windmill.length; ++i) {
            map[windmill.center.second - i][windmill.center.first + i] = 1;
            map[windmill.center.second + i][windmill.center.first - i] = 1;
        }
        break;
    }
}


// 바람개비와 게이트가 겹치는지 확인
bool Game::isGateInWindmill(const std::pair<int, int>& gate) {
    int x = gate.first;
    int y = gate.second;
    int cx = windmill.center.first;
    int cy = windmill.center.second;
    int length = windmill.length;

    // 바람개비의 팔 위치와 겹치는지 확인
    if ((x == cx && abs(y - cy) <= length) ||
        (y == cy && abs(x - cx) <= length) ||
        (abs(x - cx) <= length && abs(y - cy) <= length)) {
        return true;
    }
    return false;
}



