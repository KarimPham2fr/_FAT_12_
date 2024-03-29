/*
 * MENU.c
 *
 *  Created on: Mar 05, 2024
 *      Author: Phong Pham-Thanh
 *      Email:  Phong.PT.HUST@gmail.com
 */


/*****************************************************************************
* Includes
*****************************************************************************/
#include "menu.h"

/******************************************************************************
 * Function Prototypes
 *****************************************************************************/
void MENU_printDirec(entry_struct_t* pHead_DATA)
{
    uint32_t STT = 1;
    printf("%-5s%-75s%-10s%-10s%-17s%-10s\n", "STT", "Name", "Size", "Type", "Date modified", "Time modified");
    while (pHead_DATA != NULL)
    {
        printf("%-5d", STT);
        printf("%-75s", pHead_DATA -> fileName);
        printf("%-10d", pHead_DATA -> size);
        printf("%-10s", pHead_DATA -> type);
        printf("%d-%d-%-10d", pHead_DATA -> modify_date.year, pHead_DATA -> modify_date.month, pHead_DATA -> modify_date.day);
        printf("%d:%d:%d\n", pHead_DATA -> modify_time.hour, pHead_DATA -> modify_time.min, pHead_DATA -> modify_time.sec);
        pHead_DATA = pHead_DATA -> pNext;
        STT++;
    }
}


void MENU_printContentOfFile(uint8_t* buff, uint32_t sizeOfFile)
{
    uint32_t index = 0;

    for (index = 0; index < sizeOfFile; ++index)
    {
        printf("%c", buff[index]);
    }
    printf("\n");
}
/*****************************************************************************
* EOF
*****************************************************************************/
