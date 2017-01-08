#-----------------------------------------------------------
#
#
#
#
#-----------------------------------------------------------


SHELL = /bin/sh


#..........................................................
# COMPILER VARIABLES
#..........................................................

CC 	= gcc -O2
CXX	= g++ -O2
CDEBUG	=-g -DDEBUG
CFLAGS	=$(CDEBUG) -c -Wall -I. -fPIC
LDFLAGS	=-shared
LIBS	=

#..........................................................
# PROJECT
#..........................................................

srcdir = .

SRCSC  	= 
SRCSCXX	= I2cDevice.cpp \
	SpiDevice.cpp \
	FrameBuffer_PiSenseHat.cpp 
OBJSC  	= $(SRCSC:.c=.o)
OBJSCXX	= $(SRCSCXX:.cpp=.o)
TARGET_LIB=libnetbridge.so


#..........................................................
# TARGETS
#..........................................................

.PHONY:all
all:$(TARGET_LIB)  

$(TARGET_LIB):$(OBJSCXX) 
	$(CXX) ${LDFLAGS} -o $@ $^

%.o:%.cpp
	$(CXX) ${CFLAGS} $^

install:
	sudo cp -f ${TARGET_LIB} /usr/local/lib

indent:
	astyle -s3 -S -Y -f -p -H -y -k2 -n -v -z2 *.cpp	
	astyle -s3 -S -Y -f -p -H -y -k2 -n -v -z2 *.h
	rm -f *.o *~

.PHONY:clean
clean:
	rm -f *.o *~
	rm -f ${TARGET_LIB}






