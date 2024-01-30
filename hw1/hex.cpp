#include <iostream>
#include <string>
#include <algorithm>
#include <math.h>

using namespace std;

string binary_to_decimal(string bin) {
    int decimal = 0;
    
    int coeff = 0;

    for (int i = bin.length() - 1; i >= 0; i--) {
        decimal += (bin[i] - '0') * pow(2, coeff);
        coeff++;
    }

    return to_string(decimal);
}

string hex_to_decimal(string hex) {
    int decimal = 0;
    
    int coeff = 0;

    for (int i = hex.length() - 1; i >= 0; i--) {
        int val;

        if (hex[i] >= '0' && hex[i] <= '9') {
            val = hex[i] - '0';
        } else {
            val = 10 + (hex[i] - 'a');
        }

        decimal += val * pow(16, coeff);
        coeff++;
    }

    return to_string(decimal);
}

string decimal_to_binary(string dec_string) {
    string binary = "";

    int dec = stoi(dec_string);

    if (dec == 0) {
        return "0";
    }

    while (dec > 0) {
        binary += to_string(dec % 2);
        dec /= 2;
    }
    
    reverse(binary.begin(), binary.end());
    return binary;
}

std::string decimal_to_hex(const string & dec_string) {
    int decimal = stoi(dec_string);
    if (decimal == 0) return "0";

    std::string hex = "";

    while (decimal > 0) {
        int rem = decimal % 16;
        
        if (rem < 10) {
            hex = char('0' + rem) + hex;
        } else {
            hex = char('a' + rem - 10) + hex;
        }

        decimal /= 16;
    }

    return hex;
}

int main(int argc, char * argv[]) {
    if (argc != 4) {
        cout << "Invalid input, must input 3 arguments, provided " << argc - 1 << endl;
        return 1;
    } 

    string from = argv[1];
    string to = argv[2];
    string num = argv[3];

    if ((from != "h" && from != "b" && from != "d") || (to != "h" && to != "b" && to != "d")) {
        cout << "Usage: ./hex [ h | d | b ] [ h | d | b]" << endl;
        return 0;
    }

    if (from == "h") {
        if (to == "b") {
            cout << "hexadecimal " << num << " is binary " << decimal_to_binary(hex_to_decimal(num)) << endl;
        } else if (to == "d") {
            cout << "hexadecimal " << num << " is decimal " << hex_to_decimal(num) << endl;
        }
    } else if (from == "d") {
        if (to == "b") {
            cout << "decimal " << num << " is binary " << decimal_to_binary(num) << endl;
        } else if (to == "h") {
            cout << "decimal " << num << " is hexadecimal " << decimal_to_hex(num) << endl;
        }
    } else if (from == "b") {
        if (to == "d") {
            cout << "binary " << num << " is decimal " << binary_to_decimal(num) << endl;
        } else if (to == "h") {
            cout << "binary " << num << " is hexadecimal " << decimal_to_hex(binary_to_decimal(num)) << endl;
        }
    }
}
