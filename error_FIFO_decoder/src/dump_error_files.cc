#ifndef dump_error_files_cc
#define dump_error_files_cc

#include "dump_error_files.h"
#include "process_file.h"

int main(int argc, char **argv)
{
    if (argc < 2) {
        cerr << "Usage: ./dump_error_files Error_file.err\n";
        return 1;
    }

    ifstream in_file;
    in_file.open(argv[1], ios::binary | ios::in);
    if (!in_file) {
        cerr << "File: " << argv[1] << " not found " << endl;
        return 1; // signal error
    }
    
    data da;
    {
        flags fl;
        Process_file(fl, in_file, da);
        in_file.close();
    }

    Print_summary(da.countErrors);

    return 0;
}

#endif
