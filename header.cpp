
#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>
#include <stdexcept>
#include <sstream>
#include <typeinfo>
#include <string>
#include <algorithm>
#include <cstddef>
#include <cstring>
#include <string>

std::string little_to_big_endian(std::string s)
{
    std::string s1 = "";
    for(int i=s.length();i>=2;i-=2)
    {
        s1+=s.substr(i-2,2);
    }
    return s1;
}
std::string bytesToHex(const char* buffer, size_t length) {
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    for (size_t i = 0; i < length; ++i) {
        ss << std::setw(2) << static_cast<unsigned int>(static_cast<unsigned char>(buffer[i]));
    }
    return ss.str();
}
std::string finder(int num,FILE *file){
    char *buf = new char[num];
    //allocation helps me to not look for the null terminated strings but resolve the problem as it now allocates a new space for the buffer obtained
    int bytes_read = fread(buf,sizeof(char),num*sizeof(char),file);
    if (bytes_read != num) {
        delete[] buf;
        throw std::runtime_error("Error reading from file");
    }
    std::string s = bytesToHex(buf,num);
    return s;
}
std::string info_finder(int num,FILE* file)
{
    return(little_to_big_endian(finder(num,file)));
}
int main(int argc,char *argv[])
{   if(argc<2)
    std::cout<<"Usage is ./header.exe {filename} "<<std::endl;
    else{
      FILE *file;
      file =  fopen(argv[1],"rb+");
      if (file == NULL){
        std::cerr << "File not found" << std::endl;
      }
      else{
        std::string EpackedFileTag;
        std::string header;
        std::string LegacyUE3Version;
        std::string FileVersionUE5;
        std::string FileVersionUE4;
        std::string CustomVersionsCount;
        std::string FileVersionLicenseeUE4;
        std::string GatherableTextDataOffset;
        std::string LocalizationId;
        // std::string 
        char *waste_buf;

        header = finder(16,file);
        std::cout<<header<<std::endl;
        EpackedFileTag = little_to_big_endian(header.substr(0,8));
        std::cout<<EpackedFileTag<<std::endl;
        LegacyUE3Version = header.substr(8,8);
        std::cout<<little_to_big_endian(LegacyUE3Version)<<std::endl;
        //FILEVERSIONUE4 IS WRONG
        // FileVersionUE4 = info_finder(4,file);
        // std::cout<<FileVersionUE4<<std::endl;
        std::cout<<ftell(file)<<std::endl;
        FileVersionUE5 = info_finder(4,file);
        std::cout<<FileVersionUE5<<std::endl;
        // fread(waste_buf,sizeof(char),sizeof(char)*4,file);
        std::cout<<ftell(file)<<std::endl;
        FileVersionLicenseeUE4 = info_finder(4,file);
        std::cout<<FileVersionLicenseeUE4<<std::endl;
        CustomVersionsCount = info_finder(4,file);
        std::cout<<CustomVersionsCount<<std::endl;

        //after the filename pointer

        // GatherableTextDataOffset = info_finder(): 52-56
        // std::cout<<GatherableTextDataOffset<<std::endl

        // LocalizationId = info_finder(): 18-50
        // std::cout<<GatherableTextDataOffset<<std::endl

        //AssetRegistryDataOffset = info_finder(): 190 - 222
        // std::cout<<AssetRegistryDataOffset<<std::endl;

        //CompatibleWithEngineVersion = 

        //SavedByEngineVersion =  info_finder():_ 388  _ 408

        //FOR EACH NAME:
        //buffer = array of next 4 bytes+
        //CasePreservingHash : last 2 bytes
        //NonCasePreservingHash: first 2 bytes


}
    }
}

