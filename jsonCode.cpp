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
#include <cstdlib>
#include <string>
#include <cstdio>
#include <sstream>

std::vector<std::string> Names;

std::string hexToAscii(std::string s)
{
    std::string result;
    for (int i = 0; i < s.length(); i += 2)
    {
        std::string hex = s.substr(i, 2);
        char ch = stoul(hex, 0, 16);
        result += ch;
    }
    return result;
}
std::string little_to_big_endian(std::string s)
{
    std::string s1 = "";
    for (int i = s.length(); i >= 2; i -= 2)
    {
        s1 += s.substr(i - 2, 2);
    }
    return s1;
}
std::string bytesToHex(const char *buffer, size_t length)
{
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    for (size_t i = 0; i < length; ++i)
    {
        ss << std::setw(2) << static_cast<unsigned int>(static_cast<unsigned char>(buffer[i]));
    }
    return ss.str();
}
std::string finder(long long num, FILE *file)
{
    char *buf = new char[num];
    // allocation helps me to not look for the null terminated strings but resolve the problem as it now allocates a new space for the buffer obtained
    int bytes_read = fread(buf, sizeof(char), num * sizeof(char), file);
    if (bytes_read != num)
    {
        delete[] buf;
        throw std::runtime_error("Error reading from file");
    }
    std::string s = bytesToHex(buf, num);
    return s;
}
std::string info_finder(int num, FILE *file)
{
    return (little_to_big_endian(finder(num, file)));
}
int bytes_detector(std::string bytes, long int pos, FILE *file)
{
    std::string file_bytes = finder(4, file);
    while (file_bytes != bytes)
    {
        pos += 4;
        fseek(file, ftell(file) + 4, SEEK_SET);
        file_bytes = finder(4, file);
    }
    return pos;
}
void printGatherableData(unsigned long long GatherableDataOffset, unsigned long long GatherableDataCount, FILE *file)
{
    if (!GatherableDataCount)
    {
        std::cout << "No Gatherable Text Data Available" << std::endl;
    }

    while (GatherableDataCount--)
    {
        std::string NamespaceName = "";
        std::string SourceString = "";
        std::string KeyName = "";
        std::string SiteDescription = "";
        int IsEditorOnly;
        char ch;

        while ((ch = getc(file)) != EOF && ch != '\x00')
        {
            NamespaceName += ch;
        }
        fseek(file, 4, SEEK_CUR);

        while ((ch = getc(file)) != EOF && ch != '\x00')
        {
            SourceString += ch;
        }
        fseek(file, 4, SEEK_CUR);

        IsEditorOnly = stoi(info_finder(4, file));
        fseek(file, 4, SEEK_CUR);

        while (ch = getc(file) != EOF && ch != '\x00')
        {
            KeyName += ch;
        }
        fseek(file, 4, SEEK_CUR);

        while (ch = getc(file) != EOF && ch != '\x00')
        {
            SiteDescription += ch;
        }

        std::cout << "\tGatherable Text Data: " << std::endl;
        std::cout << "\t\tNameSpaceName: " << NamespaceName << std::endl;
        std::cout << "\t\t\tSourceString: " << SourceString << std::endl;
        std::cout << "\t\t\tKeyName: " << KeyName << std::endl;
        std::cout << "\t\t\tIsEditorOnly: " << IsEditorOnly << std::endl;
        std::cout << "\t\t\tSite Description: " << SiteDescription << std::endl;
    }
}
void printNames(unsigned long long NameOffset, unsigned long long NameCount, FILE *file) {
    fseek(file, NameOffset + 4, SEEK_SET);

    std::stringstream ss;
    
    ss << "\"Names\": [\n";

    

    for (int i = 0; i < NameCount; i++) {
        std::string name;
        std::string NonCasePreservingHash;
        std::string CasePreservingHash;

        char ch;
        while ((ch = getc(file)) != EOF && ch != 0) {
            if (ch != '\x00') {
                name += ch;
            }
        }
        NonCasePreservingHash = info_finder(2, file);
        CasePreservingHash = info_finder(2, file);

        ss << "\t{\n";
        ss << "\t\t\"Index\":\""<< i <<"\",\n";
        ss << "\t\t\"Name\": \"" << name << "\",\n";
        ss << "\t\t\"CasePreservingHash\": " << stoul(CasePreservingHash, 0, 16) << ",\n";
        ss << "\t\t\"NonCasePreservingHash\": " << stoul(NonCasePreservingHash, 0, 16) << "\n";
        ss << "\t}";
        if (i < NameCount - 1) {
            ss << ",";
        }
        ss << "\n";

        Names.push_back(name);

        fseek(file, ftell(file) + 4, SEEK_SET);
    }

    ss << "\t]\n";
  

  
    std::cout << ss.str();
}


void Generations(int GenerationCount, FILE *file) {
    std::stringstream ss;
    

    std::string Generation;
    std::string nameCount;
    std::string exportCount;
    if (GenerationCount > 0) {
        ss << "\"Generations\": [\n";
        for (int i = 0; i < GenerationCount; i++) {
            ss << "\t{\n";
            ss << "\t\t\"Generation\": " << i << ",\n";
            exportCount = info_finder(4, file);
            nameCount = info_finder(4, file);
            ss << "\t\t\"exportCount\": " << stoul(exportCount, 0, 16) << ",\n";
            ss << "\t\t\"nameCount\": " << stoul(nameCount, 0, 16) << "\n";
            ss << "\t}";
            if (i < GenerationCount) {
                ss << ",";
            }
            ss << "\n";
        }
        ss << "\t]\n";
    }
    

   
    std::cout << ss.str();
}
void SavedByEngineVersion_CompatibleWithEngineVersion(FILE *file)
{     std::stringstream ss;
    if ((info_finder(28, file)) == "00000000000000000000000000000000000000000000000000000000")
    {
       ss << "\"SavedByEngineVersion\": "<< " " << ",\n";
       ss << "\"CompatibleWithEngineVersion\": "<< " " << "\n";
    }
    else
    {
        fseek(file, ftell(file) - 28, SEEK_SET);
        std::string string;
        string = std::to_string(stoi(info_finder(2, file), 0, 16)) + "." + std::to_string(stoi(info_finder(2, file), 0, 16)) + "." + std::to_string(stoi(info_finder(2, file), 0, 16));
        std::reverse(string.begin(), string.end());
        long long number = stoull(info_finder(4, file), 0, 16);
        fseek(file, ftell(file) + 4, SEEK_SET);
        std::string version;
        for (int i = 1;; i)
        {
            if ((finder(i, file)) == "00")
            {
                break;
            }
            fseek(file, ftell(file) - 1, SEEK_SET);
            version += hexToAscii(finder(i, file));
        }
        ss << "\"SavedByEngineVersion\": " << string << "-" << number << version << ",\n";

        string = std::to_string(stoi(info_finder(2, file), 0, 16)) + "." + std::to_string(stoi(info_finder(2, file), 0, 16)) + "." + std::to_string(stoi(info_finder(2, file), 0, 16));
        std::reverse(string.begin(), string.end());
        number = stoull(info_finder(4, file), 0, 16);
        fseek(file, ftell(file) + 4, SEEK_SET);
        std::string version_1;
        for (int i = 1;; i)
        {
            if ((finder(i, file)) == "00")
            {
                break;
            }
            fseek(file, ftell(file) - 1, SEEK_SET);
            version_1 += hexToAscii(finder(i, file));
        }
        ss << "\"CompatibleWithEngineVersion\": " << string << "-" << number << version_1 << ",\n";
    }

    
    std::cout << ss.str();
}



void Thumbnails(long int ThumbnailTableOffset, FILE *file) {
    std::stringstream ss;
    fseek(file, ThumbnailTableOffset, SEEK_SET);
    int Number = stoi(info_finder(4, file), 0, 16);
    int ObjectClassNameLength;
    int ObjectPathWithoutPackageLength;
    std::string fileOffset;
    std::string ObjectClassName;
    std::string ObjectPathWithoutPackageName;

   
    ss << "\"Thumbnails\": [\n";
    for (int i = 0; i < Number; i++) {
        ObjectClassNameLength = stoi(info_finder(4, file), 0, 16);
        ObjectClassName = hexToAscii(finder(ObjectClassNameLength, file));
        ObjectPathWithoutPackageLength = stoi(info_finder(4, file), 0, 16);
        ObjectPathWithoutPackageName = hexToAscii(finder(ObjectPathWithoutPackageLength, file));
        fileOffset = info_finder(4, file);

        ss << "\t{\n";
        ss << "\t\t\"Index\": " << i << ",\n";
        ss << "\t\t\"ObjectClassName\": \"" << ObjectClassName << "\",\n";
        ss << "\t\t\"ObjectPathWithoutPackageName\": \"" << ObjectPathWithoutPackageName << "\",\n";
        ss << "\t\t\"FileOffset\": \"" << fileOffset << "\"\n";
        ss << "\t}";
        if (i < Number - 1) {
            ss << ",";
        }
        ss << "\n";
    }
    ss << "\t]\n";
   

   
    std::cout << ss.str();
}


void Depends(long int DependsOffset, FILE *file) {
    fseek(file, DependsOffset, SEEK_SET);
    signed long int FPackageIndex;
    int Count = stoi(info_finder(4, file), 0, 16);

    std::stringstream ss;
    ss << "\"Depends\": [\n";

    if (Count != 0) {
        for (int i = 0; i < Count; i++) {
            FPackageIndex = stoll(info_finder(4, file), 0, 16);

            ss << "\t{\n";
            ss << "\t\t\"FPackageIndex\": " << FPackageIndex << "\n";
            ss << "\t}";
            if (i < Count - 1) {
                ss << ",";
            }
            ss << "\n";
        }
    } else {
        ss << "\t{\n";
        ss << "\t\t\"NoDepends\": true\n";
        ss << "\t}\n";
    }

    ss << "]\n";
   
    std::cout << ss.str();
}

void SoftPackageReferences(long int Offset, long int count, FILE *file) {
    unsigned long long nameIndex;
    fseek(file, Offset, SEEK_SET);

    std::stringstream ss;
    
    std::cout << "\"SoftPackageReferences\": [\n";

    if (count != 0) {
        for (long int i = 0; i < count; i++) {
            nameIndex = stoull(info_finder(8, file), 0, 16);

            std::cout << "\t{\n";
            std::cout << "\t\t\"nameIndex\": " << nameIndex << ",\n";
            std::cout << "\t\t\"Name\": \"" << Names[nameIndex] << "\"\n";
            std::cout << "\t}";
            if (i < count - 1) {
                std::cout << ",";
            }
            std::cout << "\n";
        }
    } else {
        std::cout << "\t{\n";
        std::cout << "\t\t\"NoSoftPackageReferences\": true\n";
        std::cout << "\t}\n";
    }

    std::cout << "]\n";
   
}

void AssetRegistryData(long long int AssetRegistryDataOffset, long long int WorlTileInfoDataOffset, long long HeaderSize, FILE *file) {
    long long int offset = HeaderSize;
    std::string ObjectPath;
    std::string ObjectClass;
    long int TagCount;
    std::string key;
    std::string value;

    fseek(file, AssetRegistryDataOffset, SEEK_SET);
    if (WorlTileInfoDataOffset > 0) {
        offset = WorlTileInfoDataOffset;
    }
    unsigned long long int AssetRegistryDataSize = offset - AssetRegistryDataOffset;
    unsigned long long int DependencyDataOffset = stoll(info_finder(8, file), 0, 16);

    std::stringstream ss;

    ss << "\"DependecyDataOffset\": " << DependencyDataOffset << ",\n";
    ss << "\"AssetRegistryDataSize\": " << AssetRegistryDataSize << ",\n";
    ss << "\"AssetData\": [\n";

    long AssetDatacount = stoll(info_finder(4, file), 0, 16);
    for (int i = 0; i < AssetDatacount; i++) {
        long int ObjectPathlength = stol(info_finder(4, file), 0, 16);
        ObjectPath = hexToAscii(finder(ObjectPathlength, file));

        long int ObjectClasslength = stol(info_finder(4, file), 0, 16);
        ObjectClass = hexToAscii(finder(ObjectClasslength, file));

        TagCount = stoll(info_finder(4, file), 0, 16);

        ss << "\t{\n";
        ss << "\t\t\"ObjectPath\": \"" << ObjectPath << "\",\n";
        ss << "\t\t\"ObjectClass\": \"" << ObjectClass << "\",\n";
        ss << "\t\t\"Tags\": [\n";
        for (int j = 0; j < TagCount; j++) {
            long int keylength = stoll(info_finder(4, file), 0, 16);
            key = hexToAscii(finder(keylength, file));

            signed long valuelength = stoll(info_finder(4, file), 0, 16);
            if (valuelength < 0) {
                valuelength = 2 * abs(valuelength);
            }
            value = hexToAscii(finder(valuelength, file));

            ss << "\t\t\t{\n";
            ss << "\t\t\t\t\"Key_tag\": \"" << key << "\",\n";
            ss << "\t\t\t\t\"Value_tag\": \"" << value << "\"\n";
            ss << "\t\t\t}";
            if (j < TagCount - 1) {
                ss << ",";
            }
            ss << "\n";
        }
        ss << "\t\t]\n";
        ss << "\t}";
        if (i < AssetDatacount - 1) {
            ss << ",";
        }
        ss << "\n";
    }
    ss << "]\n";
    std::cout << ss.str();
}


void SearchableNameOffset(long long SearchableNamesOffset, FILE *file) {
    fseek(file, SearchableNamesOffset, SEEK_SET);
    long count = stol(info_finder(4, file), 0, 16);
    std::stringstream ss;
    ss << "\"SearchableNames\": [\n";

    if (count == 0) {
        ss << "\t{\n";
        ss << "\t\t\"NoSearchableNames\": true\n";
        ss << "\t}\n";
    } else {
        for (long i = 0; i < count; i++) {
            int SearchableNameLength = stoi(info_finder(4, file), 0, 16);
            std::string SearchableName = hexToAscii(finder(SearchableNameLength, file));

            ss << "\t{\n";
            ss << "\t\t\"SearchableName\": \"" << SearchableName << "\"\n";
            ss << "\t}";
            if (i < count - 1) {
                ss << ",";
            }
            ss << "\n";
        }
    }
    ss << "]\n";
    std::cout << ss.str();
}



void Imports(unsigned long long ImportCount, unsigned long long ImportOffset, FILE *file) {
    std::stringstream ss;
   
    ss << "\"Imports\": [\n";

    signed long outerIndex;
    fseek(file, ImportOffset, SEEK_SET);
    for (int i = 0; i < ImportCount; i++) {
        unsigned long long classPackageIndex = stoull(info_finder(8, file), 0, 16);
        unsigned long long classNameIndex = stoull(info_finder(8, file), 0, 16);
        outerIndex = stoll(info_finder(4, file), 0, 16);
        unsigned long long objectNameIndex = stoull(info_finder(8, file), 0, 16);
        unsigned long long packageNameIndex = stoll(info_finder(4, file), 0, 16);
        unsigned long long bImportOptional = stoll(info_finder(4, file), 0, 16);

        ss << "\t{\n";
        ss << "\t\t\"import\": " << i << ",\n";
        ss << "\t\t\"ClassPackage\": \"" << Names[classPackageIndex] << "\",\n";
        ss << "\t\t\"ClassName\": \"" << Names[classNameIndex] << "\",\n";
        ss << "\t\t\"OuterIndex\": " << outerIndex << ",\n";
        ss << "\t\t\"ObjectName\": \"" << Names[objectNameIndex] << "\",\n";
        ss << "\t\t\"PackageName\": \"" << Names[packageNameIndex] << "\",\n";
        ss << "\t\t\"bImportOptional\": " << bImportOptional << "\n";
        ss << "\t}";
        if (i < ImportCount - 1) {
            ss << ",";
        }
        ss << "\n";

        fseek(file, ftell(file) + 4, SEEK_SET);
    }

    ss << "]\n";

    std::cout << ss.str();
}


void Exports(long long Offset, long long count, FILE *file) {
    std::stringstream ss;
 
    ss << "\"Exports\": [\n";

    for (int i = 0; i < count; i++) {
        signed long ClassIndex = stoll(info_finder(4, file), 0, 16);
        signed long SuperIndex = stoll(info_finder(4, file), 0, 16);
        signed long TemplateIndex = stoll(info_finder(4, file), 0, 16);
        signed long OuterIndex = stoll(info_finder(4, file), 0, 16);
        long long index = stoll(info_finder(8, file), 0, 16);
        std::string ObjectName = Names[index];
        signed long ObjectFlags = stoll(info_finder(4, file), 0, 16);
        long long SerialSize = stoll(info_finder(8, file), 0, 16);
        long long SerialOffset = stoull(info_finder(8, file), 0, 16);
        signed long bForcedExport = stoll(info_finder(4, file), 0, 16);
        signed long bNotForClient = stoll(info_finder(4, file), 0, 16);
        signed long bNotForServer = stoll(info_finder(4, file), 0, 16);
        std::string packageGuid = info_finder(16, file);
        signed long packageFlags = stoll(info_finder(4, file), 0, 16);
        signed long bNotAlwaysLoadedForEditorGame = stoll(info_finder(4, file), 0, 16);
        signed long bIsAsset = stoll(info_finder(4, file), 0, 16);
        signed long bGeneratePublicHash = stoll(info_finder(4, file), 0, 16);
        signed long FirstExportDependency = stoll(info_finder(4, file), 0, 16);
        signed long SerializationBeforeSerializationDependency = stoll(info_finder(4, file), 0, 16);
        signed long createBeforeSerializationDependency = stoll(info_finder(4, file), 0, 16);
        signed long serializationBeforeCreateDependency = stoll(info_finder(4, file), 0, 16);
        signed long createBeforeCreateDependency = stoll(info_finder(4, file), 0, 16);

        ss << "\t{\n";
        ss << "\t\t\"Export\": " << i << ",\n";
        ss << "\t\t\"ClassIndex\": " << ClassIndex << ",\n";
        ss << "\t\t\"SuperIndex\": " << SuperIndex << ",\n";
        ss << "\t\t\"TemplateIndex\": " << TemplateIndex << ",\n";
        ss << "\t\t\"OuterIndex\": " << OuterIndex << ",\n";
        ss << "\t\t\"ObjectName\": \"" << ObjectName << "\",\n";
        ss << "\t\t\"ObjectFlags\": " << ObjectFlags << ",\n";
        ss << "\t\t\"SerialSize\": " << SerialSize << ",\n";
        ss << "\t\t\"SerialOffset\": " << SerialOffset << ",\n";
        ss << "\t\t\"bForcedExport\": " << bForcedExport << ",\n";
        ss << "\t\t\"bNotForClient\": " << bNotForClient << ",\n";
        ss << "\t\t\"bNotForServer\": " << bNotForServer << ",\n";
        ss << "\t\t\"packageGuid\": \"" << packageGuid << "\",\n";
        ss << "\t\t\"packageFlags\": " << packageFlags << ",\n";
        ss << "\t\t\"bNotAlwaysLoadedForEditorGame\": " << bNotAlwaysLoadedForEditorGame << ",\n";
        ss << "\t\t\"bIsAsset\": " << bIsAsset << ",\n";
        ss << "\t\t\"bGeneratePublicHash\": " << bGeneratePublicHash << ",\n";
        ss << "\t\t\"FirstExportDependency\": " << FirstExportDependency << ",\n";
        ss << "\t\t\"SerializationBeforeSerializationDependency\": " << SerializationBeforeSerializationDependency << ",\n";
        ss << "\t\t\"createBeforeSerializationDependency\": " << createBeforeSerializationDependency << ",\n";
        ss << "\t\t\"serializationBeforeCreateDependency\": " << serializationBeforeCreateDependency << ",\n";
        ss << "\t\t\"createBeforeCreateDependency\": " << createBeforeCreateDependency << "\n";
        ss << "\t}";
        if (i < count - 1) {
            ss << ",";
        }
        ss << "\n";
    }

    ss << "]\n";
   
    std::cout << ss.str();
}



  void to_json(int header_length, FILE* file) {
   
    std::cout << "{\n";

    std::string header = finder(header_length, file);
    std::string EpackedFileTag = little_to_big_endian(header.substr(0, 8));
    std::string LegacyFileVersion = header.substr(8, 8);
    std::string LegacyUE3Version = header.substr(16, 8);
    std::string FileVersionUE4 = header.substr(24, 8);
    std::string FileVersionUE5 = info_finder(4, file);
    std::string FileVersionLicenseeUE4 = info_finder(4, file);
    std::string CustomVersionsCount = info_finder(4, file);
    ftell(file);
    unsigned long long versions = stoul(CustomVersionsCount, 0, 16);
    unsigned long long versionKeylength = versions * 16;
    unsigned long long total_version_bytes_length = versions * 4;
    int totalCustomVersionLength = versionKeylength + total_version_bytes_length;
    fseek(file, ftell(file) + totalCustomVersionLength, SEEK_SET);
    ftell(file);
    unsigned long long int HeaderSize = stoul(info_finder(4, file), 0, 16);
            
    std::cout << "\"Header\": \"" << header << "\",\n";
    std::cout << "\"EPackedFileTag\": " << std::stoul(EpackedFileTag, 0, 16) << ",\n";
    std::cout<< "\"LegacyFileVersion\": \"" << LegacyFileVersion << "\",\n";
    std::cout << "\"LegacyUE3Version\": " << std::stoi(little_to_big_endian(LegacyUE3Version), 0, 16) << ",\n";
    std::cout << "\"FileVersionUE4\": " << std::stoi(little_to_big_endian(FileVersionUE4), 0, 16) << ",\n";
    std::cout << "\"FileVersionUE5\": " << std::stoul(FileVersionUE5, 0, 16) << ",\n";
    std::cout << "\"FileVersionLicenseeUE4\": " << std::stoul(FileVersionLicenseeUE4, 0, 16) << ",\n";
    std::cout << "\"CustomVersionsCount\": " << versions <<",\n";
    std::cout << "\"Total Header Size\": " << HeaderSize << ",\n";
    unsigned long long FolderNameSize = stoul(info_finder(4, file), 0, 16);
    std::string FolderName = finder(FolderNameSize, file).substr(0, FolderNameSize * 2 - 2);
    std::cout <<"\"FolderName\": " << hexToAscii(FolderName) << ",\n";
    std::string PackageFlags = info_finder(4, file);
    std::cout << "\"PackageFlags\": " << stoul((PackageFlags), 0, 16) << ",\n";
    unsigned long long NameCount = stoi(info_finder(4, file), 0, 16);
    std::cout << "\"NameCount\": " << NameCount <<",\n";
    unsigned long long NameOffset = stoi(info_finder(4, file), 0, 16);
    std::cout<< "\"NameOffset\": " << NameOffset << ",\n";
    long int pos = ftell(file);
    std::string Localisation_prefix = "21000000";
    bytes_detector(Localisation_prefix, pos, file);
    std::string LocalizationId = finder(32, file);
    std::cout << "\"LocalisationID\": " << hexToAscii(LocalizationId) << ",\n";
    fseek(file, ftell(file) + 1, SEEK_SET); // to skip the null byte
    unsigned long long GatherableTextDataCount = stoul(info_finder(4, file), 0, 16);
    std::cout << "\"GatherableTextDataCount\": " << GatherableTextDataCount << ",\n";
    unsigned long long GatherableTextDataOffset = stoul(info_finder(4, file), 0, 16);
    std::cout << "\"GatherableTextDataOffset\": " << GatherableTextDataOffset << ",\n";
    unsigned long long ExportCount = stol(info_finder(4, file), 0, 16);
    std::cout << "\"ExportCount\": " << ExportCount << ",\n";
    unsigned long long ExportOffset = stoul(info_finder(4, file), 0, 16);
    std::cout << "\"ExportOffset\": " << ExportOffset << ",\n";
    unsigned long long ImportCount= stoi(info_finder(4, file), 0, 16);
    std::cout << "\"ImportCount\": " << ImportCount << ",\n";
    unsigned long long ImportOffset = stoul(info_finder(4, file), 0, 16);
    std::cout << "\"ImportOffset\": " << ImportOffset << ",\n";
    unsigned long DependsOffset = stoul(info_finder(4, file), 0, 16);
    std::cout << "\"DependsOffset\": " << DependsOffset << ",\n";
    unsigned long SoftPackageReferencesCount= stoul(info_finder(4, file), 0, 16);
    std::cout << "\"SoftPackageReferencesCount\": " << SoftPackageReferencesCount << ",\n";
    unsigned long int SoftPackageReferencesOffset = stoul(info_finder(4, file), 0, 16);
    std::cout << "\"SoftPackageReferencesOffset\": " << SoftPackageReferencesOffset << ",\n";
    unsigned long long SearchableNamesOffset = stoul(info_finder(4, file), 0, 16);
    std::cout << "\"SearchableNamesOffset\": " << SearchableNamesOffset <<",\n";
    unsigned long ThumbnailTableOffset = stoul(info_finder(4, file), 0, 16);
    std::cout << "\"ThumbnailTableOffset\": " << ThumbnailTableOffset << ",\n";
    std::string GUID = finder(16, file);
    std::cout << "\"GUID\": " << GUID << ",\n";
    std::string PersistentGUID = finder(16, file);
    std::cout << "\"PersistentGUID\": " << PersistentGUID << ",\n";
    long int GenerationsCount = stoi(info_finder(4, file), 0, 16);
    std::cout << "\"GenerationsCount\": " << GenerationsCount << ",\n";
    Generations(GenerationsCount, file);
    SavedByEngineVersion_CompatibleWithEngineVersion(file);

    std::string CompressedFlags = finder(4, file);
    std::cout << "\"CompressedFlags\": " << CompressedFlags << ",\n";
    long CompressedChunksCount = stol(info_finder(4, file)); // isko bhi formatting krni hai
    std::cout << "\"CompressedChunksCount\": " << CompressedChunksCount << ",\n";
    std::string PackageSource = info_finder(4, file);
    std::cout << "\"PackageSource\": " << stoll((PackageSource), 0, 16) << ",\n";
    fseek(file, ftell(file) + 4, SEEK_SET); // yaha pr shyd koi ek property hai. ya toh NumTexture Allocations hai ya AdditionalDatatoCook hai
    unsigned long AssetRegistryDataOffset = stoll(info_finder(4, file), 0, 16);
    std::cout << "\"AssetregistryDataOffset\": " << AssetRegistryDataOffset << ",\n";
    unsigned long long BulkDataStartOffset = stoll(info_finder(8, file), 0, 16);
    std::cout << "\"BulkDataStartOffset\": " << BulkDataStartOffset << ",\n";
    unsigned long long WorlTileInfoDataOffset = stoll(info_finder(4, file), 0, 16);
    std::cout << "\"WorlTileInfoDataOffset\": " << WorlTileInfoDataOffset << ",\n";
    std::string ChunkIDs = finder(4, file); // ek baar ye chunk krke dekhna hai, isi byte pr milengi ids
    std::cout << "\"ChunkIDs\": " << ChunkIDs << ",\n";
    signed long PreloadDependencyCount = stoll(info_finder(4, file), 0, 16);
    std::cout << "\"PreloadDependencyCount\": " << PreloadDependencyCount << ",\n";
    unsigned long long PreloadDependencyOffset = stoll(info_finder(4, file), 0, 16);
    std::cout << "\"PreloadDependencyOffset\": " << PreloadDependencyOffset <<",\n";
    unsigned long NamesReferencedFromExportDataCount = stoll(info_finder(4, file), 0, 16);
    std::cout << "\"NamesReferencedFromExportDataCount\": " << NamesReferencedFromExportDataCount << ",\n";
    signed long long PayloadTocOffset = stoull(info_finder(8, file), 0, 16);
    std::cout << "\"PayloadTocOffset\": " << PayloadTocOffset << ",\n";

    Thumbnails(ThumbnailTableOffset, file);
    printNames(NameOffset, NameCount, file);
    Depends(DependsOffset, file);
    SoftPackageReferences(SoftPackageReferencesOffset, SoftPackageReferencesCount, file);
    
    SearchableNameOffset(SearchableNamesOffset, file);
    AssetRegistryData(AssetRegistryDataOffset, WorlTileInfoDataOffset, HeaderSize, file);
    Imports(ImportCount, ImportOffset, file);
    Exports(ExportOffset, ExportCount, file);

    std::cout << "}\n";
    
}


int main(int argc, char *argv[])
{
    if (argc < 2)
        std::cout << "Usage is ./header.exe {filename} " << std::endl;
    else
    {
        FILE *file;
        file = fopen(argv[1], "rb+");
        if (file == NULL)
        {
            std::cerr << "File not found" << std::endl;
        }
        else
        {
                      
            to_json(16,file) ; 
                   
        }
    }
}
