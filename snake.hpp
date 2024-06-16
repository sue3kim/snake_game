#ifndef SNAKE_HPP
#define SNAKE_HPP

#include <deque>
#include <utility>
#include <chrono>

using namespace std;

// ���� ������ - ���� �̵� ������ �����մϴ�.
enum Direction { UP, DOWN, LEFT, RIGHT };

// Snake Ŭ���� ���� - �� ���ӿ��� ���� ��Ÿ���� Ŭ�����Դϴ�.
class Snake {
public:
    // ������ - ���� �ʱ� ��ġ�� �����մϴ�.
    Snake(int startX, int startY);

    // �� ���� ������Ʈ - ���� ���¸� �ֱ������� ������Ʈ�մϴ�.
    void update();

    // �� �̵� - ���� �������� ���� �̵���ŵ�ϴ�.
    void move();

    // ���� ���� �õ� - ���� �̵� ������ �����մϴ�.
    void changeDirection(Direction newDirection);

    // ���� ��ü ��ȯ - ���� ��ü ��ǥ�� ��ȯ�մϴ�.
    deque<pair<int, int>> getBody() const;

    // ���� �Ӹ� ��ȯ - ���� �Ӹ� ��ǥ�� ��ȯ�մϴ�.
    pair<int, int> getHead() const;

    // �� ���� - ���� ���̸� ������ŵ�ϴ�.
    void grow();

    // �� ��� - ���� ���̸� ���ҽ�ŵ�ϴ�.
    void shrink();

    // �浹 �˻� - ���� �ڱ� ���̳� ���� �浹�ߴ��� �˻��մϴ�.
    bool checkCollision() const;

    // �� �Ӹ� ���� - ���ο� �Ӹ� ��ġ�� ������ �����մϴ�.
    void setHead(pair<int, int> newHead, Direction newDirection);

    // Ư�� ��ǥ�� �����ϰ� �ִ��� Ȯ�� - ���� ��ü�� Ư�� ��ǥ�� �����ϰ� �ִ��� Ȯ���մϴ�.
    bool isOccupying(int x, int y) const;

    // ���� ���� ��ȯ - ���� ���� �̵� ������ ��ȯ�մϴ�.
    Direction getDirection() const;

    // �ӵ� ���� �Լ� - ���� �̵� �ӵ��� ������ŵ�ϴ�.
    void increaseSpeed();

    // �ӵ� ���� �Լ� - ���� �̵� �ӵ��� ���ҽ�ŵ�ϴ�.
    void decreaseSpeed();

    // �̵� ���� ���� - ���� �̵� ������ �����մϴ�.
    void setMoveInterval(float interval);

    // ���� �̵� ���� ��ȯ - ���� ���� �̵� ������ ��ȯ�մϴ�.
    float getMoveInterval() const;

private:
    deque<pair<int, int>> body;  // ���� ��ü ��ǥ�� �����մϴ�.
    Direction direction;  // ���� ���� �̵� ������ �����մϴ�.
    bool growPending;  // ���� ���� ���θ� �����ϴ� �÷����Դϴ�.
    chrono::time_point<chrono::steady_clock> lastMoveTime;  // ������ �̵� �ð��� �����մϴ�.
    float moveInterval;  // ���� �̵� ����(��)�� �����մϴ�.
    chrono::time_point<chrono::steady_clock> speedChangeTime;  // �ӵ� ���� �ð��� �����մϴ�.
    bool speedIncreased;  // �ӵ� ���� ���θ� �����ϴ� �÷����Դϴ�.
    bool speedDecreased;  // �ӵ� ���� ���θ� �����ϴ� �÷����Դϴ�.
};

#endif
