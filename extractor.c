/*
 * @Author: Voltaire Chen
 * @Date :  2022-09-19 16:00:00
 * @Last Modified by: Voltaire Chen
 * @Last Modified time: 2022-09-21 02:24:00
 */

#include "extractor.h"
#include "test.h" // Deleted !! For Security Reason

EFI_STATUS Get_TSE_Setup(AMITSESETUP* tse_setup){
	if(tse_setup == NULL){
		My_Debugger("extractor.c/Get_TSE_Setup" , "Input NULL Pointer");
		return EFI_INVALID_PARAMETER;
	}

	// Variables Declearation
	EFI_STATUS Status = EFI_SUCCESS;
	EFI_GUID TseSetupGuid = AMITSESETUP_GUID;
	UINT32 SetupDataAttributes = 0;
    UINTN SetupDataSize = sizeof(AMITSESETUP);
    UINT16 Buffer[sizeof(AMITSESETUP)];

    //Initialization
    // Status = gBS->SetMem((VOID*)tse_setup , 0 , sizeof(AMITSESETUP));
    // if(EFI_ERROR(Status)){
    // 	My_Debugger("extractor.c/Get_TSE_Setup" , "Initialization Error");
    // 	return Status;
    // }

    // Get AMI TSE Setup from NVRAM by Runtime Service
	Status = gRT->GetVariable(TSE_SETUP_VAR_NAME, 
                              &TseSetupGuid, 
                              &SetupDataAttributes, 
                              &SetupDataSize, 
                              &Buffer);
	if(EFI_ERROR(Status)){
		My_Debugger("extractor.c/Get_TSE_Setup" , "Get Variable Error");
    	goto END;
	}

	CopyMem(tse_setup , Buffer , sizeof(AMITSESETUP));

	END:
	free(Buffer);

	return Status;
}