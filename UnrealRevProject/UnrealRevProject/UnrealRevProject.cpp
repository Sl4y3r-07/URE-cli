// UnrealRevProject.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>
#include <sstream>
#include <typeinfo>
#include <locale>
#include <codecvt>

std::string charactersToBytes(const std::vector<char>& chars) {
    std::ostringstream oss;
    for (char c : chars) {
        oss << "\\x" << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(static_cast<unsigned char>(c));
    }
    return oss.str();
}
std::string utf8_encode(const std::vector<char>& vec) {


    std::string str(vec.begin(), vec.end());

    std::wstring_convert<std::codecvt_utf8<char>, char> converter;
    return converter.to_bytes(str);

}
bool IsHeaderOk(std::string file_bytes) {
    //If the first 16 bytes represent the standard header bytes.
    std::string std_header = "\xc1\x83\x2a\x9e\xf8\xff\xff\xff\x60\x03\x00\x00\x0a\x02\x00\x00";
    std::cout << std_header[0] << std::endl;
    for (int i = 0; i < 16; i++) {
        if (file_bytes[i] != std_header[i])
        {
            return false;
        }
    }
    return true;
}


int main(int argc,char**argv[])
{
    //Confirming the Command Line Arguments
    //if (argc < 2) {
     //   std::cerr << "Usage: ./Main <.uasset FileName>";
       // exit(0);
    //}


    std::ifstream file("M_Metal_Burnished_Steel.uasset", std::ios::binary);

    if (!file.is_open()) {
        std::cerr << "Error opening the file."<<std::endl;
        exit(1);
    }

    std::vector<char> file_buf;
    char a;
    while (file.get(a)) {
        file_buf.push_back(a);
    }
    std::cout << (file_buf[0]) << std::endl;
    //std::string obt_string = charactersToBytes(file_buf);
   // std::string bytes_buf = utf8_encode(file_buf);
    //std::cout << bytes_buf[0] << std::endl;
    //bool flag = IsHeaderOk(bytes_buf);


}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
