/*
 * APP.h
 *
 *  Created on: Mar 04, 2024
 *      Author: Phong Pham-Thanh
 *      Email:  Phong.PT.HUST@gmail.com
 */

#ifndef _INC_APP_
#define _INC_APP_
/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <stdio.h>
#include "fat.h"
#include "menu.h"

/*******************************************************************************
 * Defines
 ******************************************************************************/
#define BACK_FROM_FILE     (0U)
#define OPEN_FILE          (1U)
#define INTO_FOLDER        (2U)
/*******************************************************************************
 * API
 ******************************************************************************/\

 /**
 * @brief: Initialize the application with the given file name.
 * @param[out] fileName: The name of the file to be used for application initialization.
 * @return: None
 */
void APP_INIT(const char* fileName);


/**
 * @brief: Process the user action according to the current state of the application.
 * @note: This function handles user actions such as selecting files or folders,
 * navigating through directories, opening files, or returning from a file.
 * It interacts with the user interface to display directory contents or file content.
 *        This function should be called periodically to process user actions.
 */
void APP_Process_Action(void);


/**
 * @brief: Enter a choice and validate the input.
 * @param[out] pHead_CD: Pointer to the head of the linked list representing the current directory.
 *                       The maximum allowable value for user input is determined by the number of
 *                       elements in this list.
 * @return: uint16_t The user's validated input choice.
 */
uint16_t APP_Enter(entry_struct_t *pHead_CD);
#endif /*_INC_APP_*/
/******************************************************************************
 * EOF
 *****************************************************************************/
