Header properties fields:
<!-- 1. EPackageFileTag -->
<!-- 2. LegacyFileVersion  -->
<!-- 3. LegacyUE3Version -->
<!-- 4. FileVersionUE4 -->
<!-- 5. FileVersionUE5 -->
<!-- 6. FileVersionLicenseeUE4 -->
<!-- 7. CustomVersions Count -->
<!-- 8. TotalHeaderSize -->
<!-- 9. FolderName -->
<!-- 10. PackageFlags  -->
<!-- 11. NameCount -->
<!-- 12. NameOffset  -->
13. SoftObjectPathsCount
14. SoftObjectPathsOffset
<!-- 15. LocalizationId -->
<!-- 16. GatherableTextDataCount -->
<!-- 17. GatherableTextDataOffset -->
<!-- 18. ExportCount -->
<!-- 19. ExportOffset -->
<!-- 20. ImportCount  -->
<!-- 21. ImportOffset -->
<!-- 22. DependsOffset -->
<!-- 23. SoftPackageReferencesCount -->
<!-- 24. SoftPackageReferencesOffset -->
<!-- 25. SearchableNamesOffset -->
<!-- 26. ThumbnailTableOffset -->
<!-- 27. Guid -->
<!-- 28. PersistentGuid -->
29. OwnerPersistentGuid
<!-- 30. GenerationsCount & Generations -->
<!-- 31. SavedByEngineVersion (Major/Minor/Path/ChangeList/Branch) -->
32. EngineChangeList
<!-- 33. CompatibleWithEngineVersion (Major/Minor/Path/ChangeList/Branch) -->
<!-- 34. CompressionFlags -->
<!-- 35. CompressedChunks Count -->
<!-- 36. PackageSource -->
37. NumTextureAllocations
<!-- 38. AssetRegistryDataOffset -->
<!-- 39. BulkDataStartOffset -->
<!-- 40. WorlTileInfoDataOffset -->
<!-- 41. ChunkIds Count -->
<!-- 42. PreloadDependencyCount -->
<!-- 43. PreloadDependencyOffset -->
<!-- 44. NamesReferencedFromExportDataCount -->
<!-- 45. PayloadTocOffset -->
46. DataResourceOffset
Blueprint aspect list:
<!-- 1. Names -->

2. GatherableTextData (last name ke baad hai ye, everything is in sequence)
4 bytes -> namespace length
then name (including null byte)
SourceData.SourceString length
SourceData.SourceString 
<!-- 3. Imports -->
<!-- 4. Exports -->
<!-- 5. Depends -->
<!-- 6. SoftPackageReferences  -->
<!-- 7. SearchableNames  -->
<!-- 8. Thumbnails  -->
<!-- 9. AssetRegistryData (including FiBData) -->
10. WorldLevelData
11. PreloadDependency  (isko dekhna pdega last step hai yeh)
12. BulkData (isko bhi)
13. Additional exports data written by WriteExports from SavePackage2.cpp and referenced
by exports. (SerialSize & SerialOffset)
14. File traile