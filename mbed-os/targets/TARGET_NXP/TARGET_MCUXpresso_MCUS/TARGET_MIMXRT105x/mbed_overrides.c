/* mbed Microcontroller Library
 * Copyright (c) 2006-2020 ARM Limited
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
#include "pinmap.h"
#include "clock_config.h"
#include "fsl_clock.h"
#include "fsl_xbara.h"
#include "fsl_iomuxc.h"
#include "fsl_gpio.h"
#include "fsl_pit.h"
#include "lpm.h"
#include "usb_phy.h"
#include "usb_device_config.h"
#include "us_ticker_defines.h"
#include "us_ticker_api.h"
#include "flash_api.h"

#if DEVICE_FLASH
#include "mimxrt_flash_api.h"
#endif

#define LPSPI_CLOCK_SOURCE_DIVIDER (7U)
#define LPI2C_CLOCK_SOURCE_DIVIDER (5U)

/* USB PHY condfiguration */
#define BOARD_USB_PHY_D_CAL     (0x0CU)
#define BOARD_USB_PHY_TXCAL45DP (0x06U)
#define BOARD_USB_PHY_TXCAL45DM (0x06U)

uint8_t mbed_otp_mac_address(char *mac);
void mbed_default_mac_address(char *mac);

/* MPU configuration. */
void BOARD_ConfigMPU(void)
{
    /* Disable I cache and D cache */
    if (SCB_CCR_IC_Msk == (SCB_CCR_IC_Msk & SCB->CCR)) {
        SCB_DisableICache();
    }
    if (SCB_CCR_DC_Msk == (SCB_CCR_DC_Msk & SCB->CCR)) {
        SCB_DisableDCache();
    }

    /* Disable MPU */
    ARM_MPU_Disable();

    /* MPU configure:
     * Use ARM_MPU_RASR(DisableExec, AccessPermission, TypeExtField, IsShareable, IsCacheable, IsBufferable, SubRegionDisable, Size)
     * API in mpu_armv7.h.
     * param DisableExec       Instruction access (XN) disable bit,0=instruction fetches enabled, 1=instruction fetches disabled.
     * param AccessPermission  Data access permissions, allows you to configure read/write access for User and Privileged mode.
     *      Use MACROS defined in mpu_armv7.h: ARM_MPU_AP_NONE/ARM_MPU_AP_PRIV/ARM_MPU_AP_URO/ARM_MPU_AP_FULL/ARM_MPU_AP_PRO/ARM_MPU_AP_RO
     * Combine TypeExtField/IsShareable/IsCacheable/IsBufferable to configure MPU memory access attributes.
     *  TypeExtField  IsShareable  IsCacheable  IsBufferable   Memory Attribtue    Shareability        Cache
     *     0             x           0           0             Strongly Ordered    shareable
     *     0             x           0           1              Device             shareable
     *     0             0           1           0              Normal             not shareable   Outer and inner write through no write allocate
     *     0             0           1           1              Normal             not shareable   Outer and inner write back no write allocate
     *     0             1           1           0              Normal             shareable       Outer and inner write through no write allocate
     *     0             1           1           1              Normal             shareable       Outer and inner write back no write allocate
     *     1             0           0           0              Normal             not shareable   outer and inner noncache
     *     1             1           0           0              Normal             shareable       outer and inner noncache
     *     1             0           1           1              Normal             not shareable   outer and inner write back write/read acllocate
     *     1             1           1           1              Normal             shareable       outer and inner write back write/read acllocate
     *     2             x           0           0              Device              not shareable
     *  Above are normal use settings, if your want to see more details or want to config different inner/outter cache policy.
     *  please refer to Table 4-55 /4-56 in arm cortex-M7 generic user guide <dui0646b_cortex_m7_dgug.pdf>
     * param SubRegionDisable  Sub-region disable field. 0=sub-region is enabled, 1=sub-region is disabled.
     * param Size              Region size of the region to be configured. use ARM_MPU_REGION_SIZE_xxx MACRO in mpu_armv7.h.
     */

    /* Region 0 setting: Memory with Device type, not shareable, non-cacheable. */
    MPU->RBAR = ARM_MPU_RBAR(0, 0x80000000U);
    MPU->RASR = ARM_MPU_RASR(0, ARM_MPU_AP_FULL, 2, 0, 0, 0, 0, ARM_MPU_REGION_SIZE_512MB);

    /* Region 1 setting: Memory with Device type, not shareable,  non-cacheable. */
    MPU->RBAR = ARM_MPU_RBAR(1, 0x60000000U);
    MPU->RASR = ARM_MPU_RASR(0, ARM_MPU_AP_FULL, 2, 0, 0, 0, 0, ARM_MPU_REGION_SIZE_512MB);

/* Region 2 setting */
#if defined(XIP_EXTERNAL_FLASH) && (XIP_EXTERNAL_FLASH == 1)
    /* Setting Memory with Normal type, not shareable, outer/inner write back. */
    MPU->RBAR = ARM_MPU_RBAR(2, 0x60000000U);
    MPU->RASR = ARM_MPU_RASR(0, ARM_MPU_AP_RO, 0, 0, 1, 1, 0, ARM_MPU_REGION_SIZE_64MB);
#endif

    /* Region 3 setting: Memory with Device type, not shareable, non-cacheable. */
    MPU->RBAR = ARM_MPU_RBAR(3, 0x00000000U);
    MPU->RASR = ARM_MPU_RASR(0, ARM_MPU_AP_FULL, 2, 0, 0, 0, 0, ARM_MPU_REGION_SIZE_1GB);

    /* Region 4 setting: Memory with Normal type, not shareable, outer/inner write back [ITCM] */
    MPU->RBAR = ARM_MPU_RBAR(4, 0x00000000U);
    MPU->RASR = ARM_MPU_RASR(0, ARM_MPU_AP_FULL, 0, 0, 1, 1, 0, ARM_MPU_REGION_SIZE_128KB);

    /* Region 5 setting: Memory with Normal type, not shareable, outer/inner write back [DTCM] */
    MPU->RBAR = ARM_MPU_RBAR(5, 0x20000000U);
    MPU->RASR = ARM_MPU_RASR(0, ARM_MPU_AP_FULL, 0, 0, 1, 1, 0, ARM_MPU_REGION_SIZE_256KB);

#if MBED_TARGET_MIMXRT1050

    /* Region 6 setting: Memory with Normal type, not shareable, outer/inner write back [OCRAM] */
    MPU->RBAR = ARM_MPU_RBAR(6, 0x20200000U);
    MPU->RASR = ARM_MPU_RASR(0, ARM_MPU_AP_FULL, 0, 0, 1, 1, 0, ARM_MPU_REGION_SIZE_128KB);

#else // MIMXRT1060

    /* Region 6 setting: Memory with Normal type, not shareable, outer/inner write back [OCRAM] */
    MPU->RBAR = ARM_MPU_RBAR(6, 0x20280000U);
    MPU->RASR = ARM_MPU_RASR(0, ARM_MPU_AP_FULL, 0, 0, 1, 1, 0, ARM_MPU_REGION_SIZE_128KB);

    /* Region 7 setting: Memory with Normal type, not shareable, outer/inner write back [OCRAM2] */
    MPU->RBAR = ARM_MPU_RBAR(7, 0x20200000U);
    MPU->RASR = ARM_MPU_RASR(0, ARM_MPU_AP_FULL, 0, 0, 1, 1, 0, ARM_MPU_REGION_SIZE_512KB);

#endif



/* The define sets the cacheable memory to shareable,
 * this suggestion is referred from chapter 2.2.1 Memory regions,
 * types and attributes in Cortex-M7 Devices, Generic User Guide */
#if defined(SDRAM_IS_SHAREABLE)
    /* Region 8 setting: Memory with Normal type, shareable, outer/inner write back, write/read allocate */
    MPU->RBAR = ARM_MPU_RBAR(8, 0x80000000U);
    MPU->RASR = ARM_MPU_RASR(0, ARM_MPU_AP_FULL, 1, 1, 1, 1, 0, ARM_MPU_REGION_SIZE_32MB);
#else
    /* Region 8 setting: Memory with Normal type, not shareable, outer/inner write back, write/read allocate */
    MPU->RBAR = ARM_MPU_RBAR(8, 0x80000000U);
    MPU->RASR = ARM_MPU_RASR(0, ARM_MPU_AP_FULL, 1, 0, 1, 1, 0, ARM_MPU_REGION_SIZE_32MB);
#endif

    /* Enable MPU */
    ARM_MPU_Enable(MPU_CTRL_PRIVDEFENA_Msk);

    /* Enable I cache and D cache */
    SCB_EnableDCache();
    SCB_EnableICache();
}

void BOARD_Init_PMIC_STBY_REQ(void) {
    CLOCK_EnableClock(kCLOCK_IomuxcSnvs);       /* iomuxc_snvs clock (iomuxc_snvs_clk_enable): 0x03U */

    /* GPIO configuration of PERI_PWREN on PMIC_STBY_REQ (pin L7) */
    gpio_pin_config_t PERI_PWREN_config = {
        .direction = kGPIO_DigitalOutput,
        .outputLogic = 0U,
        .interruptMode = kGPIO_NoIntmode
    };
    /* Initialize GPIO functionality on PMIC_STBY_REQ (pin L7) */
    GPIO_PinInit(GPIO5, 2U, &PERI_PWREN_config);

    IOMUXC_SetPinMux(
        IOMUXC_SNVS_PMIC_STBY_REQ_GPIO5_IO02,   /* PMIC_STBY_REQ is configured as GPIO5_IO02 */
        0U);                                    /* Software Input On Field: Input Path is determined by functionality */
    IOMUXC_SetPinConfig(
        IOMUXC_SNVS_PMIC_STBY_REQ_GPIO5_IO02,   /* PMIC_STBY_REQ PAD functional properties : */
        0x10B0U);                               /* Slew Rate Field: Slow Slew Rate
                                                   Drive Strength Field: R0/6
                                                   Speed Field: medium(100MHz)
                                                   Open Drain Enable Field: Open Drain Disabled
                                                   Pull / Keep Enable Field: Pull/Keeper Enabled
                                                   Pull / Keep Select Field: Keeper
                                                   Pull Up / Down Config. Field: 100K Ohm Pull Down
                                                   Hyst. Enable Field: Hysteresis Disabled */
}

// called before main
void mbed_sdk_init()
{
    BOARD_ConfigMPU();

#if MBED_CONF_TARGET_ENABLE_OVERDRIVE_MODE
    BOARD_ClockOverdrive();
#else
    BOARD_ClockFullSpeed();
#endif

    // Switch to using an application-owned flexspi config instead of what the bootloader sets up
#if DEVICE_FLASH
    mimxrt_flash_setup();
#endif

    // Initialize us ticker before LPM, because LPM uses it for timing
    us_ticker_init();

#if TARGET_EVK
    /* Since SNVS_PMIC_STBY_REQ_GPIO5_IO02 will output a high-level signal under Stop Mode(Suspend Mode) and this pin is
     * connected to LCD power switch circuit. So it needs to be configured as a low-level output GPIO to reduce the
     * current. */
    BOARD_Init_PMIC_STBY_REQ();
#endif

    LPM_Init();
}

void spi_setup_clock()
{
    // Not needed on MIMXRT105x
}

uint32_t spi_get_clock(void)
{
    return BOARD_CLOCKFULLSPEED_LPSPI_CLK_ROOT;
}

uint32_t us_ticker_get_clock()
{
    return BOARD_CLOCKFULLSPEED_PERCLK_CLK_ROOT;
}

uint32_t serial_get_clock(void)
{
    return BOARD_CLOCKFULLSPEED_UART_CLK_ROOT;
}

void i2c_setup_clock()
{
    // Not needed on MIMXRT105x
}

uint32_t i2c_get_clock()
{
    return BOARD_CLOCKFULLSPEED_LPI2C_CLK_ROOT;
}

void pwm_setup(uint32_t instance)
{
    /* Use default clock settings */
    /* Set the PWM Fault inputs to a low value */
    XBARA_Init(XBARA1);

    XBARA_SetSignalsConnection(XBARA1, kXBARA1_InputLogicHigh, kXBARA1_OutputFlexpwm1234Fault2);
    XBARA_SetSignalsConnection(XBARA1, kXBARA1_InputLogicHigh, kXBARA1_OutputFlexpwm1234Fault3);

    switch (instance) {
        case 1:
            XBARA_SetSignalsConnection(XBARA1, kXBARA1_InputLogicHigh, kXBARA1_OutputFlexpwm1Fault0);
            XBARA_SetSignalsConnection(XBARA1, kXBARA1_InputLogicHigh, kXBARA1_OutputFlexpwm1Fault1);
            break;
        case 2:
            XBARA_SetSignalsConnection(XBARA1, kXBARA1_InputLogicHigh, kXBARA1_OutputFlexpwm2Fault0);
            XBARA_SetSignalsConnection(XBARA1, kXBARA1_InputLogicHigh, kXBARA1_OutputFlexpwm2Fault1);
            break;
        case 3:
            XBARA_SetSignalsConnection(XBARA1, kXBARA1_InputLogicHigh, kXBARA1_OutputFlexpwm3Fault0);
            XBARA_SetSignalsConnection(XBARA1, kXBARA1_InputLogicHigh, kXBARA1_OutputFlexpwm3Fault1);
            break;
        case 4:
            XBARA_SetSignalsConnection(XBARA1, kXBARA1_InputLogicHigh, kXBARA1_OutputFlexpwm4Fault0);
            XBARA_SetSignalsConnection(XBARA1, kXBARA1_InputLogicHigh, kXBARA1_OutputFlexpwm4Fault1);
            break;
        default:
            break;
    }
}

uint32_t pwm_get_clock()
{
    return CLOCK_GetFreq(kCLOCK_IpgClk);
}

// Change the NMI pin to an input. This allows NMI pin to
//  be used as a low power mode wakeup.  The application will
//  need to change the pin back to NMI_b or wakeup only occurs once!
void NMI_Handler(void)
{
    pin_function(WAKEUP, 7);
    pin_mode(WAKEUP, PullDefault);
}

void mbed_mac_address(char *mac) {
    if (mbed_otp_mac_address(mac)) {
        return;
    } else {
        mbed_default_mac_address(mac);
    }
    return;
}

uint8_t mbed_otp_mac_address(char *mac) {

#if TARGET_EVK
    /* Check if a valid MAC address is programmed to the fuse bank */
    if ((OCOTP->MAC0 != 0) &&
        (OCOTP->MAC1 != 0) &&
        (OCOTP->GP3 != 0)) {
        uint16_t MAC[3];  // 3 16 bits words for the MAC

        // Read the MAC address from the OCOTP MAC registers
        MAC[0] = (uint16_t)OCOTP->MAC0;  // most significant half-word
        MAC[1] = (uint16_t)OCOTP->MAC1;
        MAC[2] = (uint16_t)OCOTP->GP3;  // least significant half word

        // The network stack expects an array of 6 bytes
        // so we copy, and shift and copy from the half-word array to the byte array
        mac[0] = MAC[0] >> 8;
        mac[1] = MAC[0];
        mac[2] = MAC[1] >> 8;
        mac[3] = MAC[1];
        mac[4] = MAC[2] >> 8;
        mac[5] = MAC[2];

        return 1;
    }
#endif

#if TARGET_TEENSY_4X
    if(OCOTP->MAC0 != 0 || OCOTP->MAC1 != 0)
    {
        mac[0] = OCOTP->MAC1 >> 8;
        mac[1] = OCOTP->MAC1 >> 0;
        mac[2] = OCOTP->MAC0 >> 24;
        mac[3] = OCOTP->MAC0 >> 16;
        mac[4] = OCOTP->MAC0 >> 8;
        mac[5] = OCOTP->MAC0 >> 0;

        return 1;
    }
#endif

    return 0;
}

void mbed_default_mac_address(char *mac) {
    mac[0] = 0x00;
    mac[1] = 0x02;
    mac[2] = 0xF7;
    mac[3] = 0xF0;
    mac[4] = 0x00;
    mac[5] = 0x00;

    return;
}

void USB_DeviceClockInit(void)
{
    usb_phy_config_struct_t phyConfig = {
        BOARD_USB_PHY_D_CAL,
        BOARD_USB_PHY_TXCAL45DP,
        BOARD_USB_PHY_TXCAL45DM,
    };

    CLOCK_EnableUsbhs0PhyPllClock(kCLOCK_Usbphy480M, 480000000U);
    CLOCK_EnableUsbhs0Clock(kCLOCK_Usb480M, 480000000U);

    USB_EhciPhyInit(CONTROLLER_ID, BOARD_XTAL0_CLK_HZ, &phyConfig);
}

uint32_t USB_DeviceGetIrqNumber(void)
{
    uint8_t irqNumber;

    uint8_t usbDeviceEhciIrq[] = USBHS_IRQS;
    irqNumber                  = usbDeviceEhciIrq[CONTROLLER_ID - kUSB_ControllerEhci0];

    return irqNumber;
}

#if MBED_CONF_TARGET_ENABLE_OVERDRIVE_MODE
#define LPM_POWER_MODE LPM_PowerModeOverRun
#else
#define LPM_POWER_MODE LPM_PowerModeFullRun
#endif

void vPortPRE_SLEEP_PROCESSING(clock_mode_t powermode)
{
    LPM_EnableWakeupSource(GPT2_IRQn);
    LPM_EnterLowPowerIdle(LPM_POWER_MODE);

    // Disable us ticker during deep sleep.
    // Note: Must do this last because SDK_DelayAtLeastUs() uses the us ticker
    PIT_StopTimer(PIT, kPIT_Chnl_0);
    PIT_StopTimer(PIT, kPIT_Chnl_2);
}

void vPortPOST_SLEEP_PROCESSING(clock_mode_t powermode)
{
    // reenable us ticker
    // Note: Must do this first because SDK_DelayAtLeastUs() uses the us ticker
    PIT_StartTimer(PIT, kPIT_Chnl_0);
    PIT_StartTimer(PIT, kPIT_Chnl_2);

    LPM_ExitLowPowerIdle(LPM_POWER_MODE);
    LPM_DisableWakeupSource(GPT2_IRQn);
}

// Override of MIMXRT SDK delay function.
// The default delay function used the full CPU clock frequency, so it produced massive overshoots
// (delaying 30x longer than intended) when the MCU is exiting sleep (and core clock is reduced to 24MHz).
// This delay function uses the us ticker which always ticks at the same speed even when the CPU clock is reduced.
void SDK_DelayAtLeastUs(uint32_t delay_us)
{
    uint32_t initialTickerValue = us_ticker_read();
    uint32_t targetTickerValue = delay_us + initialTickerValue;

    // Wait for rollover if needed
    if(targetTickerValue < initialTickerValue) {
        while(us_ticker_read() > initialTickerValue) {}
    }

    // Wait until target time
    while(us_ticker_read() < targetTickerValue) {}
}
