/*
 * APP.h
 *
 *  Created on: Mar 05, 2024
 *      Author: Phong Pham-Thanh
 *      Email:  Phong.PT.HUST@gmail.com
 */


/*****************************************************************************
* Includes
*****************************************************************************/
#include "app.h"

/******************************************************************************
 * Function Prototypes
 *****************************************************************************/
int main(int argc, char **argv) {

    if (argc != 2)
    {
        printf("Usage: %s <file_name>\n", argv[0]);
    } else {
        system("cls");
        APP_INIT(argv[1]);
        while (1)
        {
        APP_Process_Action();
        }
    }

    return 0;
}
/*****************************************************************************
* EOF
*****************************************************************************/
