/**
 * @file lv_uefi_private_timer.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "../../lvgl.h"
#include "../../core/lv_global.h"

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
 * HPET 1.0a: 2.3.1 Register Overview
 *
 * @see https://www.intel.com/content/dam/www/public/us/en/documents/technical-specifications/software-developers-hpet-spec-1-0a.pdf
 */
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

#ifdef offsetof
    LV_UEFI_STATIC_ASSERT(offsetof(lv_uefi_hpet_register_t, capabilities) == 0, "Invalid HPET register structure");
    LV_UEFI_STATIC_ASSERT(offsetof(lv_uefi_hpet_register_t, configuration) == 16, "Invalid HPET register structure");
    LV_UEFI_STATIC_ASSERT(offsetof(lv_uefi_hpet_register_t, main_counter_value) == 240, "Invalid HPET register structure");
#endif

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

static bool lv_uefi_timer_init_acpi(
    lv_uefi_timer_context_t * context);
static void lv_uefi_timer_deinit_acpi(
    lv_uefi_timer_context_t * context);

static bool lv_uefi_timer_init_cpu(
    lv_uefi_timer_context_t * context);
static void lv_uefi_timer_deinit_cpu(
    lv_uefi_timer_context_t * context);

/**********************
 *  GOLBAL VARIABLES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

static EFI_GUID _uefi_guid_timestamp = EFI_TIMESTAMP_PROTOCOL_GUID;

static lv_uefi_acpi_table_signature_t _uefi_acpi_signature_hpet = {{'H', 'P', 'E', 'T'}};
static lv_uefi_acpi_table_signature_t _uefi_acpi_signature_fadt = {{'F', 'A', 'C', 'P'}};

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * @brief Get the state of the timer
 * @param state Pointer to store the state
 * @return True if the timer has a valid state, false if not
*/
bool lv_uefi_timer_get_state(lv_uefi_timer_state_t * state)
{
    lv_uefi_timer_context_t * timer_ctx = &(LV_GLOBAL_DEFAULT()->uefi_timer_ctx);
    uint64_t current_value;

    LV_ASSERT_NULL(state);

    lv_memset(state, 0x00, sizeof(lv_uefi_timer_state_t));

    switch(timer_ctx->source) {
        case LV_UEFI_TIME_SOURCE_TIMESTAMP_PROTOCOL:
            current_value = timer_ctx->source_meta.timestamp_protocol.interface->GetTimestamp();
            break;
        case LV_UEFI_TIME_SOURCE_HPET:
            current_value = *(timer_ctx->source_meta.hpet.main_counter_value_register);
            // check if the timer is running in 32 bit mode and has rolled over
            if(current_value < timer_ctx->last_value && timer_ctx->last_value < UINT32_MAX) {
                current_value += UINT32_MAX - timer_ctx->last_value + 1;
            }
            break;
        case LV_UEFI_TIME_SOURCE_ACPI:
            current_value = (uint64_t) * timer_ctx->source_meta.acpi.timer_address;
            break;
        case LV_UEFI_TIME_SOURCE_CPU:
            current_value = lv_uefi_cpu_counter_get_value();
            break;
        default:
            return FALSE;
    }

    // check if the timer has rolled over
    if(current_value < timer_ctx->last_value) {
        current_value += timer_ctx->meta.max_value - timer_ctx->last_value + 1;
    }
    timer_ctx->tick_value += current_value - timer_ctx->last_value;
    timer_ctx->last_value = current_value;

    state->counter_value = timer_ctx->tick_value;
    state->frequency = timer_ctx->meta.frequency;

    return TRUE;
}

/**
 * @brief Initialize the timer context
*/
void lv_uefi_timer_init()
{
    lv_uefi_timer_context_t * timer_ctx = &(LV_GLOBAL_DEFAULT()->uefi_timer_ctx);

    lv_memset(timer_ctx, 0x00, sizeof(lv_uefi_timer_context_t));

    LV_LOG_INFO("Trying to find the timestamp protocol.");
    if(lv_uefi_timer_init_timestamp_protocol(timer_ctx))
        return;
    LV_LOG_INFO("Trying to find the CPU timer.");
    if(lv_uefi_timer_init_cpu(timer_ctx))
        return;
    LV_LOG_INFO("Trying to find the HPET timer.");
    if(lv_uefi_timer_init_hpet(timer_ctx))
        return;
    LV_LOG_INFO("Trying to find the ACPI timer.");
    if(lv_uefi_timer_init_acpi(timer_ctx))
        return;
    LV_LOG_WARN("No timer available.");

    lv_memset(timer_ctx, 0x00, sizeof(lv_uefi_timer_context_t));
}

/**
 * @brief Cleanup the timer context
*/
void lv_uefi_timer_deinit()
{
    lv_uefi_timer_context_t * timer_ctx = &(LV_GLOBAL_DEFAULT()->uefi_timer_ctx);

    switch(timer_ctx->source) {
        case LV_UEFI_TIME_SOURCE_TIMESTAMP_PROTOCOL:
            lv_uefi_timer_deinit_timestamp_protocol(timer_ctx);
            break;
        case LV_UEFI_TIME_SOURCE_HPET:
            lv_uefi_timer_deinit_hpet(timer_ctx);
            break;
        case LV_UEFI_TIME_SOURCE_ACPI:
            lv_uefi_timer_deinit_acpi(timer_ctx);
            break;
        case LV_UEFI_TIME_SOURCE_CPU:
            lv_uefi_timer_deinit_cpu(timer_ctx);
            break;
        default:
            break;
    }

    lv_memset(timer_ctx, 0x00, sizeof(lv_uefi_timer_context_t));
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
    const lv_uefi_acpi_table_hpet_t * hpet_acpi_table = NULL;
    const lv_uefi_hpet_register_t * hpet_register = NULL;
    uint64_t frequency;

    // try to find the HPET table
    hpet_acpi_table = (const lv_uefi_acpi_table_hpet_t *) lv_uefi_locate_acpi_table(&_uefi_acpi_signature_hpet);
    if(hpet_acpi_table == NULL) {
        LV_LOG_INFO("[lv_uefi] ACPI HPET not found.");
        return FALSE;
    }

    // we can only handle system memory addresses
    if(hpet_acpi_table->address.address_space_id != 0) return FALSE;
    // check if the address is valid
    if(hpet_acpi_table->address.address == 0) return FALSE;

    hpet_register = (const lv_uefi_hpet_register_t *)hpet_acpi_table->address.address;

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

    LV_LOG_INFO("[lv_uefi] ACPI HPET can be used, frequency: %llu Hz.", context->meta.frequency);

    return TRUE;
}

static void lv_uefi_timer_deinit_hpet(
    lv_uefi_timer_context_t * context)
{
    ;
}

static bool lv_uefi_timer_init_acpi(
    lv_uefi_timer_context_t * context)
{
    const lv_uefi_acpi_table_fadt_t * fadt_acpi_table = NULL;

    // try to find the FADT table
    fadt_acpi_table = (const lv_uefi_acpi_table_fadt_t *) lv_uefi_locate_acpi_table(&_uefi_acpi_signature_fadt);
    if(fadt_acpi_table == NULL) {
        LV_LOG_INFO("[lv_uefi] ACPI FADT not found.");
        return FALSE;
    }

    if(fadt_acpi_table->pm_timer_length != 4) {
        LV_LOG_INFO("[lv_uefi] ACPI timer is not available.");
        return FALSE;
    }

    // we can only handle system memory addresses
    if(fadt_acpi_table->x_pm_timer_block.address_space_id != 0) return FALSE;
    // check if the address is valid
    if(fadt_acpi_table->x_pm_timer_block.address == 0) return FALSE;

    context->source = LV_UEFI_TIME_SOURCE_ACPI;
    // ACPI timer has a frequency of 3.579545 MHz as long as the system is in the S0 state
    context->meta.frequency = 3579545;
    // If bit 8 in the flags field is set, the timer is a 32 bit timer and if not it is a 24 bit timer
    context->meta.max_value = (fadt_acpi_table->flags & (1 << 8)) != 0 ? UINT32_MAX : 0x00FFFFFF;
    context->source_meta.acpi.timer_address = (uint32_t *) fadt_acpi_table->x_pm_timer_block.address;

    LV_LOG_INFO("[lv_uefi] ACPI timer can be used, frequency: %llu Hz.", context->meta.frequency);

    return TRUE;
}

static void lv_uefi_timer_deinit_acpi(
    lv_uefi_timer_context_t * context)
{
    ;
}

static bool lv_uefi_timer_init_cpu(
    lv_uefi_timer_context_t * context)
{
    if(!lv_uefi_cpu_timer_is_supported()) {
        LV_LOG_INFO("[lv_uefi] CPU timer is not supported.");
        return FALSE;
    }

    context->source = LV_UEFI_TIME_SOURCE_CPU;
    context->meta.frequency = lv_uefi_cpu_timer_get_frequency();
    context->meta.max_value = UINT64_MAX;

    LV_LOG_INFO("[lv_uefi] CPU timer can be used, frequency: %llu Hz.", context->meta.frequency);

    return TRUE;
}

static void lv_uefi_timer_deinit_cpu(
    lv_uefi_timer_context_t * context)
{
    ;
}

#endif