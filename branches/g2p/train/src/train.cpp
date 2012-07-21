/*
 * train.cpp
 *
 * Port of phonetisaurus training procedure to C++
 * using openGrm NGram language modeling toolkit
 * instead of MITLM.
 *
 * for more details about phonetisaurus see
 * http://code.google.com/p/phonetisaurus/
 * http://www.openfst.org/twiki/bin/view/GRM/NGramLibrary
 *
 *  Created on: May 22, 2012
 *      Author: John Salatas <jsalatas@users.sourceforge.net>
 */

#include <cstring>
#include <typeinfo>
#include <getopt.h>
#include <ngram/ngram-input.h>
#include <ngram/ngram-make.h>
#include <ngram/ngram-kneserney.h>
#include <ngram/ngram-absolute.h>
#include <ngram/ngram-katz.h>
#include <ngram/ngram-wittenbell.h>
#include <ngram/ngram-unsmoothed.h>
#include <fst/symbol-table.h>
#include <fst/extensions/far/farscript.h>
#include <fst/extensions/far/main.h>
#include <fst/script/print.h>
#include "phonetisaurus/M2MFstAligner.hpp"
#include "utils.hpp"

#define arc_type "standard"
#define fst_type "vector"
#define far_type "default"
#define entry_type "line"
#define token_type "symbol"
#define generate_keys 0
#define unknown_symbol ""
#define keep_symbols true
#define initial_symbols true
#define allow_negative_labels false
#define file_list_input false
#define key_prefix ""
#define key_suffix ""
#define backoff false
#define bins -1
#define backoff_label 0
#define norm_eps kNormEps
#define check_consistency false
#define discount_D -1
#define witten_bell_k 1
#define acceptor false
#define show_weight_one true
#define epsilon_as_backoff false

namespace fst {

typedef LogWeightTpl<double> Log64Weight;
typedef ArcTpl<Log64Weight> Log64Arc;
typedef int StateId;
template <class Arc>
struct ToLog64Mapper {
  typedef Arc FromArc;
  typedef Log64Arc ToArc;

  ToArc operator()(const FromArc &arc) const {
    return ToArc(arc.ilabel,
                 arc.olabel,
                 arc.weight.Value(),
                 arc.nextstate);
  }

  MapFinalAction FinalAction() const { return MAP_NO_SUPERFINAL; }
  MapSymbolsAction InputSymbolsAction() const { return MAP_COPY_SYMBOLS; }
  MapSymbolsAction OutputSymbolsAction() const { return MAP_COPY_SYMBOLS;}
  uint64 Properties(uint64 props) const { return props; }
};
}

using namespace std;
using namespace ngram;
using namespace fst;

void print_help(char* appname) {
	cout << "Usage: " << appname << " [--seq1_del] [--seq2_del] [--seq1_max SEQ1_MAX] [--seq2_max SEQ2_MAX]" << endl;
	cout << "               [--seq_sep SEQ_SEP] [--s1s2_sep S1S2_SEP] [--eps EPS] " << endl;
	cout << "               [--skip SKIP] [--seq1in_sep SEQ1IN_SEP] [--seq2in_sep SEQ2IN_SEP]" << endl;
	cout << "               [--s1s2_delim S1S2_DELIM] [--iter ITER] [--order ORDER] [--smooth SMOOTH] " << endl;
	cout << "               [--noalign] --ifile IFILE --ofile OFILE" << endl;
	cout << endl;
	cout << "  --seq1_del,              Allow deletions in sequence 1. Defaults to false." << endl;
	cout << "  --seq2_del,              Allow deletions in sequence 2. Defaults to false." << endl;
	cout << "  --seq1_max SEQ1_MAX,     Maximum subsequence length for sequence 1. Defaults to 2." << endl;
	cout << "  --seq2_max SEQ2_MAX,     Maximum subsequence length for sequence 2. Defaults to 2." << endl;
	cout << "  --seq_sep SEQ_SEP,       Separator token for sequences 1 and 2. Defaults to '|'." << endl;
	cout << "  --s1s2_sep S1S2_SEP,     Separator token for seq1 and seq2 alignments. Defaults to '}'." << endl;
	cout << "  --eps EPS,               Epsilon symbol.  Defaults to '<eps>'." << endl;
	cout << "  --skip SKIP,             Skip/null symbol.  Defaults to '_'." << endl;
	cout << "  --seq1in_sep SEQ1IN_SEP, Separator for seq1 in the input training file. Defaults to ''." << endl;
	cout << "  --seq2in_sep SEQ2IN_SEP, Separator for seq2 in the input training file. Defaults to ' '." << endl;
	cout << "  --s1s2_delim S1S2_DELIM, Separator for seq1/seq2 in the input training file. Defaults to '  '." << endl;
	cout << "  --iter ITER,             Maximum number of iterations for EM. Defaults to 10." << endl;
	cout << "  --order ORDER,           N-gram order. Defaults to 9." << endl;
	cout << "  --smooth SMOOTH,         Smoothing method. Available options are: " << endl;
	cout << "                           \"presmoothed\", \"unsmoothed\", \"kneser_ney\", \"absolute\", " << endl;
	cout << "                           \"katz\", \"witten_bell\". Defaults to \"kneser_ney\"." << endl;
	cout << "  --noalign,               Do not align. Assume that the aligned corpus already exists." << endl;
	cout << "                           Defaults to false." << endl;
	cout << "  --ifile IFILE,           File containing sequences to be aligned. " << endl;
	cout << "  --ofile OFILE,           Write the alignments to file." << endl;
}


void addarcs(StateId state_id, StateId newstate, const SymbolTable* oldsyms, SymbolTable* isyms,
		SymbolTable* osyms,	SymbolTable* ssyms,	string eps,	string s1s2_sep, StdMutableFst *fst,
		StdMutableFst *out) {
	for (ArcIterator<StdFst> aiter(*fst, state_id); !aiter.Done(); aiter.Next()) {
		StdArc arc = aiter.Value();
		string oldlabel = oldsyms->Find(arc.ilabel);
		if(oldlabel == eps) {
			oldlabel = oldlabel.append("}");
			oldlabel = oldlabel.append(eps);
		}
		vector<string> tokens;
		split_string(&oldlabel, &tokens, &s1s2_sep, true);
		int64 ilabel = isyms->AddSymbol(tokens.at(0));
		int64 olabel = osyms->AddSymbol(tokens.at(1));

		int64 nextstate = ssyms->Find(convertInt(arc.nextstate));
		if(nextstate  == -1 ) {
			out->AddState();
			ssyms->AddSymbol(convertInt(arc.nextstate));
			nextstate = ssyms->Find(convertInt(arc.nextstate));
		}
		out->AddArc(newstate, StdArc(ilabel, olabel, (arc.weight != TropicalWeight::Zero())?arc.weight:TropicalWeight::One(), nextstate));
		//out->AddArc(newstate, StdArc(ilabel, olabel, arc.weight, nextstate));
	}
}

void relabel(StdMutableFst *fst, StdMutableFst *out, string out_name, string eps, string skip, string s1s2_sep, string seq_sep) {
	namespace s = fst::script;
	using fst::ostream;
	using fst::SymbolTable;

	ArcSort(fst, StdILabelCompare());
	const SymbolTable *oldsyms = fst->InputSymbols();

	// Uncomment the next line in order to save the original model
	// as created by ngram
	// fst->Write("org.fst");

	// generate new input, output and states SymbolTables
	SymbolTable *ssyms = new SymbolTable("ssyms");
	SymbolTable *isyms = new SymbolTable("isyms");
	SymbolTable *osyms = new SymbolTable("osyms");

	out->AddState();
	ssyms->AddSymbol("s0");
	out->SetStart(0);

	out->AddState();
	ssyms->AddSymbol("s1");
	out->SetFinal(1, TropicalWeight::One());

	isyms->AddSymbol(eps);
	osyms->AddSymbol(eps);

	//Add separator, phi, start and end symbols
	isyms->AddSymbol(seq_sep);
	osyms->AddSymbol(seq_sep);
	isyms->AddSymbol("<phi>");
	osyms->AddSymbol("<phi>");
	int istart = isyms->AddSymbol("<s>");
	int iend = isyms->AddSymbol("</s>");
	int ostart = osyms->AddSymbol("<s>");
	int oend = osyms->AddSymbol("</s>");

	out->AddState();
	ssyms->AddSymbol("s2");
	out->AddArc(0, StdArc(istart, ostart, TropicalWeight::One(), 2));

	for (StateIterator<StdFst> siter(*fst); !siter.Done(); siter.Next()) {
		StateId state_id = siter.Value();

		int64 newstate;
		if (state_id == fst->Start()) {
			newstate = 2;
		} else {
			newstate = ssyms->Find(convertInt(state_id));
			if(newstate == -1 ) {
				out->AddState();
				ssyms->AddSymbol(convertInt(state_id));
				newstate = ssyms->Find(convertInt(state_id));
			}
		}

		TropicalWeight weight = fst->Final(state_id);

		if (weight != TropicalWeight::Zero()) {
			// this is a final state
			StdArc a = StdArc(iend, oend, weight, 1);
			out->AddArc(newstate, a);
			out->SetFinal(newstate, TropicalWeight::Zero());
		}
		addarcs(state_id, newstate, oldsyms, isyms, osyms, ssyms, eps, s1s2_sep, fst, out);
	}


	out->SetInputSymbols(isyms);
	out->SetOutputSymbols(osyms);
//	ArcSort(out, StdOLabelCompare());
//	ArcSort(out, StdILabelCompare());

	cout << "Writing text model to disk..." << endl;
	int index = out_name.find(".");
	string symsname;
	if(index != string::npos) {
		symsname = out_name.substr(0, index).append(".");
	} else {
		symsname = out_name.append(".");
	}
	//Save syms tables
	isyms->WriteText(symsname + "input.syms");
	osyms->WriteText(symsname + "output.syms");

	string dest = out_name.append(".txt");
	ostream *ostrm = new fst::ofstream(dest.c_str());
	ostrm->precision(9);
	s::FstClass *fstc = new s::FstClass(out);
	s::PrintFst(*fstc, *ostrm, dest, isyms, osyms, NULL, acceptor, show_weight_one);
	ostrm->flush();
	delete ostrm;
}

void train_model(string eps, string s1s2_sep, string skip, int order, string smooth, string out_name, string seq_sep) {
	// create symbols file
	cout << "Generating symbols..." << endl;
	NGramInput *ingram = new NGramInput(
		"corpus.aligned", "corpus.syms",
		"", eps, unknown_symbol, "", "");
	ingram->ReadInput(0, 1);

	// compile strings into a far archive
	cout << "Compiling symbols into FAR archive..." << endl;
	fst::FarEntryType fet = fst::StringToFarEntryType(entry_type);
	fst::FarTokenType ftt = fst::StringToFarTokenType(token_type);
    fst::FarType fartype = fst::FarTypeFromString(far_type);

    delete ingram;

    vector<string> in_fname;
    in_fname.push_back("corpus.aligned");

	fst::script::FarCompileStrings(in_fname, "corpus.far", arc_type, fst_type,
						fartype, generate_keys, fet, ftt,
	                       "corpus.syms", unknown_symbol,
	                       keep_symbols, initial_symbols,
	                       allow_negative_labels,
	                       file_list_input, key_prefix,
	                       key_suffix);

	//count n-grams
	cout << "Counting n-grams..." << endl;
	NGramCounter<Log64Weight> ngram_counter(order, epsilon_as_backoff);

	FstReadOptions opts;
	FarReader<StdArc>* far_reader;
	far_reader = FarReader<StdArc>::Open("corpus.far");
	int fstnumber = 1;
	const Fst<StdArc> *ifst = 0, *lfst = 0;
	while (!far_reader->Done()) {
		if (ifst)
			delete ifst;
		ifst = far_reader->GetFst().Copy();

		VLOG(1) << opts.source << "#" << fstnumber;
		if (!ifst) {
			LOG(ERROR) << "ngramcount: unable to read fst #" << fstnumber;
		exit(1);
		}

		bool counted = false;
	    if (ifst->Properties(kString | kUnweighted, true)) {
	        counted = ngram_counter.Count(*ifst);
	    } else {
	      VectorFst<Log64Arc> log_ifst;
	      Map(*ifst, &log_ifst, ToLog64Mapper<StdArc>());
	      counted = ngram_counter.Count(&log_ifst);
	    }
	    if (!counted)
	      LOG(ERROR) << "ngramcount: fst #" << fstnumber << " skipped";

	    if (ifst->InputSymbols() != 0) {  // retain for symbol table
	        if (lfst)
	    		delete lfst;  // delete previously observed symbol table
	        lfst = ifst;
	        ifst = 0;
	    }
	    far_reader->Next();
	    ++fstnumber;
	}
	delete far_reader;

	if (!lfst) {
		LOG(ERROR) << "None of the input FSTs had a symbol table";
		exit(1);
	}

	VectorFst<StdArc> vfst;
	ngram_counter.GetFst(&vfst);
	ArcSort(&vfst, StdILabelCompare());
	vfst.SetInputSymbols(lfst->InputSymbols());
	vfst.SetOutputSymbols(lfst->InputSymbols());
	vfst.Write("corpus.cnts");

	cout << "Smoothing model..." << endl;

	// convert to WFST model
	bool prefix_norm = 0;
	StdMutableFst* fst;
	if (smooth == "presmoothed") {  // only for use with randgen counts
		prefix_norm = 1;
		smooth = "unsmoothed";  // normalizes only based on prefix count
	}
	StdMutableFst *mfst = StdMutableFst::Read("corpus.cnts", true);
	if (smooth == "kneser_ney") {
		NGramKneserNey ngram(mfst, backoff, backoff_label,
				 norm_eps, check_consistency,
				 discount_D, bins);
		ngram.MakeNGramModel();
		fst = ngram.GetMutableFst();
	} else if (smooth == "absolute") {
		NGramAbsolute ngram(mfst, backoff, backoff_label,
				norm_eps, check_consistency,
				discount_D, bins);
		ngram.MakeNGramModel();
		fst = ngram.GetMutableFst();
	} else if (smooth == "katz") {
		NGramKatz ngram(mfst, backoff, backoff_label,
				norm_eps, check_consistency,
				bins);
		ngram.MakeNGramModel();
		fst = ngram.GetMutableFst();
	} else if (smooth == "witten_bell") {
		NGramWittenBell ngram(mfst, backoff, backoff_label,
				  norm_eps, check_consistency,
					  witten_bell_k);
			ngram.MakeNGramModel();
			fst = ngram.GetMutableFst();
	} else if (smooth == "unsmoothed") {
		NGramUnsmoothed ngram(mfst, 1, prefix_norm, backoff_label,
				  norm_eps, check_consistency);
		ngram.MakeNGramModel();
		fst = ngram.GetMutableFst();
	} else {
		LOG(ERROR) << "Bad smoothing method: " << smooth;
		exit(1);
	}
	cout << "Correcting final model..." << endl;
	StdMutableFst* out = new StdVectorFst();
	relabel(fst, out, out_name, eps, skip, s1s2_sep, seq_sep);

	cout << "Writing binary model to disk..." << endl;
	out->Write(out_name);
}


void align(string input_file, int seq1_del, int seq2_del, int seq1_max,
		int seq2_max, string seq_sep, string s1s2_sep,
		string eps, string skip, string seq1in_sep, string seq2in_sep,
		string s1s2_delim, int iter) {

	ifstream dict(input_file.c_str(), ifstream::in);

	ofstream ofile("corpus.aligned", ifstream::out);
	cout << "Loading..." << endl;
	M2MFstAligner fstaligner(seq1_del, seq2_del, seq1_max, seq2_max, seq_sep,
			seq_sep, s1s2_sep, eps, skip, true);


	string line;
	vector<string> tokens;
	vector<string> seq1;
	vector<string> seq2;
	if (dict.is_open()) {
		while (dict.good()) {
			getline(dict, line);
			if (line.empty())
				continue;
			if (line.substr(0, 3).compare(";;;") == 0)
				continue;
			tokens.clear();
			split_string(&line, &tokens, &s1s2_delim);
			seq1.clear();
			split_string(&tokens.at(0), &seq1, &seq1in_sep);
			seq2.clear();
			split_string(&tokens.at(1), &seq2, &seq2in_sep);
			fstaligner.entry2alignfst(seq1, seq2);
		}
	}
	dict.close();

	cout << "Starting EM..." << endl;
	int i = 1;
	float change;
	change = fstaligner.maximization(false);
	for (i = 1; i <= iter; i++) {
		fstaligner.expectation();
		change = fstaligner.maximization(false);
		cout << "Iteration " << i << " : " << change << endl;
	}
	fstaligner.expectation();
	change = fstaligner.maximization(true);
	cout << "Iteration " << i << " : " << change << endl;

	cout << "Generating best alignments..." << endl;
	for (int i = 0; i < fstaligner.fsas.size(); i++) {
		vector<PathData> paths = fstaligner.write_alignment(fstaligner.fsas[i],
				1);
		for (int k = 0; k < paths.size(); k++) {
			for (int j = 0; j < paths[k].path.size(); j++) {
				ofile << paths[k].path[j];
				//if (j < paths[k].path.size() - 1)
					ofile << " ";
			}
			ofile << endl;
		}
	}
}


int main(int argc, char* argv[]) {
	int seq1_del = 0;
	int seq2_del = 0;
	int noalign = 0;
	int seq1_max = 2;
	int seq2_max = 2;
	string seq_sep = "|";
	string s1s2_sep = "}";
	string eps = "<eps>";
	string skip = "_";
	string seq1in_sep = "";
	string seq2in_sep = " ";
	string s1s2_delim = "  ";
	int iter = 10;
	int order = 9;
	string smooth = "kneser_ney";
	string input_file = "";
	string output_file = "";

	static struct option long_options[] = {
			{ "seq1_del",	no_argument,		&seq1_del,	1 },
			{ "seq2_del",	no_argument, 		&seq2_del, 	1 },
			{ "noalign",	no_argument, 		&noalign, 	1 },
			{ "seq1_max",	required_argument,	NULL, 		'a' },
			{ "seq2_max",	required_argument,	NULL,		'b' },
			{ "seq_sep",	required_argument,	NULL,		'c' },
			{ "s1s2_sep",	required_argument,	NULL,		'd' },
			{ "eps",		required_argument,	NULL,		'e' },
			{ "skip", 		required_argument,	NULL,		'f' },
			{ "seq1in_sep", required_argument,	NULL,		'g' },
			{ "seq2in_sep",	required_argument,	NULL,		'h' },
			{ "s1s2_delim",	required_argument,	NULL,		'i' },
			{ "iter", 		required_argument,	NULL,		'j' },
			{ "order",		required_argument,	NULL,		'k' },
			{ "smooth",		required_argument,	NULL,		'l' },
			{ "ifile",		required_argument,	NULL,		'm' },
			{ "ofile",		required_argument,	NULL,		'n' },
			{ NULL, 		0, 					NULL, 		0 }
	};
	int option_index = 0;
	int c;

	while ((c = getopt_long(argc, argv, "a:b:c:d:e:f:g:h:i:j:k:l:m:n:", long_options, &option_index))
			!= -1) {
		switch (c) {
		case 0:
			/* If this option set a flag, do nothing else now. */
			if (long_options[option_index].flag != 0) {
				break;
			}
		case 'a':
			seq1_max = (atoi(optarg) == 0) ? 2 : atoi(optarg);
			break;
		case 'b':
			seq2_max = (atoi(optarg) == 0) ? 2 : atoi(optarg);
			break;
		case 'c':
			seq_sep = std::string(optarg);
			break;
		case 'd':
			s1s2_sep = std::string(optarg);
			break;
		case 'e':
			eps = std::string(optarg);
			break;
		case 'f':
			skip = std::string(optarg);
			break;
		case 'g':
			seq1in_sep = std::string(optarg);
			break;
		case 'h':
			seq2in_sep = std::string(optarg);
			break;
		case 'i':
			s1s2_delim = std::string(optarg);
			break;
		case 'j':
			iter = atoi(optarg);
			break;
		case 'k':
			order = atoi(optarg);
			break;
		case 'l':
			smooth = std::string(optarg);
			break;
		case 'm':
			input_file = std::string(optarg);
			break;
		case 'n':
			output_file = std::string(optarg);
			break;
		case '?':
			cout << "Cannot parse command line arguments." << endl;
			print_help(argv[0]);
			exit(0);
			break;
		}

	}

	if (input_file.empty() && noalign == 0) {
		cout << "Input file not provided" << endl;
		print_help(argv[0]);
		exit(0);
	}
	if (output_file.empty()) {
		cout << "Output file not provided" << endl;
		print_help(argv[0]);
		exit(0);
	}

	if (noalign == 0) {
		cout << "Using file: " << input_file << endl;
		align(input_file, seq1_del, seq2_del, seq1_max,
				seq2_max, seq_sep, s1s2_sep,
				eps, skip, seq1in_sep, seq2in_sep,
				s1s2_delim, iter);
	}

	train_model(eps, s1s2_sep, skip, order, smooth, output_file, seq_sep);

	return 0;
}
