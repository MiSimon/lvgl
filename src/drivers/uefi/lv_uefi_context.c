/*********************
 *      INCLUDES
 *********************/

#include "lv_uefi_context.h"
#include "lv_uefi_private.h"
#include "lv_uefi_fs.h"
#if LV_USE_UEFI

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef enum {
    LV_UEFI_TIME_SOURCE_UNKNOWN = 0,
    LV_UEFI_TIME_SOURCE_TIMESTAMP_PROTOCOL,
    LV_UEFI_TIME_SOURCE_HPET,
} lv_uefi_time_source_t;

typedef struct _lv_uefi_timer_context_t {
    lv_uefi_time_source_t source;
    uint64_t tick_value;
    uint64_t last_value;
    struct {
        uint64_t max_value;
        uint64_t frequency;
    } meta;
    union {
        struct {
            EFI_TIMESTAMP_PROTOCOL * interface;
        } timestamp_protocol;
        struct {
            uint64_t * main_counter_value_register;
        } hpet;
    } source_meta;
} lv_uefi_timer_context_t;

#pragma pack(push, 1)
typedef struct _lv_uefi_acpi_table_signature_t {
    char signature[4];
} lv_uefi_acpi_table_signature_t;

typedef struct _lv_uefi_acpi_address_t {
    uint8_t address_space_id;
    uint8_t register_bit_width;
    uint8_t register_bit_offset;
    uint8_t reserved;
    uint64_t address;
} lv_uefi_acpi_address_t;

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

typedef struct _lv_uefi_acpi_table_xsdt_t {
    lv_uefi_acpi_table_header_t header;
    lv_uefi_acpi_table_header_t * entries[0];
} lv_uefi_acpi_table_xsdt_t;

typedef struct _lv_uefi_acpi_table_hpet_t {
    lv_uefi_acpi_table_header_t header;
    uint32_t event_timer_block_id;
    lv_uefi_acpi_address_t address;
    uint8_t hpet_number;
    uint16_t main_counter_minimum_clock_tick;
    uint8_t attributes;
} lv_uefi_acpi_table_hpet_t;

typedef struct _lv_uefi_hpet_register_t {
    uint64_t capabilities;
    uint64_t reserved1;
    uint64_t configuration;
    uint64_t reserved2;
    uint64_t interrupt_status;
    uint64_t reserved3;
    uint8_t  reserved4[0xF0 - 0x30];
    uint64_t main_counter_value;
} lv_uefi_hpet_register_t;

#pragma pack(pop)

/**********************
 *  STATIC PROTOTYPES
 **********************/

static bool lv_uefi_timer_init_timestamp_protocol(
    lv_uefi_timer_context_t * context);
static void lv_uefi_timer_deinit_timestamp_protocol(
    lv_uefi_timer_context_t * context);

static bool lv_uefi_timer_init_hpet(
    lv_uefi_timer_context_t * context);
static void lv_uefi_timer_deinit_hpet(
    lv_uefi_timer_context_t * context);

static void lv_uefi_timer_init();
static void lv_uefi_timer_deinit();

static const void * lv_uefi_locate_acpi_table(const lv_uefi_acpi_table_signature_t * signature);
static const lv_uefi_acpi_table_xsdt_t * lv_uefi_locate_acpi_2_0_xsdt();
static const lv_uefi_acpi_rsdp_t * lv_uefi_locate_acpi_2_0_rsdp();

/**********************
 *  GOLBAL VARIABLES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

static EFI_GUID _uefi_guid_acpi_rsdp = { 0x8868E871, 0xE4F1, 0x11D3, {0xBC, 0x22, 0x00, 0x80, 0xC7, 0x3C, 0x88, 0x81 }};
static lv_uefi_acpi_table_signature_t _uefi_acpi_signature_hpet = {{'H', 'P', 'E', 'T'}};
static lv_uefi_acpi_table_signature_t _uefi_acpi_signature_xsdt = {{'X', 'S', 'D', 'T'}};
static lv_uefi_timer_context_t timer_context;

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
    lv_fs_uefi_deinit();
    lv_uefi_timer_deinit();
}

/**
 * @brief Get the current millisecond value.
 * @return The value or 0xFFFFFFFF if the value is unknown.
 */
uint32_t lv_uefi_get_milliseconds()
{
    EFI_STATUS status;
    uint64_t current_value;

    switch(timer_context.source) {
        case LV_UEFI_TIME_SOURCE_TIMESTAMP_PROTOCOL:
            current_value = timer_context.source_meta.timestamp_protocol.interface->GetTimestamp();
            break;
        case LV_UEFI_TIME_SOURCE_HPET:
            current_value = *timer_context.source_meta.hpet.main_counter_value_register;
            // check if the timer is running in 32 bit mode and has rolled over
            if(current_value < timer_context.last_value && timer_context.last_value < UINT32_MAX) {
                current_value += UINT32_MAX - timer_context.last_value + 1;
            }
            break;
        default:
            return UINT32_MAX;
    }

    // check if the timer has rolled over
    if(current_value < timer_context.last_value) {
        current_value += timer_context.meta.max_value - timer_context.last_value + 1;
    }
    timer_context.tick_value += current_value - timer_context.last_value;
    timer_context.last_value = current_value;

    return (uint32_t)((timer_context.tick_value * 1000) / timer_context.meta.frequency);
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

static bool lv_uefi_timer_init_timestamp_protocol(
    lv_uefi_timer_context_t * context)
{
    EFI_STATUS status;
    EFI_GUID guid = EFI_TIMESTAMP_PROTOCOL_GUID;
    EFI_TIMESTAMP_PROTOCOL * interface = NULL;
    EFI_TIMESTAMP_PROPERTIES properties;

    LV_ASSERT_NULL(context);

    status = gLvEfiBS->LocateProtocol(&guid, NULL, (VOID **)&interface);
    if(status != EFI_SUCCESS)
        return FALSE;

    status = interface->GetProperties(&properties);
    if(status != EFI_SUCCESS)
        return FALSE;

    if(properties.Frequency < 1000)
        return FALSE;

    context->source = LV_UEFI_TIME_SOURCE_TIMESTAMP_PROTOCOL;
    context->meta.frequency = properties.Frequency;
    context->meta.max_value = properties.EndValue;
    context->source_meta.timestamp_protocol.interface = interface;

    return TRUE;
}

static void lv_uefi_timer_deinit_timestamp_protocol(
    lv_uefi_timer_context_t * context)
{
    ;
}

static bool lv_uefi_timer_init_hpet(
    lv_uefi_timer_context_t * context)
{
    const lv_uefi_acpi_table_hpet_t * hpet_description_table = NULL;
    const lv_uefi_hpet_register_t * hpet_register = NULL;
    uint64_t frequency;

    // try to find the HPET descripton table
    hpet_description_table = (const lv_uefi_acpi_table_hpet_t *) lv_uefi_locate_acpi_table(&_uefi_acpi_signature_hpet);
    if(hpet_description_table == NULL) {
        LV_LOG_INFO("[lv_uefi] ACPI HPET not found.");
        return FALSE;
    }

    // we can only handle system memory addresses
    if(hpet_description_table->address.address_space_id != 0) return FALSE;

    hpet_register = (const lv_uefi_hpet_register_t *)hpet_description_table->address.address;

    // check if the timer is enabled
    if((hpet_register->configuration & (1 << 0)) == 0) {
        LV_LOG_INFO("[lv_uefi] ACPI HPET not enabled.");
        return FALSE;
    }

    frequency = (1ULL * 1000U * 1000U * 1000U * 1000U * 1000U) / (uint64_t)(hpet_register->capabilities >> 32);
    if(frequency < 1000) {
        LV_LOG_INFO("[lv_uefi] ACPI HPET frequency too low.");
        return FALSE;
    }

    context->source = LV_UEFI_TIME_SOURCE_HPET;
    context->meta.frequency = frequency;
    // We can only check if the value is 32 bit width but not if it runs in 64 bit mode
    context->meta.max_value = UINT64_MAX;
    context->source_meta.hpet.main_counter_value_register = (uint64_t *) &hpet_register->main_counter_value;

    LV_LOG_INFO("[lv_uefi] ACPI HPET can be used, frequency: %llu Hz.", frequency);

    return TRUE;
}

static void lv_uefi_timer_deinit_hpet(
    lv_uefi_timer_context_t * context)
{
    ;
}

static void lv_uefi_timer_init()
{
    lv_memset(&timer_context, 0x00, sizeof(lv_uefi_timer_context_t));

    LV_LOG_INFO("Trying to find the timestamp protocol.");
    if(lv_uefi_timer_init_timestamp_protocol(&timer_context))
        return;
    LV_LOG_INFO("Trying to find the HPET timer.");
    if(lv_uefi_timer_init_hpet(&timer_context))
        return;
    LV_LOG_WARN("No timer available.");

    lv_memset(&timer_context, 0x00, sizeof(lv_uefi_timer_context_t));
}

static void lv_uefi_timer_deinit()
{
    switch(timer_context.source) {
        case LV_UEFI_TIME_SOURCE_TIMESTAMP_PROTOCOL:
            lv_uefi_timer_deinit_timestamp_protocol(&timer_context);
            break;
        case LV_UEFI_TIME_SOURCE_HPET:
            lv_uefi_timer_deinit_hpet(&timer_context);
            break;
        default:
            break;
    }

    lv_memset(&timer_context, 0x00, sizeof(lv_uefi_timer_context_t));
}

static const void * lv_uefi_locate_acpi_table(const lv_uefi_acpi_table_signature_t * signature)
{
    EFI_STATUS status;
    UINTN index;
    UINTN no_entries;
    const lv_uefi_acpi_table_xsdt_t * xsdt = NULL;

    xsdt = lv_uefi_locate_acpi_2_0_xsdt();
    if(xsdt == NULL) return NULL;

    no_entries = xsdt->header.length - sizeof(lv_uefi_acpi_table_xsdt_t) / 8;

    for(index = 0; index < no_entries; index++) {
        if(lv_memcmp(signature, &xsdt->entries[index]->signature, sizeof(lv_uefi_acpi_table_signature_t)) != 0) {
            continue;
        }
        return xsdt->entries[index];
    }

    return NULL;
}

static const lv_uefi_acpi_table_xsdt_t * lv_uefi_locate_acpi_2_0_xsdt()
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

static const lv_uefi_acpi_rsdp_t * lv_uefi_locate_acpi_2_0_rsdp()
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

#endif