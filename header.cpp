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

int bytes_detector(std::string bytes,long int pos,FILE* file)
{   std::string file_bytes = finder(4,file);   
    while(file_bytes!=bytes)
    {   pos+=4;
        fseek(file,ftell(file) + 4,SEEK_SET); 
        file_bytes =finder(4,file);
    }
  return pos;
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
        std::string GatherableTextDataCount;
        std::string LocalizationId;
        std::string HeaderSize;
        std::string FolderName;
        std::string PackageFlags;
        std::string ExportCount;
        std::string ExportOffset;
        std::string ImportCount;
        std::string ImportOffset;
        std::string DependsOffset;
        std::string SoftPackageReferencesCount;
        std::string SoftPackageReferencesOffset;

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
        std::cout<<"FileVersionLicenseeUE4: "<<stoul(FileVersionLicenseeUE4,0,16)<<std::endl;
        CustomVersionsCount = info_finder(4,file);
        unsigned long long versions = stoul(CustomVersionsCount,0,16); 
        std::cout<<"CustomVersionsCount: "<<versions<<std::endl;
        ftell(file); 

        unsigned long long versionKeylength = versions*16;
        unsigned long long total_version_bytes_length = versions*4;
        int totalCustomVersionLength = versionKeylength + total_version_bytes_length;
        fseek(file,ftell(file) + totalCustomVersionLength,SEEK_SET); 
        ftell(file);

        HeaderSize = info_finder(4,file);
        std::cout<<"Total Header Size: "<<stoul(HeaderSize,0,16)<<std::endl;
        unsigned long long FolderNameSize = stoul(info_finder(4,file),0,16);
        FolderName = finder(FolderNameSize,file).substr(0,FolderNameSize*2-2);
        std::cout<<"FolderName: "<<hexToAscii(FolderName)<<std::endl;
        PackageFlags = info_finder(4,file);
        std::cout<<"PackageFlags: "<<stoul((PackageFlags),0,16)<<std::endl;
        NameCount = stoi(info_finder(4,file),0,16);
        std::cout<<"NameCount: "<<NameCount<<std::endl; 
        NameOffset = stoi(info_finder(4,file),0,16);
        std::cout<<"NameOffset: "<<NameOffset<<std::endl; 
        long int pos= ftell(file); 
        std::string Localisation_prefix = "21000000";
        bytes_detector(Localisation_prefix,pos,file);
        LocalizationId = finder(32,file);
        std::cout<<"LocalisationID: "<<hexToAscii(LocalizationId)<<std::endl;
      
        fseek(file,ftell(file) + 1,SEEK_SET); //to skip the null byte
        GatherableTextDataCount = info_finder(4,file);
        std::cout<<"GatherableTextDataCount: "<<stoul(GatherableTextDataCount,0,16)<<std::endl;
        GatherableTextDataOffset = info_finder(4,file);
        std::cout<<"GatherableTextDataOffset: "<<stoul(GatherableTextDataOffset,0,16)<<std::endl;
        ExportCount = info_finder(4,file);
        std::cout<<"ExportCount: "<<stoi(ExportCount,0,16)<<std::endl;
        ExportOffset = info_finder(4,file);
        std::cout<<"ExportOffset: "<<stoul(ExportOffset,0,16)<<std::endl; 
        ImportCount = info_finder(4,file);
        std::cout<<"ImportCount: "<<stoi(ImportCount,0,16)<<std::endl;
        ImportOffset = info_finder(4,file);
        std::cout<<"ImportOffset: "<<stoul(ImportOffset,0,16)<<std::endl;
        DependsOffset = info_finder(4,file);
        std::cout<<"DependsOffset: "<<stoul(DependsOffset,0,16)<<std::endl; 
        SoftPackageReferencesCount = info_finder(4,file);
        std::cout<<"SoftPackageReferencesCount: "<<stoul(SoftPackageReferencesCount,0,16)<<std::endl;
        SoftPackageReferencesOffset = info_finder(4,file);
        std::cout<<"SoftPackageReferencesOffset: "<<stoul(SoftPackageReferencesOffset,0,16)<<std::endl;
        std::cout<<"Names: "<<std::endl;
        printNames(NameOffset,NameCount,file);    


        
        //after the filename pointer



        //AssetRegistryDataOffset = info_finder(): 190 - 222
        // std::cout<<AssetRegistryDataOffset<<std::endl;

        //CompatibleWithEngineVersion = 

        //SavedByEngineVersion =  info_finder():_ 388  _ 408

        
}
    }
}
