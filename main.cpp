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
        getline(cin, input);
        if (input.empty()) return false;
        if (input == "y" || input == "Y" || input == "是") {
            return true;
        } else if (input == "n" || input == "N" || input == "否") {
            return false;
        }
        printLine("输入无效，请重新输入 (y/n)：");
    }
}
string selectCppStandard() {
    int choice;
    printLine("\n=== 选择C++标准版本 ===");
    printLine("1. C++11");
    printLine("2. C++14");
    printLine("3. C++17");
    printLine("4. C++20");
    printLine("请输入选项(1-4，默认1): ");
    string input;
    getline(cin, input);
    if (input.empty()) return "-std=c++11";
    try {
        choice = stoi(input);
        switch (choice) {
            case 1: return "-std=c++11";
            case 2: return "-std=c++14";
            case 3: return "-std=c++17";
            case 4: return "-std=c++20";
            default:
                printLine("无效选项，将使用默认C++11标准");
                return "-std=c++11";
        }
    } catch (...) {
        printLine("无效输入，将使用默认C++11标准");
        return "-std=c++11";
    }
}
void analyzeErrorReason(const string& cppPath, const string& gppPath, const string& cppStandard, const string& exePath) {
    printLine("\n=== 错误原因分析 ===");
    int reasonCount = 1;
    if (!isFileExists(cppPath)) {
        printLine(to_string(reasonCount++) + ". 源文件不存在：找不到 '" + cppPath + "'");
        printLine("   可能输入错误，或文件已被移动/删除");
        return;
    }
    if (!isFileExists(gppPath)) {
        printLine(to_string(reasonCount++) + ". g++路径无效：找不到 '" + gppPath + "'");
        printLine("   解决：删除用户目录下g++_path.data后重新运行，配置正确路径");
        return;
    }
    if (isFileInUse(exePath)) {
        printLine(to_string(reasonCount++) + ". 输出文件被占用：" + exePath);
        printLine("   可能原因：程序正在运行中，或被杀毒软件/文件管理器锁定");
        printLine("   解决方法：");
        printLine("   1. 关闭所有可能正在运行的该程序窗口");
        printLine("   2. 等待几秒钟后重试");
        printLine("   3. 若仍无法解决，重启电脑后再试");
    }
    if (cppStandard == "-std=c++20") {
        printLine(to_string(reasonCount++) + ". C++20标准可能不被支持：需要g++9.1及以上版本");
        printLine("   建议尝试C++17或更低版本");
    }
    else if (cppStandard == "-std=c++17") {
        printLine(to_string(reasonCount++) + ". C++17标准可能不被支持：需要g++7.1及以上版本");
        printLine("   建议尝试C++14或更低版本");
    }
    printLine(to_string(reasonCount++) + ". 代码存在语法错误");
    printLine("   例如：变量未定义、缺少分号、括号不匹配等");
    printLine("   解决：查看编译器输出的错误信息，找到标红的行号修改");
}
bool compileFile(const string& gppPath, const string& cppPath, const string& cppStandard) {
    string normalizedPath = normalizeCppPath(cppPath);
    if (!isFileExists(normalizedPath)) {
        cerr << "[错误] 找不到文件: " << normalizedPath << endl;
        return false;
    }
    string exePath = getExePath(normalizedPath);
    printLine("\n📌 编译目标：" + exePath);
    if (isFileExists(exePath) && isFileInUse(exePath)) {
        cerr << "[错误] 输出文件被占用：" << exePath << endl;
        cerr << "       请先关闭该程序后重试" << endl;
        return false;
    }
    if (isFileExists(exePath)) {
        if (!askOverwrite(exePath)) {
            printLine("已取消编译");
            return false;
        }
        if (remove(exePath.c_str()) != 0) {
            cerr << "[错误] 无法删除现有文件，可能被占用" << endl;
            cerr << "       请关闭所有相关程序后重试" << endl;
            return false;
        }
    }
    string errorLog = "compile_errors.txt";
    string compileCmd = gppPath + " " + cppStandard + " \"" + normalizedPath + "\" -o \"" + exePath + "\" 2> " + errorLog;
    printLine("\n=== 开始编译 ===");
    printLine("编译命令: " + compileCmd);
    printLine("正在编译...");
    int result = system(compileCmd.c_str());
    if (result == 0) {
        printLine("\n✅ 编译成功!");
        printLine("生成文件: " + exePath);
        if (isFileExists(errorLog)) remove(errorLog.c_str());
        return true;
    } else {
        cerr << "\n❌ 编译失败!" << endl;
        if (isFileExists(errorLog)) {
            printLine("\n=== 编译器错误信息 ===");
            ifstream errFile(errorLog);
            string line;
            while (getline(errFile, line)) {
                cerr << line << endl;
            }
            errFile.close();
            remove(errorLog.c_str());
        }
        analyzeErrorReason(normalizedPath, gppPath, cppStandard, exePath);
        if (isFileExists(exePath)) {
            remove(exePath.c_str());
            printLine("\n已清理不完整的输出文件");
        }
        return false;
    }
}
string getGppPath() {
    string configFile = getProfileConfigPath();
    if (isFileExists(configFile)) {
        ifstream infile(configFile);
        if (infile.is_open()) {
            string path;
            getline(infile, path);
            infile.close();
            if (isFileExists(path)) {
                return path;
            }
            printLine("[提示] g++路径无效，需要重新配置");
        }
    }
    ofstream outfile(configFile);
    if (!outfile.is_open()) {
        cerr << "[错误] 无法创建配置文件，请检查权限" << endl;
        return "";
    }
    string path;
    printLine("\n=== 配置g++编译器路径 ===");
    printLine("请输入g++的完整路径（例如：C:\\MinGW64\\bin\\g++.exe）: ");
    while (true) {
        getline(cin, path);
        if (!path.empty() && path.front() == '"' && path.back() == '"') {
            path = path.substr(1, path.size() - 2);
        }
        if (isFileExists(path)) {
            break;
        }
        cerr << "[错误] 无效的路径，请重新输入: ";
    }
    outfile << path;
    outfile.close();
    printLine("✅ g++路径已保存");
    return path;
}
int main(int argc, char* argv[]) {
    setConsoleEncoding();
    string cppPath;
    if (argc > 1) {
        cppPath = argv[1];
    } else {
        printLine("=== C++编译工具 ===");
        printLine("请输入要编译的.cpp文件路径（例如：test.cpp）: ");
        getline(cin, cppPath);
    }
    string normalizedPath = normalizeCppPath(cppPath);
    if (normalizedPath.find("test.cpp") != string::npos) {
        printLine("\n📝 检测到test.cpp，将输出test.exe");
    }
    string cppStandard = selectCppStandard();
    string gppPath = getGppPath();
    if (gppPath.empty()) {
        system("pause");
        return 1;
    }
    compileFile(gppPath, cppPath, cppStandard);
    printLine("\n按任意键退出...");
    system("pause");
    return 0;
}