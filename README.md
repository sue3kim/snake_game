:: Snake Game ::

이 프로젝트는 C++과 'ncurses' 라이브러리를 사용하여 구현한 스네이크 게임 프로젝트입니다.


:: 설치 및 실행 방법::

1-1. 코드 다운로드

첨부파일을 활용해 코드 파일을 직접 다운로드
cd snake_game(cd 명령어를 사용해 코드가 다운받아진 해당 경로로 이동)

1-2. 리포지토리 클론

git clone https://github.com/sue3kim/snake_game.git
cd snake_game(cd 명령어를 사용해 코드가 다운받아진 해당 경로로 이동)


2. ncurses 라이브러리 설치
   
이 게임은 ncurses 라이브러리를 사용합니다. ncurses 라이브러리를 설치하기 위해 각 운영 체제에 맞는 명령어를 사용하여 설치합니다.

Ubuntu/Debian:
sudo apt-get update
sudo apt-get install libncurses5-dev libncursesw5-dev

Fedora:
sudo dnf install ncurses-devel

macOS:
brew install ncurses


3. 게임 컴파일 및 실행

제공된 Makefile을 사용하여 게임을 컴파일한 후 실행합니다.
다음 명령어를 입력해 게임을 컴파일 및 실행합니다.
   
make
./snake_game

3-1. 클린업(옵션)

컴파일된 파일을 제거하고 디렉토리를 정리하기 위해서는 다음 명령어를 실행합니다.

make clean


**게임 방법**

방향키를 사용하여 스네이크의 방향을 조작합니다.
아이템을 수집하여 스네이크를 더 길게 만듭니다.
벽이나 자신의 몸에 충돌하지 않도록 주의하세요.
게이트를 통과하여 다음 레벨로 진행합니다.
