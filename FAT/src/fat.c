/*****************************************************************************
* Includes
*****************************************************************************/
#include "fat.h"

static boot_sector_struct_t bootSector;

/******************************************************************************
 * Function Prototypes
 *****************************************************************************/
/**
 * @brief: Find the next cluster in the FAT table.
 * @param[out] cluster: The current cluster whose next cluster needs to be found.
 * @return The next cluster in the FAT.
 */
static uint16_t findNextCluster(uint16_t cluster)
{
    uint16_t first_byte     = 0;
    uint16_t second_byte    = 0;
    uint16_t next_cluster   = 0;
    uint16_t checkReadSec   = 0;
    uint8_t  fat_sector     = 0;
    uint8_t  fat_buff[bootSector.BytePerSec];
    uint8_t  fat_buff_2sec[bootSector.BytePerSec * 2];

    first_byte = cluster * 3 / 2;
    /*Calculate the first sector in the fat to read*/
    fat_sector = first_byte / bootSector.BytePerSec;
    checkReadSec = first_byte - (bootSector.BytePerSec * (first_byte / bootSector.BytePerSec));

    if (checkReadSec == read2sec) /*Case read 2 sector*/
    {
        HAL_KMC_Read_Multi_Sector((bootSector.RsvdSecCnt + fat_sector), 2, fat_buff_2sec);
        first_byte  = bootSector.BytePerSec - 1;
        second_byte = first_byte + 1;
        /*Calculator next cluster*/
        if (cluster %2 == 0)
        {
            next_cluster = ((fat_buff_2sec[second_byte] & 0x0F) << 8) | fat_buff_2sec[first_byte];
        } else {
            next_cluster = (fat_buff_2sec[second_byte] << 4) | (fat_buff_2sec[first_byte] >> 4);
        }
    } else { /*Case read 1 sector*/
        HAL_KMC_Read_Sector((bootSector.RsvdSecCnt + fat_sector), fat_buff);
        first_byte  = cluster * 3 / 2 - (fat_sector * bootSector.BytePerSec);
        second_byte = first_byte + 1;
        /*Calculator next cluster*/
        if (cluster %2 == 0)
        {
            next_cluster = ((fat_buff[second_byte] & 0x0F) << 8) | fat_buff[first_byte];
        } else {
            next_cluster = (fat_buff[second_byte] << 4) | (fat_buff[first_byte] >> 4);
        }
    }

    return next_cluster;
}

/**
 * @brief: Find the short filename for a file.
 * @param[out] entry: Pointer to the entry in the FAT containing information about the file name.
 * @param[in] name: Array to store the short filename of the file after it is found.
 */
static void FAT_SFN(const uint8_t* const entry, uint8_t* const name)
{
    name[0]  = entry[1];
    name[1]  = entry[3];
    name[2]  = entry[5];
    name[3]  = entry[7];
    name[4]  = entry[9];
    name[5]  = entry[14];
    name[6]  = entry[16];
    name[7]  = entry[18];
    name[8]  = entry[20];
    name[9]  = entry[22];
    name[10] = entry[24];
    name[11] = entry[26];
    name[12] = 0;
}

/**
 * @brief: Extracts the long filename from the given buffer and stores it in the name array.
 * @param[out/in] buff: Pointer to the buffer containing the long filename entries.
 * @param[in] name: Pointer to the array where the long filename will be stored.
 * @param[in] numEntryLFN: Number of long filename entries in the buffer.
 */
static void FAT_LFN(uint8_t* const buff, uint8_t* const name, uint8_t numEntryLFN)
{
    uint8_t index   = 0;
    uint8_t index_2 = 0;
    uint8_t length  = 0;

    buff[0] = 0;
    for (index = 0; index < numEntryLFN; ++index)
    {
        buff[index * 0x20 + 0x0D] = 0; /*set byte checksum in entry LFN= 0*/
    }

    for (index = 0; index < (numEntryLFN * 32); ++index)
    {
        if ((buff[index] >= '0' && buff[index] <= '9') ||
            (buff[index] >= 'A' && buff[index] <= 'Z') ||
            (buff[index] >= 'a' && buff[index] <= 'z') ||
            (buff[index] == ' ') || (buff[index] == '.'))
            {
            buff[index_2++] = buff[index];
            }
    }

    buff[index_2] = '\0';
    while (buff[length] != '\0')
    {
        length++;
    }
    index_2 = length % 13;

    for (index = 0; index < (numEntryLFN - 1); ++index)
    {
        memcpy((name + index * 13), buff + length - (index + 1) * 13, 13);
    }

    memcpy(name + (numEntryLFN - 1) * 13, buff, index_2);
    name[length] = 0;

}

fat_handle_enum_t FAT_Init(const char* file_name)
{
    fat_handle_enum_t reVal          = FAT_INIT_UNSUCCESSFULLY;
    handle_file_enum_t statusHalInit = HAL_INIT_UNSECCESSFULLY;
    uint16_t   numberOfBytesRead     = 0;
    uint8_t    buff[number_bytes_firstime_read];

    statusHalInit = HAL_Init(file_name);
    if (statusHalInit == HAL_INIT_SUCCESSFULLY)
    {
        numberOfBytesRead = HAL_KMC_Read_Sector(0, buff);
        if (numberOfBytesRead == number_bytes_firstime_read)
        {
            bootSector.BytePerSec = (buff[high_byte_sectorSizes] << 8)  | buff[low_byte_sectorSizes];
            bootSector.SecPerClus =  buff[secPerCluster];
            bootSector.RsvdSecCnt = (buff[high_byte_secBeforeFat] << 8) | buff[low_byte_secBeforeFat];
            bootSector.NumFATs    =  buff[number_fat_table];
            bootSector.RootEntCnt = (buff[high_byte_max_entry] << 8)    | buff[low_byte_max_entry];
            bootSector.TotSec     = (buff[high_byte_total_sector] << 8) | buff[low_byte_total_sector];
            bootSector.FATSz      = (buff[high_byte_fat_size] << 8)     | buff[low_byte_fat_size];
            /*Update size of sector*/
            HAL_Update_BytePerSector(bootSector.BytePerSec);
            reVal = FAT_INIT_SUCCESSFULLY;
        }
    } else {
        reVal = FAT_INIT_UNSUCCESSFULLY;
    }

    return reVal;
}

void FAT_Parse_Root_Directory(entry_struct_t** pHead_Entry)
{
    uint8_t  loop_until_empty_sector = true;
    uint16_t numOfBytesRead          = 0;
    uint8_t  buff[bootSector.BytePerSec];
    uint32_t index                   = 0;
    uint8_t  index_2                 = 0;
    uint16_t modify_time             = 0;
    uint16_t modify_date             = 0;
    uint8_t  sec_next                = 0;
    uint8_t  numEntryLFN             = 0;
    uint8_t  LFN                     = false;
    uint8_t  name[255];

    /*Read each sector in the root to parse*/
    while (loop_until_empty_sector) {
        HAL_KMC_Read_Sector((bootSector.RsvdSecCnt) + (bootSector.NumFATs) * (bootSector.FATSz) + sec_next, buff);
        sec_next++;

        if (buff[0] != 0) { /*Check empty sector*/
            for (index = 0; index < ((bootSector.BytePerSec) / byte_per_entry); ++index)
            {   /*Handle SFN entry*/
                if ((buff[index * byte_per_entry] == 0x41) && (buff[index * byte_per_entry + 0x0B] == 0x0F))
                {
                    FAT_SFN(&buff[index * byte_per_entry], name);
                    LFN = false;
                }
                /***** Handle LFN entry *****/
                else if ((buff[index * byte_per_entry] > 0x41) && (buff[index * byte_per_entry + 0x0B] == 0x0F))
                {
                    numEntryLFN = buff[index * byte_per_entry] - 0x40;
                    uint8_t* entryLFN = (uint8_t*)malloc(numEntryLFN * byte_per_entry);
                    if (entryLFN != NULL)
                    {
                        memcpy(entryLFN, &buff[index * byte_per_entry], numEntryLFN * byte_per_entry);
                        FAT_LFN(entryLFN, name, numEntryLFN);
                        free(entryLFN);
                        index = index + numEntryLFN - 1;
                        LFN = true;
                    }
                }
                /*Consider each main entry*/
                else if ((!((buff[index * byte_per_entry + 26] == 0) && (buff[index * byte_per_entry + 27] == 0))))
                {
                    /*Save infor of files or folders in the root directory to the list*/
                    entry_struct_t* pNew_Entry = NULL;
                    pNew_Entry = (entry_struct_t*)malloc(sizeof(entry_struct_t));
                    if (pNew_Entry != NULL) {

                        pNew_Entry->pNext = NULL;

                        /*add node at the end of linkedlist*/
                        if (*pHead_Entry == NULL)
                        {
                            /*if linkedlist is empty*/
                            *pHead_Entry = pNew_Entry;
                        } else {
                            /*if linkedlist is not empty*/
                            entry_struct_t* current = *pHead_Entry;
                            while (current->pNext != NULL)
                            {
                                current = current->pNext;
                            }
                            current->pNext = pNew_Entry;
                        }

                        /*Calculator file name*/
                        pNew_Entry->fileName[8] = 0;
                        memcpy(pNew_Entry->fileName, &buff[index * byte_per_entry], 8);

                        /*Calculator modify time*/
                        modify_time = (buff[index * byte_per_entry + 23] << 8) | buff[index * byte_per_entry + 22];
                        pNew_Entry->modify_time.hour = modify_time >> 11;
                        pNew_Entry->modify_time.min  = (modify_time >> 5) & 0x3F;
                        pNew_Entry->modify_time.sec  = (modify_time & 0x1F) * 2;

                        /*Calculator modify date*/
                        modify_date = (buff[index * byte_per_entry + 25] << 8) | buff[index * byte_per_entry + 24];
                        pNew_Entry->modify_date.year  = (modify_date >> 9) + 1980;
                        pNew_Entry->modify_date.month = (modify_date >> 5) & 0x0F;
                        pNew_Entry->modify_date.day   =  modify_date & 0x1F;

                        /*Calculator start cluster*/
                        pNew_Entry->start_cluster = (buff[index * byte_per_entry + 27] << 8) | buff[index * byte_per_entry + 26];
                        pNew_Entry->size = (buff[index * byte_per_entry + 31] << 24) | (buff[index * byte_per_entry + 30] << 16) \
                        | (buff[index * byte_per_entry + 29] << 8) | buff[index * byte_per_entry + 28];

                        /*Calculator file attribute*/
                        pNew_Entry->file_attribute = buff[index * byte_per_entry + 11];
                        pNew_Entry->lowercase = buff[index * byte_per_entry + 0x0C];

                        /*reformat type of file*/
                        if ((pNew_Entry->file_attribute) != Folder)
                        {
                            pNew_Entry->type[3] = 0;
                            memcpy(pNew_Entry->type, &buff[index * 32 + 8], 3);
                            for (index_2 = 0; index_2 <3; ++index_2)
                            {
                                pNew_Entry->type[index_2] = pNew_Entry->type[index_2] + 32; /*lower char*/
                            }
                            if (LFN = false) /*reformat file name (short file name)*/
                            {
                                memcpy(pNew_Entry->fileName, &name[0], 12);
                            } else {
                                memcpy(pNew_Entry->fileName, &name[0], 255);
                            }
                        } else {
                            pNew_Entry->type[6] = 0;
                            memcpy(pNew_Entry->type, "folder", 6);
                            /*reformat folder name*/
                            if ((pNew_Entry->lowercase >> 3) & 0x01)
                            {
                                for (index_2 = 0; index_2 < 8; ++index_2)
                                {
                                    if ((pNew_Entry->fileName[index_2] != 32) && !(((pNew_Entry->fileName[index_2]) >= '1' && (pNew_Entry->fileName[index_2]) <= '9')))
                                    {
                                        pNew_Entry->fileName[index_2] = pNew_Entry->fileName[index_2] + 32;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        } else {
            loop_until_empty_sector = false;
        }
    }
}

void FAT_Parse_Sub_Directory(entry_struct_t** pHead_Entry, uint16_t first_cluster)
{
    uint16_t cluster      = 0;
    uint16_t modify_time  = 0;
    uint16_t modify_date  = 0;
    uint8_t  index        = 0;
    uint8_t  index_2      = 0;
    uint8_t  buff[bootSector.BytePerSec];

    cluster = first_cluster;
    while (cluster != 0xFFF)
    {
        HAL_KMC_Read_Multi_Sector(cluster - 2 + (bootSector.RsvdSecCnt + \
        bootSector.NumFATs * bootSector.FATSz + (bootSector.RootEntCnt * 32 / (bootSector.BytePerSec))), \
        bootSector.SecPerClus, buff);
        for (index = 0; index < ((bootSector.BytePerSec * bootSector.SecPerClus) / 32); ++index)
        {
            if (buff[index * byte_per_entry + 0] != 0)
            {
                /*Save infor of files or folders in subfolders to the list*/
                entry_struct_t* pNew_Entry = NULL;
                pNew_Entry = (entry_struct_t*)malloc(sizeof(entry_struct_t));
                if (pNew_Entry != NULL) {

                    pNew_Entry->pNext = NULL;

                    /*add node at the end of linkedlist*/
                    if (*pHead_Entry == NULL)
                    {
                        /*if linkedlist is empty*/
                        *pHead_Entry = pNew_Entry;
                    } else {
                        /*if linkedlist is not empty*/
                        entry_struct_t* current = *pHead_Entry;
                        while (current->pNext != NULL)
                        {
                            current = current->pNext;
                        }
                        current->pNext = pNew_Entry;
                    }

                    /*Calculator file name*/
                    pNew_Entry->fileName[8] = 0;
                    memcpy(pNew_Entry->fileName, &buff[index * byte_per_entry], 8);

                    /*Calculator modify time*/
                    modify_time = (buff[index * byte_per_entry + 23] << 8) | buff[index * byte_per_entry + 22];
                    pNew_Entry->modify_time.hour = modify_time >> 11;
                    pNew_Entry->modify_time.min  = (modify_time >> 5) & 0x3F;
                    pNew_Entry->modify_time.sec  = (modify_time & 0x1F) * 2;

                    /*Calculator modify date*/
                    modify_date = (buff[index * byte_per_entry + 25] << 8) | buff[index * byte_per_entry + 24];
                    pNew_Entry->modify_date.year  = (modify_date >> 9) + 1980;
                    pNew_Entry->modify_date.month = (modify_date >> 5) & 0x0F;
                    pNew_Entry->modify_date.day   = modify_date & 0x1F;

                    /*Calculator start cluster*/
                    pNew_Entry->start_cluster = (buff[index * byte_per_entry + 27] << 8) | buff[index * byte_per_entry + 26];
                    pNew_Entry->size = (buff[index * byte_per_entry + 31] << 24) | (buff[index * byte_per_entry + 30] << 16) \
                    | (buff[index * byte_per_entry + 29] << 8) | buff[index * byte_per_entry + 28];

                    /*Calculator file attribute*/
                    pNew_Entry->file_attribute = buff[index * byte_per_entry + 11];

                    /*reformat name & type of file*/
                    if ((pNew_Entry->file_attribute) == 0)
                    {
                        pNew_Entry->type[3] = 0;
                        memcpy(pNew_Entry->type, &buff[index * 32 + 8], 3);

                        for (index_2 = 1; index_2 < 8; ++index_2)
                        {
                            if ((pNew_Entry->fileName[index_2] != 32) && !(((pNew_Entry->fileName[index_2]) >= '1' && (pNew_Entry->fileName[index_2]) <= '9')))
                            {
                                pNew_Entry->fileName[index_2] = pNew_Entry->fileName[index_2] + 32;
                            }
                        }

                        for (index_2 = 0; index_2 < 3; ++index_2)
                        {
                            pNew_Entry->type[index_2] = pNew_Entry->type[index_2] + 32;
                        }
                    } else {
                        pNew_Entry->type[6] = 0;
                        memcpy(pNew_Entry->type, "folder", 6);
                    }
                }
            }
        }
        cluster = findNextCluster(cluster);
    }
}

uint8_t* FAT_Read_Content_File(uint16_t first_cluster, uint32_t sizeOfFile)
{
    uint8_t* buff    = NULL;
    uint16_t cluster = 0;
    uint16_t numLoop = 0;

    cluster = first_cluster;
    buff = (uint8_t*)malloc((sizeOfFile / bootSector.BytePerSec + 1) * bootSector.BytePerSec);
    if (buff != NULL)
    {
        while (cluster != 0xFFF)
        {
            /*Read the content of each cluster and save it to buff*/
            HAL_KMC_Read_Multi_Sector(cluster - 2 + (bootSector.RsvdSecCnt + \
            (bootSector.NumFATs * bootSector.FATSz) + (bootSector.RootEntCnt * 32 / (bootSector.BytePerSec))), \
            bootSector.SecPerClus, buff + numLoop * (bootSector.BytePerSec));
            cluster = findNextCluster(cluster);
            numLoop++;
        }
    } else {

    }

    return buff;
}

void FAT_Free_List(entry_struct_t** pHead)
{
    entry_struct_t* pTemp = NULL;

    while (*pHead != NULL)
    {
        pTemp = *pHead;
        *pHead = (*pHead)->pNext;
        free(pTemp);
    }
}

void FAT_Free_Content(uint8_t* buff)
{
    if (buff != NULL) {
        free(buff);
    }
}
/*****************************************************************************
* EOF
*****************************************************************************/

