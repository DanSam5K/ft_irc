#include <iostream>
#include <string>
#include <vector>

// Simple tokenizer to debug PING command parsing
std::vector<std::string> simpleTokenize(const std::string& input) {
    std::vector<std::string> tokens;
    std::string current;
    bool inExtended = false;
    
    for (size_t i = 0; i < input.length(); i++) {
        char c = input[i];
        
        if (c == ' ' && !inExtended) {
            if (!current.empty()) {
                tokens.push_back(current);
                current.clear();
            }
        } else if (c == ':' && !inExtended && current.empty()) {
            inExtended = true;
            current += c;
        } else {
            current += c;
        }
    }
    
    if (!current.empty()) {
        tokens.push_back(current);
    }
    
    return tokens;
}

int main() {
    std::string test1 = "PING :test";
    std::string test2 = "PING test";
    
    std::cout << "Testing: " << test1 << std::endl;
    std::vector<std::string> tokens1 = simpleTokenize(test1);
    std::cout << "Tokens: ";
    for (size_t i = 0; i < tokens1.size(); i++) {
        std::cout << "[" << tokens1[i] << "] ";
    }
    std::cout << std::endl;
    
    std::cout << "\nTesting: " << test2 << std::endl;
    std::vector<std::string> tokens2 = simpleTokenize(test2);
    std::cout << "Tokens: ";
    for (size_t i = 0; i < tokens2.size(); i++) {
        std::cout << "[" << tokens2[i] << "] ";
    }
    std::cout << std::endl;
    
    return 0;
}
