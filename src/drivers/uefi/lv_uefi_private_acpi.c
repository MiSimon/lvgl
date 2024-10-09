/**
 * @file lv_uefi_private_acpi.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "../../lvgl.h"

#if LV_USE_UEFI

#include "lv_uefi_private.h"

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

static EFI_GUID _uefi_guid_acpi_rsdp = { 0x8868E871, 0xE4F1, 0x11D3, {0xBC, 0x22, 0x00, 0x80, 0xC7, 0x3C, 0x88, 0x81 }};
static lv_uefi_acpi_table_signature_t _uefi_acpi_signature_xsdt = {{'X', 'S', 'D', 'T'}};

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * @brief Try to find the table that matches the signature
 * @return A pointer to the table or NULL if the table was not found
*/
const void * lv_uefi_locate_acpi_table(const lv_uefi_acpi_table_signature_t * signature)
{
    EFI_STATUS status;
    UINTN index;
    UINTN no_entries;
    const lv_uefi_acpi_table_xsdt_t * xsdt = NULL;

    xsdt = lv_uefi_locate_acpi_2_0_xsdt();
    if(xsdt == NULL) return NULL;

    no_entries = (xsdt->header.length - sizeof(lv_uefi_acpi_table_xsdt_t)) / 8;

    for(index = 0; index < no_entries; index++) {
        if(lv_memcmp(signature, &xsdt->entries[index]->signature, sizeof(lv_uefi_acpi_table_signature_t)) != 0) {
            continue;
        }
        return xsdt->entries[index];
    }

    return NULL;
}

/**
 * @brief Try to find the XSDT table
 * @return A pointer to the table or NULL if the table was not found
*/
const lv_uefi_acpi_table_xsdt_t * lv_uefi_locate_acpi_2_0_xsdt()
{
    const lv_uefi_acpi_table_xsdt_t * xsdt = NULL;
    const lv_uefi_acpi_rsdp_t * rsdp = NULL;

    rsdp = lv_uefi_locate_acpi_2_0_rsdp();
    if(rsdp == NULL) return NULL;
    if(rsdp->revision != 0x02) {
        LV_LOG_WARN("[lv_uefi] ACPI unsupported revision.");
        return NULL;
    }

    xsdt = (const lv_uefi_acpi_table_xsdt_t *)(rsdp->xsdt_address);

    if(lv_memcmp(&xsdt->header.signature, &_uefi_acpi_signature_xsdt, 4) != 0) {
        LV_LOG_WARN("[lv_uefi] ACPI XSDT not found.");
        return NULL;
    }

    return xsdt;
}

/**
 * @brief Try to find the RSDP
 * @return A pointer to the RSDP or NULL if the table was not found
*/
const lv_uefi_acpi_rsdp_t * lv_uefi_locate_acpi_2_0_rsdp()
{
    UINTN index;

    for(index = 0; index < gLvEfiST->NumberOfTableEntries; index++) {
        if(lv_memcmp(&_uefi_guid_acpi_rsdp, &gLvEfiST->ConfigurationTable[index].VendorGuid, sizeof(EFI_GUID)) != 0) {
            continue;
        }
        return (lv_uefi_acpi_rsdp_t *) gLvEfiST->ConfigurationTable[index].VendorTable;
    }

    LV_LOG_WARN("[lv_uefi] ACPI RSDP not found.");

    return NULL;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif