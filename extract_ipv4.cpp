// Extract one valid IPv4 address, with an optional port, from a line of text.
// A candidate is a maximal run of digits, '.' and ':'. The whole run must match
// the grammar; a valid address is never sliced out of a longer run.

#include <cctype>
#include <iostream>
#include <string>

namespace {

bool isTokenChar(char c) {
    const unsigned char uc = static_cast<unsigned char>(c);
    return std::isdigit(uc) != 0 || c == '.' || c == ':';
}

// Read one octet or port. A leading zero is legal only for the value 0 itself.
// More than maxDigits, or a value above maxValue, rejects the component.
bool parseComponent(const std::string& token, std::size_t& index, int maxDigits,
                    int maxValue, int& value) {
    if (index >= token.size() ||
        std::isdigit(static_cast<unsigned char>(token[index])) == 0) {
        return false;
    }

    if (token[index] == '0') {
        value = 0;
        ++index;
        if (index < token.size() &&
            std::isdigit(static_cast<unsigned char>(token[index])) != 0) {
            return false;
        }
        return true;
    }

    int accumulated = 0;
    int digits = 0;
    while (index < token.size() &&
           std::isdigit(static_cast<unsigned char>(token[index])) != 0 &&
           digits < maxDigits) {
        accumulated = accumulated * 10 + (token[index] - '0');
        ++index;
        ++digits;
    }

    if (index < token.size() &&
        std::isdigit(static_cast<unsigned char>(token[index])) != 0) {
        return false;
    }
    if (digits == 0 || accumulated > maxValue) {
        return false;
    }

    value = accumulated;
    return true;
}

// Require octet.octet.octet.octet, then either the end of the run or :port
// through the end of the run. Leftover characters reject the entire run.
bool parseToken(const std::string& token, unsigned long& address, int& port) {
    std::size_t index = 0;
    int octets[4] = {0, 0, 0, 0};

    for (int part = 0; part < 4; ++part) {
        if (part > 0) {
            if (index >= token.size() || token[index] != '.') {
                return false;
            }
            ++index;
        }
        if (!parseComponent(token, index, 3, 255, octets[part])) {
            return false;
        }
    }

    if (index == token.size()) {
        port = -1;
    } else if (token[index] == ':') {
        ++index;
        int portValue = 0;
        if (!parseComponent(token, index, 5, 65535, portValue)) {
            return false;
        }
        if (index != token.size()) {
            return false;
        }
        port = portValue;
    } else {
        return false;
    }

    address = (static_cast<unsigned long>(octets[0]) << 24) |
              (static_cast<unsigned long>(octets[1]) << 16) |
              (static_cast<unsigned long>(octets[2]) << 8) |
              static_cast<unsigned long>(octets[3]);
    return true;
}

}  // namespace

bool extractIPv4(const std::string& str, unsigned long& outAddress, int& outPort) {
    outAddress = 0;
    outPort = -1;

    for (std::size_t i = 0; i < str.size();) {
        if (!isTokenChar(str[i])) {
            ++i;
            continue;
        }

        const std::size_t start = i;
        while (i < str.size() && isTokenChar(str[i])) {
            ++i;
        }

        unsigned long address = 0;
        int port = -1;
        if (parseToken(str.substr(start, i - start), address, port)) {
            outAddress = address;
            outPort = port;
            return true;
        }
    }

    return false;
}

int main() {
    std::string line;
    while (true) {
        std::cout << "Enter a string (or 'END' to quit): " << std::flush;
        if (!std::getline(std::cin, line)) {
            break;
        }
        if (line == "END") {
            std::cout << "Program terminated.\n";
            break;
        }

        unsigned long address = 0;
        int port = -1;
        if (!extractIPv4(line, address, port)) {
            std::cout << "Invalid input: no valid IPv4 address found\n\n";
            continue;
        }

        const unsigned long a = (address >> 24) & 0xFFul;
        const unsigned long b = (address >> 16) & 0xFFul;
        const unsigned long c = (address >> 8) & 0xFFul;
        const unsigned long d = address & 0xFFul;

        std::cout << "Extracted IPv4 address: " << a << '.' << b << '.' << c << '.'
                  << d << " (decimal value: " << address << ", port: ";
        if (port < 0) {
            std::cout << "none";
        } else {
            std::cout << port;
        }
        std::cout << ")\n\n";
    }

    return 0;
}
