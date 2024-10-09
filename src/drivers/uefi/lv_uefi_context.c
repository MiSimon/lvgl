/*********************
 *      INCLUDES
 *********************/

#include "lv_uefi_context.h"
#include "lv_uefi_private.h"
#if LV_USE_UEFI

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  GOLBAL VARIABLES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * @brief Initialize the UEFI chache variables.
 * @remark This has to be called before lv_init().
*/
void lv_uefi_init(
    EFI_HANDLE image_handle,
    EFI_SYSTEM_TABLE * system_table)
{
    if(image_handle == NULL) return;
    if(system_table == NULL) return;

    gLvEfiImageHandle = image_handle;
    gLvEfiST = system_table;
    gLvEfiBS = gLvEfiST->BootServices;
    gLvEfiRT = gLvEfiST->RuntimeServices;
}

/**
 * @brief Initialize the LVGL UEFI backend.
 * @remark This is a private API which is used for LVGL UEFI backend
 *         implementation. LVGL users shouldn't use that because the
 *         LVGL has already used it in lv_init.
 */
void lv_uefi_platform_init()
{
    LV_ASSERT_NULL(gLvEfiImageHandle);
    LV_ASSERT_NULL(gLvEfiST);
    LV_ASSERT_NULL(gLvEfiBS);
    LV_ASSERT_NULL(gLvEfiRT);

    lv_uefi_timer_init();
}

void lv_uefi_platform_deinit()
{
    lv_uefi_timer_deinit();
}

/**
 * @brief Get the current millisecond value.
 * @return The value or MAX_UINT32 if the value is unknown.
 */
uint32_t lv_uefi_get_milliseconds()
{
    lv_uefi_timer_state_t state;

    if(!lv_uefi_timer_get_state(&state)) return MAX_UINT32;

    return (uint32_t)((state.counter_value * 1000) / state.frequency);
}

void lv_uefi_log_cb(lv_log_level_t level, const char * buf)
{
    CHAR16 buffer[256];
    UINTN index;

    for(index = 0; index < 255 && buf[index] != '\0'; index ++) {
        buffer[index] = buf[index];
    }
    buffer[index] = L'\0';

    gLvEfiST->ConOut->OutputString(
        gLvEfiST->ConOut, buffer);

    gLvEfiST->ConOut->OutputString(
        gLvEfiST->ConOut, L"\r");
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif