/*
 * Copyright (c) 2006-2022, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author            Notes
 * 2022-01-25     iysheng           first version
 */

#include <board.h>
#include <sys/time.h>

#define DBG_TAG "drv.rtc"
#define DBG_LVL DBG_LOG

#include <rtdbg.h>

#ifdef RT_USING_RTC

#define RTC_CLOCK_SOURCE_LXTAL
#define BKP_VALUE 0x32F0

rtc_parameter_struct rtc_initpara;
__IO uint32_t prescaler_a = 0, prescaler_s = 0;
uint32_t RTCSRC_FLAG = 0;

typedef struct {
    struct rt_device rtc_dev;
} gd32_rtc_device;

static gd32_rtc_device g_gd32_rtc_dev;

static int bcd2dec(uint8_t data)
{
    int temp;
    temp = ((data >> 4) * 10 + (data & 0x0f));
    return temp;
}

static uint8_t dec2bcd(int data)
{
    uint8_t temp;
    temp = (((data / 10) << 4) + (data % 10));
    return temp;
}

static time_t gd32_rtctime2stamp(rtc_parameter_struct rtctime)
{
    struct tm tmp;
    tmp.tm_year = bcd2dec(rtctime.year) + 100;
    tmp.tm_mon  = bcd2dec(rtctime.month) - 1;
    tmp.tm_mday = bcd2dec(rtctime.date);
    tmp.tm_hour = bcd2dec(rtctime.hour);
    tmp.tm_min  = bcd2dec(rtctime.minute);
    tmp.tm_sec  = bcd2dec(rtctime.second);
    return mktime(&tmp);
}

static void gd32_stamp2rtctime(time_t *stamp)
{
    rtc_parameter_struct rtctime;

    struct tm *tmp = localtime(stamp);

    rtc_initpara.year   = dec2bcd(tmp->tm_year - 100);
    rtc_initpara.month  = dec2bcd(tmp->tm_mon + 1);
    rtc_initpara.date   = dec2bcd(tmp->tm_mday);
    rtc_initpara.hour   = dec2bcd(tmp->tm_hour);
    rtc_initpara.minute = dec2bcd(tmp->tm_min);
    rtc_initpara.second = dec2bcd(tmp->tm_sec);
}

/*!
    \brief      display the current time
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void rtc_show_time(void)
{
    rtc_current_time_get(&rtc_initpara);
    time_t now = gd32_rtctime2stamp(rtc_initpara);
    rt_kprintf("local time: %.*s", 25, ctime(&now));
    rt_kprintf("timestamps: %ld\n", (long)now);
    rt_kprintf("Current time: %0.2x:%0.2x:%0.2x\n\r",
               rtc_initpara.hour, rtc_initpara.minute, rtc_initpara.second);
}
MSH_CMD_EXPORT(rtc_show_time, show the rtc time)

static rt_err_t rt_gd32_rtc_control(rt_device_t dev, int cmd, void *args)
{
    rt_err_t result = RT_EOK;

    RT_ASSERT(dev != RT_NULL);
    switch (cmd) {
        case RT_DEVICE_CTRL_RTC_GET_TIME:
            rtc_current_time_get(&rtc_initpara);
            time_t now        = gd32_rtctime2stamp(rtc_initpara);
            *(uint32_t *)args = (uint32_t)now;
            break;
        case RT_DEVICE_CTRL_RTC_GET_TIMEVAL:
            *(uint32_t *)args = rtc_subsecond_get();
            break;
        case RT_DEVICE_CTRL_RTC_SET_TIME:
            printf("set %ld\n", *(long *)args);
            gd32_stamp2rtctime((time_t *)args);
            if (ERROR == rtc_init(&rtc_initpara)) {
                LOG_E("** RTC time configuration failed! **");
            }
            break;
    }

    return result;
}

#ifdef RT_USING_DEVICE_OPS
const static struct rt_device_ops g_gd32_rtc_ops =
    {
        RT_NULL,
        RT_NULL,
        RT_NULL,
        RT_NULL,
        RT_NULL,
        rt_gd32_rtc_control};
#endif

/*!
    \brief      RTC configuration function
    \param[in]  none
    \param[out] none
    \retval     none
*/
void rtc_pre_config(void)
{
#if defined(RTC_CLOCK_SOURCE_IRC32K)
    rcu_osci_on(RCU_IRC32K);
    rcu_osci_stab_wait(RCU_IRC32K);
    rcu_rtc_clock_config(RCU_RTCSRC_IRC32K);

    prescaler_s = 0x13F;
    prescaler_a = 0x63;
#elif defined(RTC_CLOCK_SOURCE_LXTAL)
    rcu_osci_on(RCU_LXTAL);
    rcu_osci_stab_wait(RCU_LXTAL);
    rcu_rtc_clock_config(RCU_RTCSRC_LXTAL);

    prescaler_s                    = 0xFF;
    prescaler_a                    = 0x7F;
#else
#error RTC clock source should be defined.
#endif /* RTC_CLOCK_SOURCE_IRC32K */

    rcu_periph_clock_enable(RCU_RTC);
    rtc_register_sync_wait();
}

/*!
    \brief      use hyperterminal to setup RTC time and alarm
    \param[in]  none
    \param[out] none
    \retval     none
*/
void rtc_setup(void)
{
    rtc_initpara.factor_asyn = prescaler_a;
    rtc_initpara.factor_syn  = prescaler_s;
    rtc_initpara.year        = 0x20;
    rtc_initpara.month       = RTC_APR;
    rtc_initpara.date        = 0x30;
    // rtc_initpara.display_format = RTC_24HOUR;
    rtc_initpara.hour = 0x11;

    /* RTC current time configuration */
    if (ERROR == rtc_init(&rtc_initpara)) {
        LOG_E("** RTC time configuration failed! **");
    } else {
        LOG_D("** RTC time configuration success! **");
        rtc_show_time();
        RTC_BKP0 = BKP_VALUE;
    }
}

static int rt_hw_rtc_init(void)
{
    rt_err_t ret;
    /* enable PMU clock */
    rcu_periph_clock_enable(RCU_PMU);
    /* enable the access of the RTC registers */
    pmu_backup_write_enable();

    rtc_pre_config();
    /* get RTC clock entry selection */
    RTCSRC_FLAG = GET_BITS(RCU_BDCTL, 8, 9);

    /* check if RTC has aready been configured */
    if ((BKP_VALUE != RTC_BKP0) || (0x00 == RTCSRC_FLAG)) {
        /* backup data register value is not correct or not yet programmed
        or RTC clock source is not configured (when the first time the program
        is executed or data in RCU_BDCTL is lost due to Vbat feeding) */
        rtc_setup();
    } else {
        /* detect the reset source */
        if (RESET != rcu_flag_get(RCU_FLAG_PORRST)) {
            LOG_I("power on reset occurred....");
        } else if (RESET != rcu_flag_get(RCU_FLAG_EPRST)) {
            LOG_I("external reset occurred....");
        }
        LOG_D("no need to configure RTC....");
        rtc_show_time();
    }

    rcu_all_reset_flag_clear();

#ifdef RT_USING_DEVICE_OPS
    g_gd32_rtc_dev.rtc_dev.ops = &g_gd32_rtc_ops;
#else
    g_gd32_rtc_dev.rtc_dev.init    = RT_NULL;
    g_gd32_rtc_dev.rtc_dev.open    = RT_NULL;
    g_gd32_rtc_dev.rtc_dev.close   = RT_NULL;
    g_gd32_rtc_dev.rtc_dev.read    = RT_NULL;
    g_gd32_rtc_dev.rtc_dev.write   = RT_NULL;
    g_gd32_rtc_dev.rtc_dev.control = rt_gd32_rtc_control;
#endif
    g_gd32_rtc_dev.rtc_dev.type        = RT_Device_Class_RTC;
    g_gd32_rtc_dev.rtc_dev.rx_indicate = RT_NULL;
    g_gd32_rtc_dev.rtc_dev.tx_complete = RT_NULL;
    g_gd32_rtc_dev.rtc_dev.user_data   = RT_NULL;

    ret = rt_device_register(&g_gd32_rtc_dev.rtc_dev, "rtc",
                             RT_DEVICE_FLAG_RDWR);
    if (ret != RT_EOK) {
        LOG_E("failed register internal rtc device, err=%d", ret);
    }

    return ret;
}
INIT_DEVICE_EXPORT(rt_hw_rtc_init);
#endif
