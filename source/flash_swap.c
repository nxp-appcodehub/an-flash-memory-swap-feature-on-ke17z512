/*
 * Copyright 2023 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */



#include "flash_swap.h"

status_t flash_swap_get_swap_indicator_addr(flash_config_t *s_flashDriver, uint32_t *swapIndicatorAddress)
{
    status_t result;
    
    ftfx_swap_state_config_t SwapInfo;
    flashSwapIfrFieldData_t flashSwapIfrFieldData;
    
    memset(&flashSwapIfrFieldData, 0, sizeof(flashSwapIfrFieldData));

    /* Get original swap indicator address */
    result = FLASH_ReadResource(s_flashDriver, s_flashDriver->ftfxConfig[0].ifrDesc.resRange.pflashSwapIfrStart, (uint8_t *)&flashSwapIfrFieldData, sizeof(flashSwapIfrFieldData), kFTFx_ResourceOptionFlashIfr);
                                
    /* The high bits value of Swap Indicator Address is stored in Program Flash Swap IFR Field,
     * the low severval bit value of Swap Indicator Address is always 1'b0 */
    *swapIndicatorAddress = (uint32_t)flashSwapIfrFieldData.swapIndicatorAddress * s_flashDriver->ftfxConfig[0].opsConfig.addrAligment.swapCtrlCmd;

    return result;
}



status_t flash_swap_report_status(flash_config_t *s_flashDriver, ftfx_swap_state_config_t *SwapInfo)
{
    /* It doesn't matter what the provided address is, when option is kFlashSwap_ReportStatus*/
    return FTFx_CMD_SwapControl(&s_flashDriver->ftfxConfig[0], FSL_FEATURE_FLASH_PFLASH_SWAP_CONTROL_CMD_ADDRESS_ALIGMENT, kFTFx_SwapControlOptionReportStatus, SwapInfo);     
}


void flash_swap_dump_status(ftfx_swap_state_config_t *swap_info)
{
    if(swap_info->currentSwapBlockStatus == kFTFx_SwapBlockStatusLowerHalfProgramBlocksAtZero)
    {
        PRINTF("CURRENT SWAP STAT: LowerAtZero(Physical Address:0x%X, mapped to Logial 0x00000000)\r\n", LOWER_PFLASH_BASE);
    }
    
    if(swap_info->currentSwapBlockStatus == kFTFx_SwapBlockStatusUpperHalfProgramBlocksAtZero)
    {
        PRINTF("CURRENT SWAP STAT: UpperAtZero(Physical Address:0x%X, mapped to Logial 0x00000000)\r\n", UPPER_PFLASH_BASE);
    }

    
    switch (swap_info->flashSwapState)
    {
        case kFTFx_SwapStateUninitialized:
            PRINTF("Current swap system status: Uninitialized \r\n");
            break;
        case kFTFx_SwapStateReady:
            PRINTF("Current swap system status: Ready \r\n");
            break;
        case kFTFx_SwapStateUpdate:
            PRINTF("Current swap system status: Update \r\n");
            break;
        case kFTFx_SwapStateUpdateErased:
            PRINTF("Current swap system status: UpdateErased \r\n");
            break;
        case kFTFx_SwapStateComplete:
            PRINTF("Current swap system status: Complete \r\n");
            break;
        case kFTFx_SwapStateDisabled:
            PRINTF("Current swap system status: Disabled \r\n");
            break;
        default: break;
        }
}
