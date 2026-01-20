# SREC

S Record is a file format which stores binary data in hex format primarily used to transfer executable code and data to embedded systems.

An S Record format consists of 5 fields

S11306E86E746F686C006667657473007265637610

S1      : Type   
13      : Byte Count (Address + Data + Checksum)   
06E8    : 16 bit starting address   
6E-76   : Data Bytes   
10      : Checksum (1s complement of Byte Count + Address + Data)   

An S record can be of different Types

S0 - SREC Header (Same for all types)   
S1 - 16-bit addresses   
S2 - 24-bit addresses   
S3 - 32-bit addresses   
S7 - S3 Terminator   
S8 - S2 Terminator   
S9 - S3 Terminator

# ELF - Executable and Linkable Format

The current code can convert an ELF file (binary) into S Record (srec) format  
The code makes uses of <elf.h> library for this function  

An ELF file contains both program data and metadata   
This data is split into parts inside the ELF   
These parts are categorized into header tables primarily in two ways    

Program Header Table - PHT   
Section Header Table - SHT   

PHT and SHT refer to the same data but are segemented or sectioned differently inside the binary   

The code uses SHT to convert Data into SREC format   
Converting Data using SHT has the advantage of individual sections in SREC output having similar data

The ELF header and SHT are put into struct **Elf64_Ehdr** and  **Elf64_Shdr** respectively

**Elf64_Ehdr** is used to find the Number of Sections, SHT offset and SHT entry size    
**ELF64_Shdr** then stores details about each header like its size, flags, type, address and offset   

The code loops through each sections  
Using flags the code will check if memory is allocated for section   
It uses type to check if actual program data is present in section   
It will then find the address of the section and set it as starting address  

If file size is lesser than 64 KB data is converted to S1 format, else S2

The Data conversion is similar for both S1 and S2

In S1 , Each line starts with S1 occupying the first two characters.  
The number of Data bytes are set 16  
If 16 Data is not left in the section, The number of Data bytes is what's left   
The Byte count is DataBytes + 2 Bytes of Address + 1 Byte of Checksum   
Address is first set as Section address and incremented by 16 everytime until Section Ends  
Checksum is calculated by adding every Byte from byte count to last data byte, removing MSB and taking 1s complement  
This is then written into the file sequentially to form one line of the SREC file. this is repeated until data ends  

S2 follows the same steps but Address is 3 Bytes instead thus byte Count is DataBytes + 2 Bytes of Address + 1 Byte of Checksum   

The Header blocks always has 2 byte address and contains the file name format.srec as hex  
The Termination block is S9 if S1 and S8 if S2   
The byte count is 03 and 04 respectively   
It contains Entry point address from which the program starts execution  