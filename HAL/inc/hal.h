#ifndef _INC_HAL_
#define _INC_HAL_
/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

/*******************************************************************************
 * Typedef & Enums
 ******************************************************************************/
/**
 *@brief: enum defines return codes when processing files
 **/
typedef enum {
    UPDATE_BYTE_PER_SECTOR_SUCCESSFULLY,
    UPDATE_BYTE_PER_SECTOR_UNSUCCESSFULly,
    HAL_INIT_SUCCESSFULLY,
    HAL_INIT_UNSECCESSFULLY,
    HAL_DEINIT_SUCCESSFULLY,
    HAL_DEINIT_UNSECCESSFULLY,
} handle_file_enum_t;

/*******************************************************************************
 * API
 ******************************************************************************/
 
  /**
 * @brief: Initialize the HAL (Hardware Abstraction Layer) from a given file.
 * @param[out] file_name: The name of the file to open for initializing HAL.
 * @return: handle_file_enum_t The status of HAL initialization:
 *          - HAL_INIT_SUCCESSFULLY if initialization is successful.
 *          - HAL_INIT_UNSECCESSFULLY if initialization fails.
 */
handle_file_enum_t HAL_Init(const char* file_name);

   /**
 * @brief: Deinitialize the HAL (Hardware Abstraction Layer) from a given file.
 * @param[out] file_name: The name of the file to open for Deinitializing HAL.
 * @return: handle_file_enum_t The status of HAL Deinitialization:
 *          - HAL_DEINIT_SUCCESSFULLY if Deinitialization is successful.
 *          - HAL_DEINIT_UNSECCESSFULLY if Deinitialization fails.
 */
handle_file_enum_t HAL_DeInit(const char* file_name);

/**
 * @brief: Update the byte per sector parameter for the HAL (Hardware Abstraction Layer).
 * @param[out] bytePerSector: The value of the byte per sector parameter to be updated.
 * @return: handle_file_enum_t The status of the byte per sector update:
 *         - UPDATE_BYTE_PER_SECTOR_SUCCESSFULLY if the update is successful.
 *         - UPDATE_BYTE_PER_SECTOR_UNSUCCESSFULLY if the update fails.
 */
handle_file_enum_t HAL_Update_BytePerSector(uint16_t bytePerSector);

/**
 * @brief: Read a sector with the specified index and store the read data into the buffer.
 * @param[out] index: The index of the sector to be read.
 * @param[in] buff: Pointer to the buffer where the read data will be stored.
 * @return: int32_t The number of bytes read, or 0 if an error occurs.
 */
int32_t HAL_KMC_Read_Sector(uint32_t index, uint8_t *buff);

/**
 * @brief: Read multiple sectors starting from the index and store the read data into the buffer.
 * @param[out] index: The index of the first sector to be read.
 * @param[out] num: The number of sectors to read consecutively.
 * @param[in] buff Pointer to the buffer where the read data will be stored.
 * @return int32_t The total number of bytes read, or 0 if an error occurs.
 */
int32_t HAL_KMC_Read_Multi_Sector(uint32_t index, uint32_t num, uint8_t *buff);

#endif /*_INC_HAL_*/
/******************************************************************************
 * EOF
 *****************************************************************************/
