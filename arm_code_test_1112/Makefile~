TOP_DIR := $(shell pwd)

#编译工具
CROSS=arm-linux-

CC=$(CROSS)gcc
CXX=$(CROSS)g++
LINK=$(CROSS)g++ -o
LIBRARY_LINK=$(CROSS)ar cr

OUT=$(TOP_DIR)/OUT
SRCDIR=$(TOP_DIR)/src

#库路径
LIBAVDIR=/home/summer/friendlyARM/ffmpeg
LIBX264DIR=/home/summer/friendlyARM/x264
LIBJRTPDIR=/home/summer/friendlyARM/jrtp
LIBJTHREAD=/home/summer/friendlyARM/jthread


#头文件
INCLUDE= -I$(LIBAVDIR)/include -I$(TOP_DIR)/src -I$(LIBX264DIR)/include -I$(LIBJRTPDIR)/include -I$(LIBJTHREAD)/include

#库文件
LIBDIR= -L$(LIBAVDIR)/lib -L$(LIBX264DIR)/lib -L$(LIBJRTPDIR)/lib  -L$(LIBJTHREAD)/lib 
LIBS =  -lavutil -lswscale -lx264 -ljrtp -lpthread -ldl -ljthread
LDFLAGS=$(LIBDIR) $(LIBS)

#编译选项
CXXFLAGS=
CXXFLAGS= -g $(INCLUDE)
CFLAGS=
CFLAGS=-g $(INCLUDE)

#编译目标 ·target·
all: test

$(OUT)/%.o:$(SRCDIR)/%.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS) $(LDFLAGS)
$(OUT)/%.o:$(SRCDIR)/%.c
	$(CC) -c $< -o $@ $(CFLAGS) $(LDFLAGS)

CPPOBJS =$(OUT)/capture.o $(OUT)/yuv422_to_I420.o $(OUT)/encoder.o $(OUT)/jrtpsend.o 
$(SRCDIR)/code.cpp:$(SRCDIR)/capture.h
$(SRCDIR)/yuv422_to_I420.cpp:$(SRCDIR)/yuv422_to_I420.h
$(SRCDIR)/encoder.cpp:$(SRCDIR)/encoder.h
$(SRCDIR)/jrtpsend.cpp:$(SRCDIR)/jrtpsend.h

test: $(OUT)/main.o $(CPPOBJS) 
		$(LINK) $@ $< $(CPPOBJS) $(CXXFLAGS) $(LDFLAGS)
$(OUT)/main.o: $(SRCDIR)/main.cpp 
	$(CXX) -c $< -o $@ $(CXXFLAGS) $(LDFLAGS)

clean: 
	rm $(OUT)/* 
