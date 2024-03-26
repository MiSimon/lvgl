#include "lv_efi.h"

EFI_BOOT_SERVICES * gBS;
EFI_RUNTIME_SERVICES * gRS;
EFI_SYSTEM_TABLE * gST;
EFI_HANDLE * gImageHandle;

typedef struct {
    EFI_SIMPLE_TEXT_INPUT_PROTOCOL * protocol;
    CHAR16 last_key;
    BOOLEAN last_key_needs_to_be_released;
} _kb_private_data;

typedef struct {
    EFI_GRAPHICS_OUTPUT_PROTOCOL * protocol;
    UINTN render_buffer_size;
    UINT8 * render_buffer;
} _gop_private_data;

static void _s_print_cb(lv_log_level_t level, const char * buf);
static void _s_display_flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map);
static void _s_indev_kb_read_cb(lv_indev_t * indev, lv_indev_data_t * data);

EFI_STATUS lv_efi_init(EFI_HANDLE image_handle, EFI_SYSTEM_TABLE * system_table)
{
    if(image_handle == NULL || system_table == NULL) {
        return EFI_INVALID_PARAMETER;
    }

    gImageHandle = image_handle;
    gBS = system_table->BootServices;
    gRS = system_table->RuntimeServices;
    gST = system_table;

    lv_init();

    if(!lv_is_initialized()) {
        return EFI_DEVICE_ERROR;
    }

    lv_log_register_print_cb(_s_print_cb);

    return EFI_SUCCESS;
}

void lv_efi_deinit()
{
    lv_deinit();
}

lv_display_t * lv_efi_init_display(EFI_GRAPHICS_OUTPUT_PROTOCOL * protocol)
{
    EFI_STATUS status;
    lv_display_t * display = NULL;
    _gop_private_data * private_data = NULL;

    if(protocol == NULL) {
        return NULL;
    }

    status = gBS->AllocatePool(EfiLoaderData, sizeof(_gop_private_data), (VOID **) &private_data);
    if(status != EFI_SUCCESS) {
        LV_LOG_ERROR("Unable to allocate the display specific data.");
        goto error;
    }

    gBS->SetMem(private_data, sizeof(_gop_private_data), 0x00);

    private_data->protocol = protocol;
    private_data->render_buffer_size = sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL) * protocol->Mode->Info->HorizontalResolution *
                                       protocol->Mode->Info->VerticalResolution;

    status = gBS->AllocatePool(EfiLoaderData, private_data->render_buffer_size, (VOID **) &private_data->render_buffer);
    if(status != EFI_SUCCESS) {
        LV_LOG_ERROR("Unable to allocate the render buffer.");
        goto error;
    }

    display = lv_display_create(protocol->Mode->Info->HorizontalResolution, protocol->Mode->Info->VerticalResolution);
    if(display == NULL) {
        goto error;
    }

    lv_display_set_user_data(display, private_data);
    lv_display_set_buffers(display, private_data->render_buffer, NULL, private_data->render_buffer_size,
                           LV_DISPLAY_RENDER_MODE_DIRECT);
    lv_display_set_flush_cb(display, _s_display_flush_cb);

    goto finish;

error:
    if(private_data) {
        if(private_data->render_buffer) {
            gBS->FreePool(private_data->render_buffer);
        }
        gBS->FreePool(private_data);
    }

    if(display) {
        lv_display_delete(display);
        display = NULL;
    }

finish:
    return display;
}

void lv_efi_deinit_display(lv_display_t * display)
{
    _gop_private_data * private_data;

    if(display == NULL) {
        return;
    }

    private_data = (_gop_private_data *) lv_display_get_user_data(display);
    lv_display_set_user_data(display, NULL);
    if(private_data != NULL) {
        if(private_data->render_buffer != NULL) {
            gBS->FreePool(private_data->render_buffer);
        }
        gBS->FreePool(private_data);
    }
}

lv_indev_t * lv_efi_init_keyboard(EFI_SIMPLE_TEXT_INPUT_PROTOCOL * protocol)
{
    EFI_STATUS status;
    lv_indev_t * indev = NULL;
    _kb_private_data * private_data = NULL;

    if(protocol == NULL) {
        return NULL;
    }

    status = gBS->AllocatePool(EfiLoaderData, sizeof(_kb_private_data), (VOID **) &private_data);
    if(status != EFI_SUCCESS) {
        LV_LOG_ERROR("Unable to allocate the display specific data.");
        goto error;
    }

    gBS->SetMem(private_data, sizeof(_kb_private_data), 0x00);

    private_data->protocol = protocol;

    indev = lv_indev_create();
    if(indev == NULL) {
        goto error;
    }

    lv_indev_set_type(indev, LV_INDEV_TYPE_KEYPAD);
    lv_indev_set_user_data(indev, private_data);
    lv_indev_set_read_cb(indev, _s_indev_kb_read_cb);

    goto finish;

error:
    if(private_data) {
        gBS->FreePool(private_data);
    }

    if(indev) {
        lv_indev_delete(indev);
        indev = NULL;
    }

finish:
    return indev;
}

void lv_efi_deinit_keyboard(lv_indev_t * indev)
{
    _kb_private_data * private_data;

    if(indev == NULL) {
        return;
    }

    private_data = (_kb_private_data *) lv_indev_get_user_data(indev);
    lv_indev_set_user_data(indev, NULL);
    if(private_data != NULL) {
        gBS->FreePool(private_data);
    }
}

static void _s_print_cb(lv_log_level_t level, const char * buf)
{
    CHAR16 unicode_string[2];

    if(!buf) {
        return;
    }

    unicode_string[1] = L'\0';
    while(*buf != '\0') {
        if(*buf >= 0x20 && *buf < 0x7F) {
            unicode_string[0] = (CHAR16) * buf;
            gST->ConOut->OutputString(gST->ConOut, unicode_string);
        }
        buf++;
    }
}

static void _s_display_flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map)
{
    EFI_STATUS status;
    int32_t w;
    int32_t h;
    _gop_private_data * private_data = (_gop_private_data *)lv_display_get_user_data(disp);

    if(!private_data) {
        lv_display_flush_ready(disp);
        return;
    }

    w = (int32_t)area->x2 - (int32_t)area->x1 + 1;
    h = (int32_t)area->y2 - (int32_t)area->y1 + 1;

    if(w < 0 || h < 0) {
        LV_LOG_ERROR("Invalid lv_display_flush_cb call (invalid rect).");
        lv_display_flush_ready(disp);
        return;
    }

    if((area->x1 + w) > private_data->protocol->Mode->Info->HorizontalResolution) {
        LV_LOG_ERROR("Invalid lv_display_flush_cb call (invalid width).");
        lv_display_flush_ready(disp);
        return;
    }

    if((area->y1 + h) > private_data->protocol->Mode->Info->HorizontalResolution) {
        LV_LOG_ERROR("Invalid lv_display_flush_cb call (invalid height).");
        lv_display_flush_ready(disp);
        return;
    }

    status = private_data->protocol->Blt(
                 private_data->protocol,
                 (EFI_GRAPHICS_OUTPUT_BLT_PIXEL *) private_data->render_buffer,
                 EfiBltBufferToVideo,
                 area->x1,
                 area->y1,
                 area->x1,
                 area->y1,
                 w,
                 h,
                 private_data->protocol->Mode->Info->HorizontalResolution * sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL));

    if(status != EFI_SUCCESS) {
        LV_LOG_ERROR("Blt failed.");
        lv_display_flush_ready(disp);
        return;
    }

    lv_display_flush_ready(disp);
}

static void _s_indev_kb_read_cb(lv_indev_t * indev, lv_indev_data_t * data)
{
    EFI_STATUS status;
    _kb_private_data * private_data = (_kb_private_data *)lv_indev_get_user_data(indev);

    if(!private_data) {
        return;
    }

    if(private_data->last_key_needs_to_be_released) {
        private_data->last_key_needs_to_be_released = false;
        data->key = private_data->last_key;
        data->state = LV_INDEV_STATE_RELEASED;
    }
    else {
        EFI_INPUT_KEY key_data;
        status = private_data->protocol->ReadKeyStroke(
                     private_data->protocol,
                     &key_data);
        if(status != EFI_SUCCESS) {
            if(status != EFI_NOT_READY) {
                LV_LOG_ERROR("ReadKeyStroke failed, error code: %08llx.", status);
            }
            return;
        }

        if(key_data.ScanCode != 0) {
            switch(key_data.ScanCode) {
                case 0x08:
                    data->key = LV_KEY_DEL;
                    break;
                case 0x05:
                    data->key = LV_KEY_HOME;
                    break;
                case 0x06:
                    data->key = LV_KEY_END;
                    break;
                case 0x17:
                    data->key = LV_KEY_ESC;
                    break;
                case 0x01:
                    data->key = LV_KEY_UP;
                    break;
                case 0x02:
                    data->key = LV_KEY_DOWN;
                    break;
                case 0x04:
                    data->key = LV_KEY_LEFT;
                    break;
                case 0x03:
                    data->key = LV_KEY_RIGHT;
                    break;
                default:
                    break;
            }
        }
        else if(key_data.UnicodeChar != 0) {
            switch(key_data.UnicodeChar) {
                case 0x0A:
                case 0x0D:
                    data->key = LV_KEY_ENTER;
                    break;
                case 0x1b:
                    data->key = LV_KEY_ESC;
                    break;
                default:
                    data->key = key_data.UnicodeChar;
                    break;
            }
        }

        private_data->last_key = data->key;
        if(data->key != 0) {
            private_data->last_key_needs_to_be_released = true;
            data->state = LV_INDEV_STATE_PRESSED;
        }
    }
}
