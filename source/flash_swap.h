/*
 * Copyright 2023 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _FLASH_SWAP_H_
#define _FLASH_SWAP_H_

#include "fsl_device_registers.h"
#include "fsl_flash.h"
#include "fsl_common.h"
#include "fsl_debug_console.h"


#define LOWER_PFLASH_BASE       FSL_FEATURE_FLASH_PFLASH_START_ADDRESS
#define UPPER_PFLASH_BASE       (FSL_FEATURE_FLASH_PFLASH_BLOCK_SIZE * (FSL_FEATURE_FLASH_PFLASH_BLOCK_COUNT >> 1))

typedef struct
{
    uint16_t swapIndicatorAddress; /*!< A Swap indicator address field.*/
    uint16_t swapEnableWord;       /*!< A Swap enable word field.*/
    uint8_t reserved0[4];          /*!< A reserved field.*/
    uint8_t reserved1[2];          /*!< A reserved field.*/
    uint16_t swapDisableWord;      /*!< A Swap disable word field.*/
    uint8_t reserved2[4];          /*!< A reserved field.*/
} flashSwapIfrFieldData_t;
        

status_t flash_swap_get_swap_indicator_addr(flash_config_t *s_flashDriver, uint32_t *swapIndicatorAddress);
status_t flash_swap_report_status(flash_config_t *s_flashDriver, ftfx_swap_state_config_t *SwapInfo);
void flash_swap_dump_status(ftfx_swap_state_config_t *swap_info);

#endif
