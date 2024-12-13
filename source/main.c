/*
 * Copyright 2023 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "board.h"
#include "fsl_flash.h"
#include "fsl_common.h"
#include "flash_swap.h"

#define EXAMPLE_IMAGE_SIZE      (256*1024)
#define IMAGE_VERSION_ADDR      (0x24)



static flash_config_t s_flashDriver;

static void app_security_check(void)
{
    status_t result;
    ftfx_security_state_t securityStatus = kFTFx_SecurityStateNotSecure; /* Return protection status */
    
    /* Check security status. */
    result = FLASH_GetSecurityState(&s_flashDriver, &securityStatus);

    switch (securityStatus)
    {
        case kFTFx_SecurityStateNotSecure:
            PRINTF("Flash is UNSECURE!");
            break;
        case kFTFx_SecurityStateBackdoorEnabled:
            PRINTF("Flash is SECURE, BACKDOOR is ENABLED!\r\n");
            break;
        case kFTFx_SecurityStateBackdoorDisabled:
            PRINTF("Flash is SECURE, BACKDOOR is DISABLED!\r\n");
            break;
        default:
            break;
    }
    PRINTF("\r\n");
    
//    /* Debug message for user. */
//    /* Test pflash swap feature only if flash is unsecure. */
//    if (kFTFx_SecurityStateNotSecure != securityStatus)
//    {

//    }
}


static void app_dump_flash_info(void)
{
    uint32_t pflashTotalSize  = 0;
    uint32_t pflashBlockCount = 0;
    uint32_t pflashSectorSize = 0;
    

    FLASH_GetProperty(&s_flashDriver, kFLASH_PropertyPflash0TotalSize, &pflashTotalSize);
    FLASH_GetProperty(&s_flashDriver, kFLASH_PropertyPflash0BlockCount, &pflashBlockCount);
    FLASH_GetProperty(&s_flashDriver, kFLASH_PropertyPflash0SectorSize, &pflashSectorSize);

        
    PRINTF("Total Program Flash Size:              %d KB, Hex: (0x%x)\r\n", (pflashTotalSize / 1024), pflashTotalSize);
    PRINTF("Total Program Flash Block Count:       %d\r\n", pflashBlockCount);
    PRINTF("Program Flash Sector Size:             %d KB, Hex: (0x%x)\r\n", (pflashSectorSize / 1024), pflashSectorSize);
    PRINTF("LOWER_PFLASH_BASE:                     0x%x\r\n", LOWER_PFLASH_BASE);
    PRINTF("UPPER_PFLASH_BASE:                     0x%x\r\n", UPPER_PFLASH_BASE);
}


int main(void)
{
    char ch;
   
    status_t result;

    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitDebugConsole();

    FLASH_Init(&s_flashDriver);

    /* SDK bug fix, KE17Z512 has 512K flash and sectoer size is 2K */
    s_flashDriver.ftfxConfig[0].flashDesc.sectorSize = 2048;
    s_flashDriver.ftfxConfig[0].flashDesc.totalSize = 512*1024;
    
    PRINTF("\r\n\r\n");
    PRINTF("PFlash Swap Example Start\r\n");
    PRINTF("APP VERSION location offset:0x%X\r\n", IMAGE_VERSION_ADDR);
    PRINTF("APP VERSION:%d\r\n", *(uint32_t*)IMAGE_VERSION_ADDR);

    uint32_t swapIndicatorAddress;
    ftfx_swap_state_config_t swap_info;

    flash_swap_get_swap_indicator_addr(&s_flashDriver, &swapIndicatorAddress);
    flash_swap_report_status(&s_flashDriver, &swap_info);
    
    /* show status */
    flash_swap_dump_status(&swap_info);

    if(swap_info.flashSwapState == kFTFx_SwapStateUninitialized)
    {
        swapIndicatorAddress = UPPER_PFLASH_BASE - s_flashDriver.ftfxConfig[0].flashDesc.sectorSize;
    }
    
    PRINTF("swapIndicatorAddress:0x%X\r\n", swapIndicatorAddress);
        
    while(1)
    {
        PRINTF("\r\n");
        PRINTF("PLEASE SELECT...\r\n");
        PRINTF("1 - Show flash info\r\n");
        PRINTF("2 - Simulate update firmware\r\n");
        PRINTF("3 - Swap flash and reboot\r\n");
        PRINTF("4 - Reboot\r\n");

        ch = GETCHAR();

        PRINTF("YOU SELECT:%c\r\n", ch);
        switch(ch)
        {
            case '1':
                app_dump_flash_info();
                app_security_check();
                break;
            case '2':
                PRINTF("Simluate update firmware: write data to UPPER_PFLASH_BASE(0x%X)\r\n", UPPER_PFLASH_BASE);
                
                static uint32_t program_buf[64];
                FLASH_Erase(&s_flashDriver, UPPER_PFLASH_BASE, EXAMPLE_IMAGE_SIZE, kFTFx_ApiEraseKey);
                
                /* program image */
                int i = 0;
                while(i < EXAMPLE_IMAGE_SIZE)
                {
                    memcpy(program_buf, (uint8_t*)(LOWER_PFLASH_BASE+i), sizeof(program_buf));
                    
                    /* we do nothing but increase APP_VERSION field */
                    if(i == 0)
                    {
                        program_buf[9]++; /* ADDR:0x24 */
                    }
                    
                    FLASH_Program(&s_flashDriver, UPPER_PFLASH_BASE+i, (uint8_t *)program_buf, sizeof(program_buf));
                    i += sizeof(program_buf);
                }
                PRINTF("Complete firmware update!\r\n");
                
                NVIC_SystemReset();
                break;
            case '3':
                PRINTF("SWAP AND REBOOT\r\n");
            
                /* check if the upper image is valid */
                if(*(uint32_t*)UPPER_PFLASH_BASE != 0xFFFFFFFF)
                {
                    result = FLASH_Swap(&s_flashDriver, swapIndicatorAddress, true);
                    NVIC_SystemReset();
                }
                else
                {
                    PRINTF("The upper Image it not valid, ABORT!\r\n");
                }

                break;
            case '4':
                NVIC_SystemReset();
                break;
        }
       
    }
    return 0;
}


void HardFault_Handler(void)
{
    PRINTF("HardFault_Handler\r\n");
    while(1);
}
