#include <iostream>
#include <fstream>
#include <string>
#include <streambuf>
#include <cstdio>
#include <vector>

#include "iofile.hpp"

using namespace std;

FILE *read(string filename)
{
    FILE *file = fopen(filename.c_str(), "r");

    if (!file)
    {
        cerr << "can't open file: " << filename << endl;
        return NULL;
    }

    return file;
}

void write(string filename, vector<string> code)
{
    ofstream file(filename);
    vector<string>::iterator it;
    for (it = code.begin(); it != code.end(); it++)
    {
        file << *it << endl;
    }

    file.close();
}
