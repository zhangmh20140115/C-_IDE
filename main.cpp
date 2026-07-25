#include <bits/stdc++.h>
#include <iostream>
#include <string>
#include <fstream>
#include <windows.h>
#include <sstream>
using namespace std;
string getProfileConfigPath()
{
    char buf[MAX_PATH];
    GetEnvironmentVariableA("USERPROFILE", buf, MAX_PATH);
    string path(buf);
    path += "\\g++_path.data";
    return path;
}
void setConsoleEncoding() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);
}
void printLine(const string& text) {
    cout << text << endl;
}
bool isFileExists(const string& name) {
    ifstream f(name.c_str());
    return f.good();
}
bool isFileInUse(const string& filePath) {
    ofstream file(filePath, ios::app);
    return !file.is_open();
}
string normalizeCppPath(const string& inputPath) {
    size_t extPos = inputPath.find_last_of('.');
    if (extPos != string::npos) {
        string ext = inputPath.substr(extPos);
        if (ext == ".cpp" || ext == ".c++" || ext == ".cc") {
            return inputPath;
        }
    }
    return inputPath + ".cpp";
}
string getExePath(const string& cppPath) {
    size_t posSlash = cppPath.find_last_of("\\/");
    size_t posDot = cppPath.find_last_of('.');
    string fileName;
    if (posSlash != string::npos) {
        if (posDot != string::npos && posDot > posSlash) {
            fileName = cppPath.substr(posSlash + 1, posDot - posSlash - 1);
        } else {
            fileName = cppPath.substr(posSlash + 1);
        }
    } else {
        if (posDot != string::npos) {
            fileName = cppPath.substr(0, posDot);
        } else {
            fileName = cppPath;
        }
    }
    string dirPath = (posSlash != string::npos) ? cppPath.substr(0, posSlash + 1) : "";
    return dirPath + fileName + ".exe";
}
bool askOverwrite(const string& filePath) {
    string input;
    while (true) {
        printLine("⚠️  文件已存在: " + filePath);
        printLine("是否覆盖? (y=是/n=否，默认n): ");
        getli... [Truncated for speed]