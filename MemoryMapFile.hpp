//
//  MemoryMapFile.hpp
//  Test
//
//  Created by 周贺伟 on 2017/8/3.
//  Copyright © 2017年 zhouhewei. All rights reserved.
//

#ifndef MemoryMapFile_hpp
#define MemoryMapFile_hpp

#include <stdio.h>
#include "IMemoryMapFile.h"


class CMemoryMapFile:public IMemoryMapFile
{
public:
    CMemoryMapFile();
    CMemoryMapFile(_IN_ const char* fileName, _IN_ long initFileSize);
    virtual ~CMemoryMapFile();
public:
    
    virtual long getWritePosition();
    virtual bool setWritePosition(_IN_ long pos);
    virtual bool writeData(_IN_ const void* buffer, _IN_ long size);
    virtual bool writeData(_IN_ long startPosition, _IN_ const void* buffer, _IN_ long size);
    
    virtual long getReadPosition();
    virtual bool setReadPosition(_IN_ long pos);
    virtual bool readData(_OUT_ void* buffer, _IN_ long size);
    virtual bool readData(_IN_ long startPosition, _OUT_ void* buffer, _IN_ long size);
    
    virtual long getCapacity();
    virtual long getFileSize();
    
    virtual const void* getRawMemory();
public:
    bool MMOpen(_IN_ const char* fileName, _IN_ long initFileSize);
    bool MMClose();
private:
    bool reSizeFile(long new_size);
private:
    
    int             m_fd;
    long            m_capacity;
    long            m_size;
    void*           m_map_file_ptr;
    char*           m_file_write_ptr;
    char*           m_file_read_ptr;
};

#endif /* MemoryMapFile_hpp */
