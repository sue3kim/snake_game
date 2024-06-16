#ifndef SNAKE_HPP
#define SNAKE_HPP

#include <deque>
#include <utility>
#include <chrono>

using namespace std;

// 방향 열거형 - 뱀의 이동 방향을 정의합니다.
enum Direction { UP, DOWN, LEFT, RIGHT };

// Snake 클래스 정의 - 뱀 게임에서 뱀을 나타내는 클래스입니다.
class Snake {
public:
    // 생성자 - 뱀을 초기 위치에 생성합니다.
    Snake(int startX, int startY);

    // 뱀 상태 업데이트 - 뱀의 상태를 주기적으로 업데이트합니다.
    void update();

    // 뱀 이동 - 현재 방향으로 뱀을 이동시킵니다.
    void move();

    // 방향 변경 시도 - 뱀의 이동 방향을 변경합니다.
    void changeDirection(Direction newDirection);

    // 뱀의 몸체 반환 - 뱀의 몸체 좌표를 반환합니다.
    deque<pair<int, int>> getBody() const;

    // 뱀의 머리 반환 - 뱀의 머리 좌표를 반환합니다.
    pair<int, int> getHead() const;

    // 뱀 성장 - 뱀의 길이를 증가시킵니다.
    void grow();

    // 뱀 축소 - 뱀의 길이를 감소시킵니다.
    void shrink();

    // 충돌 검사 - 뱀이 자기 몸이나 벽과 충돌했는지 검사합니다.
    bool checkCollision() const;

    // 뱀 머리 설정 - 새로운 머리 위치와 방향을 설정합니다.
    void setHead(pair<int, int> newHead, Direction newDirection);

    // 특정 좌표를 차지하고 있는지 확인 - 뱀의 몸체가 특정 좌표를 차지하고 있는지 확인합니다.
    bool isOccupying(int x, int y) const;

    // 현재 방향 반환 - 뱀의 현재 이동 방향을 반환합니다.
    Direction getDirection() const;

    // 속도 증가 함수 - 뱀의 이동 속도를 증가시킵니다.
    void increaseSpeed();

    // 속도 감소 함수 - 뱀의 이동 속도를 감소시킵니다.
    void decreaseSpeed();

    // 이동 간격 설정 - 뱀의 이동 간격을 설정합니다.
    void setMoveInterval(float interval);

    // 현재 이동 간격 반환 - 뱀의 현재 이동 간격을 반환합니다.
    float getMoveInterval() const;

private:
    deque<pair<int, int>> body;  // 뱀의 몸체 좌표를 저장합니다.
    Direction direction;  // 뱀의 현재 이동 방향을 저장합니다.
    bool growPending;  // 뱀의 성장 여부를 저장하는 플래그입니다.
    chrono::time_point<chrono::steady_clock> lastMoveTime;  // 마지막 이동 시간을 저장합니다.
    float moveInterval;  // 뱀의 이동 간격(초)을 저장합니다.
    chrono::time_point<chrono::steady_clock> speedChangeTime;  // 속도 변경 시간을 저장합니다.
    bool speedIncreased;  // 속도 증가 여부를 저장하는 플래그입니다.
    bool speedDecreased;  // 속도 감소 여부를 저장하는 플래그입니다.
};

#endif
