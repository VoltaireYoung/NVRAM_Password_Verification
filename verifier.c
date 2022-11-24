/*
 * @Author: Voltaire Chen
 * @Date :  2022-09-19 16:00:00
 * @Last Modified by: Voltaire Chen
 * @Last Modified time: 2022-09-21 02:24:00
 */

#include "verifier.h"
#include "extractor.h"
#include "ui.h"
#include "test.h" // Deleted !! For Security Reason

// Verification Main Function
EFI_STATUS TSE_Setup_Password_Verification(){
	// Variables Declearation
	EFI_STATUS Status = EFI_SUCCESS;

	BOOLEAN IsVerified = FALSE;

	User_Input_Password* user_input = NULL;
	CHAR16* Temp = NULL;

	AMITSESETUP* tse_setup = NULL;
	UINT16* Password_Ciphertext = NULL;
	UINT8 tse_setup_information = 0;
	UINT8 admin_password_information = 0;
	UINT8 user_password_information = 0;
	UINT8 temp_passowrd_information = 0;
	BOOLEAN haveAdminPassword = FALSE;
	BOOLEAN haveUserPassword = FALSE;

	// Initialization
	user_input = (User_Input_Password*)malloc(sizeof(User_Input_Password));
	if(user_input == NULL){
		My_Debugger("verifier.c/TSE_Setup_Password_Verification" , "Variable \"user_input_password\" Memory Allocation Error");
		Status = EFI_OUT_OF_RESOURCES;
		goto END;
	}
	SetMem(user_input->user_input_password , SETUP_PASSWORD_LENGTH * 2 , 0);

	tse_setup = (AMITSESETUP*)malloc(sizeof(AMITSESETUP));
	if(tse_setup == NULL){
		My_Debugger("verifier.c/TSE_Setup_Password_Verification" , "Variable \"tse_setup\" Memory Allocation Error");
		Status = EFI_OUT_OF_RESOURCES;
		goto END;
	}

	// Extract password from NVRAM
	Status = Get_TSE_Setup(tse_setup);
	if(EFI_ERROR(Status)){
		My_Debugger("verifier.c/TSE_Setup_Password_Verification" , "Extract AMITSESETUP Error");
		goto END;
	}

	   // Check NVRAM Variable Specification
	   // => Hash Algorithm
	Status = Check_AMITSESETUP(tse_setup , &tse_setup_information);
	if(EFI_ERROR(Status)){
		My_Debugger("verifier.c/TSE_Setup_Password_Verification" , "Extract TSE Setup Error");
		goto END;
	}

	admin_password_information = (tse_setup_information & 0xF0) >> 4;         // Information Specification
	user_password_information = tse_setup_information & 0x0F;                 // _ _ _ _ | _ _ _ _
	haveAdminPassword = (admin_password_information & 0x8) ? TRUE : FALSE;    //  Admin  |   User
	haveUserPassword = (user_password_information & 0x8) ? TRUE : FALSE;      // Lift Significant Bit : Mark for having such password
	                                                                          // Rest 3 Bit: Hash Algorithm Code (111 reserved as invalid state)

	// Verify Password
	// Only verify one of password (Admin or User Password)
	if(haveUserPassword){
		Password_Ciphertext = tse_setup->UserPassword;
		temp_passowrd_information = user_password_information;
		Print(L"Please input user password : ");
	}
	else if(haveAdminPassword){
		Password_Ciphertext = tse_setup->AdminPassword;
		temp_passowrd_information = admin_password_information;
		Print(L"Please input administrator password : ");
	}
	else{
		Print(L"No Password in NVRAM\n");
		Status = EFI_SUCCESS;
		goto END;
	}

		// Get user input password for verification
	Status = UI_Get_String(&Temp , &(user_input->size_of_user_input_password));
	if(EFI_ERROR(Status)){
		My_Debugger("verifier.c/TSE_Setup_Password_Verification" , "Get User Input Error");
		goto END;
	}
		// Check Input Length
	if(user_input->size_of_user_input_password > SETUP_PASSWORD_LENGTH){
		My_Debugger("verifier.c/TSE_Setup_Password_Verification" , "Input String Length longer than limitation");
		Status = EFI_UNSUPPORTED;
		goto END;
	}
	else{
		CopyMem(user_input->user_input_password , Temp , user_input->size_of_user_input_password * 2);
	}

		// Choose Verification Process according to the hash algorithm
	Status = Password_Verification(Password_Ciphertext , user_input , &temp_passowrd_information , &IsVerified);
	if(EFI_ERROR(Status)){
		My_Debugger("verifier.c/TSE_Setup_Password_Verification" , "Password Verification Error");
		goto END;
	}
	

	if(!IsVerified){
		Print(L"Password Verification Fail\n");
	}
	else{
		Print(L"Password Verification Success\n");
	}

	Status = EFI_SUCCESS;

	END:
	free(tse_setup);
	free(user_input);

	return Status;
}

// OTP
EFI_STATUS OTP(UINT16* input , UINT16* output , UINTN size_of_patterns){
	if(input == NULL || output == NULL){
		My_Debugger("In verifier.c/OTP" , "Input NULL Pointer");
		return EFI_INVALID_PARAMETER;
	}
	
	// Variables Declearation
	EFI_STATUS Status = EFI_SUCCESS;

	SETUP_PASSWORD_WORD_TYPE* masks;
	int current_pattern_index;

	// Initialization
	masks = (SETUP_PASSWORD_WORD_TYPE*)malloc(size_of_patterns * sizeof(SETUP_PASSWORD_WORD_TYPE));
	if(masks == NULL){
		My_Debugger("verifier.c/OTP" , "Variable \"masks\" Memory Allocation Error");
		Status = EFI_OUT_OF_RESOURCES;
		goto END;
	}

	Status = Get_OTP_Masks(masks);
	if(EFI_ERROR(Status)){
		My_Debugger("verifier.c/OTP" , "Get OTP Mask Error");
		Status = EFI_LOAD_ERROR;
		goto END;
	}

	// Hashing
	for(current_pattern_index = 0 ; current_pattern_index < size_of_patterns ; current_pattern_index++){
		output[current_pattern_index] = input[current_pattern_index] ^ masks[current_pattern_index];
	}

	END:
	free(masks);

	return Status;
}

EFI_STATUS Get_OTP_Masks(UINT16* masks){
	if(masks == NULL){
		My_Debugger("verifier.c/Get_OTP_Masks" , "Input NULL Pointer");
		return EFI_INVALID_PARAMETER;
	}

	// Variables Declearation
	int total_number_of_masks = SETUP_PASSWORD_LENGTH;
	int current_mask_index;

	UINT16 key = OTP_Key;

	// Calculate Masks
	for(current_mask_index = 0 ; current_mask_index < total_number_of_masks ; current_mask_index++){
		masks[current_mask_index] = (UINT16)((key * (current_mask_index + 1)) & 0xffff);
	}

	return EFI_SUCCESS;
}

// Password Verification
EFI_STATUS Password_Verification(UINT16* Password_Ciphertext , User_Input_Password* User_Input , UINT8* Information , BOOLEAN* IsVerified){
	if(Password_Ciphertext == NULL ||
	   User_Input == NULL ||
	   Information == NULL ||
	   IsVerified == NULL){
		My_Debugger("verifier.c/Password_Verification" , "Input NULL Pointer");
		return EFI_INVALID_PARAMETER;
	}

	// Variables Declearation
	EFI_STATUS Status = EFI_SUCCESS;

	UINT16* Password_Temp = NULL;

	UINT8 Password_exist_bit = 0;
	BOOLEAN HavePassword = FALSE;

	UINT8 Passoword_Hash_Code = 0;
	BOOLEAN Valid_Hash = FALSE;

	UINT16* Password_1 = NULL;
	UINT16* Password_2 = NULL;
	UINTN Password_Length = 0;
	UINTN Number_of_bytes_to_compare = 0;

	BOOLEAN SHA_Success = FALSE;

	// Initialization
	Password_Temp = (UINT16*)malloc(SETUP_PASSWORD_LENGTH * sizeof(UINT16));
	if(Password_Temp == NULL){
		My_Debugger("verifier.c/Password_Verification" , "Variable \"Password_Temp\" Memory Allocation Error");
		Status = EFI_OUT_OF_RESOURCES;
		goto END;
	}
	SetMem(Password_Temp , SETUP_PASSWORD_LENGTH * 2 , 0);

	*IsVerified = FALSE;

	// Verify passowrd
		// Check if password exists
	Password_exist_bit = (*Information & HAVE_PASSWORD); // Get No.4 bit
	HavePassword = (Password_exist_bit) ? TRUE : FALSE;
	if(!HavePassword){
		My_Debugger("verifier.c/Password_Verification" , "Input Parameter Invalid");
		Status = EFI_INVALID_PARAMETER;
		goto END;
	}

	Passoword_Hash_Code = (*Information & HASH_CODE_MASK); // Get No.1 ~ No.3 bit
	Valid_Hash = (Passoword_Hash_Code == INVALID_HASH) ? FALSE : TRUE;
	if(!Valid_Hash){
		My_Debugger("verifier.c/Password_Verification" , "Invalid Hash Algorithm");
		Status = EFI_UNSUPPORTED;
		goto END;
	}

		// Case Convert
	if(CASE_SENSITIVE){
		Status = Convert_to_Upper_Case(User_Input->user_input_password , User_Input->size_of_user_input_password);

		if(EFI_ERROR(Status)){
			My_Debugger("verifier.c/TSE_Setup_Password_Verification" , "Case Convert Error");
			goto END;
		}
	}

		// Verify passowrd according to the hash algorithm
	if(ALGORITHM_OTP == Passoword_Hash_Code){ // OTP
		Status = OTP(Password_Ciphertext , Password_Temp , SETUP_PASSWORD_LENGTH);
		if(EFI_ERROR(Status)){
			My_Debugger("verifier.c/Password_Verification" , "OTP Decode Error");
			goto END;
		}

		Status = Get_Password_Length(Password_Temp , &Password_Length);
		if(EFI_ERROR(Status)){
			My_Debugger("verifier.c/Password_Verification" , "Get Password Length Error");
			goto END;
		}

		if(Password_Length != User_Input->size_of_user_input_password){
			*IsVerified = FALSE;
			Status = EFI_LOAD_ERROR;
			goto END;
		}

		Password_1 = Password_Temp;
		Password_2 = (UINT16*)(User_Input->user_input_password);
		Number_of_bytes_to_compare = SETUP_PASSWORD_LENGTH * 2;
	}
	else if(ALGORITHM_SHA256 == Passoword_Hash_Code){ // SHA256
		SHA_Success = Sha256HashAll(/* Deleted !! For Security Reason*/);

		if(!SHA_Success){
			My_Debugger("verifier.c/Password_Verification" , "SHA256 Error");
			Status = EFI_LOAD_ERROR;
			goto END;
		}

		Password_1 = Password_Ciphertext;
		Password_2 = Password_Temp;
		Number_of_bytes_to_compare = SHA256_DIGEST_SIZE; // CryptoPkg/Include/Library/BaseCrpytLib.h
	}
	else{
		My_Debugger("verifier.c/Password_Verification" , "Invalid Password Hash Code");
		Status = EFI_UNSUPPORTED;
		goto END;
	}

	*IsVerified = (CompareMem(Password_1 , Password_2 , Number_of_bytes_to_compare) == 0) ? TRUE : FALSE;

	Status = EFI_SUCCESS;

	END:
	free(Password_Temp);

	return Status;
}


// Check AMITSESETUP
EFI_STATUS Check_AMITSESETUP(AMITSESETUP* tse_setup , UINT8* information){
	if(tse_setup == NULL){
		My_Debugger("verifier.c/Check_AMITSESETUP" , "Input NULL Pointer");
		return EFI_INVALID_PARAMETER;
	}

	// Variables Declearation
	EFI_STATUS Status = EFI_SUCCESS;

	UINT8 temp_information = 0;
	UINT8 temp_algorithm = 0;
	BOOLEAN IsEmpty = TRUE;

	// Password Information Specification
	// Information(UINT8):_ _ _ _ | _ _ _ _
	//                    (Admin)    (User)
	// Left Significant Bit: Notify as having certain type of password
	// Rest of bits: Adopted Hash Algorithm Code
	//               => 0x0 :OTP
	//                  0x1 :SHA 256
	//                  0x7 :Reserved

	// Classify Password
	// Check Admin Password
	temp_information = 0;
	temp_algorithm = 0;
	IsEmpty = TRUE;
    Status = Check_Empty(tse_setup->AdminPassword , SETUP_PASSWORD_LENGTH , &IsEmpty);
    if(EFI_ERROR(Status)){
    	My_Debugger("verifier.c/Check_AMITSESETUP" , "Check Empty Error");
    	goto END;
    }

	if(!IsEmpty){
		temp_information = 0x8;

		temp_algorithm = 0;
		Status = Check_Algorithm(tse_setup->AdminPassword , SETUP_PASSWORD_LENGTH , &temp_algorithm);
		if(EFI_ERROR(Status)){
			My_Debugger("verifier.c/Check_AMITSESETUP" , "Check Algorithm Error");
			goto END;
		}
		temp_information |= temp_algorithm;
	}
	*information += temp_information;

	// Check User Password
	*information <<= 4;
	temp_information = 0;
	temp_algorithm = 0;
	IsEmpty = TRUE;
	Status = Check_Empty(tse_setup->UserPassword , SETUP_PASSWORD_LENGTH , &IsEmpty);
	if(EFI_ERROR(Status)){
    	My_Debugger("verifier.c/Check_AMITSESETUP" , "Check Empty Error");
    	goto END;
    }

	if(!IsEmpty){
		temp_information = 0x8;

		temp_algorithm = 0;
		Status = Check_Algorithm(tse_setup->UserPassword , SETUP_PASSWORD_LENGTH , &temp_algorithm);
		if(EFI_ERROR(Status)){
			My_Debugger("verifier.c/Check_AMITSESETUP" , "Check Algorithm Error");
			goto END;
		}
		temp_information |= temp_algorithm;
	}
	*information += temp_information;

	END:

	return Status;
}

EFI_STATUS Check_Empty(UINT16* array , UINTN checking_size , BOOLEAN* IsEmpty){
	if(array == NULL || checking_size <= 0 || IsEmpty == NULL){
		My_Debugger("verifier.c/Check_Empty" , "Input NULL Pointer");
		return EFI_INVALID_PARAMETER;
	}

	// Variables Declearation
	int current_index = 0;

	// Initialization
	*IsEmpty = TRUE;

	// Checking wheather empty
	while(current_index < checking_size){
		if(array[current_index] != 0){
			*IsEmpty = FALSE;
			break;
		}
		current_index++;
	}

	return EFI_SUCCESS;
}

EFI_STATUS Check_Algorithm(UINT16* password , UINTN checking_size , UINT8* algorithm){
	if(password == NULL){
		My_Debugger("verifier.c/Check_Algorithm" , "Input NULL Pointer");
		return EFI_INVALID_PARAMETER;
	}

	// Variables Declearation
	UINTN counter = 0;
	int current_index = 0;

	// Initialization
	*algorithm = 0;

	// Element Counting
	while(current_index < checking_size){
		if(password[current_index] == 0){
			break;
		}

		current_index++;
	}

	// Classify Hash Algorithm
	counter = current_index;

	if(counter == checking_size){ // OTP
		*algorithm = ALGORITHM_OTP;
	}
	else if(counter == SHA256_DIGEST_SIZE / 2){ // SHA256 -> Hash Value Size = 32 bytes (256 bits)
		*algorithm = ALGORITHM_SHA256;          // The Reason of divide by 2 : Counting by UINT16
	}
	else{ // Default for no match
		*algorithm = ALGORITHM_RESERVED;
	}

	// For security
	*algorithm = (*algorithm >= ALGORITHM_RESERVED || *algorithm < 0) ? ALGORITHM_RESERVED : *algorithm;

	return EFI_SUCCESS;
}

// Tools
EFI_STATUS Get_Password_Length(UINT16* password , UINTN* password_length){
	if(password == NULL || password_length == NULL){
		My_Debugger("verifier.c/Get_Password_Length" , "Input NULL Pointer");
		return EFI_INVALID_PARAMETER;
	}

	// Variables Declearation
	int index;

	// Initialization
	*password_length = 0;

	// Counting
	for(index = 0 ; index < SETUP_PASSWORD_LENGTH ; index++){
		if(password[index] == 0){
			break;
		}

		(*password_length)++;
	}

	return EFI_SUCCESS;
}

EFI_STATUS Convert_to_Upper_Case(CHAR16* password , UINTN size_of_password){
	if(password == NULL){
		My_Debugger("verifier.c/Convert_to_Upper_Case" , "Input NULL Pointer");
		return EFI_LOAD_ERROR;
	}

	// Variables Declearation
	int index;
	UINT16 current_offset;

	// Convert
	for(index = 0 ; index < size_of_password ; index++){
		if(password[index] >= 'a' && password[index] <= L'z'){
			current_offset = ((UINT16*)password)[index] - (UINT16)(L'a');
			password[index] = ((UINT16)(L'A') + current_offset);
		}
	}

	return EFI_SUCCESS;
}