%module fstreader
%{
#include "fstread.hpp"
%}

struct ExportedArc{
  int ilabel;
  int olabel;
  double weight;
  int nextstate;
};

class FstRead {
public:
    FstRead( char* model_file);
	bool modelLoaded();
	int numStates();
	int numArcs(int state_id);
	int numInputSyms();
	int numOutputSyms();
	const char* inputSym(int index);
	const char* outputSym(int index);
	double finalWeight(int state_id);
	ExportedArc FstRead::getArc(int state_id, int arc_id);
};
