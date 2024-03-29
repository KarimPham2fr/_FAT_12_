/*
 * FAT.h
 *
 *  Created on: Mar 03, 2024
 *      Author: Phong Pham-Thanh
 *      Email:  Phong.PT.HUST@gmail.com
 */


#ifndef _INC_FAT_
#define _INC_FAT_
/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "hal.h"
#include <string.h>
/*******************************************************************************
 * Defines
 ******************************************************************************/
#define   true                        1
#define   false                       0
#define   Folder                      0x10
#define   read2sec                    511 /*Constant used to check whether 2 sectors in the fat table need to be read.*/
#define   number_bytes_firstime_read  512
#define   byte_per_entry              32
#define   low_byte_sectorSizes        0x0B
#define   high_byte_sectorSizes       0x0C
#define   secPerCluster               0x0D
#define   low_byte_secBeforeFat       0x0E
#define   high_byte_secBeforeFat      0x0F
#define   number_fat_table            0x10
#define   low_byte_max_entry          0x11
#define   high_byte_max_entry         0x12
#define   low_byte_total_sector       0x13
#define   high_byte_total_sector      0x14
#define   low_byte_fat_size           0x16
#define   high_byte_fat_size          0x17

/*******************************************************************************
 * Typedef & Enums
 ******************************************************************************/

 /**
 *@brief: struct stores information parsed from entries in the root.
 **/
typedef struct
{
    uint16_t BytePerSec; /*Number of bytes in a sector*/
    uint8_t  SecPerClus; /*Number of sector in a cluster*/
    uint16_t RsvdSecCnt; /*Sector number comes before the fat table*/
    uint8_t  NumFATs;    /*Number of fat table*/
    uint16_t RootEntCnt; /*Number of entries in the root directory*/
    uint16_t TotSec;     /*Total number of sectors*/
    uint16_t FATSz;      /*Number of sector in a fat table*/
} boot_sector_struct_t;


typedef struct
{
    uint8_t hour;
    uint8_t min;
    uint8_t sec;
} modify_time_t;


typedef struct
{
    uint16_t year;
    uint8_t  month;
    uint8_t  day;
} modify_date_t;

 /**
 *@brief: struct stores information about a node in the list.
 **/
typedef struct entry_struct_t
{
    uint8_t        fileName[255];
    uint8_t        type[7];
    uint8_t        file_attribute;
    uint8_t        lowercase; /*Check lower case basename*/
    modify_time_t  modify_time;
    modify_date_t  modify_date;
    uint16_t       start_cluster;
    uint32_t       size;
    struct entry_struct_t* pNext;
} entry_struct_t;

/**
 *@brief: enum defines return codes when processing in fat layer
 **/
typedef enum
{
    FAT_INIT_SUCCESSFULLY,
    FAT_INIT_UNSUCCESSFULLY,
} fat_handle_enum_t;
/*******************************************************************************
 * API
 ******************************************************************************/

 /**
 * @brief: Initialize the FAT layer from a given file.
 *
 * @param[out] file_name: The name of the file to be used for FAT initialization.
 * @return: fat_handle_enum_t The status of FAT initialization:
 *         - FAT_INIT_SUCCESSFULLY if initialization is successful.
 *         - FAT_INIT_UNSUCCESSFULLY if initialization fails.
 */
fat_handle_enum_t FAT_Init(const char* file_name);


/**
 * @brief: Parse the root directory entries and populate the linked list of entries.
 * @param[in/out] pHead_Entry: Pointer to the head of the linked list where the parsed entries will be stored.
 * @return: None
 */
void FAT_Parse_Root_Directory(entry_struct_t** pHead_Entry);


/**
 * @brief: Read the content of a file from data area and store it in a buffer.
 * @param[out] first_cluster: The address of the first cluster need read.
 * @param[out] sizeOfFile: The size of the file in bytes.
 * @return: uint8_t* Pointer to the memory location where the file content is stored.
 * Returns NULL if memory allocation fails.
 */
uint8_t* FAT_Read_Content_File(uint16_t first_cluster, uint32_t sizeOfFile);


/**
 * @brief: Free the linked list of directory entries.
 * @param[in/out] pHead: Pointer to the head of the linked list to be freed.
 * @Note: This function should be called after entering a new directory.
 */
void FAT_Free_List(entry_struct_t** pHead);


/**
 * @brief: Free the memory allocated for file content.
 * @param[out] buff: Pointer to the buffer containing the file data.
 * @Note: This function should be called immediately after finishing operations with the file data.
 */
void FAT_Free_Content(uint8_t* buff);

/**
 * @brief: Parse the sub directory entries and populate the linked list of entries.
 * @param[in/out] pHead_Entry: Pointer to the head of the linked list where the parsed entries will be stored.
 * @return: None
 */
void FAT_Parse_Sub_Directory(entry_struct_t** pHead_Entry, uint16_t first_cluster);
#endif /*_INC_FAT_*/
/******************************************************************************
 * EOF
 *****************************************************************************/
