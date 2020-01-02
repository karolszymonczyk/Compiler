#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <streambuf>
#include <cstdio>
#include <vector>

using namespace std;

FILE *read(string filename);
void write(string filename, vector<string> code);
