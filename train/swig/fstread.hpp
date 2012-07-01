/*
 * fstread.hpp
 *
 *  Created on: Jul 1, 2012
 *      Author: John Salatas <jsalatas@users.sourceforge.net>
 */

#ifndef FSTREAD_HPP_
#define FSTREAD_HPP_

#include <fst/fstlib.h>

using namespace fst;

struct ExportedArc{
  int ilabel;
  int olabel;
  double weight;
  int nextstate;
};

class FstRead {
    /*
     Utility class to export FSTs to other languages using swig
    */
public:
//	FstRead( );
	FstRead( char* model_file);
	~FstRead( );
	bool modelLoaded();
	int numStates();
	int numArcs(int state_id);
	int numInputSyms();
	int numOutputSyms();
	const char* inputSym(int index);
	const char* outputSym(int index);
	double finalWeight(int state_id);
	ExportedArc getArc(int state_id, int arc_id);


private:
    StdMutableFst* fst;
    bool model_loaded;
};



#endif /* FSTREAD_HPP_ */
