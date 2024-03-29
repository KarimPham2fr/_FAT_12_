/*****************************************************************************
* Includes
*****************************************************************************/
#include "app.h"

static entry_struct_t* pHead_CurrentD = NULL;
static uint8_t file_openning          = false;

/******************************************************************************
 * Function Prototypes
 *****************************************************************************/
void APP_INIT(const char* fileName)
{
    FAT_Init(fileName);
    entry_struct_t* pHead_RD = NULL;
    FAT_Parse_Root_Directory(&pHead_RD);
    /*Print out root directory*/
    MENU_printDirec(pHead_RD); 
    pHead_CurrentD = pHead_RD;
}

void APP_Process_Action(void)
{
    uint16_t user_choice = 0;
    uint16_t index       = 0;
    uint8_t  cmd         = 0; 
    entry_struct_t* pTemp_CD = NULL;

    user_choice = APP_Enter(pHead_CurrentD);

    if (user_choice == 0)
    {
        cmd = BACK_FROM_FILE;
    } else {
        pTemp_CD = pHead_CurrentD;
        for (index = 1; index < user_choice; ++index)
        {
            pTemp_CD = pTemp_CD -> pNext;
        }
        if ((pTemp_CD -> file_attribute) == 0)
        {
            cmd = OPEN_FILE; 
        } else {
            cmd = INTO_FOLDER; 
        }
    }
    switch (cmd)
    {   /*Return to the directory from file*/
        case BACK_FROM_FILE:
            system("cls");
            MENU_printDirec(pHead_CurrentD);
            file_openning = false;
            break;
        /*Print out content of file*/
        case OPEN_FILE:
            system("cls");
            uint8_t* ptrContent = NULL;
            ptrContent = FAT_Read_Content_File(pTemp_CD -> start_cluster, pTemp_CD -> size);
            if (ptrContent != NULL)
            {
                MENU_printContentOfFile(ptrContent, pTemp_CD -> size);
                /*Free heap memory*/
                FAT_Free_Content(ptrContent);
            }
            file_openning = true;
            break;
        /*Into folder*/
        case INTO_FOLDER:
            system("cls");
            if (pTemp_CD -> start_cluster == 0x00)
            {
                FAT_Free_List(&pHead_CurrentD);
                FAT_Parse_Root_Directory(&pHead_CurrentD);
                MENU_printDirec(pHead_CurrentD);
            } else {
                FAT_Free_List(&pHead_CurrentD);
                FAT_Parse_Sub_Directory(&pHead_CurrentD, pTemp_CD -> start_cluster);
                MENU_printDirec(pHead_CurrentD);
            }
            break;
        default:
            break;
    }
}

uint16_t APP_Enter(entry_struct_t *pHead_CD)
{
    uint8_t input[10];
    uint16_t reVal      = 0;
    uint16_t maxElement = 0;
    uint8_t loop        = 1;
    uint8_t length      = 0;
    uint8_t index       = 0;
    uint8_t arrayNumber = true;
    
    printf("\nEnter the STT to open the file or go to a folder\n");
    printf("Enter 0 to return from file\n");
    printf("Enter: ");
    while (pHead_CD != NULL) {
        maxElement++;
        pHead_CD = pHead_CD->pNext;
    }

    while (loop)
    {
        arrayNumber = true;
        fflush(stdin);
        gets(input);
        length = strlen(input);
        for (index = 0; index < length; ++index)
        {
        if (!(input[index] >= '0' && input[index] <= '9'))
            {
                index = length; /*Break loop*/
                arrayNumber = false;
            }
        }
        if (arrayNumber == true)
        {
            reVal = atoi(input);
            if (file_openning == true)
            {
                if (reVal != 0)
                {
                    printf("Invalid input. Please enter 0 to back folder: ");
                } else {
                    loop = false;
                }
            } else {
                if ((reVal >= 1) && (reVal <= maxElement))
                {
                    loop = false;
                } else {
                    printf("Invalid input. Please enter a number between 1 and %d: ", maxElement);
                }
            }
        } else {
            printf("Invalid input. Please enter a number: ", maxElement);
        }
    }

    return reVal;
}
/*****************************************************************************
* EOF
*****************************************************************************/
