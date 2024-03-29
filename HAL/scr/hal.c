/*
 * APP.h
 *
 *  Created on: Mar 01, 2024
 *      Author: Phong Pham-Thanh
 *      Email:  Phong.PT.HUST@gmail.com
 */


/*****************************************************************************
* Includes
*****************************************************************************/
#include "hal.h"

uint16_t sizeOfSector = 512; /*Default*/
FILE* ptr_file        = NULL;

/******************************************************************************
 * Function Prototypes
 *****************************************************************************/
handle_file_enum_t HAL_Init(const char* file_name)
{
    handle_file_enum_t reVal = HAL_INIT_UNSECCESSFULLY;

    ptr_file = fopen(file_name , "rb");
    if (ptr_file != NULL)
    {
        reVal = HAL_INIT_SUCCESSFULLY;
    }

    return reVal;
}


handle_file_enum_t HAL_DeInit(const char* file_name)
{
    handle_file_enum_t reVal = HAL_DEINIT_UNSECCESSFULLY;
    uint8_t status           = 0;

    status = fclose(ptr_file);
    if (status == 0)
    {
        reVal = HAL_DEINIT_SUCCESSFULLY;
    }

    return reVal;
}


handle_file_enum_t HAL_Update_BytePerSector(uint16_t bytePerSector)
{
    handle_file_enum_t reVal = UPDATE_BYTE_PER_SECTOR_UNSUCCESSFULly;

    if ((bytePerSector == 512) || (bytePerSector == 1024) || (bytePerSector == 2048) || (bytePerSector == 4096))
    {
        sizeOfSector = bytePerSector;
        reVal = UPDATE_BYTE_PER_SECTOR_SUCCESSFULLY;

    } else {
        reVal = UPDATE_BYTE_PER_SECTOR_UNSUCCESSFULly;
    }

    return reVal;
}


int32_t HAL_KMC_Read_Sector(uint32_t index, uint8_t *buff)
{
    size_t reVal = 0;

    if (buff != NULL)
    {
        fseek(ptr_file, sizeOfSector * index, SEEK_SET);
        reVal = fread(buff, 1, sizeOfSector, ptr_file);
    } else {

    }

    return reVal;
}


int32_t HAL_KMC_Read_Multi_Sector(uint32_t index, uint32_t num, uint8_t *buff)
{
    size_t reVal = 0;

    if (buff != NULL)
    {
        fseek(ptr_file, sizeOfSector * index, SEEK_SET);
        reVal = fread(buff, 1, (num * sizeOfSector), ptr_file);
    } else {

    }

    return reVal;
}
/*****************************************************************************
* EOF
*****************************************************************************/
