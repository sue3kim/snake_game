# 컴파일러와 컴파일 옵션 설정
CXX = g++

CXXFLAGS = -Wall -Wextra -std=c++11

# ncurses 라이브러리 설정
LDFLAGS = -lncurses

# 타겟 실행 파일 이름
TARGET = snake

# 소스 파일
SRCS = main.cpp game.cpp snake.cpp

# 오브젝트 파일
OBJS = $(SRCS:.cpp=.o)

# 기본 타겟
all: $(TARGET)

# 실행 파일 생성 규칙
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) $(LDFLAGS)

# 오브젝트 파일 생성 규칙
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# clean 타겟: 생성된 파일들을 삭제
clean:
	rm -f $(TARGET) $(OBJS)

.PHONY: all clean
