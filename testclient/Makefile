###### c++编译静态lib或可执行文件通用Makefile ######

## 一般不需要修改的编译选项配置 ##
#c编译器
CC = gcc
#c++编译器
CXX = g++
#c编译选项
CFLAGS = -g -O0 -Wall
#c++编译选项
CXXFLAGS = -g -O0 -Wall -fno-strict-aliasing -Wno-write-strings -frtti -fexceptions -DDEBUG -std=c++11
#静态库打包工具
AR = ar -r
#非空
YES = X

## 可能需要修改的文件配置 ##
#库文件目录
LIBS = -L/usr/lib -L../common 
#具体库文件
LIBS += -lstdc++ -lcommon
#c++头文件目录
INCPATH = -I/usr/include -I../common
#目标文件
TARGET = test
#依赖文件
OBJECTS = testclient.o

all: $(TARGET)

$(TARGET): $(OBJECTS)
	@if [ $(YES) = $(findstring lib, $(TARGET))$(YES) ]; then \
		echo $(LIBS);\
		$(CXX) $(CXXFLAGS) $(INCPATH) -o $@ $< $(LIBS); \
	else \
		$(AR) $(TARGET) $(OBJECTS); \
	fi
	
## 隐式规则 ##
.SUFFIXES: .cpp .cc .cxx .c
.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $< $(LIBS)

.cc.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $< $(LIBS)

.cxx.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $< $(LIBS)

.c.o:
	$(CC) -c $(CFLAGS) $(INCPATH) -o $@ $< $(LIBS)

## 伪目标 ##
.PHONY : clean
clean:
	$(RM) *.o $(TARGET)
