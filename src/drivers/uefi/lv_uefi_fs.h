#ifndef __LV_UEFI_FS_H__
#define __LV_UEFI_FS_H__

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../misc/lv_fs.h"

#if LV_USE_UEFI

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * @brief Register a driver for the File system interface.
 * @param handle The handle on which an instance of the
 * EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID protocol is installed.
 * If NULL the device_handle from the LOADED_IMAGE protocol will be used.
 * @param read_only If set to 1 only read access will be allowed.
*/
void lv_fs_uefi_init(
    void * handle,
    uint8_t read_only);

void lv_fs_uefi_deinit();

/**********************
 *      MACROS
 **********************/

#endif

#ifdef __cplusplus
}
#endif

#endif //__LV_UEFI_FS_H__