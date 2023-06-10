#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <regex>
#include <chrono>
#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif
#include <random>

std::unordered_map<std::string, std::string> obfuscateVariables(const std::string& code) {
    std::unordered_map<std::string, std::string> variableMap;
    std::regex variableRegex(R"(\b(bool|std::string|int|char)\s+([a-zA-Z_][a-zA-Z0-9_]*)\b(?!.*"))");

    std::smatch match;
    std::string obfuscatedPrefix = "atramis";

    // Random number generator
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis(100, 9999);

    auto variableIterator = std::sregex_iterator(code.begin(), code.end(), variableRegex);
    const auto variableEnd = std::sregex_iterator();

    while (variableIterator != variableEnd) {
        match = *variableIterator;
        const std::string& variableType = match[1].str();
        const std::string& variableName = match[2].str();

        // Exclude standard library identifiers and 'main' function from obfuscation
        if (variableType != "int" || (variableType == "int" && variableName != "main")) {
            std::string obfuscatedName = obfuscatedPrefix + std::to_string(dis(gen));

            // Add random numbers before and after atramis
            obfuscatedName = std::to_string(dis(gen)) + obfuscatedName + std::to_string(dis(gen));

            // Add a random number between 'a' and 'm' in atramis
            std::size_t pos = obfuscatedName.find("atramis");
            if (pos != std::string::npos) {
                int randomNum = dis(gen);
                obfuscatedName.insert(pos + 1, std::to_string(randomNum));
            }

            variableMap[variableName] = obfuscatedName;
        }

        ++variableIterator;
    }

    return variableMap;
}

std::string obfuscateCode(const std::string& code) {
    std::unordered_map<std::string, std::string> variableMap = obfuscateVariables(code);
    std::string obfuscatedCode = code;

    for (const auto& variable : variableMap) {
        const std::string& variableName = variable.first;
        const std::string& obfuscatedName = variable.second;
        std::regex variableRegex(R"(\b)" + variableName + R"(\b(?!.*"))");
        obfuscatedCode = std::regex_replace(obfuscatedCode, variableRegex, obfuscatedName);
    }

    return obfuscatedCode;
}

void setConsoleTitle(const std::string& title) {
#ifdef _WIN32
    SetConsoleTitleA(title.c_str());
#else
    std::cout << "\033]0;" << title << "\007";
#endif
}

void clearConsole() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

int main() {
    // Set console title
    setConsoleTitle("Atramis");
    std::cout << "github.com/vortexsys/atramis" << std::endl;
    clearConsole();
    std::cout << "github.com/vortexsys/atramis" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));

    // Create input file if it doesn't exist
    std::ifstream inputFile("code.txt");
    if (!inputFile) {
        std::ofstream createInputFile("code.txt");
        if (!createInputFile) {
            std::cerr << "Failed to create input file." << std::endl;
            return 1;
        }
        createInputFile.close();

        std::cout << "Please put the code into 'code.txt' and press enter to continue." << std::endl;
        std::cin.ignore();
    }
    inputFile.close();

    // Read code from file
    std::ifstream inputFileRead("code.txt");
    std::string code((std::istreambuf_iterator<char>(inputFileRead)), std::istreambuf_iterator<char>());
    inputFileRead.close();

    if (code.empty()) {
        std::cout << "No code found in 'code.txt'. Please put the code into the file and run the program again." << std::endl;
        std::cin.ignore();
        return 1;
    }

    // Obfuscate the code
    std::string obfuscatedCode = obfuscateCode(code);

    // Create output file if it doesn't exist
    std::ofstream outputFile("obfuscated.txt");
    if (!outputFile) {
        std::cerr << "Failed to create output file." << std::endl;
        return 1;
    }
    outputFile << obfuscatedCode;
    outputFile.close();

    // Wait for user input before exiting
    std::cout << "After obfuscating it, you may need to rewrite things from ";
    std::cout << "std::cout as it may display weird characters." << std::endl << "We are currently working on fixing it." << std::endl;
    std::cout << "Obfuscation complete. Press enter to exit." << std::endl;
    std::cin.ignore();

    return 0;
}