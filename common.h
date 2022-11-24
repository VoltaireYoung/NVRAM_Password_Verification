/*
 * @Author: Voltaire Chen
 * @Date :  2022-09-19 16:00:00
 * @Last Modified by: Voltaire Chen
 * @Last Modified time: 2022-09-19 16:00:00
 */

#ifndef _COMMON_
#define _COMMON_

#include <Library/BaseLib.h>
#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/PrintLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <stdio.h>
#include <stdlib.h>

// CopyMem Needed Library
#include <Base.h>
#include <Library/BaseMemoryLib.h>

// SHA
#include <Library/BaseCryptLib.h>

// Shell
#include  <Library/ShellLib.h>

extern EFI_RUNTIME_SERVICES      *gRT;
extern EFI_SYSTEM_TABLE          *gST;

#define USER_INPUT_LENGTH 100

#define TSE_SETUP_VAR_NAME // Deleted !! For Security Reason
#define AMITSESETUP_GUID // Deleted !! For Security Reason

#define SETUP_PASSWORD_LENGTH	0x14
#define SETUP_PASSWORD_LENGTH_MINIMUM 0x3
#define SETUP_PASSWORD_WORD_TYPE UINT16

#define HAVE_PASSWORD 0x8
#define INVALID_HASH 0x7
#define HASH_CODE_MASK 0x7

#define ALGORITHM_OTP 0x0
#define ALGORITHM_SHA256 0x1
#define ALGORITHM_RESERVED 0x7

#define CASE_SENSITIVE 1

typedef struct _AMITSESETUP
{
    // Deleted !! For Security Reason
}AMITSESETUP;

#define OTP_Key // Deleted !! For Security Reason

#define SETUPPASS_VAR_NAME_ANY L"$SETUPPASSWDANY"
#define SETUPPASSWD_GUID { 0x5855CE1B, 0xFB8E, 0x47E4, {0xBC, 0x1A, 0x39, 0xEC, 0xAA, 0x0C, 0x96, 0xCF}}

typedef struct _User_Input_Password{
    CHAR16 user_input_password[SETUP_PASSWORD_LENGTH];
    UINTN size_of_user_input_password;
}User_Input_Password;

#endif

#ifndef _MY_DEBUGGER_
#define _MY_DEBUGGER_

#define Debugger_Switch 1

#define My_Debugger(position , message) \
        if(Debugger_Switch){ \
            Print(L"In %a\n" , position); \
            Print(L"%a\n" , message);  \
        }

#endif