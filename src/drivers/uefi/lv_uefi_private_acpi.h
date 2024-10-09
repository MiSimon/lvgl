/**
 * @file lv_uefi_private_acpi.h
 *
 */

#ifndef __LV_UEFI_PRIVATE_ACPI_H__
#define __LV_UEFI_PRIVATE_ACPI_H__

#ifdef __cplusplus
extern "C" {
#endif

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

#pragma pack(push, 1)

/**
 * ACPI 6.5: 5.2.6. System Description Table Header
 *
 * @see https://uefi.org/specs/ACPI/6.5/
 */
typedef struct _lv_uefi_acpi_table_signature_t {
    char signature[4];
} lv_uefi_acpi_table_signature_t;

/**
 * ACPI 6.5: 5.2.3.2. Generic Address Structure
 *
 * @see https://uefi.org/specs/ACPI/6.5/
 */
typedef struct _lv_uefi_acpi_address_t {
    uint8_t address_space_id;
    uint8_t register_bit_width;
    uint8_t register_bit_offset;
    uint8_t reserved;
    uint64_t address;
} lv_uefi_acpi_address_t;

/**
 * ACPI 6.5: 5.2.6. System Description Table Header
 *
 * @see https://uefi.org/specs/ACPI/6.5/
 */
typedef struct _lv_uefi_acpi_table_header_t {
    lv_uefi_acpi_table_signature_t signature;
    uint32_t length;
    uint8_t revision;
    uint8_t checksum;
    char oem_id[6];
    char oem_table_id[8];
    uint32_t oem_revision;
    uint32_t creator_id;
    uint32_t creator_revision;
} lv_uefi_acpi_table_header_t;

/**
 * ACPI 6.5: 5.2.5.3. Root System Description Pointer (RSDP) Structure
 *
 * @see https://uefi.org/specs/ACPI/6.5/
 */
typedef struct _lv_uefi_acpi_rsdp_t {
    char signature[8];
    uint8_t checksum;
    char oem_id[6];
    uint8_t revision;
    uint32_t rsdt_address;
    // the following fields are only present in verion > 2.0
    uint32_t length;
    uint64_t xsdt_address;
    uint8_t extended_checksum;
    uint8_t reserved[3];
} lv_uefi_acpi_rsdp_t;

#ifdef offsetof
LV_UEFI_STATIC_ASSERT(offsetof(lv_uefi_acpi_rsdp_t, xsdt_address) == 24, "Invalid RSDP structure");
#endif

/**
 * ACPI 6.5: 5.2.8. Extended System Description Table (XSDT)
 *
 * @see https://uefi.org/specs/ACPI/6.5/
 */
typedef struct _lv_uefi_acpi_table_xsdt_t {
    lv_uefi_acpi_table_header_t header;
    lv_uefi_acpi_table_header_t * entries[0];
} lv_uefi_acpi_table_xsdt_t;

#ifdef offsetof
LV_UEFI_STATIC_ASSERT(offsetof(lv_uefi_acpi_table_xsdt_t, entries) == 36, "Invalid XSDT structure");
#endif

/**
 * ACPI 6.5: 5.2.9. Fixed ACPI Description Table (FADT)
 *
 * @see https://uefi.org/specs/ACPI/6.5/
 */
typedef struct _lv_uefi_acpi_table_fadt_t {
    lv_uefi_acpi_table_header_t header;
    uint32_t firmware_ctrl;
    uint32_t dsdt;
    uint8_t reserved;
    uint8_t prefered_power_management_profile;
    uint16_t sci_interrupt;
    uint32_t smi_command_port;
    uint8_t acpi_enable;
    uint8_t acpi_disable;
    uint8_t s4bios_req;
    uint8_t pstate_control;
    uint32_t pm1a_event_block;
    uint32_t pm1b_event_block;
    uint32_t pm1a_control_block;
    uint32_t pm1b_control_block;
    uint32_t pm2_control_block;
    uint32_t pm_timer_block;
    uint32_t gpe0_block;
    uint32_t gpe1_block;
    uint8_t pm1_event_length;
    uint8_t pm1_control_length;
    uint8_t pm2_control_length;
    uint8_t pm_timer_length;
    uint8_t gpe0_length;
    uint8_t gpe1_length;
    uint8_t gpe1_base;
    uint8_t c_state_control;
    uint16_t worst_c2_latency;
    uint16_t worst_c3_latency;
    uint16_t flush_size;
    uint16_t flush_stride;
    uint8_t duty_offset;
    uint8_t duty_width;
    uint8_t day_alarm;
    uint8_t month_alarm;
    uint8_t century;
    // the following fields are only present in verion > 2.0
    uint16_t boot_architecture_flags;
    uint8_t reserved2;
    uint32_t flags;
    lv_uefi_acpi_address_t reset_reg;
    uint8_t reset_value;
    uint8_t reserved3[3];
    uint64_t x_firmware_control;
    uint64_t x_dsdt;
    lv_uefi_acpi_address_t x_pm1a_event_block;
    lv_uefi_acpi_address_t x_pm1b_event_block;
    lv_uefi_acpi_address_t x_pm1a_control_block;
    lv_uefi_acpi_address_t x_pm1b_control_block;
    lv_uefi_acpi_address_t x_pm2_control_block;
    lv_uefi_acpi_address_t x_pm_timer_block;
    lv_uefi_acpi_address_t x_gpe0_block;
    lv_uefi_acpi_address_t x_gpe1_block;
} lv_uefi_acpi_table_fadt_t;

#ifdef offsetof
LV_UEFI_STATIC_ASSERT(offsetof(lv_uefi_acpi_table_fadt_t, pm_timer_block) == 76, "Invalid FADT structure");
LV_UEFI_STATIC_ASSERT(offsetof(lv_uefi_acpi_table_fadt_t, pm_timer_length) == 91, "Invalid FADT structure");
LV_UEFI_STATIC_ASSERT(offsetof(lv_uefi_acpi_table_fadt_t, flags) == 112, "Invalid FADT structure");
LV_UEFI_STATIC_ASSERT(offsetof(lv_uefi_acpi_table_fadt_t, x_pm_timer_block) == 208, "Invalid FADT structure");
#endif

/**
 * HPET 1.0a: 3.2.4 The ACPI 2.0 HPET Description Table (HPET)
 *
 * @see https://www.intel.com/content/dam/www/public/us/en/documents/technical-specifications/software-developers-hpet-spec-1-0a.pdf
 */
typedef struct _lv_uefi_acpi_table_hpet_t {
    lv_uefi_acpi_table_header_t header;
    uint32_t event_timer_block_id;
    lv_uefi_acpi_address_t address;
    uint8_t hpet_number;
    uint16_t main_counter_minimum_clock_tick;
    uint8_t attributes;
} lv_uefi_acpi_table_hpet_t;

#ifdef offsetof
LV_UEFI_STATIC_ASSERT(offsetof(lv_uefi_acpi_table_hpet_t, address) == 40, "Invalid HPET structure");
#endif

#pragma pack(pop)

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * @brief Try to find the table that matches the signature
 * @return A pointer to the table or NULL if the table was not found
*/
const void * lv_uefi_locate_acpi_table(const lv_uefi_acpi_table_signature_t * signature);

/**
 * @brief Try to find the XSDT table
 * @return A pointer to the table or NULL if the table was not found
*/
const lv_uefi_acpi_table_xsdt_t * lv_uefi_locate_acpi_2_0_xsdt();

/**
 * @brief Try to find the RSDP
 * @return A pointer to the RSDP or NULL if the table was not found
*/
const lv_uefi_acpi_rsdp_t * lv_uefi_locate_acpi_2_0_rsdp();

/**********************
 *      MACROS
 **********************/

#endif

#ifdef __cplusplus
}
#endif

#endif //__LV_UEFI_PRIVATE_ACPI_H__