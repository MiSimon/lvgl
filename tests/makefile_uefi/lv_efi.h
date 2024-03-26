#ifndef __LVGL_TESTS_MAKEFILE_UEFI_LV_EFI_H__
#define __LVGL_TESTS_MAKEFILE_UEFI_LV_EFI_H__

#include "../../lvgl.h"

#ifdef __cplusplus
#define extern "C" {
#endif

/*************************************
 * GLOBALS
*************************************/
extern EFI_BOOT_SERVICES *gBS;
extern EFI_RUNTIME_SERVICES *gRS;
extern EFI_SYSTEM_TABLE *gST;
extern EFI_HANDLE *gImageHandle;

/*************************************
 * API
*************************************/
EFI_STATUS lv_efi_init(EFI_HANDLE image_handle, EFI_SYSTEM_TABLE* system_table);
void lv_efi_deinit();

lv_display_t* lv_efi_init_display(EFI_GRAPHICS_OUTPUT_PROTOCOL* protocol);
void lv_efi_deinit_display(lv_display_t* display);

lv_indev_t* lv_efi_init_keyboard(EFI_SIMPLE_TEXT_INPUT_PROTOCOL* protocol);
void lv_efi_deinit_keyboard(lv_indev_t* indev);

#ifdef __cplusplus
}
#endif

#endif