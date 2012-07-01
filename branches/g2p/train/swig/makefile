CFLAGS := -c -fpic
CPPFLAGS := -I /usr/java/default/include/ -I /usr/java/default/include/linux 
LDLIBS = -L/usr/local/lib64/fst -L/usr/local/lib/fst -lfst

all: libfstreader_jni.so jar

jar: fstreader_wrap.cxx
	ant jar

libfstreader_jni.so: fstreader_wrap.o
	g++ $(LDLIBS) -shared fstread.o fstreader_wrap.o -o libfstreader.so

fstreader_wrap.o: fstreader_wrap.cxx  fstread.o
	g++ $(CFLAGS) $(CPPFLAGS) fstreader_wrap.cxx 

fstread.o: fstread.cpp
	g++ $(CFLAGS) fstread.cpp 

fstreader_wrap.cxx: fstreader.i
	mkdir -p edu/cmu/sphinx/fst/swig 
	swig -c++ -java -package edu.cmu.sphinx.fst.swig -outdir edu/cmu/sphinx/fst/swig fstreader.i

clean:
	rm -f *.so *_wrap.cxx *.o *.jar edu/cmu/sphinx/fst/swig/* edu/cmu/sphinx/fst/openfst/*.class
	rmdir edu/cmu/sphinx/fst/swig
