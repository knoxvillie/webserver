#include <iostream>
#include <sstream>
#include <string>

int main() {
    std::string input = "42 3.14 Hello";

    std::stringstream ss(input);

    int intValue;
    double doubleValue;
    std::string stringValue;

    ss >> intValue >> doubleValue >> stringValue;

    std::cout << "Integer: " << intValue << "\n";
    std::cout << "Double: " << doubleValue << "\n";
    std::cout << "String: " << stringValue;

    return 0;
}
