#ifndef _INC_MENU_
#define _INC_MENU_
/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdio.h>
#include "fat.h"
/*******************************************************************************
 * API
 ******************************************************************************/
 
 /**
 * @brief: Print the contents of a directory.
 * @param[out] pHead_DATA: Pointer to the head of the linked list containing directory contents.
 * @Note: This function prints the contents of a directory, including file names, sizes, types,
 * and modification dates and times.
 * @return: None.
 */
void MENU_printDirec(entry_struct_t* pHead_DATA);


/**
 * @brief: Print the content of a file.
 * @param[out] buff: Pointer to the memory buffer containing the file content.
 * @param[out] sizeOfFile: Size of the file content in bytes.
 * @return: None.
 */
void MENU_printContentOfFile(uint8_t* buff, uint32_t sizeOfFile);

#endif /*_INC_MENU_*/
/******************************************************************************
 * EOF
 *****************************************************************************/
 
