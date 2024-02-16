14/02/2024
- We got approach to parse the data (i.e. going byte by byte)
- Analysis of uasset file
- got some github repos regarding parsing of uasset files

15/02/2024
- to start scripting 
- need to explore terms mentioned in the description of the PS.


## SEARCHING OFFSET SUBSTRINGS:
#### Version and Main Program ClassName

- `++UE<version>` + `+` + `<ClassName>`.

#### "EPackageFileTag" [Done]
- `First 4 bytes` of the file.
#### LegacyUE3Version [Done]
- 9th and 10th bytes
#### FileVersionUE4 [Done]
- 13th and 14th bytes
#### FileVersionUE5 [Done]
-  This is just after the Header bytes in big endian system format `17 and 18` offsets.
#### CustomVersionsCount [Done]
- `Most probably 25th byte`

#### LocalizationId

- 32 bytes long and having capital hex characters example: `50B4947A4816F182AF72CC889D9D0EB8`.
- Offset: Unknown till now but it is after the FolderName and near to the `Header` file name.
- The bytes will be taken with first two zeroes that will not be included.

#### FolderName

- `\xbbF.` nearest to the header and thus the string after the `12 bytes` is the FolderName where the file is saved or made inside the system.
- `None` in this case.
- Stored just as same as starting with `\x00` and then stored as packed with it.


#### 17-02-2024
We need a type of counter that stores the current position, that will help to parse the data proceeding further.



