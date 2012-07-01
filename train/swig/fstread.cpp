/*
 * fstread.c
 *
 *  Created on: Jul 1, 2012
 *      Author: John Salatas <jsalatas@users.sourceforge.net>
 */


#include <fst/mutable-fst.h>
#include <fst/symbol-table.h>
#include "fstread.hpp"

using namespace fst;

FstRead::FstRead( char* model_file) {
	fst = StdMutableFst::Read(model_file);
	model_loaded = !(fst == NULL);
}
FstRead::~FstRead( ) {
	delete fst;
}

bool FstRead::modelLoaded() {
	return model_loaded;
}

int FstRead::numStates() {
	if (fst != NULL) {
		return fst->NumStates();
	}

	return -1;
}
int FstRead::numArcs(int state_id) {
	if (fst != NULL) {
		return fst->NumArcs(state_id);
	}

	return -1;
}

int FstRead::numInputSyms() {
	if (fst != NULL) {
		const SymbolTable* isyms = fst->InputSymbols();
		if (isyms != NULL) {
			return isyms->NumSymbols();
		}
	}
	return -1;
}

int FstRead::numOutputSyms() {
	if (fst != NULL) {
		const SymbolTable* osyms = fst->OutputSymbols();
		if (osyms != NULL) {
			return osyms->NumSymbols();
		}
	}
	return -1;
}

const char* FstRead::inputSym(int index) {
	if (fst != NULL) {
		const SymbolTable* isyms = fst->InputSymbols();
		if (isyms != NULL) {
			return isyms->Find(index).c_str();
		}
	}
	return "";
}

const char* FstRead::outputSym(int index) {
	if (fst != NULL) {
		const SymbolTable* osyms = fst->OutputSymbols();
		if (osyms != NULL) {
			return osyms->Find(index).c_str();
		}
	}
	return "";
}


double FstRead::finalWeight(int state_id) {
	return fst->Final(state_id).Value();
}

ExportedArc FstRead::getArc(int state_id, int arc_id) {
	ArcIterator<StdFst> aiter(*fst, state_id);
	aiter.Seek(arc_id);
	ExportedArc ea;
	ea.ilabel = aiter.Value().ilabel;
	ea.olabel = aiter.Value().olabel;
	ea.nextstate = aiter.Value().nextstate;
	return ea;
}

