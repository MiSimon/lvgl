/**
 * @file lv_uefi_indev.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "../../lvgl.h"
#include "../../stdlib/lv_mem.h"
#include "../../misc/lv_types.h"
#include "../../misc/lv_text.h"

#if LV_USE_UEFI

#include "lv_uefi_indev.h"
#include "lv_uefi_private.h"

#if LV_USE_UEFI_GNU_EFI
    // gnu-efi uses a type name that is different to the one in the specification
    typedef EFI_SIMPLE_TEXT_IN_PROTOCOL EFI_SIMPLE_TEXT_INPUT_PROTOCOL;
#endif

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct _lv_uefi_pointer_context_t {
    EFI_HANDLE handle;
    BOOLEAN absolute;
    lv_point_t display_res;
    union {
        struct {
            lv_point_t position;
            lv_point_t pixel_per_step_8;
        } simple;
        struct {
            lv_point_t range;
            lv_point_t factor_8;
        } absolute;
    } meta;
    union {
        EFI_SIMPLE_POINTER_PROTOCOL * simple;
        EFI_ABSOLUTE_POINTER_PROTOCOL * absolute;
    } protocol;
} lv_uefi_pointer_context_t;

typedef struct _lv_uefi_keyboard_context_t {
    EFI_HANDLE handle;
    BOOLEAN ex;
    struct {
        BOOLEAN release_is_next;
        UINT32 last_key;
    } meta;
    union {
        EFI_SIMPLE_TEXT_INPUT_PROTOCOL * simple;
        EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL * ex;
    } protocol;
} lv_uefi_keyboard_context_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void lv_uefi_pointer_indev_event_cb(lv_event_t * e);
static void lv_uefi_keyboard_indev_event_cb(lv_event_t * e);

static void lv_uefi_simple_pointer_read_cb(lv_indev_t * indev, lv_indev_data_t * data);
static void lv_uefi_absolute_pointer_read_cb(lv_indev_t * indev, lv_indev_data_t * data);

static void lv_uefi_simple_text_input_read_cb(lv_indev_t * indev, lv_indev_data_t * data);
static void lv_uefi_simple_text_input_ex_read_cb(lv_indev_t * indev, lv_indev_data_t * data);

static uint32_t _utf8_from_unicode(UINT32 unicode);
static uint32_t lv_key_from_uefi_key(EFI_INPUT_KEY * key, UINT32 shift_state);

static void lv_uefi_pointer_context_free(lv_uefi_pointer_context_t * indev_ctx);
static void lv_uefi_keyboard_context_free(lv_uefi_keyboard_context_t * indev_ctx);
static bool lv_uefi_simple_input_interface_is_valid(const EFI_SIMPLE_TEXT_INPUT_PROTOCOL * interface);
static bool lv_uefi_simple_input_ex_interface_is_valid(const EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL * interface);
static bool lv_uefi_simple_pointer_interface_is_valid(const EFI_SIMPLE_POINTER_PROTOCOL * interface);
static bool lv_uefi_absolute_pointer_interface_is_valid(const EFI_ABSOLUTE_POINTER_PROTOCOL * interface);

/**********************
 *  STATIC VARIABLES
 **********************/

static EFI_GUID _uefi_guid_simple_pointer = EFI_SIMPLE_POINTER_PROTOCOL_GUID;
static EFI_GUID _uefi_guid_simple_text_input = EFI_SIMPLE_TEXT_INPUT_PROTOCOL_GUID;
static EFI_GUID _uefi_guid_simple_text_input_ex = EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL_GUID;
static EFI_GUID _uefi_guid_absolute_pointer = EFI_ABSOLUTE_POINTER_PROTOCOL_GUID;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * @brief Create a LVGL indev object.
 * @param handle The handle on which an instance of the EFI_SIMPLE_POINTER_PROTOCOL protocol is installed.
 * @return The created LVGL indev object.
 */
lv_indev_t * lv_uefi_create_simple_pointer_indev(
    void * handle,
    lv_point_t * display_res)
{
    lv_indev_t * indev = NULL;
    lv_uefi_pointer_context_t * indev_ctx = NULL;

    if(!lv_uefi_test_protocol(handle, &_uefi_guid_simple_pointer)) return NULL;

    indev_ctx = lv_calloc(1, sizeof(lv_uefi_pointer_context_t));
    LV_ASSERT_MALLOC(indev_ctx);

    indev_ctx->absolute = FALSE;
    indev_ctx->handle = handle;
    indev_ctx->display_res.x = display_res->x;
    indev_ctx->display_res.y = display_res->y;

    indev_ctx->meta.simple.position.x = 0;
    indev_ctx->meta.simple.position.y = 0;

    indev_ctx->protocol.simple = (EFI_SIMPLE_POINTER_PROTOCOL *)lv_uefi_open_protocol(handle, &_uefi_guid_simple_pointer);
    if(!lv_uefi_simple_pointer_interface_is_valid(indev_ctx->protocol.simple)) {
        LV_LOG_WARN("[lv_uefi] The SIMPLE_POINTER interface is not valid.");
        goto error;
    }

    indev_ctx->meta.simple.pixel_per_step_8.x = (((indev_ctx->display_res.x) << 8) / 50) /
                                                indev_ctx->protocol.simple->Mode->ResolutionX;
    indev_ctx->meta.simple.pixel_per_step_8.y = (((indev_ctx->display_res.y) << 8) / 50) /
                                                indev_ctx->protocol.simple->Mode->ResolutionY;

    indev = lv_indev_create();
    lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
    lv_indev_set_user_data(indev, indev_ctx);
    lv_indev_add_event_cb(indev, lv_uefi_pointer_indev_event_cb, LV_EVENT_DELETE, indev);
    lv_indev_set_read_cb(indev, lv_uefi_simple_pointer_read_cb);

    goto finish;

error:
    if(indev != NULL) {
        lv_indev_set_user_data(indev, NULL);
        lv_indev_delete(indev);
        indev = NULL;
    }

    if(indev_ctx != NULL) lv_uefi_pointer_context_free(indev_ctx);

finish:
    return indev;
}

/**
 * @brief Create a LVGL indev object.
 * @param handle The handle on which an instance of the EFI_ABSOLUTE_POINTER_PROTOCOL protocol is installed.
 * @return The created LVGL indev object.
 */
lv_indev_t * lv_uefi_create_absolute_pointer_indev(
    void * handle,
    lv_point_t * display_res)
{
    lv_indev_t * indev = NULL;
    lv_uefi_pointer_context_t * indev_ctx = NULL;

    if(!lv_uefi_test_protocol(handle, &_uefi_guid_absolute_pointer)) return NULL;

    indev_ctx = lv_calloc(1, sizeof(lv_uefi_pointer_context_t));
    LV_ASSERT_MALLOC(indev_ctx);

    indev_ctx->absolute = TRUE;
    indev_ctx->handle = handle;
    indev_ctx->display_res.x = display_res->x;
    indev_ctx->display_res.y = display_res->y;

    indev_ctx->protocol.absolute = (EFI_ABSOLUTE_POINTER_PROTOCOL *)lv_uefi_open_protocol(handle,
                                                                                          &_uefi_guid_absolute_pointer);
    if(!lv_uefi_absolute_pointer_interface_is_valid(indev_ctx->protocol.absolute)) {
        LV_LOG_WARN("[lv_uefi] The ABSOLUTE_POINTER interface is not valid.");
        goto error;
    }

    indev_ctx->meta.absolute.range.x = indev_ctx->protocol.absolute->Mode->AbsoluteMaxX -
                                       indev_ctx->protocol.absolute->Mode->AbsoluteMinX;
    indev_ctx->meta.absolute.range.y = indev_ctx->protocol.absolute->Mode->AbsoluteMaxY -
                                       indev_ctx->protocol.absolute->Mode->AbsoluteMinY;

    indev_ctx->meta.absolute.factor_8.x = (indev_ctx->display_res.x << 8) / indev_ctx->meta.absolute.range.x;
    indev_ctx->meta.absolute.factor_8.y = (indev_ctx->display_res.y << 8) / indev_ctx->meta.absolute.range.y;

    indev = lv_indev_create();
    lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
    lv_indev_set_user_data(indev, indev_ctx);
    lv_indev_add_event_cb(indev, lv_uefi_pointer_indev_event_cb, LV_EVENT_DELETE, indev);
    lv_indev_set_read_cb(indev, lv_uefi_absolute_pointer_read_cb);

    goto finish;

error:
    if(indev != NULL) {
        lv_indev_set_user_data(indev, NULL);
        lv_indev_delete(indev);
        indev = NULL;
    }

    if(indev_ctx != NULL) lv_uefi_pointer_context_free(indev_ctx);

finish:
    return indev;
}

/**
 * @brief Create a LVGL indev object.
 * @param handle The handle on which an instance of the EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL or EFI_SIMPLE_TEXT_INPUT_PROTOCOL protocol is installed.
 * @return The created LVGL indev object.
 */
lv_indev_t * lv_uefi_create_keyboard_indev(
    void * handle)
{
    lv_indev_t * indev = NULL;
    lv_uefi_keyboard_context_t * indev_ctx = NULL;

    indev_ctx = lv_calloc(1, sizeof(lv_uefi_keyboard_context_t));
    LV_ASSERT_MALLOC(indev_ctx);

    indev_ctx->handle = handle;

    if(lv_uefi_test_protocol(handle, &_uefi_guid_simple_text_input_ex)) {
        indev_ctx->ex = TRUE;

        indev_ctx->protocol.ex = lv_uefi_open_protocol(handle, &_uefi_guid_simple_text_input_ex);
        if(!lv_uefi_simple_input_ex_interface_is_valid(indev_ctx->protocol.ex)) {
            LV_LOG_WARN("[lv_uefi] The SIMPLE_TEXT_INPUT_EX interface is not valid.");
            goto error;
        }

        indev = lv_indev_create();
        lv_indev_set_type(indev, LV_INDEV_TYPE_KEYPAD);
        lv_indev_set_user_data(indev, indev_ctx);
        lv_indev_add_event_cb(indev, lv_uefi_keyboard_indev_event_cb, LV_EVENT_DELETE, indev);
        lv_indev_set_read_cb(indev, lv_uefi_simple_text_input_ex_read_cb);
    }
    else if(lv_uefi_test_protocol(handle, &_uefi_guid_simple_text_input)) {
        indev_ctx->ex = FALSE;

        indev_ctx->protocol.simple = lv_uefi_open_protocol(handle, &_uefi_guid_simple_text_input);
        if(!lv_uefi_simple_input_interface_is_valid(indev_ctx->protocol.simple)) {
            LV_LOG_WARN("[lv_uefi] The SIMPLE_TEXT_INPUT interface is not valid.");
            goto error;
        }

        indev = lv_indev_create();
        lv_indev_set_type(indev, LV_INDEV_TYPE_KEYPAD);
        lv_indev_set_user_data(indev, indev_ctx);
        lv_indev_add_event_cb(indev, lv_uefi_keyboard_indev_event_cb, LV_EVENT_DELETE, indev);
        lv_indev_set_read_cb(indev, lv_uefi_simple_text_input_read_cb);
    }

    goto finish;

error:
    if(indev != NULL) {
        lv_indev_set_user_data(indev, NULL);
        lv_indev_delete(indev);
        indev = NULL;
    }

    if(indev_ctx != NULL) lv_uefi_keyboard_context_free(indev_ctx);

finish:
    return indev;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_uefi_pointer_indev_event_cb(lv_event_t * e)
{
    lv_indev_t * indev;
    lv_uefi_pointer_context_t * indev_ctx;

    if(lv_event_get_code(e) != LV_EVENT_DELETE) return;

    indev = (lv_indev_t *)lv_event_get_user_data(e);

    indev_ctx = (lv_uefi_pointer_context_t *)lv_indev_get_user_data(indev);
    lv_indev_set_user_data(indev, NULL);

    if(indev_ctx != NULL) lv_uefi_pointer_context_free(indev_ctx);
}

static void lv_uefi_keyboard_indev_event_cb(lv_event_t * e)
{
    lv_indev_t * indev;
    lv_uefi_keyboard_context_t * indev_ctx;

    if(lv_event_get_code(e) != LV_EVENT_DELETE) return;

    indev = (lv_indev_t *)lv_event_get_user_data(e);

    indev_ctx = (lv_uefi_keyboard_context_t *)lv_indev_get_user_data(indev);
    lv_indev_set_user_data(indev, NULL);

    if(indev_ctx != NULL) lv_uefi_keyboard_context_free(indev_ctx);
}

static void lv_uefi_simple_pointer_read_cb(lv_indev_t * indev, lv_indev_data_t * data)
{
    EFI_STATUS status;
    EFI_SIMPLE_POINTER_STATE state;
    lv_uefi_pointer_context_t * indev_ctx = (lv_uefi_pointer_context_t *)lv_indev_get_user_data(indev);
    lv_point_t pointer_mov;

    LV_ASSERT_NULL(indev_ctx);

    if(indev_ctx->absolute) return;

    status = indev_ctx->protocol.simple->GetState(
                 indev_ctx->protocol.simple,
                 &state);
    if(status == EFI_NOT_READY) return;
    if(status != EFI_SUCCESS) {
        LV_LOG_ERROR("[lv_uefi] GetState failed.");
        return;
    }

    pointer_mov.x = (state.RelativeMovementX * indev_ctx->meta.simple.pixel_per_step_8.x) >> 8;
    pointer_mov.y = (state.RelativeMovementY * indev_ctx->meta.simple.pixel_per_step_8.y) >> 8;

    indev_ctx->meta.simple.position.x += pointer_mov.x;
    indev_ctx->meta.simple.position.y += pointer_mov.y;

    if(indev_ctx->meta.simple.position.x < 0) {
        indev_ctx->meta.simple.position.x = 0;
    }
    else if(indev_ctx->meta.simple.position.x > indev_ctx->display_res.x - 1) {
        indev_ctx->meta.simple.position.x = indev_ctx->display_res.x - 1;
    }

    if(indev_ctx->meta.simple.position.y < 0) {
        indev_ctx->meta.simple.position.y = 0;
    }
    else if(indev_ctx->meta.simple.position.y > indev_ctx->display_res.y - 1) {
        indev_ctx->meta.simple.position.y = indev_ctx->display_res.y - 1;
    }

    data->point.x = indev_ctx->meta.simple.position.x;
    data->point.y = indev_ctx->meta.simple.position.y;

    data->state = state.LeftButton ? LV_INDEV_STATE_PRESSED : LV_INDEV_STATE_RELEASED;
    data->continue_reading = FALSE;
}

static void lv_uefi_absolute_pointer_read_cb(lv_indev_t * indev, lv_indev_data_t * data)
{
    EFI_STATUS status;
    EFI_ABSOLUTE_POINTER_STATE state;
    lv_uefi_pointer_context_t * indev_ctx = (lv_uefi_pointer_context_t *)lv_indev_get_user_data(indev);
    lv_point_t pointer_pos;

    LV_ASSERT_NULL(indev_ctx);

    if(!indev_ctx->absolute) return;

    status = indev_ctx->protocol.absolute->GetState(
                 indev_ctx->protocol.absolute,
                 &state);
    if(status == EFI_NOT_READY) return;
    if(status != EFI_SUCCESS) {
        LV_LOG_ERROR("[lv_uefi] GetState failed.");
        return;
    }

    // verify the state
    if(state.CurrentX < indev_ctx->protocol.absolute->Mode->AbsoluteMinX) return;
    if(state.CurrentY < indev_ctx->protocol.absolute->Mode->AbsoluteMinY) return;

    pointer_pos.x = state.CurrentX - indev_ctx->protocol.absolute->Mode->AbsoluteMinX;
    pointer_pos.y = state.CurrentY - indev_ctx->protocol.absolute->Mode->AbsoluteMinY;

    data->point.x = (pointer_pos.x * indev_ctx->meta.absolute.factor_8.x) >> 8;
    data->point.y = (pointer_pos.y * indev_ctx->meta.absolute.factor_8.y) >> 8;

    data->state = (state.ActiveButtons & EFI_ABSP_TouchActive) ? LV_INDEV_STATE_PRESSED : LV_INDEV_STATE_RELEASED;
    data->continue_reading = FALSE;
}

static void lv_uefi_simple_text_input_read_cb(lv_indev_t * indev, lv_indev_data_t * data)
{
    EFI_STATUS status;
    EFI_INPUT_KEY state;
    lv_uefi_keyboard_context_t * indev_ctx = (lv_uefi_keyboard_context_t *)lv_indev_get_user_data(indev);

    LV_ASSERT_NULL(indev_ctx);

    if(indev_ctx->ex) return;

    // UEFI does not inform you when the key is released, so we send a release event following the pressed event
    if(indev_ctx->meta.release_is_next) {
        data->key = indev_ctx->meta.last_key;
        data->state = LV_INDEV_STATE_RELEASED;
        data->continue_reading = FALSE;
        indev_ctx->meta.release_is_next = FALSE;
        return;
    }

    status = indev_ctx->protocol.simple->ReadKeyStroke(
                 indev_ctx->protocol.simple,
                 &state);
    if(status == EFI_NOT_READY) return;
    if(status != EFI_SUCCESS) {
        LV_LOG_ERROR("[lv_uefi] ReadKeyStroke failed.");
        return;
    }

    // Store the key for the release event
    indev_ctx->meta.last_key = lv_key_from_uefi_key(&state, 0);
    indev_ctx->meta.release_is_next = TRUE;
    data->key = indev_ctx->meta.last_key;
    data->state = LV_INDEV_STATE_PRESSED;
    data->continue_reading = TRUE;
}

static void lv_uefi_simple_text_input_ex_read_cb(lv_indev_t * indev, lv_indev_data_t * data)
{
    EFI_STATUS status;
    EFI_KEY_DATA state;
    lv_uefi_keyboard_context_t * indev_ctx = (lv_uefi_keyboard_context_t *)lv_indev_get_user_data(indev);

    LV_ASSERT_NULL(indev_ctx);

    if(!indev_ctx->ex) return;

    // UEFI does not inform you when the key is released, so we send a release event following the pressed event
    if(indev_ctx->meta.release_is_next) {
        data->key = indev_ctx->meta.last_key;
        data->state = LV_INDEV_STATE_RELEASED;
        data->continue_reading = FALSE;
        indev_ctx->meta.release_is_next = FALSE;
        return;
    }

    status = indev_ctx->protocol.ex->ReadKeyStrokeEx(
                 indev_ctx->protocol.ex,
                 &state);
    if(status == EFI_NOT_READY) return;
    if(status != EFI_SUCCESS) {
        LV_LOG_ERROR("[lv_uefi] ReadKeyStrokeEx failed.");
        return;
    }

    // Store the key for the release event
    indev_ctx->meta.last_key = lv_key_from_uefi_key(&state.Key, state.KeyState.KeyShiftState);
    indev_ctx->meta.release_is_next = TRUE;
    data->key = indev_ctx->meta.last_key;
    data->state = LV_INDEV_STATE_PRESSED;
    data->continue_reading = TRUE;
}

static uint32_t _utf8_from_unicode(UINT32 unicode)
{
    uint8_t bytes[4] = {0, 0, 0, 0};

    // unicode < 128 -> 1 byte
    if(unicode < 128) {
        bytes[0] |= unicode;
    }
    // unicode < 2048 -> 2 byte
    else if(unicode < 2048) {
        bytes[0] = 0xC0;
        bytes[0] |= unicode >> 6;
        bytes[1] = 0x80;
        bytes[1] |= (unicode & 0x003F);
    }
    // unicode < 65536 -> 3 byte
    else if(unicode < 65536) {
        bytes[0] = 0xE0;
        bytes[0] |= unicode >> 12;
        bytes[1] = 0x80;
        bytes[1] |= ((unicode >> 6) & 0x003F);
        bytes[2] = 0x80;
        bytes[2] |= (unicode & 0x003F);
    }

    return *((uint32_t *)bytes);
}

static uint32_t lv_key_from_uefi_key(EFI_INPUT_KEY * key, UINT32 shift_state)
{
    LV_ASSERT_NULL(key);

    switch(key->ScanCode) {
        case 0x01:
            return LV_KEY_UP;
        case 0x02:
            return LV_KEY_DOWN;
        case 0x04:
            return LV_KEY_LEFT;
        case 0x03:
            return LV_KEY_RIGHT;
        case 0x08:
            return LV_KEY_DEL;
        case 0x05:
            return LV_KEY_HOME;
        case 0x06:
            return LV_KEY_END;
        case 0x17:
            return LV_KEY_ESC;
        // ignore all other scan codes
        default:
            break;
    }

    switch(key->UnicodeChar) {
        case 0x09:
            return (shift_state & EFI_SHIFT_STATE_VALID & (EFI_RIGHT_SHIFT_PRESSED | EFI_LEFT_SHIFT_PRESSED)) ? LV_KEY_PREV :
                   LV_KEY_NEXT;
        case 0x08:
            return LV_KEY_BACKSPACE;
        case 0x0D:
            return LV_KEY_ENTER;
        case 0x18:
            return LV_KEY_ESC;
        default:
            return _utf8_from_unicode(key->UnicodeChar);
    }
}

static void lv_uefi_pointer_context_free(lv_uefi_pointer_context_t * indev_ctx)
{
    if(indev_ctx == NULL) {
        return;
    }

    if(indev_ctx->absolute &&
       indev_ctx->protocol.absolute != NULL) lv_uefi_close_protocol(indev_ctx->handle, &_uefi_guid_absolute_pointer);
    else if(!indev_ctx->absolute &&
            indev_ctx->protocol.simple != NULL) lv_uefi_close_protocol(indev_ctx->handle, &_uefi_guid_simple_pointer);

    lv_free(indev_ctx);
}

static void lv_uefi_keyboard_context_free(lv_uefi_keyboard_context_t * indev_ctx)
{
    if(indev_ctx == NULL) {
        return;
    }

    if(indev_ctx->ex &&
       indev_ctx->protocol.ex != NULL) lv_uefi_close_protocol(indev_ctx->handle, &_uefi_guid_simple_text_input_ex);
    if(!indev_ctx->ex &&
       indev_ctx->protocol.simple != NULL) lv_uefi_close_protocol(indev_ctx->handle, &_uefi_guid_simple_text_input);

    lv_free(indev_ctx);
}

static bool lv_uefi_simple_input_interface_is_valid(const EFI_SIMPLE_TEXT_INPUT_PROTOCOL * interface)
{
    if(interface == NULL) return FALSE;
    return TRUE;
}

static bool lv_uefi_simple_input_ex_interface_is_valid(const EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL * interface)
{
    if(interface == NULL) return FALSE;
    return TRUE;
}

static bool lv_uefi_simple_pointer_interface_is_valid(const EFI_SIMPLE_POINTER_PROTOCOL * interface)
{
    if(interface == NULL) return FALSE;
    if(interface->Mode == NULL) return FALSE;
    if(interface->Mode->ResolutionX == 0) return FALSE;
    if(interface->Mode->ResolutionX >= 256) return FALSE;
    if(interface->Mode->ResolutionY == 0) return FALSE;
    if(interface->Mode->ResolutionY >= 256) return FALSE;
    return TRUE;
}

static bool lv_uefi_absolute_pointer_interface_is_valid(const EFI_ABSOLUTE_POINTER_PROTOCOL * interface)
{
    if(interface == NULL) return FALSE;
    if(interface->Mode == NULL) return FALSE;
    if(interface->Mode->AbsoluteMaxX <= interface->Mode->AbsoluteMinX) return FALSE;
    if(interface->Mode->AbsoluteMaxY <= interface->Mode->AbsoluteMinY) return FALSE;
    return TRUE;
}

#endif