/*
 * @Author: Voltaire Chen
 * @Date :  2022-09-19 16:00:00
 * @Last Modified by: Voltaire Chen
 * @Last Modified time: 2022-09-21 02:24:00
 */

#include "common.h"

#ifndef _VERIFIER_
#define _VERIFIER_
// Verification Main Function
EFI_STATUS TSE_Setup_Password_Verification();

// OTP
EFI_STATUS OTP(SETUP_PASSWORD_WORD_TYPE* input , SETUP_PASSWORD_WORD_TYPE* output , UINTN size_of_patterns);
EFI_STATUS Get_OTP_Masks(SETUP_PASSWORD_WORD_TYPE* masks);

// Check Password
EFI_STATUS Password_Verification(UINT16* Password_Ciphertext , User_Input_Password* User_Input , UINT8* Information , BOOLEAN* IsVerified);

// Check AMITSESETUP
EFI_STATUS Check_AMITSESETUP(AMITSESETUP* tse_setup , UINT8* information);
EFI_STATUS Check_Empty(UINT16* array , UINTN checking_size , BOOLEAN* IsEmpty);
EFI_STATUS Check_Algorithm(UINT16* password , UINTN checking_size , UINT8* algorithm);

// Tools
EFI_STATUS Get_Password_Length(UINT16* password , UINTN* password_length);
EFI_STATUS Convert_to_Upper_Case(CHAR16* password , UINTN size_of_password);

#endif