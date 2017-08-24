//
//  MemoryMapFile.cpp
//  Test
//
//  Created by 周贺伟 on 2017/8/3.
//  Copyright © 2017年 zhouhewei. All rights reserved.
//

#include "MemoryMapFile.hpp"

#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <string>

IMemoryMapFile* createMemoryFileInstance(const char* fileName, long initFileSize)
{
    return new CMemoryMapFile(fileName, initFileSize);
}

CMemoryMapFile::CMemoryMapFile()
{
    m_size          = 0;
    m_capacity      = 0;
    m_map_file_ptr  = NULL;
    m_file_write_ptr = NULL;
    m_file_read_ptr = NULL;
    m_fd            = -1;
}

CMemoryMapFile::CMemoryMapFile(_IN_ const char* fileName, _IN_ long initFileSize)
{
    m_size          = 0;
    m_capacity      = 0;
    m_map_file_ptr  = NULL;
    m_file_write_ptr = NULL;
    m_file_read_ptr = NULL;
    m_fd            = -1;
    
    MMOpen(fileName, initFileSize);
}

CMemoryMapFile::~CMemoryMapFile()
{
    MMClose();
}

//write functions

long CMemoryMapFile::getWritePosition()
{
    if (!m_file_write_ptr) {
        return 0;
    }

    return m_file_write_ptr - (char*)m_map_file_ptr;
}

bool CMemoryMapFile::setWritePosition(_IN_ long pos)
{
    if (!m_file_write_ptr) {
        return false;
    }

    if (pos >= m_capacity) {
        return false;
    }
    m_file_write_ptr = (char*)m_map_file_ptr + pos;
    return true;
}

bool CMemoryMapFile::writeData(_IN_ const void* buffer, _IN_ long size)
{
    
    if (!m_file_write_ptr) {
        return false;
    }
    
    long writeSpace = m_capacity - (m_file_write_ptr - (char*)m_map_file_ptr);
    
    if (writeSpace < size) {
        if(!reSizeFile(size - writeSpace + m_capacity)){
            return false;
        }
    }
    
    memcpy(m_file_write_ptr, buffer, size);
    m_file_write_ptr += size;
    
    if (m_file_write_ptr > (char*)m_map_file_ptr + m_size) {
        m_size = m_file_write_ptr - (char*)m_map_file_ptr;
    }
    
    return true;
}

bool CMemoryMapFile::writeData(_IN_ long startPosition, _IN_ const void* buffer, _IN_ long size)
{
    if (!m_file_write_ptr) {
        return false;
    }
    
    if (setWritePosition(startPosition)) {
        return writeData(buffer, size);
    }
    return false;
}


//read functions

long CMemoryMapFile::getReadPosition()
{
    if (!m_file_read_ptr) {
        return 0;
    }
    
    return m_file_read_ptr - (char*)m_map_file_ptr;
}

bool CMemoryMapFile::setReadPosition(_IN_ long pos)
{
    if (!m_file_read_ptr) {
        return false;
    }
    
    if (pos >= m_size) {
        return false;
    }
    m_file_read_ptr = (char*)m_map_file_ptr + pos;
    return true;
}

bool CMemoryMapFile::readData(_OUT_ void* buffer, _IN_ long size)
{
    if (!m_file_read_ptr) {
        return false;
    }
    
    long readSpace = m_size - (m_file_read_ptr - (char*)m_map_file_ptr);
    if (readSpace < size) {
        return false;
    }
    
    memcpy(buffer, m_file_read_ptr, size);
    m_file_read_ptr += size;
    return true;
}

bool CMemoryMapFile::readData(_IN_ long startPosition, _OUT_ void* buffer, _IN_ long size)
{
    if (!m_file_read_ptr) {
        return false;
    }
    
    if (setReadPosition(startPosition)) {
        return readData(buffer, size);
    }
    return false;
}

//get property functions

const void* CMemoryMapFile::getRawMemory()
{
    return m_map_file_ptr;
}

long CMemoryMapFile::getCapacity()
{
    return m_capacity;
}

long CMemoryMapFile::getFileSize()
{
    return m_size;
}

//control functions

bool CMemoryMapFile::MMOpen(_IN_ const char* fileName, _IN_ long initFileSize)
{
    
    std::string file_name = fileName;
    
    if (file_name.length() == 0) {
        return false;
    }
    
    m_fd = open(file_name.c_str() ,O_RDWR | O_CREAT | O_TRUNC, 0666);
    
    if(m_fd == -1){
        printf("open file %s failed!:%m\n", file_name.c_str());
        return false;
    }
    
    lseek(m_fd, initFileSize - 1, SEEK_SET);
    write(m_fd, "z", 1);
    
    struct stat st;
    int re = fstat(m_fd, &st);
    if(re == -1){
        printf("get file %s 's size failed!:%m\n", file_name.c_str());
        close(m_fd);
        return  false;
    }
    
    if (st.st_size != initFileSize) {
        close(m_fd);
        return false;
    }
    
    m_map_file_ptr = mmap(NULL, initFileSize, PROT_READ | PROT_WRITE, MAP_SHARED, m_fd, 0);
    
    if (!m_map_file_ptr) {
        close(m_fd);
        return false;
    }
    
    m_size      = 0;
    m_capacity  = initFileSize;
    m_file_write_ptr = (char*)m_map_file_ptr;
    m_file_read_ptr = (char*)m_map_file_ptr;
    return true;
}

bool CMemoryMapFile::MMClose()
{
    if (!m_map_file_ptr) {
        return true;
    }
    
    int ret = munmap(m_map_file_ptr, m_capacity);
    
    if (ret) {
        printf("close file failed!:%m\n");
        return false;
    }
    
    m_map_file_ptr = NULL;
    m_size = 0;
    m_capacity = 0;
    m_file_write_ptr = NULL;
    m_file_read_ptr = NULL;
    return true;
}

//private funtcions

bool CMemoryMapFile::reSizeFile(long new_size)
{
    if (new_size <= m_capacity) {
        return false;
    }
    
    long current_write_pos = m_file_write_ptr - (char*)m_map_file_ptr;
    long current_read_pos = m_file_read_ptr - (char*)m_map_file_ptr;
    
    int ret = munmap(m_map_file_ptr, m_capacity);
    
    if (ret) {
        printf("close file failed!:%m\n");
        return false;
    }
    
    lseek(m_fd, new_size - 1, SEEK_SET);
    write(m_fd, "z", 1);
    
    m_map_file_ptr = mmap(NULL, new_size, PROT_READ | PROT_WRITE, MAP_SHARED, m_fd, 0);
    
    if (!m_map_file_ptr) {
        printf("resize file failed, file will be closed!:%m\n");
        close(m_fd);
        return false;
    }
    
    m_file_write_ptr = (char*)m_map_file_ptr + current_write_pos;
    m_file_read_ptr = (char*)m_map_file_ptr + current_read_pos;
    m_capacity = new_size;
    
    return true;
}


