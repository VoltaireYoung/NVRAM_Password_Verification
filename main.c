/*
 * @Author: Voltaire Chen
 * @Date :  2022-09-19 16:00:00
 * @Last Modified by: Voltaire Chen
 * @Last Modified time: 2022-09-21 02:24:00
 */

#include "common.h"
#include "verifier.h"
#include "ui.h"
#include "test.h" // Deleted !! For Security Reason

EFI_STATUS
EFIAPI
ShellAppMain(IN UINTN Argc , IN CHAR16 **Argv){
    UI_Base(Argc , Argv);

    return 0;
}