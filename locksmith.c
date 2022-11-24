#include "locksmith.h"
#include "test.h" // Deleted !! For Security Reason

EFI_STATUS Check_NvLock_Status(BOOLEAN* isLocked){
	EFI_STATUS Status = EFI_SUCCESS;

	EFI_GUID NVLockMailboxVariableGuid = SETUPNVLOCK_MAILBOX_ADDRESS_VARIABLE_GUID;

	SETUPNVLOCK_MAILBOX NvLockMailbox_original_status;

	UINTN mailbox_size;

	// Initialization
	*isLocked = TRUE;

	// Check Current Setting
	mailbox_size = sizeof(NvLockMailbox_original_status);
	Status = gRT->GetVariable(L"NvLockMailbox", 
                              &NVLockMailboxVariableGuid, 
                              NULL, 
                              &mailbox_size, 
                              &NvLockMailbox_original_status);
	if(EFI_ERROR(Status)){
		My_Debugger("locksmith.c/Check_NvLock_Status" , "Get Variable \"NvLockMailBox\" Error");
		goto END;
	}

	if(NvLockMailbox_original_status.NvLockState != 1 || NvLockMailbox_original_status.NvLockState != 0){
		My_Debugger("locksmith.c/Check_NvLock_Status" , "Invalid Nvlock Status");
		Status = EFI_LOAD_ERROR;
		Print(L"NvLock Status Error\n");
		goto END;
	}

	*isLocked = (NvLockMailbox_original_status.NvLockState == 1) ? TRUE : FALSE;

	END:

	return Status;
}

EFI_STATUS NvLock_Unlock(CHAR16* user_input_password){
	// Variables Declearation
	EFI_STATUS Status = EFI_SUCCESS;

	BOOLEAN isLocked = TRUE;

	EFI_GUID SetupPwdGuid = SETUPPASSWD_GUID;
	
	CHAR16* password;
	UINTN size_of_input_password = 0;

	// Check Input Parameter
	if(user_input_password == NULL){
		My_Debugger("locksmith.c/NvLock_Unlock" , "Input NULL Parameter");
		Status = EFI_INVALID_PARAMETER;
		goto END;
	}

	size_of_input_password = StrLen(user_input_password);
	if(size_of_input_password > SETUP_PASSWORD_LENGTH || size_of_input_password <= SETUP_PASSWORD_LENGTH_MINIMUM - 1){ // Check Password Length
		My_Debugger("locksmith.c/NvLock_Unlock" , "Password Length Invalid");
		Status = EFI_INVALID_PARAMETER;
		goto END;
	}

	// Check Current NvLock Status
	Status = Check_NvLock_Status(&isLocked);
	if(EFI_ERROR(Status)){
		My_Debugger("locksmith.c/NvLock_Unlock" , "Check NvLock Error");
		goto END;
	}

	if(!isLocked){
		Print(L"Already unlocked\n");
		goto END;
	}

	// NvLock Unlock
	password = (CHAR16*)malloc(size_of_input_password * sizeof(CHAR16));
	if(password == NULL){
		My_Debugger("locksmith.c/NvLock_Unlock" , "Memory Allocation Error");
		Status = EFI_OUT_OF_RESOURCES;
		goto END;
	}
	else{
		CopyMem(password , user_input_password , size_of_input_password * sizeof(CHAR16));
	}

	if(CASE_SENSITIVE){
		Status = Convert_to_Upper_Case(password , size_of_input_password);

		if(EFI_ERROR(Status)){
			My_Debugger("locksmith.c/NvLock_Unlock" , "Case Convert Error");
			goto END;
		}
	}

	// Unlock Process
	Status = gRT->SetVariable(L"$SETUPPASSWD",
		                      &SetupPwdGuid,
		                      0,
		                      size_of_input_password * sizeof(CHAR16),
		                      password);
	if(EFI_ERROR(Status)){
		My_Debugger("locksmith.c/NvLock_Unlock" , "Set Variable Error");
		goto END;
	}

	// Check unlock result
	isLocked = TRUE;
	Status = Check_NvLock_Status(&isLocked);
	if(EFI_ERROR(Status)){
		My_Debugger("locksmith.c/NvLock_Unlock" , "Check NvLock Error");
		goto END;
	}

	if(isLocked){
		My_Debugger("locksmith.c/NvLock_Unlock" , "NvLock Unlock Error");
		Status = EFI_LOAD_ERROR;
		goto END;
	}

	Print(L"Unlock Success\n");

	END:
	free(password);

	return Status;
}

EFI_STATUS NvLock_Change_Password(CHAR16* password , BOOLEAN ChangeAdmin){
	EFI_STATUS Status = EFI_SUCCESS;

	EFI_GUID TseSetupGuid = AMITSESETUP_GUID;
	UINT32 SetupDataAttributes; // Deleted !! For Security Reason
    UINTN SetupDataSize = sizeof(AMITSESETUP);
    UINT16 Buffer_1[sizeof(AMITSESETUP)];
    UINT16 Buffer_2[sizeof(AMITSESETUP)];

    CHAR16* temp_password = NULL;
    BOOLEAN SHA256_Status = FALSE;

    // Initialization
    SetMem(Buffer_1 , sizeof(AMITSESETUP) , 0);
    SetMem(Buffer_2 , sizeof(AMITSESETUP) , 0);

    // Check Current Setting
    Print(L"Check Current Setting \n");
	Status = gRT->GetVariable(TSE_SETUP_VAR_NAME, 
                              &TseSetupGuid, 
                              &SetupDataAttributes, 
                              &SetupDataSize, 
                              &Buffer_1);
	if(EFI_ERROR(Status)){
		goto END;
	}

	Print(L"Check Current Password:\n");
	Print_Array(2 , ((AMITSESETUP*)(Buffer_1))->AdminPassword , SETUP_PASSWORD_LENGTH , 10);

	// Hashing
	Print(L"Start Hashing\n");
	temp_password = NULL;
	temp_password = (CHAR16*)malloc(SETUP_PASSWORD_LENGTH * sizeof(CHAR16));
	if(temp_password == NULL){
		My_Debugger("test.c/Test_Change_Password" , "Memory Allocation Error");
		goto END;
	}
	SetMem(temp_password , SETUP_PASSWORD_LENGTH * 2 , 0);
	SHA256_Status = Sha256HashAll(password , SETUP_PASSWORD_LENGTH * 2 , (UINT8*)temp_password);
	if(!SHA256_Status){
		Print(L"SHA-256 Error\n");
		goto END;
	}

	Print(L"Check Hashing Result\n");
	Print_Array(1 , temp_password , SETUP_PASSWORD_LENGTH * 2 , 10);
	Print(L"\n");


	// Set Password
	Print(L"Set Password -- ");
	CopyMem(((AMITSESETUP*)(Buffer_2))->AdminPassword , temp_password , SETUP_PASSWORD_LENGTH * 2);

	// Set Password // Deleted !! For Security Reason

	if(EFI_ERROR(Status)){
		Print(L"Error\n");
		Print(L"Error Code : %d\n" , Status);
		goto END;
	}
	else{
		Print(L"Success\n");
	}

	END:
	free(temp_password);
	free(Buffer_1);
	free(Buffer_2);

	return Status;
}