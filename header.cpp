
#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>
#include <sstream>
#include <typeinfo>
#include <string>
#include <algorithm>

bool IsHeaderOk(const std::string& file_bytes) {
    std::string std_header = "c1832a9ef8ffffff600300000a020000";
    for (int i = 0; i < std_header.length(); i++) {
        if (file_bytes[i] != std_header[i] )
        {
            return false;
        }
    }
    return true;
}

std::string little_to_big_endian(char bytes[])
{
     std::reverse(bytes, bytes+sizeof(bytes));
     std::stringstream ss;
     for (int i = 0; i < sizeof(bytes); i++){
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(static_cast<unsigned char>(bytes[i]));
     }
     return ss.str();
}

long long hexToDecimal(const std::string& hexString)
{
   long long decimalNumber=std::stoll(hexString,nullptr,16);

    return decimalNumber;
}

std::string Header(const std::vector<char>& file_bytes)
{
    char header[16];
    std::stringstream ss;
    std::copy(file_bytes.begin(), file_bytes.begin() + sizeof(header), header);
    for (int i = 0; i < sizeof(header); i++){
        ss << std::hex << std::setw(2) << std::setfill('0') <<static_cast<int>(static_cast<unsigned char>(header[i]));
        }
    return ss.str(); 
}

std::string EPackedFileTag(const std::vector<char>& file_bytes)
{
     char bytes[4];
     std::copy(file_bytes.begin(), file_bytes.begin() + sizeof(bytes), bytes);
     std::string hexString = little_to_big_endian(bytes); 
     long long value= hexToDecimal(hexString);
     std:: string result= std::to_string(value);
     
   return result;
}

int main(int argc,char *argv[])
{   if(argc<2)
    std::cout<<"Usage is ./header.exe {filename} "<<std::endl;
    else{
      std::ifstream file(argv[1], std::ios::binary);
      if (!file){
        std::cerr << "File not found" << std::endl;
      }
      else{
          std::vector<char> file_buf;
          char a;
          while (file.get(a)) {
            file_buf.push_back(a);
            } 
          std::string header = Header(file_buf);
          std::cout <<"Header is :- "<< header << std::endl;
          bool var = IsHeaderOk(header);
          if(var==1){
            std::cout<<"Valid Header of UASSET File"<<std::endl;
            std::cout<<"EPackedFileTag is:- "<<EPackedFileTag(file_buf)<<std::endl;
            }
          else{
            std::cout<<"Invalid Header"<<std::endl;
            }
         }
    }
}

