14/02/2024
- We got approach to parse the data (i.e. going byte by byte)
- Analysis of uasset file
- got some github repos regarding parsing of uasset files

15/02/2024
- to start scripting 
- need to explore terms mentioned in the description of the PS.


## SEARCHING OFFSET SIBSTRINGS:

#### Version and Main Program ClassName

- `++UE<version>` + `+` + `<ClassName>`.

#### "FileVersionUE5"

- This is just after the Header bytes in big endian system format.
- `17 and 18` offsets.

#### "EPackageFileTag"

- `First 4 bytes` of the file.

#### LocalizationId

- 32 bytes long and having capital hex characters example: `50B4947A4816F182AF72CC889D9D0EB8`.
- Offset: Unknown till now but it is after the FolderName and near to the `Header` file name.
- The bytes will be taken with first two zeroes that will not be included.

#### FolderName

- `\xbbF.` nearest to the header and thus the string after the `12 bytes` is the FolderName where the file is saved or made inside the system.
- `None` in this case.

#### 

