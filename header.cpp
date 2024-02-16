
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

std::string LegacyUE3Version(const std::vector<char>& file_bytes)
{
  char bytes[4];
  std::copy(file_bytes.begin()+8, file_bytes.begin()+12, bytes);
  std::string hexString = little_to_big_endian(bytes);
  long long value= hexToDecimal(hexString);
  std:: string result= std::to_string(value);
     
   return result;
}
std::string FileVersionUE4(const std::vector<char>& file_bytes)
{
  char bytes[4];
  std::copy(file_bytes.begin()+12, file_bytes.begin()+16, bytes);
  std::string hexString = little_to_big_endian(bytes);
  long value= hexToDecimal(hexString);
  std:: string result= std::to_string(value);
     
   return result;
}
std::string FileVersionUE5(const std::vector<char>& file_bytes)
{
  char bytes[4];
  std::copy(file_bytes.begin()+16, file_bytes.begin()+20, bytes);
  std::string hexString = little_to_big_endian(bytes);
  long value= hexToDecimal(hexString);
  std:: string result= std::to_string(value);
     
   return result;
}
int CustomVersionsCount(const std::vector<char>& file_bytes)
{
  char bytes[4];
  std::copy(file_bytes.begin()+24, file_bytes.begin()+28, bytes);
  std::string hexString = little_to_big_endian(bytes);
  int value= hexToDecimal(hexString);
  return value;
}

std::string FileVersionLicenseeUE4(const std::vector<char>& file_bytes)
{
  char bytes[4];
  std::copy(file_bytes.begin()+20, file_bytes.begin()+24, bytes);
  std::string hexString = little_to_big_endian(bytes);
  long value= hexToDecimal(hexString);
  std:: string result= std::to_string(value);                                                                 
  return result;
}

long long HeaderSize(const std::vector<char>& file_bytes, int totalCustomVersionLength )
{ char bytes[4];
  std::copy(file_bytes.begin()+28+totalCustomVersionLength, file_bytes.begin()+28+totalCustomVersionLength+4, bytes);
  std::string hexString = little_to_big_endian(bytes);
  long long value= hexToDecimal(hexString);
  return value;
}
std::string FolderName(const std::vector<char>& file_bytes, int totalCustomVersionLength)
{
  char bytes[4];
  std::copy(file_bytes.begin()+28+totalCustomVersionLength+4, file_bytes.begin()+28+totalCustomVersionLength+8, bytes);
  int length = hexToDecimal(little_to_big_endian(bytes));
  std::string name(file_bytes.begin() + 28 + totalCustomVersionLength + 4 + 4,file_bytes.begin() + 28 + totalCustomVersionLength + 8 + length);
  return name;
}
long long PackageFlags(const std::vector<char>& file_bytes, int totalCustomVersionLength){
  char bytes[4];
  std::copy(file_bytes.begin()+28+totalCustomVersionLength+4, file_bytes.begin()+28+totalCustomVersionLength+8, bytes);
  int length = hexToDecimal(little_to_big_endian(bytes));
  std::copy(file_bytes.begin()+28+totalCustomVersionLength+4+4+length, file_bytes.begin()+28+totalCustomVersionLength+8+length+4, bytes);
  std::string hexString = little_to_big_endian(bytes);
  long long value= hexToDecimal(hexString);
  return value;
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
          std::cout <<"Header is : "<< header << std::endl;
          bool var = IsHeaderOk(header);
          if(var==1){
            std::cout<<"Valid Header of UASSET File"<<std::endl;
            std::cout<<"EPackedFileTag is: "<<EPackedFileTag(file_buf)<<std::endl;
            std::cout<<"LegacyUE3Version is: "<< LegacyUE3Version(file_buf)<<std::endl;
            std::cout<<"FileVersionUE4 is: "<< FileVersionUE4(file_buf)<<std::endl;
            std::cout<<"FileVersionUE5 is: "<<FileVersionUE5(file_buf)<<std::endl;
            std::cout<<"CustomVersions Count is: "<<CustomVersionsCount(file_buf)<<std::endl;

            int VersionsCount = CustomVersionsCount(file_buf);
            int total_versionKeys_length = VersionsCount*(4*4);
            int total_version_bytes_length = VersionsCount*4;
            int totalCustomVersionLength = total_versionKeys_length+ total_version_bytes_length;

            std::cout<<"FileVersionLicenseeUE4 is: "<<FileVersionLicenseeUE4(file_buf)<<std::endl;
            std::cout<<"Total Header Size is: "<<HeaderSize(file_buf, totalCustomVersionLength)<<std::endl;
            std::cout<<"FolderName : "<<FolderName(file_buf, totalCustomVersionLength)<<std::endl;
            std::cout<<"Package Flags : "<<PackageFlags(file_buf, totalCustomVersionLength)<<std::endl;

            }
          else{
            std::cout<<"Invalid Header"<<std::endl;
            }
         }
    }
}

