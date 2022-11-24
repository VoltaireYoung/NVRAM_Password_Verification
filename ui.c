/*
 * @Author: Voltaire Chen
 * @Date :  2022-09-19 16:00:00
 * @Last Modified by: Voltaire Chen
 * @Last Modified time: 2022-09-21 02:24:00
 */

#include "ui.h"
#include "test.h" // Deleted !! For Security Reason

// UI for showing
EFI_STATUS UI_Base(UINTN Argc , CHAR16* Argv[]){
    // Variable Declearation
    EFI_STATUS Status = EFI_SUCCESS;

    CHAR16* first_instruction = Argv[1];
    int instruction_code = 0;

    // Parameters Check
    if(Argc <= 1){
        UI_Help(Argc , Argv);
        Status = EFI_INVALID_PARAMETER;
        goto END;
    }

    // Instruction UI
    instruction_code = Check_Instruction(first_instruction);
    if(instruction_code == -1 || instruction_code >= Number_of_Instructions){
        My_Debugger("ui.c/UI_Base" , "Invalid Instruction");
        Print(L"Invalid Instruction\n");
        UI_Help(Argc , Argv);
        goto END;
    }

    // Function Pointers
    Status = UI_Function[instruction_code](Argc , Argv);
    if(EFI_ERROR(Status)){
        My_Debugger("ui.c/UI_Base" , "UI Operation Error");
        goto END;
    }

    END:
    return Status;
}

EFI_STATUS UI_Help(UINTN Argc , CHAR16** Argv){
    // Variable Decleartion
    EFI_STATUS Status = EFI_SUCCESS;

    Status = Print_Instruction_Detail(0 , Number_of_Instructions);
    
    return Status;
}

EFI_STATUS UI_Help_Certain(CHAR16* instruction){
    if(instruction == NULL){
        My_Debugger("ui.c/UI_Help_Certain" , "Input NULL Instruction");
        return EFI_INVALID_PARAMETER;
    }

    EFI_STATUS Status = EFI_SUCCESS;
    int instruction_code = Check_Instruction(instruction);

    if(instruction_code < 0 || instruction_code >= Number_of_Instructions){
        My_Debugger("ui.c/UI_Help_Certain" , "Invalid Instruction");
        Status = EFI_INVALID_PARAMETER;
    }
    else{
        Status = Print_Instruction_Detail(instruction_code , instruction_code + 1);
    }

    return Status;
}

EFI_STATUS UI_NvLock_Status(UINTN Argc , CHAR16** Argv){
    EFI_STATUS Status = EFI_SUCCESS;

    CHAR16* instruction = Argv[1];
    BOOLEAN isValid = FALSE;
    BOOLEAN isLocked = TRUE;

    // Check number of paramters
    if(Argc != 2){
        My_Debugger("ui.c/UI_NvLock_Status" , "Not Enough Parameters");
        Status = UI_Help(Argc , Argv);
        goto END;
    }

    // Check instruction
    isValid = (StrCmp(instruction , Instruction_NvLock_Status) != 0) ? FALSE : TRUE;

    if(!isValid){
        My_Debugger("ui.c/UI_NvLock_Status" , "Invalid Instruction");
        Status = UI_Help_Certain(Instruction_NvLock_Status);
        goto END;
    }

    Status = Check_NvLock_Status(&isLocked);
    if(EFI_ERROR(Status)){
        My_Debugger("ui.c/UI_NvLock_Status" , "Check NvLock Status Error");
        goto END;
    }

    if(isLocked){
        Print(L"NvLock Status : Locked\n");
    }
    else{
        Print(L"NvLock Status : Unlocked\n");
    }

    END:
    return Status;
}

EFI_STATUS UI_Unlock(UINTN Argc , CHAR16** Argv){
    EFI_STATUS Status = EFI_SUCCESS;

    CHAR16* instruction;
    CHAR16* password;
    UINTN password_length = 0;

    BOOLEAN isValid = FALSE;

    // Check number of parameters
    if(Argc != 3){
        My_Debugger("ui.c/UI_Unlock" , "Invalid Number of Parameteres");
        Status = UI_Help_Certain(Instruction_Unlock);
        goto END;
    }

    // Check parameters
    instruction = Argv[1];
    password = Argv[2];

    isValid = (StrCmp(instruction , Instruction_Unlock) != 0) ? FALSE : TRUE;  // Check Instruction
    if(!isValid){
        My_Debugger("ui.c/UI_Unlock" , "Invalid Instruction");
        Status = UI_Help_Certain(Instruction_Unlock);
        goto END;
    }

    password_length = StrLen(password);
    if(password_length < SETUP_PASSWORD_LENGTH_MINIMUM || password_length > SETUP_PASSWORD_LENGTH){
        Print(L"Invalid Password Length\n");
        My_Debugger("ui.c/UI_Unlock" , "Invalid Password Length");
        Status = UI_Help_Certain(Instruction_Unlock);
        goto END;
    }

    // Unlock Process
    Status = NvLock_Unlock(password);
    if(EFI_ERROR(Status)){
        My_Debugger("ui.c/UI_Unlock" , "Unlock Process Error");
        goto END;
    }

    END:
    return Status;
}

// EFI_STATUS UI_Password_Verification(UINTN Argc , CHAR16** Argv){
//     Print(L"In UI_Password_Verification\n");
//     return EFI_SUCCESS;
// }

EFI_STATUS UI_Change_Password(UINTN Argc , CHAR16** Argv){
    EFI_STATUS Status = EFI_SUCCESS;

    CHAR16* instruction;
    CHAR16* password;
    CHAR16* temp_password = NULL;
    CHAR16* target;
    UINTN password_length = 0;

    BOOLEAN isValid = FALSE;
    BOOLEAN Change_Admin_Password = FALSE;

    // Variable Initialization
    temp_password = (CHAR16*)malloc(SETUP_PASSWORD_LENGTH * sizeof(CHAR16));
    if(temp_password == NULL){
        My_Debugger("ui.c/UI_Change_Password" , "Variable \"temp_password\" Memory Allocation Error");
        Status = EFI_OUT_OF_RESOURCES;
        goto END;
    }
    SetMem(temp_password , SETUP_PASSWORD_LENGTH * 2 , 0);

    // Check number of parameters
    if(Argc < 3 || Argc >= 5){
        My_Debugger("ui.c/UI_Change_Password" , "Not Enough Parameters");
        Status = UI_Help(Argc , Argv);
        goto END;
    }

    // Check Parameters
    instruction = Argv[1];

    if(Argc == 3){ // Without Certain Target instruction
        target = L"-admin";
        password = Argv[2];
    }
    else if(Argc == 4){ // With Certain Target instruction
        target = Argv[2];
        password = Argv[3];
    }
    else{ // Default
        target = L"-admin";
        password = Argv[2];
    }

    isValid = (StrCmp(instruction , Instruction_Change_Password) != 0) ? FALSE : TRUE;  // Check Instruction
    if(!isValid){
        My_Debugger("ui.c/UI_Change_Password" , "Invalid Instruction");
        Status = UI_Help_Certain(Instruction_Change_Password);
        goto END;
    }

    password_length = StrLen(password);
    if(password_length < SETUP_PASSWORD_LENGTH_MINIMUM || password_length > SETUP_PASSWORD_LENGTH){
        Print(L"Invalid Password Length\n");
        My_Debugger("ui.c/UI_Change_Password" , "Invalid Password Length");
        Status = UI_Help(Argc , Argv);
        goto END;
    }

    Change_Admin_Password = (StrCmp(target , L"-admin") == 0) ? TRUE : FALSE;
    CopyMem(temp_password , password , password_length * 2);
    if(CASE_SENSITIVE){
        Status = Convert_to_Upper_Case(temp_password , password_length);

        if(EFI_ERROR(Status)){
            My_Debugger("ui.c/UI_Change_Password" , "Case Convert Error");
            goto END;
        }
    }

    // Change Password Process
    Status = NvLock_Change_Password(temp_password , Change_Admin_Password);
    if(EFI_ERROR(Status)){
        My_Debugger("ui.c/UI_Change_Password" , "Change Password Process Error");
        goto END;
    }

    END:
    free(temp_password);
    return Status;
}

// UI Base Operation
EFI_STATUS UI_Get_String(CHAR16** output , UINTN* size_of_input){
    if(/*output == NULL ||*/ size_of_input == NULL){
        My_Debugger("ui.h" , "Input NULL Pointer");
        return EFI_INVALID_PARAMETER;
    }

    // Variables Declearation
    EFI_STATUS Status = 0;
    static CHAR16* Buffer;

    // Get Input from Shell
    Status = ShellPromptForResponse(ShellPromptResponseTypeFreeform,
                                    L"",
                                    (VOID**)&Buffer);

    if(EFI_ERROR(Status)){
        My_Debugger("ui.c/UI_Get_String" , "Get Input from Shell Error\n");
        return Status;
    }

    *output = Buffer;
    *size_of_input = StrLen(*output);
    Status = EFI_SUCCESS;

    return Status;
}

int Check_Instruction(CHAR16* instruction){
    // Variable Declearation
    BOOLEAN isValid = FALSE;

    int current_index = -1;

    // Check Instruction Lists
    if(sizeof(Instructions) <= 0){
        My_Debugger("ui.c/Check_Instruction" , "Instructions List Error");
        goto END;
    }

    // Check Instruction
    for(current_index = 0 ; current_index < Number_of_Instructions ; current_index++){
        if(StrCmp(instruction , Instructions[current_index]) == 0){
            isValid = TRUE;
            break;
        }
    }

    current_index = (current_index >= Number_of_Instructions) ? -1 : current_index;

    END:
    return current_index;
}

EFI_STATUS Print_Instruction_Detail(UINTN start , UINTN end){
    // Check Input
    if( (start > end) || 
        (start < 0 || start >= Number_of_Instructions) ||
         end < 0 || end > Number_of_Instructions){
        My_Debugger("ui.c/Print_Instruction_Detail" , "Invalid Input Parameters");
        return EFI_INVALID_PARAMETER;
    }

    UINTN current_instruction_index;

    for(current_instruction_index = start ; current_instruction_index < end ; current_instruction_index++){
        Print(L"<%s>\n" , Instructions[current_instruction_index]);
        Print(L"%s\n" , Instructions_Description[current_instruction_index]);
        Print(L"\n");
    }

    return EFI_SUCCESS;
}