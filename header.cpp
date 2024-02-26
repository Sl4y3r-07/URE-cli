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

std::string hexToAscii(std::string s){
    std::string result;
    for(int i=0;i<s.length();i+=2){
        std::string hex = s.substr(i,2);
        char ch = stoul(hex,0,16);
        result += ch;
    }
    return result;
}
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
void printNames(unsigned long long NameOffset,unsigned long long NameCount,FILE* file){
    fseek(file,NameOffset + 4,SEEK_SET);
    for(int i=0;i<NameCount;i++){
        std::string name;
        std::string NonCasePreservingHash;
        std::string CasePreservingHash;
        
        char ch;
        while((ch = getc(file))!=EOF && ch!=0){
            if(ch!='\x00'){
                name += ch;
            }
        }
        NonCasePreservingHash = info_finder(2,file);
        CasePreservingHash = info_finder(2,file);
        std::cout<<"\tName "<<i<<"\t"<<name<<std::endl;
        std::cout<<"\t\tCasePreservingHash:\t"<<stoul(CasePreservingHash,0,16)<<std::endl;
        std::cout<<"\t\tNonCasePreservingHash:\t"<<stoul(NonCasePreservingHash,0,16)<<std::endl;
        fseek(file,ftell(file)+4,SEEK_SET);
    }

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
        std::string LegacyFileVersion;
        std::string LegacyUE3Version;
        std::string FileVersionUE5;
        std::string FileVersionUE4;
        std::string CustomVersionsCount;
        std::string FileVersionLicenseeUE4;
        std::string GatherableTextDataOffset;
        std::string LocalizationId;
        std::string HeaderSize;
        std::string FolderName;
        std::string PackageFlags;


        unsigned long long NameCount;
        unsigned long long NameOffset;

   
        header = finder(16,file);
        std::cout<<"Header: "<<header<<std::endl;
        EpackedFileTag = little_to_big_endian(header.substr(0,8));
        std::cout<<"EPackedFileTag: "<<stoul(EpackedFileTag,0,16)<<std::endl;
        LegacyFileVersion = header.substr(8,8);
        std::cout<<"LegacyFileVersion: "<<LegacyFileVersion<<std::endl;
        LegacyUE3Version = header.substr(16,8);
        std::cout<<"LegacyUE3Version: "<<stoi(little_to_big_endian(LegacyUE3Version),0,16)<<std::endl;
        FileVersionUE4 = header.substr(24,8);
        std::cout<<"FileVersionUE4: "<<stoi(little_to_big_endian(FileVersionUE4),0,16)<<std::endl;
        FileVersionUE5 = info_finder(4,file);
        std::cout<<"FileVersionUE5: "<<stoul(FileVersionUE5,0,16)<<std::endl;
        FileVersionLicenseeUE4 = info_finder(4,file);
        std::cout<<"FileVersionLicenseeUE: "<<stoul(FileVersionLicenseeUE4,0,16)<<std::endl;
        CustomVersionsCount = info_finder(4,file);
        unsigned long long versions = stoul(CustomVersionsCount,0,16); 
        std::cout<<"CustomVersionsCount: "<<versions<<std::endl;
        std::cout<<ftell(file)<<std::endl; 

        unsigned long long versionKeylength = versions*16;
        unsigned long long total_version_bytes_length = versions*4;
        int totalCustomVersionLength = versionKeylength + total_version_bytes_length;
        std::cout<<"1: "<<totalCustomVersionLength<<std::endl;
        std::cout<<"2: "<<versionKeylength<<std::endl;
        std::cout<<"3: "<<total_version_bytes_length<<std::endl;
        fseek(file,ftell(file) + totalCustomVersionLength,SEEK_SET); 
        std::cout<<ftell(file)<<std::endl;

        HeaderSize = info_finder(4,file);
        std::cout<<stoul(HeaderSize,0,16)<<std::endl;
        std::cout<<ftell(file)<<std::endl;

        unsigned long long FolderNameSize = stoul(info_finder(4,file),0,16);
        std::cout<<FolderNameSize<<std::endl;
        std::cout<<ftell(file)<<std::endl;

        FolderName = finder(FolderNameSize,file).substr(0,FolderNameSize*2-2);
        std::cout<<hexToAscii(FolderName)<<std::endl;
        std::cout<<ftell(file)<<std::endl;

        PackageFlags = info_finder(4,file);
        std::cout<<"PackageFlags: "<<stoul((PackageFlags),0,16)<<std::endl;

        NameCount = stoul(info_finder(4,file),0,16);
        std::cout<<"NameCount: "<<NameCount<<std::endl; 

        NameOffset = stoul(info_finder(4,file),0,16);
        std::cout<<"NameOffset: "<<NameOffset<<std::endl; 
        long int pos= fseek(file,ftell(file)+4,SEEK_SET);
        //Localization ID
        //first name from 502-540
        std::cout<<"Names: "<<std::endl;
        printNames(NameOffset,NameCount,file); //NameCount will be the required argument






        
        //after the filename pointer



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
