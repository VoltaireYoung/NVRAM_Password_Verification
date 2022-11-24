/*
 * @Author: Voltaire Chen
 * @Date :  2022-09-19 16:00:00
 * @Last Modified by: Voltaire Chen
 * @Last Modified time: 2022-09-21 02:24:00
 */

#include "common.h"
#include "verifier.h"

#ifndef _UI_BASE_SHOWING_
#define _UI_BASE_SHOWING_

UINTN Number_of_Instructions = 4;

#define Instruction_Help L"-help"
#define Instruction_NvLock_Status L"-nvlock_status"
#define Instruction_Unlock L"-unlock"
#define Instruction_Password_Verification L"-password_verification"
#define Instruction_Change_Password L"-change_password"

CHAR16* Instructions[4] = {
    Instruction_Help,
    Instruction_NvLock_Status,
    Instruction_Unlock,
    Instruction_Change_Password
};

CHAR16* Instructions_Description[] = {
    L"List all instructions of this tool.",
    L"Show current NvLock status.",
    L"NvLock unlock.",
    L"Change password."
};

EFI_STATUS UI_Base(UINTN argc , CHAR16* argv[]);
EFI_STATUS UI_Help(UINTN Argc , CHAR16** Argv);
EFI_STATUS UI_NvLock_Status(UINTN Argc , CHAR16** Argv);
EFI_STATUS UI_Unlock(UINTN Argc , CHAR16** Argv);
//EFI_STATUS UI_Password_Verification(UINTN Argc , CHAR16** Argv);
EFI_STATUS UI_Change_Password(UINTN Argc , CHAR16** Argv);

EFI_STATUS UI_Help_Certain(CHAR16* instruction);

EFI_STATUS (*UI_Function[]) (UINTN Argc , CHAR16** Argv) = {
    UI_Help,
    UI_NvLock_Status,
    UI_Unlock,
    UI_Change_Password
};

#endif

#ifndef _UI_BASE_OPERATION_
#define _UI_BASE_OPERATION_

EFI_STATUS UI_Get_String(CHAR16** output , UINTN* size_of_input);

int Check_Instruction(CHAR16* instruction);
EFI_STATUS Print_Instruction_Detail(UINTN start , UINTN end);

#endif