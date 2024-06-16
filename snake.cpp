#include "snake.hpp"
#include <ncurses.h>
#include <iostream>  // std::cout, std::endl을 사용하기 위함

// Snake 생성자: 초기 위치 설정
Snake::Snake(int startX, int startY)
    : direction(RIGHT), growPending(false), lastMoveTime(std::chrono::steady_clock::now()),
    moveInterval(0.2f), speedIncreased(false), speedDecreased(false) {
    body.push_back({ startX, startY });
    body.push_back({ startX - 1, startY });
    body.push_back({ startX - 2, startY });
}

// 상태 업데이트: 이동 시간에 따라 이동 결정
void Snake::update() {
    auto now = std::chrono::steady_clock::now();
    std::chrono::duration<float> elapsed = now - lastMoveTime;

    // 속도 증가 또는 감소 상태에서 일정 시간이 지나면 원래 속도로 복구
    if (speedIncreased || speedDecreased) {
        std::chrono::duration<float> speedElapsed = now - speedChangeTime;
        if (speedElapsed.count() >= 5.0f) {  // 5초 후 원래 속도로 복구
            moveInterval = 0.2f;
            speedIncreased = false;
            speedDecreased = false;
        }
    }

    // 이동 시간이 지나면 이동
    if (elapsed.count() >= moveInterval) {
        move();
        lastMoveTime = now;
    }
}

// 속도 증가 함수
void Snake::increaseSpeed() { //클래스: 
    moveInterval = 0.1f;  // 이동 간격을 줄여 속도 증가
    speedChangeTime = std::chrono::steady_clock::now();
    speedIncreased = true;
    speedDecreased = false;
}

// 속도 감소 함수
void Snake::decreaseSpeed() {
    moveInterval = 0.4f;  // 이동 간격을 늘려 속도 감소
    speedChangeTime = std::chrono::steady_clock::now();
    speedIncreased = false;
    speedDecreased = true;
}

// 뱀 이동
void Snake::move() {
    auto head = body.front();  // 현재 머리 위치를 복사
    switch (direction) {
    case UP: head.second--; break;
    case DOWN: head.second++; break;
    case LEFT: head.first--; break;
    case RIGHT: head.first++; break;
    }
    body.push_front(head);  // 새로운 위치에 머리 추가

    // 성장 상태를 확인하여 꼬리를 제거할지 결정
    if (!growPending) {
        body.pop_back();  // 꼬리 제거
    }
    else {
        growPending = false;
    }
}

// 방향 변경 시도
void Snake::changeDirection(Direction newDirection) {
    // 같은 방향 통제 및 반대 방향으로 이동하려고 할 때 무시
    if (direction == newDirection) return;
    if ((direction == UP && newDirection == DOWN) ||
        (direction == DOWN && newDirection == UP) ||
        (direction == LEFT && newDirection == RIGHT) ||
        (direction == RIGHT && newDirection == LEFT)) {
        endwin();
        exit(0);
    }
    direction = newDirection;  // 다음 방향 업데이트
}

// 뱀의 몸체 반환
std::deque<std::pair<int, int>> Snake::getBody() const {
    return body;
}

// 뱀의 머리 반환
std::pair<int, int> Snake::getHead() const {
    return body.front();
}

// 뱀 성장: 꼬리에 새로운 세그먼트 추가
void Snake::grow() {
    growPending = true;  // 성장 상태 설정
}

// 뱀 축소: 꼬리 제거
void Snake::shrink() {
    if (body.size() > 1) {
        body.pop_back();
    }
}

// 충돌 검사: 머리가 몸체와 충돌하는지 확인
bool Snake::checkCollision() const {
    auto& head = body.front();
    for (auto it = ++body.begin(); it != body.end(); ++it) {
        if (*it == head) {
            return true;
        }
    }
    return false;
}

// 새로운 머리 위치 및 방향 설정
void Snake::setHead(std::pair<int, int> newHead, Direction newDirection) {
    body.push_front(newHead);
    direction = newDirection;
    if (!growPending) {
        body.pop_back();
    }
    else {
        growPending = false;
    }
}

// 특정 좌표를 차지하고 있는지 확인
bool Snake::isOccupying(int x, int y) const {
    for (const auto& segment : body) {
        if (segment.first == x && segment.second == y) {
            return true;
        }
    }
    return false;
}

// 현재 방향 반환
Direction Snake::getDirection() const {
    return direction;
}

// 현재 이동 간격 반환
float Snake::getMoveInterval() const {
    return moveInterval;
}

// 이동 간격 설정
void Snake::setMoveInterval(float interval) {
    moveInterval = interval;
}
