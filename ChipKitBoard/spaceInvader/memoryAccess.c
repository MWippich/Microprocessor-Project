/* memoryAccess.c
  Created 2019 by Emmy Yin and Max Wippich
  Updated 2019-12-06
*/
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */

unsigned int NVMUnlock(unsigned int nvmop){
    unsigned int status;
    asm volatile ("di %0" : "=r" (status));
    NVMCON = (nvmop | 0x4000);
    NVMKEY = 0x00000000;
    NVMKEY = 0xAA996655;
    NVMKEY = 0x556699AA;
    NVMCONSET = 0x8000;
    while (NVMCON & 0x8000);
    if (status & 0x00000001)
            asm volatile ("ei");
    else
            asm volatile ("di");
    NVMCONCLR = 0x0004000;
    return (NVMCON & 0x3000);
}

unsigned int NVMWriteWord(void* address, unsigned int data){
    unsigned int res;
    NVMDATA = data;
    NVMADDR = (unsigned int)address;
    res = NVMUnlock(0x4001);
    return res;
}

unsigned int NVMErasePage(void* address){
    unsigned int res;
    NVMADDR = (unsigned int) address;
    res = NVMUnlock(0x4004);
    return res;
}