/*
 * align.cpp
 *
 * Port of phonetisaurus m2m-aligner python script to C++
 *
 * for more details about phonetisaurus see
 * http://code.google.com/p/phonetisaurus/
 *
 *  Created on: May 22, 2012
 *      Author: John Salatas <jsalatas@users.sourceforge.net>
 */

#include <cstring>
#include "phonetisaurus/M2MFstAligner.hpp"
#include <getopt.h>

void print_help(char* appname) {
	cout << "Usage: " << appname << " [--seq1_del] [--seq2_del] [--seq1_max SEQ1_MAX] [--seq2_max SEQ2_MAX]" << endl;
	cout << "               [--seq1_sep SEQ1_SEP] [--seq2_sep SEQ2_SEP] [--s1s2_sep S1S2_SEP] " << endl;
	cout << "               [--eps EPS] [--skip SKIP] [--seq1in_sep SEQ1IN_SEP] [--seq2in_sep SEQ2IN_SEP]" << endl;
	cout << "               [--s1s2_delim S1S2_DELIM] [--iter ITER] --ifile IFILE --ofile OFILE" << endl;
	cout << endl;
	cout << "  --seq1_del,              Allow deletions in sequence 1. Defaults to false." << endl;
	cout << "  --seq2_del,              Allow deletions in sequence 2. Defaults to false." << endl;
	cout << "  --seq1_max SEQ1_MAX,     Maximum subsequence length for sequence 1. Defaults to 2." << endl;
	cout << "  --seq2_max SEQ2_MAX,     Maximum subsequence length for sequence 2. Defaults to 2." << endl;
	cout << "  --seq1_sep SEQ1_SEP,     Separator token for sequence 1. Defaults to '|'." << endl;
	cout << "  --seq2_sep SEQ2_SEP,     Separator token for sequence 2. Defaults to '|'." << endl;
	cout << "  --s1s2_sep S1S2_SEP,     Separator token for seq1 and seq2 alignments. Defaults to '}'." << endl;
	cout << "  --eps EPS,               Epsilon symbol.  Defaults to '<eps>'." << endl;
	cout << "  --skip SKIP,             Skip/null symbol.  Defaults to '_'." << endl;
	cout << "  --seq1in_sep SEQ1IN_SEP, Separator for seq1 in the input training file. Defaults to ''." << endl;
	cout << "  --seq2in_sep SEQ2IN_SEP, Separator for seq2 in the input training file. Defaults to ' '." << endl;
	cout << "  --s1s2_delim S1S2_DELIM, Separator for seq1/seq2 in the input training file. Defaults to '  '." << endl;
	cout << "  --iter ITER,             Maximum number of iterations for EM. Defaults to 10." << endl;
	cout << "  --ifile IFILE,           File containing sequences to be aligned. " << endl;
	cout << "  --ofile OFILE,           Write the alignments to file." << endl;

}

void split_string(string* input, vector<string>* tokens, string* delim) {
	size_t start = 0;
	size_t len = 0;
	size_t pos = 0;

	while (start < input->size()) {
		if (delim->empty()) {
			len = 1;
		} else {
			pos = input->find(*delim, start);
			if (pos != string::npos) {
				len = pos - start;
			} else {
				len = input->size() - start;
			}
		}
		tokens->push_back(input->substr(start, len));
		if (delim->empty()) {
			start = start + len;
		} else {
			start = start + len + delim->size();
		}
	}
}

int main(int argc, char* argv[]) {
	int seq1_del = 0;
	int seq2_del = 0;
	int seq1_max = 2;
	int seq2_max = 2;
	string seq1_sep = "|";
	string seq2_sep = "|";
	string s1s2_sep = "}";
	string eps = "<eps>";
	string skip = "_";
	string seq1in_sep = "";
	string seq2in_sep = " ";
	string s1s2_delim = "  ";
	int iter = 10;
	string input_file = "";
	string output_file = "";

	static struct option long_options[] = {
			{ "seq1_del",	no_argument,		&seq1_del,	1 },
			{ "seq2_del",	no_argument, 		&seq2_del, 	1 },
			{"seq1_max",	required_argument,	NULL, 		'a' },
			{ "seq2_max",	required_argument,	NULL,		'b' },
			{ "seq1_sep",	required_argument,	NULL,		'c' },
			{ "seq2_sep",	required_argument,	NULL,		'd' },
			{"s1s2_sep",	required_argument,	NULL,		'e' },
			{ "eps",		required_argument,	NULL,		'f' },
			{ "skip", 		required_argument,	NULL,		'g' },
			{ "seq1in_sep", required_argument,	NULL,		'h' },
			{ "seq2in_sep",	required_argument,	NULL,		'i' },
			{ "s1s2_delim",	required_argument,	NULL,		'j' },
			{ "iter", 		required_argument,	NULL,		'k' },
			{ "ifile",		required_argument,	NULL,		'l' },
			{ "ofile",		required_argument,	NULL,		'm' },
			{ NULL, 		0, 					NULL, 		0 }
	};
	int option_index = 0;
	int c;

	while ((c = getopt_long(argc, argv, "a:b:c:d:", long_options, &option_index))
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
			seq1_sep = std::string(optarg);
			break;
		case 'd':
			seq2_sep = std::string(optarg);
			break;
		case 'e':
			s1s2_sep = std::string(optarg);
			break;
		case 'f':
			eps = std::string(optarg);
			break;
		case 'g':
			skip = std::string(optarg);
			break;
		case 'h':
			seq1in_sep = std::string(optarg);
			break;
		case 'i':
			seq2in_sep = std::string(optarg);
			break;
		case 'j':
			s1s2_delim = std::string(optarg);
			break;
		case 'k':
			iter = atoi(optarg);
			break;
		case 'l':
			input_file = std::string(optarg);
			break;
		case 'm':
			output_file = std::string(optarg);
			break;
		case '?':
			cout << "Cannot parse command line arguments." << endl;
			print_help(argv[0]);
			exit(0);
			break;
		}

	}

	if (input_file.empty()) {
		cout << "Input file not provided" << endl;
		print_help(argv[0]);
		exit(0);
	}
	if (input_file.empty()) {
		cout << "Output file not provided" << endl;
		print_help(argv[0]);
		exit(0);
	}

	cout << "Using file: " << input_file << endl;
	cout << "Loading..." << endl;
	M2MFstAligner fstaligner(seq1_del, seq2_del, seq1_max, seq2_max, seq1_sep,
			seq2_sep, s1s2_sep, eps, skip, true);

	string line;
	ifstream infile(input_file.c_str(), ifstream::in);
	vector<string> tokens;
	vector<string> seq1;
	vector<string> seq2;
	if (infile.is_open()) {
		while (infile.good()) {
			getline(infile, line);
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
		infile.close();
	}

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

	cout << "Writing best alignments to file: " << output_file << endl;
	ofstream ofile(output_file.c_str(), ifstream::out);
	for (int i = 0; i < fstaligner.fsas.size(); i++) {
		vector<PathData> paths = fstaligner.write_alignment(fstaligner.fsas[i],
				1);
		for (int k = 0; k < paths.size(); k++) {
			for (int j = 0; j < paths[k].path.size(); j++) {
				ofile << paths[k].path[j];
				if (j < paths[k].path.size() - 1)
					ofile << " ";
			}
			ofile << endl;
		}
	}
	ofile.close();

	exit(0);
}
