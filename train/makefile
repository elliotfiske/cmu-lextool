SRC = src/align.cpp \
      src/phonetisaurus/M2MFstAligner.cpp \
      src/phonetisaurus/FstPathFinder.cpp 
OBJS = src/align.o \
      src/phonetisaurus/M2MFstAligner.o \
      src/phonetisaurus/FstPathFinder.o 

LDLIBS = -l fst -l dl

CFLAGS = -c -g
LDFLAGS = -g

all: align  

align: $(OBJS)
	g++ ${LDFLAGS} ${LDLIBS} -o src/align ${OBJS}  
	
	
%.o: %.cpp
	g++ ${CFLAGS} -o $@ $<
	
clean: 
	rm src/align src/*.o src/phonetisaurus/*.o

