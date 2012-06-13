SRC = src/train.cpp \
      src/phonetisaurus/M2MFstAligner.cpp \
      src/phonetisaurus/FstPathFinder.cpp 
OBJS = src/train.o  \
      src/phonetisaurus/M2MFstAligner.o \
      src/phonetisaurus/FstPathFinder.o 

LDLIBS = -L/usr/local/lib64/fst -lfst -lfstfar -lfstfarscript -ldl -lngram

CFLAGS = -c -g
LDFLAGS = -g

all: train

train: $(OBJS)
	g++ ${LDFLAGS} ${LDLIBS} -o train ${OBJS} 
	
%.o: %.cpp
	g++ ${CFLAGS} -o $@ $<
	
clean: 
	rm train src/*.o src/phonetisaurus/*.o

