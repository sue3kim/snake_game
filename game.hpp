#ifndef GAME_HPP
#define GAME_HPP

#include "snake.hpp"
#include <ncurses.h>
#include <vector>
#include <utility>
#include <iostream>
#include <ctime>

using namespace std;

// 바람개비 관련 구조체
struct Windmill {
    pair<int, int> center;  // 중심 좌표
    int length;  // 팔 길이
    int state;  // 현재 상태 (회전 각도)
};

extern Windmill windmill;  // Windmill 변수 선언

class Game {
public:
    Game(int width, int height);  // 생성자
    ~Game();  // 소멸자
    void run();  // 게임 실행
    void gameOver();  // 게임 오버 함수

private:
    void draw();  // 화면 그리기
    void update();  // 게임 상태 업데이트
    void processInput();  // 입력 처리
    void placeItems();  // 아이템 배치
    void initializeMap();  // 맵 초기화
    void initializeStage();  // 새로운 스테이지 초기화
    void reset();  // 게임 상태 초기화
    void removeOldItems();  // 오래된 아이템 제거
    void moveThroughGate(const std::pair<int, int>& exitGate);  // 게이트 통과
    void updateMissionStatus();  // 미션 상태 업데이트
    void nextStage();  // 다음 스테이지로 진행
    void initializeWindmill();  // 바람개비 초기화
    void rotateWindmill();  // 바람개비 회전
    bool isGateInWindmill(const std::pair<int, int>& gate);  // 게이트가 바람개비 내에 있는지 확인

    // 아이템 타이머와 위치 정보
    vector<pair<int, int>> growthItems;  // 성장 아이템 위치
    vector<pair<int, int>> poisonItems;  // 독 아이템 위치
    vector<time_t> growthItemTimes;  // 성장 아이템 생성 시간
    vector<time_t> poisonItemTimes;  // 독 아이템 생성 시간
    vector<pair<int, int>> speedUpItems;  // 속도 증가 아이템 위치
    vector<pair<int, int>> slowDownItems;  // 속도 감소 아이템 위치
    vector<time_t> speedUpItemTimes;  // 속도 증가 아이템 생성 시간
    vector<time_t> slowDownItemTimes;  // 속도 감소 아이템 생성 시간

    Snake snake;  // Snake 객체
    int width, height;  // 게임 화면 크기
    vector<vector<int>> map;  // 맵 데이터
    pair<int, int> gateA, gateB;  // 게이트 위치
    int scoreGrowth, scorePoison, scoreGate;  // 점수
    int maxLength;  // 최대 도달 길이
    int currentStage;  // 현재 스테이지
    int missionB, missionGrowth, missionPoison, missionGate;  // 미션 목표
    int missionMaxLength;  // 최대 도달 길이 미션 목표
    bool missionBComplete, missionGrowthComplete, missionPoisonComplete, missionGateComplete;  // 미션 완료 여부
    bool missionMaxLengthComplete;  // 최대 도달 길이 미션 완료 여부
    bool gateCreated;  // 게이트 생성 여부
    bool windmillPaused;  // 바람개비 일시 정지 여부
    time_t pauseStart;  // 바람개비 일시 정지 시작 시간

    time_t gateCreationTime;  // 게이트 생성 시간
    time_t startTime;  // 게임 시작 시간
    int gameSpeed;  // 게임 속도

    WINDOW* win;  // 게임 화면 윈도우
    WINDOW* scoreWin;  // 점수 화면 윈도우
    WINDOW* missionWin;  // 미션 화면 윈도우
    WINDOW* timeWin;  // 시간 화면 윈도우
};

#endif
