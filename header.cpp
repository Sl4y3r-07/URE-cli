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
void printNames(unsigned long long NameOffset, unsigned long long NameCount, FILE *file)
{
    fseek(file, NameOffset + 4, SEEK_SET);
    for (int i = 0; i < NameCount; i++)
    {
        std::string name;
        std::string NonCasePreservingHash;
        std::string CasePreservingHash;

        char ch;
        while ((ch = getc(file)) != EOF && ch != 0)
        {
            if (ch != '\x00')
            {
                name += ch;
            }
        }
        NonCasePreservingHash = info_finder(2, file);
        CasePreservingHash = info_finder(2, file);
        std::cout << "\tName " << i << "\t" << name << std::endl;
        Names.push_back(name);
        std::cout << "\t\tCasePreservingHash:\t" << stoul(CasePreservingHash, 0, 16) << std::endl;
        std::cout << "\t\tNonCasePreservingHash:\t" << stoul(NonCasePreservingHash, 0, 16) << std::endl;
        fseek(file, ftell(file) + 4, SEEK_SET);
    }
}
void Generations(int GenerationCount, FILE *file)
{
    std::string Generation;
    std::string nameCount;
    std::string exportCount;
    if (GenerationCount > 0)
    {
        for (int i = 1; i <= GenerationCount; i++)
        {
            std::cout << "\tGeneration " << i << std::endl;
            exportCount = info_finder(4, file);
            nameCount = info_finder(4, file);
            std::cout << "\t\texportCount:\t" << stoul(exportCount, 0, 16) << std::endl;
            std::cout << "\t\tnameCount:\t" << stoul(nameCount, 0, 16) << std::endl;
        }
    }
}
void SavedByEngineVersion_CompatibleWithEngineVersion(FILE *file)
{
    if ((info_finder(28, file)) == "00000000000000000000000000000000000000000000000000000000")
    {
        std::cout << "SavedByEngineVersion: "
                  << " " << std::endl;
        std::cout << "CompatibleWithEngineVersion: "
                  << " " << std::endl;
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
        std::cout << "SavedByEngineVersion: " << string << "-" << number << version << std::endl;

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
        std::cout << "CompatibleWithEngineVersion: " << string << "-" << number << version_1 << std::endl;
    }
}
void Thumbnails(long int ThumbnailTableOffset, FILE *file)
{
    fseek(file, ThumbnailTableOffset, SEEK_SET);
    int Number = stoi(info_finder(4, file), 0, 16);
    int ObjectClassNameLength;
    int ObjectPathWithoutPackageLength;
    std::string fileOffset;
    std::string ObjectClassName;
    std::string ObjectPathWithoutPackageName;
    std::cout << "Thumbnail Info" << std::endl;
    for (int i = 0; i < Number; i++)
    {
        std::cout << "\tIndex: " << i << std::endl;
        ObjectClassNameLength = stoi(info_finder(4, file), 0, 16);
        ObjectClassName = hexToAscii(finder(ObjectClassNameLength, file));
        ObjectPathWithoutPackageLength = stoi(info_finder(4, file), 0, 16);
        ObjectPathWithoutPackageName = hexToAscii(finder(ObjectPathWithoutPackageLength, file));
        fileOffset = info_finder(4, file);
        std::cout << "\t\tObjectClassName: " << ObjectClassName << std::endl;
        std::cout << "\t\tObjectPathWithoutPackageName: " << ObjectPathWithoutPackageName << std::endl;
        std::cout << "\t\tFileOffset: " << fileOffset << std::endl;
    }
}

void Depends(long int DependsOffset, FILE *file)
{
    fseek(file, DependsOffset, SEEK_SET);
    signed long int FPackageIndex;
    int Count = stoi(info_finder(4, file), 0, 16);
    std::cout << "Depends" << std::endl;
    if (Count != 0)
    {
        for (int i = 0; i < Count; i++)
        {
            FPackageIndex = stoll(info_finder(4, file), 0, 16);
            std::cout << "\t\tFPackageIndex: " << FPackageIndex << std::endl;
        }
    }
    else
    {
        std::cout << "\tNo Depends " << std::endl;
    }
}

void SoftPackageReferences(long int Offset, long int count, FILE *file)
{
    unsigned long long nameIndex;
    fseek(file, Offset, SEEK_SET);
    std::cout << "SoftPackageReferences: " << std::endl;
    if (count != 0)
    {
        for (long int i = 0; i < count; i++)
        {
            nameIndex = stoull(info_finder(8, file), 0, 16);
            std::cout << "\t nameIndex: " << nameIndex << std::endl;
            std::cout << "\t Name: " << Names[nameIndex] << std::endl;
        }
    }
    else
    {
        std::cout << "No SoftPAckageReferences " << std::endl;
    }
}

void AssetRegistryData(long long int AssetRegistryDataOffset, long long int WorlTileInfoDataOffset, long long HeaderSize, FILE *file)
{
    long long int offset = HeaderSize;
    std::string ObjectPath;
    std::string ObjectClass;
    long int TagCount;
    std::string key;
    std::string value;

    fseek(file, AssetRegistryDataOffset, SEEK_SET);
    if (WorlTileInfoDataOffset > 0)
    {
        offset = WorlTileInfoDataOffset;
    }
    unsigned long long int AssetRegistryDataSize = offset - AssetRegistryDataOffset;
    unsigned long long int DependencyDataOffset = stoll(info_finder(8, file), 0, 16);
    std::cout << "DependecyDataOffset: " << DependencyDataOffset << std::endl;
    std::cout << "AssetRegistryDataSize: " << AssetRegistryDataSize << std::endl;
    long AssetDatacount = stoll(info_finder(4, file), 0, 16);
    for (int i = 0; i < AssetDatacount; i++)
    {
        long int ObjectPathlength = stol(info_finder(4, file), 0, 16);
        ObjectPath = hexToAscii(finder(ObjectPathlength, file));
        std::cout << "\tObjectPath: " << ObjectPath << std::endl;
        long int ObjectClasslength = stol(info_finder(4, file), 0, 16);
        ObjectClass = hexToAscii(finder(ObjectClasslength, file));
        std::cout << "\tObjectClass: " << ObjectClass << std::endl;
        TagCount = stoll(info_finder(4, file), 0, 16);
        std::cout << "\tTagCount: " << TagCount << std::endl;
        for (int j = 0; j < TagCount; j++)
        {
            std::cout << "\t\tTag: " << j + 1 << std::endl;
            long int keylength = stoll(info_finder(4, file), 0, 16);
            key = hexToAscii(finder(keylength, file));
            std::cout << "\t\tKey_tag: " << key << std::endl;

            signed long valuelength = stoll(info_finder(4, file), 0, 16);
            std::cout << "\t\tvallength: " << valuelength << std::endl;
            if (valuelength < 0)
            {
                valuelength = 2 * abs(valuelength);
            }
            value = hexToAscii(finder(valuelength, file));
            std::cout << "\t\tValue_tag: " << value << std::endl;
        }
    }
}
void SearchableNameOffset(long long SearchableNamesOffset, FILE *file)
{
    fseek(file, SearchableNamesOffset, SEEK_SET);
    long count = stol(info_finder(4, file), 0, 16);
    int SearchableNamelength;
    std::string SearchableName;
    if (count == 0)
    {
        std::cout << "No Searchable Names present" << std::endl;
    }
    else
    {
        for (long i = 0; i < count; i++)
        {
            SearchableNamelength = stoi(info_finder(4, file), 0, 16);
            SearchableName = hexToAscii(finder(SearchableNamelength, file));
            std::cout << "\tSearchable Name: " << SearchableName << std::endl;
        }
    }
}
void Imports(unsigned long long ImportCount, unsigned long long ImportOffset, FILE *file)
{
    std::cout << "Imports " << std::endl;
    signed long outerIndex;
    fseek(file, ImportOffset, SEEK_SET);
    for (int i = 0; i < ImportCount; i++)
    {
        std::cout << "\timport: " << i << std::endl;
        unsigned long long classPackageIndex = stoull(info_finder(8, file), 0, 16);
        std::cout << "\t\tClassPackage: " << Names[classPackageIndex] << std::endl;
        unsigned long long classNameIndex = stoull(info_finder(8, file), 0, 16);
        std::cout << "\t\tClassName: " << Names[classNameIndex] << std::endl;
        outerIndex = stoll(info_finder(4, file), 0, 16);
        std::cout << "\t\touterIndex: " << outerIndex << std::endl;
        unsigned long long objectNameIndex = stoull(info_finder(8, file), 0, 16);
        std::cout << "\t\tObjectName: " << Names[objectNameIndex] << std::endl;
        unsigned long long packageNameIndex = stoll(info_finder(4, file), 0, 16);
        std::cout << "\t\tPackageName: " << Names[packageNameIndex] << std::endl;
        unsigned long long bImportOptional = stoll(info_finder(4, file), 0, 16);
        std::cout << "\t\tbImportOptional: " << bImportOptional << std::endl;
        fseek(file, ftell(file) + 4, SEEK_SET);
    }
}
void Exports(long long Offset, long long count, FILE *file)
{
    std::cout << "Export: " << std::endl;
    signed long ClassIndex;
    signed long SuperIndex;
    signed long TemplateIndex;
    signed long OuterIndex;
    std::string ObjectName;
    signed long ObjectFlags;
    long long SerialSize;
    long long SerialOffset;
    signed long bForcedExport;
    signed long bNotForClient;
    signed long bNotForServer;
    std::string packageGuid;
    signed long packageFlags;
    signed long bNotAlwaysLoadedForEditorGame;
    signed long bIsAsset;
    signed long bGeneratePublicHash;
    signed long FirstExportDependency;
    signed long SerializationBeforeSerializationDependency;
    signed long createBeforeSerializationDependency;
    signed long serializationBeforeCreateDependency;
    signed long createBeforeCreateDependency;
    fseek(file, Offset, SEEK_SET);
    for (int i = 0; i < count; i++)
    {

        std::cout << "\tExport: " << i << std::endl;
        ClassIndex = stoll(info_finder(4, file), 0, 16);
        SuperIndex = stoll(info_finder(4, file), 0, 16);
        std::cout << "\t\tClassIndex:\t" << ClassIndex << std::endl;
        std::cout << "\t\tSuperIndex:\t" << SuperIndex << std::endl;
        TemplateIndex = stoll(info_finder(4, file), 0, 16);
        OuterIndex = stoll(info_finder(4, file), 0, 16);
        long long index = stoll(info_finder(8, file), 0, 16);
        ObjectName = Names[index];
        ObjectFlags = stoll(info_finder(4, file), 0, 16);
        SerialSize = stoll(info_finder(8, file), 0, 16);
        SerialOffset = stoull(info_finder(8, file), 0, 16);
        bForcedExport = stoll(info_finder(4, file), 0, 16);
        bNotForClient = stoll(info_finder(4, file), 0, 16);
        bNotForServer = stoll(info_finder(4, file), 0, 16);
        packageGuid = info_finder(16, file);
        packageFlags = stoll(info_finder(4, file), 0, 16);
        bNotAlwaysLoadedForEditorGame = stoll(info_finder(4, file), 0, 16);
        bIsAsset = stoll(info_finder(4, file), 0, 16);
        bGeneratePublicHash = stoll(info_finder(4, file), 0, 16);
        FirstExportDependency = stoll(info_finder(4, file), 0, 16);
        SerializationBeforeSerializationDependency = stoll(info_finder(4, file), 0, 16);
        createBeforeSerializationDependency = stoll(info_finder(4, file), 0, 16);
        serializationBeforeCreateDependency = stoll(info_finder(4, file), 0, 16);
        createBeforeCreateDependency = stoll(info_finder(4, file), 0, 16);

        std::cout << "\t\tTemplateIndex:\t" << TemplateIndex << std::endl;
        std::cout << "\t\tOuterIndex:\t" << OuterIndex << std::endl;
        std::cout << "\t\tObjectName:\t" << ObjectName << std::endl;
        std::cout << "\t\tObjectFlags:\t" << ObjectFlags << std::endl;
        std::cout << "\t\tSerialSize:\t" << SerialSize << std::endl;
        std::cout << "\t\tSerialOffset:\t" << SerialOffset << std::endl;
        std::cout << "\t\tbForcedExport:\t" << bForcedExport << std::endl;
        std::cout << "\t\tbNotForClient:\t" << bNotForClient << std::endl;
        std::cout << "\t\tbNotForServer:\t" << bNotForServer << std::endl;
        std::cout << "\t\tpackageGuid:\t" << packageGuid << std::endl;
        std::cout << "\t\tpackageFlags:\t" << packageFlags << std::endl;
        std::cout << "\t\tbNotAlwaysLoadedForEditorGame:\t" << bNotAlwaysLoadedForEditorGame << std::endl;
        std::cout << "\t\tbIsAsset:\t" << bIsAsset << std::endl;
        std::cout << "\t\tbGeneratePublicHash:\t" << bGeneratePublicHash << std::endl;
        std::cout << "\t\tFirstExportDependency:\t" << FirstExportDependency << std::endl;
        std::cout << "\t\tSerializationBeforeSerializationDependency:\t" << SerializationBeforeSerializationDependency << std::endl;
        std::cout << "\t\tcreateBeforeSerializationDependency:\t" << createBeforeSerializationDependency << std::endl;
        std::cout << "\t\tserializationBeforeCreateDependency:\t" << serializationBeforeCreateDependency << std::endl;
        std::cout << "\t\tcreateBeforeCreateDependency:\t" << createBeforeCreateDependency << std::endl;
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
            std::string EpackedFileTag;
            std::string header;
            std::string LegacyUE3Version;
            std::string LegacyFileVersion;
            std::string FileVersionUE5;
            std::string FileVersionUE4;
            std::string CustomVersionsCount;
            std::string FileVersionLicenseeUE4;
            std::string LocalizationId;
            std::string FolderName;
            std::string PackageFlags;
            std::string GUID;
            std::string PersistentGUID;
            std::string CompressedFlags;
            std::string PackageSource;
            std::string ChunkIDs;

            long int GenerationsCount;
            unsigned long DependsOffset;
            unsigned long ThumbnailTableOffset;
            unsigned long SoftPackageReferencesCount;
            unsigned long long ImportOffset;
            unsigned long long ImportCount;
            unsigned long long ExportCount;
            unsigned long long ExportOffset;
            unsigned long int SoftPackageReferencesOffset;
            unsigned long long NameCount;
            unsigned long long NameOffset;
            unsigned long long SearchableNamesOffset;
            unsigned long long GatherableTextDataOffset;
            unsigned long long int HeaderSize;
            unsigned long long GatherableTextDataCount;
            long CompressedChunksCount;
            unsigned long AssetRegistryDataOffset;
            unsigned long long BulkDataStartOffset;
            unsigned long long WorlTileInfoDataOffset;
            signed long PreloadDependencyCount;
            unsigned long long PreloadDependencyOffset;
            unsigned long NamesReferencedFromExportDataCount;
            signed long long PayloadTocOffset;
            header = finder(16, file);
            std::cout << "Header: " << header << std::endl;
            EpackedFileTag = little_to_big_endian(header.substr(0, 8));
            std::cout << "EPackedFileTag: " << stoul(EpackedFileTag, 0, 16) << std::endl;
            LegacyFileVersion = header.substr(8, 8);
            std::cout << "LegacyFileVersion: " << LegacyFileVersion << std::endl;
            LegacyUE3Version = header.substr(16, 8);
            std::cout << "LegacyUE3Version: " << stoi(little_to_big_endian(LegacyUE3Version), 0, 16) << std::endl;
            FileVersionUE4 = header.substr(24, 8);
            std::cout << "FileVersionUE4: " << stoi(little_to_big_endian(FileVersionUE4), 0, 16) << std::endl;
            FileVersionUE5 = info_finder(4, file);
            std::cout << "FileVersionUE5: " << stoul(FileVersionUE5, 0, 16) << std::endl;
            FileVersionLicenseeUE4 = info_finder(4, file);
            std::cout << "FileVersionLicenseeUE4: " << stoul(FileVersionLicenseeUE4, 0, 16) << std::endl;
            CustomVersionsCount = info_finder(4, file);
            unsigned long long versions = stoul(CustomVersionsCount, 0, 16);
            std::cout << "CustomVersionsCount: " << versions << std::endl;
            ftell(file);

            unsigned long long versionKeylength = versions * 16;
            unsigned long long total_version_bytes_length = versions * 4;
            int totalCustomVersionLength = versionKeylength + total_version_bytes_length;
            fseek(file, ftell(file) + totalCustomVersionLength, SEEK_SET);
            ftell(file);

            HeaderSize = stoul(info_finder(4, file), 0, 16);
            std::cout << "Total Header Size: " << HeaderSize << std::endl;
            unsigned long long FolderNameSize = stoul(info_finder(4, file), 0, 16);
            FolderName = finder(FolderNameSize, file).substr(0, FolderNameSize * 2 - 2);
            std::cout << "FolderName: " << hexToAscii(FolderName) << std::endl;
            PackageFlags = info_finder(4, file);
            std::cout << "PackageFlags: " << stoul((PackageFlags), 0, 16) << std::endl;
            NameCount = stoi(info_finder(4, file), 0, 16);
            std::cout << "NameCount: " << NameCount << std::endl;
            NameOffset = stoi(info_finder(4, file), 0, 16);
            std::cout << "NameOffset: " << NameOffset << std::endl;
            long int pos = ftell(file);
            std::string Localisation_prefix = "21000000";
            bytes_detector(Localisation_prefix, pos, file);
            LocalizationId = finder(32, file);
            std::cout << "LocalisationID: " << hexToAscii(LocalizationId) << std::endl;

            fseek(file, ftell(file) + 1, SEEK_SET); // to skip the null byte
            GatherableTextDataCount = stoul(info_finder(4, file), 0, 16);
            std::cout << "GatherableTextDataCount: " << GatherableTextDataCount << std::endl;
            GatherableTextDataOffset = stoul(info_finder(4, file), 0, 16);
            std::cout << "GatherableTextDataOffset: " << GatherableTextDataOffset << std::endl;
            ExportCount = stol(info_finder(4, file), 0, 16);
            std::cout << "ExportCount: " << ExportCount << std::endl;
            ExportOffset = stoul(info_finder(4, file), 0, 16);
            std::cout << "ExportOffset: " << ExportOffset << std::endl;
            ImportCount = stoi(info_finder(4, file), 0, 16);
            std::cout << "ImportCount: " << ImportCount << std::endl;
            ImportOffset = stoul(info_finder(4, file), 0, 16);
            std::cout << "ImportOffset: " << ImportOffset << std::endl;
            DependsOffset = stoul(info_finder(4, file), 0, 16);
            std::cout << "DependsOffset: " << DependsOffset << std::endl;
            SoftPackageReferencesCount = stoul(info_finder(4, file), 0, 16);
            std::cout << "SoftPackageReferencesCount: " << SoftPackageReferencesCount << std::endl;
            SoftPackageReferencesOffset = stoul(info_finder(4, file), 0, 16);
            std::cout << "SoftPackageReferencesOffset: " << SoftPackageReferencesOffset << std::endl;
            SearchableNamesOffset = stoul(info_finder(4, file), 0, 16);
            std::cout << "SearchableNamesOffset: " << SearchableNamesOffset << std::endl;
            ThumbnailTableOffset = stoul(info_finder(4, file), 0, 16);
            std::cout << "ThumbnailTableOffset: " << ThumbnailTableOffset << std::endl;
            GUID = finder(16, file);
            std::cout << "GUID: " << GUID << std::endl;
            PersistentGUID = finder(16, file);
            std::cout << "PersistentGUID: " << PersistentGUID << std::endl;
            GenerationsCount = stoi(info_finder(4, file), 0, 16);
            std::cout << "GenerationsCount: " << GenerationsCount << std::endl;
            Generations(GenerationsCount, file);
            SavedByEngineVersion_CompatibleWithEngineVersion(file);
            CompressedFlags = finder(4, file);
            std::cout << "CompressedFlags: " << CompressedFlags << std::endl;
            CompressedChunksCount = stol(info_finder(4, file)); // isko bhi formatting krni hai
            std::cout << "CompressedChunksCount: " << CompressedChunksCount << std::endl;
            PackageSource = info_finder(4, file);
            std::cout << "PackageSource: " << stoll((PackageSource), 0, 16) << std::endl;
            fseek(file, ftell(file) + 4, SEEK_SET); // yaha pr shyd koi ek property hai. ya toh NumTexture Allocations hai ya AdditionalDatatoCook hai
            AssetRegistryDataOffset = stoll(info_finder(4, file), 0, 16);
            std::cout << "AssetregistryDataOffset: " << AssetRegistryDataOffset << std::endl;
            BulkDataStartOffset = stoll(info_finder(8, file), 0, 16);
            std::cout << "BulkDataStartOffset: " << BulkDataStartOffset << std::endl;
            WorlTileInfoDataOffset = stoll(info_finder(4, file), 0, 16);
            std::cout << "WorlTileInfoDataOffset: " << WorlTileInfoDataOffset << std::endl;
            ChunkIDs = finder(4, file); // ek baar ye chunk krke dekhna hai, isi byte pr milengi ids
            std::cout << "ChunkIDs: " << ChunkIDs << std::endl;
            PreloadDependencyCount = stoll(info_finder(4, file), 0, 16);
            std::cout << "PreloadDependencyCount: " << PreloadDependencyCount << std::endl;
            PreloadDependencyOffset = stoll(info_finder(4, file), 0, 16);
            std::cout << "PreloadDependencyOffset: " << PreloadDependencyOffset << std::endl;
            NamesReferencedFromExportDataCount = stoll(info_finder(4, file), 0, 16);
            std::cout << "NamesReferencedFromExportDataCount: " << NamesReferencedFromExportDataCount << std::endl;
            PayloadTocOffset = stoull(info_finder(8, file), 0, 16);
            std::cout << "PayloadTocOffset: " << PayloadTocOffset << std::endl;

            Thumbnails(ThumbnailTableOffset, file);
            std::cout << "=============Names========" << std::endl;
            printNames(NameOffset, NameCount, file);
            Depends(DependsOffset, file);
            SoftPackageReferences(SoftPackageReferencesOffset, SoftPackageReferencesCount, file);
            SearchableNameOffset(SearchableNamesOffset, file);
            AssetRegistryData(AssetRegistryDataOffset, WorlTileInfoDataOffset, HeaderSize, file);
            Imports(ImportCount, ImportOffset, file);
            Exports(ExportOffset, ExportCount, file);
         
        }
    }
}
