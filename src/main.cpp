#include "hhga.hpp"

using namespace std;
using namespace hhga;

void printUsage(int argc, char** argv) {

    cerr << "usage: " << argv[0] << " [-b FILE]" << endl
         << endl
         << "options:" << endl
         << "    -h, --help            this dialog" << endl
         << "    -f, --fasta-reference FILE  the reference sequence" << endl
         << "    -b, --bam FILE        use this BAM as input (multiple allowed)" << endl
         << "    -r, --region REGION   limit output to those in this region (chr:start-end)" << endl
         << endl
         << "Generates reports on the rate of putative mutations or errors in the input alignment data." << endl
         << "Alignments are read from the specified files, or stdin if none are specified" << endl
         << endl
         << "authors: Erik Garrison <erik.garrison@gmail.com> and Nicolás Della Penna <nikete@gmail.com>" << endl;

}

int main(int argc, char** argv) {

    vector<string> inputFilenames;
    string vcf_file_name;

    string region_string;

    string fastaFile;

    // parse command-line options
    int c;

    while (true) {

        static struct option long_options[] =
        {
            {"help", no_argument, 0, 'h'},
            {"bam",  required_argument, 0, 'b'},
            {"vcf", required_argument, 0, 'v'},
            {"region", required_argument, 0, 'r'},
            {"fasta-reference", required_argument, 0, 'f'},
            {0, 0, 0, 0}
        };
        /* getopt_long stores the option index here. */
        int option_index = 0;

        c = getopt_long (argc, argv, "hb:r:f:v:",
                         long_options, &option_index);

        if (c == -1)
            break;
 
        switch (c) {

        case '?':
            printUsage(argc, argv);
            return 0;
            break;

        case 'h':
            printUsage(argc, argv);
            return 0;
            break;

        case 'b':
            inputFilenames.push_back(optarg);
            break;

        case 'v':
            vcf_file_name = optarg;
            break;

        case 'r':
            region_string = optarg;
            break;

        case 'f':
            fastaFile = optarg;
            break;

        default:
            return 1;
            break;
        }
    }

    if (fastaFile.empty()) {
        cerr << "no FASTA reference specified" << endl;
        printUsage(argc, argv);
        return 1;
    }

    if (inputFilenames.empty()) {
        cerr << "no input files specified" << endl;
        printUsage(argc, argv);
        return 1;
    }

    if (region_string.empty()) {
        cerr << "no target region given" << endl;
        printUsage(argc, argv);
        return 1;
    }

    BamMultiReader bam_reader;
    if (!bam_reader.Open(inputFilenames)) {
        cerr << "could not open input BAM files" << endl;
        return 1;
    }

    vcflib::VariantCallFile vcf_file;
    if (!vcf_file_name.empty()) {
        vcf_file.open(vcf_file_name);
        if (!vcf_file.is_open()) {
            cerr << "could not open " << vcf_file_name << endl;
            return 1;
        }
    }

    FastaReference fasta_ref;
    fasta_ref.open(fastaFile);

    // for now, just process the entire site at once
    // objective is to build up
    HHGA hhga(region_string, bam_reader, fasta_ref);
    cout << hhga.str() << endl;

    return 0;

}
