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
// #include "rapidjson/document.h"
// #include "rapidjson/filewritestream.h"
// #include "rapidjson/writer.h"
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
void printGatherableData(unsigned long long GatherableDataOffset,unsigned long long GatherableDataCount,FILE* file){
    if(!GatherableDataCount){
        std::cout<<"No Gatherable Text Data Available"<<std::endl;
    }
    
    while(GatherableDataCount--){
    std::string NamespaceName="";
    std::string SourceString="";
    std::string KeyName="";
    std::string SiteDescription="";
    int IsEditorOnly;
    char ch;
    
    while((ch=getc(file))!=EOF && ch!='\x00'){
        NamespaceName += ch;
    }
    fseek(file,4,SEEK_CUR);

    while((ch=getc(file))!=EOF && ch!='\x00'){
        SourceString += ch;
    }
    fseek(file,4,SEEK_CUR);

    IsEditorOnly = stoi(info_finder(4,file));
    fseek(file,4,SEEK_CUR);

    while(ch=getc(file)!=EOF && ch!='\x00'){
        KeyName += ch;
    }
    fseek(file,4,SEEK_CUR);

    while(ch=getc(file)!=EOF && ch!='\x00'){
        SiteDescription += ch;
    }

    std::cout<<"\tGatherable Text Data: "<<std::endl;
    std::cout<<"\t\tNameSpaceName: "<<NamespaceName<<std::endl;
    std::cout<<"\t\t\tSourceString: "<<SourceString<<std::endl;
    std::cout<<"\t\t\tKeyName: "<<KeyName<<std::endl;
    std::cout<<"\t\t\tIsEditorOnly: "<<IsEditorOnly<<std::endl;
    std::cout<<"\t\t\tSite Description: "<<SiteDescription<<std::endl;
    }

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
void Generations(int GenerationCount,FILE* file){
    std::string Generation;
    std::string nameCount;
    std::string exportCount;
    if(GenerationCount>0){
    for(int i =1;i<=GenerationCount;i++){
        std::cout<<"\tGeneration "<<i<<std::endl;
        exportCount = info_finder(4,file);
        nameCount = info_finder(4,file);
        std::cout<<"\t\texportCount:\t"<<stoul(exportCount,0,16)<<std::endl;
        std::cout<<"\t\tnameCount:\t"<<stoul(nameCount,0,16)<<std::endl; 
}
    }
}
void SavedByEngineVersion_CompatibleWithEngineVersion(FILE* file)
{     
    if ((info_finder(28,file))=="00000000000000000000000000000000000000000000000000000000")
    {
        std::cout<<"SavedByEngineVersion: "<< " "<<std::endl;
        std::cout<<"CompatibleWithEngineVersion: "<< " "<<std::endl;
        
    }

    else{
        fseek(file,ftell(file)-28,SEEK_SET);
        std::string string;
        string= std::to_string(stoi(info_finder(2,file),0,16))+"."+std::to_string(stoi(info_finder(2,file),0,16))+"."+std::to_string(stoi(info_finder(2,file),0,16));
        std::reverse(string.begin(), string.end());
        long long number=stoull(info_finder(4,file),0,16);
        fseek(file,ftell(file)+4,SEEK_SET);
        std::string version;
        for(int i=1; ;i){
          if((finder(i,file))=="00"){
          break;}
          fseek(file,ftell(file)-1,SEEK_SET);
          version += hexToAscii(finder(i,file));
        }
        std::cout<<"SavedByEngineVersion: "<<string<<"-"<<number<<version<<std::endl;
 
        string= std::to_string(stoi(info_finder(2,file),0,16))+"."+std::to_string(stoi(info_finder(2,file),0,16))+"."+std::to_string(stoi(info_finder(2,file),0,16));
        std::reverse(string.begin(), string.end());
        number=stoull(info_finder(4,file),0,16);
        fseek(file,ftell(file)+4,SEEK_SET);
        std::string version_1;
        for(int i=1; ;i){
          if((finder(i,file))=="00"){
          break;}
          fseek(file,ftell(file)-1,SEEK_SET);
          version_1 += hexToAscii(finder(i,file));
        }
        std::cout<<"CompatibleWithEngineVersion: "<<string<<"-"<<number<<version_1<<std::endl;
    }


}
// void generateJSONdata() {
//     rapidjson::Document d;
//     d.SetObject();
//     FILE* file;
//     char* buffer  = new char();

//     d.AddMember("Name: ", "Mark", d.GetAllocator());
//     d.AddMember("Agr: ", "30", d.GetAllocator());
//     file = fopen("result.json", "w");

//     rapidjson::FileWriteStream os(file, buffer, sizeof(buffer));
//     rapidjson::Writer<rapidjson::FileWriteStream> writer(os);
//     d.Accept(writer);
// }

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
        std::string SearchableNamesOffset;
        std::string ThumbnailTableOffset;
        std::string GUID;
        std::string PersistentGUID;
        std::string CompressedFlags;
        std::string PackageSource;
        std::string ChunkIDs;

        
        long int GenerationsCount;
        unsigned long long NameCount;
        unsigned long long NameOffset;
        unsigned long long GatherableTextDataOffset;
        unsigned long long GatherableTextDataCount;
        long CompressedChunksCount;
        unsigned long AssetRegistryDataOffset;
        unsigned long long BulkDataStartOffset;
        unsigned long long WorlTileInfoDataOffset;
        signed long PreloadDependencyCount;
        unsigned long long PreloadDependencyOffset;
        unsigned long NamesReferencedFromExportDataCount;
        signed long long PayloadTocOffset;
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
        GatherableTextDataCount = stoul(info_finder(4,file),0,16);
        std::cout<<"GatherableTextDataCount: "<<GatherableTextDataCount<<std::endl;
        GatherableTextDataOffset = stoul(info_finder(4,file),0,16);
        std::cout<<"GatherableTextDataOffset: "<<GatherableTextDataOffset<<std::endl;
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
        SearchableNamesOffset= info_finder(4,file);
        std::cout<<"SearchableNamesOffset: "<<stoul(SearchableNamesOffset,0,16)<<std::endl;
        ThumbnailTableOffset=info_finder(4,file);
        std::cout<<"ThumbnailTableOffset: "<<stoul(ThumbnailTableOffset,0,16)<<std::endl;
        GUID = finder(16,file);
        std::cout<<"GUID: "<<GUID<<std::endl;
        PersistentGUID = finder(16,file);
        std::cout<<"PersistentGUID: "<<PersistentGUID<<std::endl;
        GenerationsCount=stoi(info_finder(4,file),0,16);
        std::cout<<"GenerationsCount: "<<GenerationsCount<<std::endl;
        Generations(GenerationsCount,file);
        SavedByEngineVersion_CompatibleWithEngineVersion(file);
        CompressedFlags = finder(4,file);
        std::cout<<"CompressedFlags: "<< CompressedFlags<<std::endl;
        CompressedChunksCount = stol(info_finder(4,file)); //isko bhi formatting krni hai
        std::cout<<"CompressedChunksCount: "<<CompressedChunksCount<<std::endl;
        PackageSource = info_finder(4,file);
        std::cout<<"PackageSource: "<<stoll((PackageSource),0,16)<<std::endl;
         fseek(file,ftell(file) + 4,SEEK_SET); // yaha pr shyd koi ek property hai. ya toh NumTexture Allocations hai ya AdditionalDatatoCook hai
        AssetRegistryDataOffset= stoll(info_finder(4,file),0,16);
        std::cout<<"AssetregistryDataOffset: "<<AssetRegistryDataOffset<<std::endl;
        BulkDataStartOffset = stoll(info_finder(8,file),0,16);
        std::cout<<"BulkDataStartOffset: "<<BulkDataStartOffset<<std::endl;
        WorlTileInfoDataOffset = stoll(info_finder(4,file),0,16);
        std::cout<<"WorlTileInfoDataOffset: "<<WorlTileInfoDataOffset<<std::endl;
        ChunkIDs= finder(4,file); //ek baar ye chunk krke dekhna hai, isi byte pr milengi ids
        std::cout<<"ChunkIDs: "<<ChunkIDs<<std::endl;
        PreloadDependencyCount = stoll(info_finder(4,file),0,16);
        std::cout<<"PreloadDependencyCount: "<<PreloadDependencyCount<<std::endl;
        PreloadDependencyOffset = stoll(info_finder(4,file),0,16);
        std::cout<<"PreloadDependencyOffset: "<<PreloadDependencyOffset<<std::endl;
        NamesReferencedFromExportDataCount = stoll(info_finder(4,file),0,16);
        std::cout<<"NamesReferencedFromExportDataCount: "<<NamesReferencedFromExportDataCount<<std::endl;
        PayloadTocOffset=stoull(info_finder(8,file),0,16);
        std::cout<<"PayloadTocOffset: "<<PayloadTocOffset<<std::endl;

        // std::cout<<"=============Names========"<<std::endl;
        // printNames(NameOffset,NameCount,file);    
        // printGatherableData(GatherableTextDataOffset,GatherableTextDataCount,file);
        
        // generateJSONdata();
        //after the filename pointer
        //Now pointer
        //gatherabletextData
            //NamespaceName till '\x00'
            // SourceData
                // SourceString: +4 till'\x00'
                    //SourceStringMetaData: 
            // SourceKeycontexts        
}
}
}
