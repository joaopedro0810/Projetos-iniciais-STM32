/* mbed Microcontroller Library
 * Copyright (c) 2006-2017 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "stm32f7xx_hal.h"
#include "reset_reason_api.h"

reset_reason_t hal_reset_reason_get(void)
{
    if (__HAL_RCC_GET_FLAG(RCC_FLAG_LPWRRST)) {
        return RESET_REASON_WAKE_LOW_POWER;
    }

    if (__HAL_RCC_GET_FLAG(RCC_FLAG_WWDGRST)) {
        return RESET_REASON_WATCHDOG;
    }

    if (__HAL_RCC_GET_FLAG(RCC_FLAG_IWDGRST)) {
        return RESET_REASON_WATCHDOG;
    }

    if (__HAL_RCC_GET_FLAG(RCC_FLAG_SFTRST)) {
        return RESET_REASON_SOFTWARE;
    }

    if (__HAL_RCC_GET_FLAG(RCC_FLAG_PORRST)) {
        return RESET_REASON_POWER_ON;
    }

    if (__HAL_RCC_GET_FLAG(RCC_FLAG_BORRST)) {
        return RESET_REASON_BROWN_OUT;
    }

    if (__HAL_RCC_GET_FLAG(RCC_FLAG_PINRST)) {
        return RESET_REASON_PIN_RESET;
    }
    
    if (__HAL_RCC_GET_FLAG(RCC_FLAG_LSIRDY)) {
        return RESET_REASON_LSIRDY;
    }

    return RESET_REASON_UNKNOWN;
}


uint32_t hal_reset_reason_get_raw(void)
{
#if TARGET_STM32H7
    return RCC->RSR;
#else /* TARGET_STM32H7 */
    return RCC->CSR;
#endif /* TARGET_STM32H7 */
}


void hal_reset_reason_clear(void)
{
    __HAL_RCC_CLEAR_RESET_FLAGS();
}
