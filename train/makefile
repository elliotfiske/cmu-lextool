TRAIN_OBJS = src/train.o  \
      src/phonetisaurus/M2MFstAligner.o \
      src/phonetisaurus/FstPathFinder.o 

EVALUATE_OBJS = src/evaluate.o  \
				src/g2p.o  

LDLIBS = -L/usr/local/lib64/fst -L/usr/local/lib/fst -lfst -lfstfar -lfstfarscript -lfstscript -ldl -lngram

CFLAGS = -c -g -I src/include
LDFLAGS = -g

all: train 

train: $(TRAIN_OBJS)
	g++ ${LDFLAGS} ${LDLIBS} -o train ${TRAIN_OBJS} 
	
evaluate: $(EVALUATE_OBJS)
	g++ ${LDFLAGS} ${LDLIBS} -o evaluate ${EVALUATE_OBJS} 
	
%.o: %.cpp
	g++ ${CFLAGS} -o $@ $<

clean: 
	rm train evaluate src/*.o src/phonetisaurus/*.o
