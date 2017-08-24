//
//  IMemoryMapFile.h
//  Test
//
//  Created by 周贺伟 on 2017/8/3.
//  Copyright © 2017年 zhouhewei. All rights reserved.
//

#ifndef IMemoryMapFile_h
#define IMemoryMapFile_h

#define  _IN_
#define  _OUT_

class IMemoryMapFile {
public:
    virtual long getWritePosition() = 0;
    virtual bool setWritePosition(_IN_ long pos) = 0;
    virtual bool writeData(_IN_ const void* buffer, _IN_ long size) = 0;
    virtual bool writeData(_IN_ long startPosition, _IN_ const void* buffer, _IN_ long size) = 0;
    
    virtual long getReadPosition() = 0;
    virtual bool setReadPosition(_IN_ long pos) = 0;
    virtual bool readData(_OUT_ void* buffer, _IN_ long size) = 0;
    virtual bool readData(_IN_ long startPosition, _OUT_ void* buffer, _IN_ long size) = 0;
    
    virtual long getCapacity() = 0;
    virtual long getFileSize() = 0;
    
    virtual const void* getRawMemory() = 0;
};

IMemoryMapFile* createMemoryFileInstance(const char* fileName, long initFileSize);

#endif /* IMemoryMapFile_h */
