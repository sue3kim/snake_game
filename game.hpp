#ifndef GAME_HPP
#define GAME_HPP

#include "snake.hpp"
#include <ncurses.h>
#include <vector>
#include <utility>
#include <iostream>
#include <ctime>

using namespace std;

// �ٶ����� ���� ����ü
struct Windmill {
    pair<int, int> center;  // �߽� ��ǥ
    int length;  // �� ����
    int state;  // ���� ���� (ȸ�� ����)
};

extern Windmill windmill;  // Windmill ���� ����

class Game {
public:
    Game(int width, int height);  // ������
    ~Game();  // �Ҹ���
    void run();  // ���� ����
    void gameOver();  // ���� ���� �Լ�

private:
    void draw();  // ȭ�� �׸���
    void update();  // ���� ���� ������Ʈ
    void processInput();  // �Է� ó��
    void placeItems();  // ������ ��ġ
    void initializeMap();  // �� �ʱ�ȭ
    void initializeStage();  // ���ο� �������� �ʱ�ȭ
    void reset();  // ���� ���� �ʱ�ȭ
    void removeOldItems();  // ������ ������ ����
    void moveThroughGate(const std::pair<int, int>& exitGate);  // ����Ʈ ���
    void updateMissionStatus();  // �̼� ���� ������Ʈ
    void nextStage();  // ���� ���������� ����
    void initializeWindmill();  // �ٶ����� �ʱ�ȭ
    void rotateWindmill();  // �ٶ����� ȸ��
    bool isGateInWindmill(const std::pair<int, int>& gate);  // ����Ʈ�� �ٶ����� ���� �ִ��� Ȯ��

    // ������ Ÿ�̸ӿ� ��ġ ����
    vector<pair<int, int>> growthItems;  // ���� ������ ��ġ
    vector<pair<int, int>> poisonItems;  // �� ������ ��ġ
    vector<time_t> growthItemTimes;  // ���� ������ ���� �ð�
    vector<time_t> poisonItemTimes;  // �� ������ ���� �ð�
    vector<pair<int, int>> speedUpItems;  // �ӵ� ���� ������ ��ġ
    vector<pair<int, int>> slowDownItems;  // �ӵ� ���� ������ ��ġ
    vector<time_t> speedUpItemTimes;  // �ӵ� ���� ������ ���� �ð�
    vector<time_t> slowDownItemTimes;  // �ӵ� ���� ������ ���� �ð�

    Snake snake;  // Snake ��ü
    int width, height;  // ���� ȭ�� ũ��
    vector<vector<int>> map;  // �� ������
    pair<int, int> gateA, gateB;  // ����Ʈ ��ġ
    int scoreGrowth, scorePoison, scoreGate;  // ����
    int maxLength;  // �ִ� ���� ����
    int currentStage;  // ���� ��������
    int missionB, missionGrowth, missionPoison, missionGate;  // �̼� ��ǥ
    int missionMaxLength;  // �ִ� ���� ���� �̼� ��ǥ
    bool missionBComplete, missionGrowthComplete, missionPoisonComplete, missionGateComplete;  // �̼� �Ϸ� ����
    bool missionMaxLengthComplete;  // �ִ� ���� ���� �̼� �Ϸ� ����
    bool gateCreated;  // ����Ʈ ���� ����
    bool windmillPaused;  // �ٶ����� �Ͻ� ���� ����
    time_t pauseStart;  // �ٶ����� �Ͻ� ���� ���� �ð�

    time_t gateCreationTime;  // ����Ʈ ���� �ð�
    time_t startTime;  // ���� ���� �ð�
    int gameSpeed;  // ���� �ӵ�

    WINDOW* win;  // ���� ȭ�� ������
    WINDOW* scoreWin;  // ���� ȭ�� ������
    WINDOW* missionWin;  // �̼� ȭ�� ������
    WINDOW* timeWin;  // �ð� ȭ�� ������
};

#endif
