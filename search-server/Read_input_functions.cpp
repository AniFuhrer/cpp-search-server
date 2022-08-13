#include "Read_input_functions.h"

using namespace std;

string ReadLine() {
    string s;
    getline(std::cin, s);
    return s;
}

int ReadLineWithNumber() {
    int result;
    std::cin >> result;
    ReadLine();
    return result;
}