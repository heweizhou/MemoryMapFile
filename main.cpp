//
//  main.c
//  Test
//
//  Created by 周贺伟 on 2017/2/27.
//  Copyright © 2017年 zhouhewei. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include "IMemoryMapFile.h"

int main(int argc, const char * argv[]) {
    
    IMemoryMapFile* f = createMemoryFileInstance("/Users/zhouhewei/user.txt", 6);
    bool ret = f->writeData(2, "d111", 4);
    ret = f->writeData("e222", 4);
    
    char buffer[2] = {0};
    ret = f->readData(buffer, 1);
}

