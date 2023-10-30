#include "stm32f10x.h"
#include "oled.h"
#include "key.h"
#include "common.h"
#include "iap.h"

int main()
{

    Oled oled;
    uint16_t oled_scl = GPIO_Pin_8;
    uint16_t oled_sda = GPIO_Pin_9;
    Oled_Init(&oled, RCC_APB2Periph_GPIOB, GPIOB, oled_scl, oled_sda);

    Key key;
    uint16_t key1    = GPIO_Pin_15; // 在 PB15 和 GND 之间加开关，按住后开机进入 Menu, 可以通过串口操作
    uint16_t keys[2] = {key1};

    Key_Init(&key, RCC_APB2Periph_GPIOB, GPIOB, keys);

    IAP_Init();

    if (Key_IsPressed(&key, key1) == 1) {
        IAP_WriteFlag(INIT_FLAG_DATA);
    } else if (IAP_RunApp()) {
        IAP_WriteFlag(INIT_FLAG_DATA);
    }

    while (1) {
        Oled_Clear(&oled);
        Oled_ShowString(&oled, 1, 1, "Welcome!");

        switch (IAP_ReadFlag()) {
            case APPRUN_FLAG_DATA: // jump to app
                Oled_ShowString(&oled, 4, 1, "Loading...");

                if (IAP_RunApp())
                    IAP_WriteFlag(INIT_FLAG_DATA);
                break;
            case INIT_FLAG_DATA: // initialze state (blank mcu)
                Oled_ShowString(&oled, 4, 1, "Wating...");

                IAP_Main_Menu();
                break;
            case UPDATE_FLAG_DATA: // download app state
                Oled_ShowString(&oled, 4, 1, "Updating...");

                if (!IAP_Update())
                    IAP_WriteFlag(APPRUN_FLAG_DATA);
                else
                    IAP_WriteFlag(INIT_FLAG_DATA);
                break;
            // case UPLOAD_FLAG_DATA: // upload app state
            //     Oled_ShowString(&oled, 4, 1, "Uploading...");

            //     if (!IAP_Upload())
            //         IAP_WriteFlag(APPRUN_FLAG_DATA);
            //     else
            //         IAP_WriteFlag(INIT_FLAG_DATA);
            //     break;
            case ERASE_FLAG_DATA: // erase app state
                Oled_ShowString(&oled, 4, 1, "Erasing...");

                IAP_Erase();
                IAP_WriteFlag(INIT_FLAG_DATA);
                break;
            default:
                break;
        }
    }
}
