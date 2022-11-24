#include "common.h"
#include "verifier.h"
#include "extractor.h"

#ifndef _LOCKSMITH_
#define _LOCKSMITH_

EFI_STATUS Check_NvLock_Status(BOOLEAN* isLocked);
EFI_STATUS NvLock_Unlock(CHAR16* user_input_password);
EFI_STATUS NvLock_Change_Password(CHAR16* password , BOOLEAN ChangeAdmin);

#endif

#ifndef _NVLOCK_MAILBOX_
#define _NVLOCK_MAILBOX_

#define SETUPNVLOCK_MAILBOX_ADDRESS_VARIABLE_GUID \ // Deleted !! For Security Reason

typedef struct {
    UINTN NvLockState;
} SETUPNVLOCK_MAILBOX;

#endif

#ifndef _NVLOCK_SPECIFICATION_
#define _NVLOCK_SPECIFICATION_

#define SETUPPASSWD_GUID \
    { 0x5855CE1B, 0xFB8E, 0x47E4, {0xBC, 0x1A, 0x39, 0xEC, 0xAA, 0x0C, 0x96, 0xCF}}

#endif