# GD32F4xx USB CDC 初始化和调用总结

## 1. 概述

本文档总结了 GD32F4xx 系列微控制器中 USB CDC (Communication Device Class) 的初始化过程和相关函数调用。分析基于以下文件：

- `application/main.c` - 主程序文件
- `application/usb_app.c` - USB 硬件配置实现
- `firmware/GD32F4xx_usb_library/device/class/cdc/` - USB CDC 类实现

## 2. USB 初始化流程

### 2.1 主程序中的初始化调用

在 `application/main.c` 中，USB 初始化流程如下：

```c
int main(void)
{
    // 打印启动信息
    printf(" ____ ____ _________  _____ _  _  ____   ___  _ \n");
    printf(" / ___|  _ \\___ /___ \\|  ___| || || ___| / _ \\| |\n");
    printf("| |  _| | | ||_ \\ __) | |_  | || ||___ \\| | | | |\n");
    printf("| |_| | |_| |__) / __/|  _| |__   _|__) | |_| |_|\n");
    printf(" \\____|____/____/_____|_|      |_||____/ \\___/(_)\n");

    // USB 硬件配置
    usb_gpio_config();    // 配置 USB GPIO 引脚
    usb_rcu_config();     // 配置 USB 时钟
    usb_timer_init();     // 初始化 USB 定时器

    // USB 设备初始化
    usbd_init(&cdc_acm,
#ifdef USE_USB_FS
              USB_CORE_ENUM_FS,
#elif defined(USE_USB_HS)
              USB_CORE_ENUM_HS,
#endif /* USE_USB_FS */
              &cdc_desc,
              &cdc_class);

    // USB 中断配置
    usb_intr_config();

    /* 主循环 */
    while (1) {
        if (USBD_CONFIGURED == cdc_acm.dev.cur_status) {
            if (0U == cdc_acm_check_ready(&cdc_acm)) {
                cdc_acm_data_receive(&cdc_acm);
            } else {
                cdc_acm_data_send(&cdc_acm);
            }
        }
    }
}
```

### 2.2 USB 硬件配置

在 `application/usb_app.c` 中实现了 USB 硬件配置函数：

#### 2.2.1 时钟配置
```c
void usb_rcu_config(void)
{
#ifdef USE_USB_FS
    // 使用内部 48MHz 时钟
    rcu_ck48m_clock_config(RCU_CK48MSRC_IRC48M);
    // 使能 USBFS 时钟
    rcu_periph_clock_enable(RCU_USBFS);
#elif defined(USE_USB_HS)
#ifdef USE_EMBEDDED_PHY
    rcu_pll48m_clock_config(RCU_PLL48MSRC_PLLQ);
    rcu_ck48m_clock_config(RCU_CK48MSRC_PLL48M);
#elif defined(USE_ULPI_PHY)
    rcu_periph_clock_enable(RCU_USBHSULPI);
#endif /* USE_EMBEDDED_PHY */
    // 使能 USBHS 时钟
    rcu_periph_clock_enable(RCU_USBHS);
#endif /* USB_USBFS */
}
```

#### 2.2.2 GPIO 配置
```c
void usb_gpio_config(void)
{
    rcu_periph_clock_enable(RCU_SYSCFG);

#ifdef USE_USB_FS
    // 使能 GPIOA 时钟
    rcu_periph_clock_enable(RCU_GPIOA);
    // 配置 USBFS_DM(PA11) 和 USBFS_DP(PA12) 引脚
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_11 | GPIO_PIN_12);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, GPIO_PIN_11 | GPIO_PIN_12);
    gpio_af_set(GPIOA, GPIO_AF_10, GPIO_PIN_11 | GPIO_PIN_12);
#elif defined(USE_USB_HS)
    // 根据使用的 PHY 类型配置相应的引脚
    // ...
#endif /* USE_USBFS */
}
```

#### 2.2.3 中断配置
```c
void usb_intr_config(void)
{
    nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);

#ifdef USE_USB_FS
    nvic_irq_enable((uint8_t)USBFS_IRQn, 2U, 0U);
#if USBFS_LOW_POWER
    // 配置低功耗唤醒中断
    // ...
#endif /* USBFS_LOW_POWER */
#elif defined(USE_USB_HS)
    nvic_irq_enable((uint8_t)USBHS_IRQn, 2U, 0U);
#if USBHS_LOW_POWER
    // 配置低功耗唤醒中断
    // ...
#endif /* USBHS_LOW_POWER */
#endif /* USE_USB_FS */

#ifdef USB_HS_DEDICATED_EP1_ENABLED
    nvic_irq_enable(USBHS_EP1_Out_IRQn, 1, 0);
    nvic_irq_enable(USBHS_EP1_In_IRQn, 1, 0);
#endif /* USB_HS_DEDICATED_EP1_ENABLED */
}
```

#### 2.2.4 定时器初始化
```c
void usb_timer_init(void)
{
    // 配置优先级分组
    nvic_priority_group_set(NVIC_PRIGROUP_PRE2_SUB2);
    // 使能 TIMER2 全局中断
    nvic_irq_enable((uint8_t)TIMER2_IRQn, 1U, 0U);
    // 使能 TIMER2 时钟
    rcu_periph_clock_enable(RCU_TIMER2);
}
```

### 2.3 USB 设备初始化

在 `firmware/GD32F4xx_usb_library/device/core/Source/usbd_core.c` 中实现了 `usbd_init` 函数：

```c
void usbd_init (usb_core_driver *udev, usb_core_enum core, usb_desc *desc, usb_class_core *class_core)
{
    udev->dev.desc = desc;
    // 设置类回调函数
    udev->dev.class_core = class_core;
    // 创建序列号字符串
    serial_string_get(udev->dev.desc->strings[STR_IDX_SERIAL]);
    // 配置 USB 基本功能
    (void)usb_basic_init (&udev->bp, &udev->regs, core);
    // 禁用 USB 全局中断
    usb_globalint_disable(&udev->regs);
    // 初始化 USB 核心
    (void)usb_core_init (udev->bp, &udev->regs);
    // 设置设备断开连接
    usbd_disconnect (udev);
#ifndef USE_OTG_MODE
    usb_curmode_set(&udev->regs, DEVICE_MODE);
#endif /* USE_OTG_MODE */
    // 初始化设备模式
    (void)usb_devcore_init (udev);
    // 使能 USB 全局中断
    usb_globalint_enable(&udev->regs);
    // 设置设备连接
    usbd_connect (udev);
    // 设置设备状态为默认状态
    udev->dev.cur_status = (uint8_t)USBD_DEFAULT;
}
```

## 3. CDC 类实现

### 3.1 CDC 描述符

在 `firmware/GD32F4xx_usb_library/device/class/cdc/Source/cdc_acm_core.c` 中定义了 CDC 设备描述符：

```c
/* USB 标准设备描述符 */
__ALIGN_BEGIN const usb_desc_dev cdc_dev_desc __ALIGN_END =
{
    .header = 
     {
         .bLength          = USB_DEV_DESC_LEN, 
         .bDescriptorType  = USB_DESCTYPE_DEV,
     },
    .bcdUSB                = 0x0200U,
    .bDeviceClass          = USB_CLASS_CDC,
    .bDeviceSubClass       = 0x00U,
    .bDeviceProtocol       = 0x00U,
    .bMaxPacketSize0       = USB_FS_EP0_MAX_LEN,
    .idVendor              = USBD_VID,
    .idProduct             = USBD_PID,
    .bcdDevice             = 0x0100U,
    .iManufacturer         = STR_IDX_MFC,
    .iProduct              = STR_IDX_PRODUCT,
    .iSerialNumber         = STR_IDX_SERIAL,
    .bNumberConfigurations = USBD_CFG_MAX_NUM,
};

/* USB 设备配置描述符 */
__ALIGN_BEGIN const usb_cdc_desc_config_set cdc_config_desc __ALIGN_END = 
{
    .config = 
    {
        // 配置描述符
    },
    .cmd_itf = 
    {
        // 命令接口描述符
    },
    .cdc_header =
    {
        // CDC 头部功能描述符
    },
    .cdc_call_managment =
    {
        // CDC 调用管理功能描述符
    },
    .cdc_acm =
    {
        // CDC 抽象控制模型功能描述符
    },
    .cdc_union =
    {
        // CDC 联合功能描述符
    },
    .cdc_cmd_endpoint =
    {
        // CDC 命令端点描述符
    },
    .cdc_data_interface =
    {
        // CDC 数据接口描述符
    },
    .cdc_out_endpoint =
    {
        // CDC 数据 OUT 端点描述符
    },
    .cdc_in_endpoint =
    {
        // CDC 数据 IN 端点描述符
    }
};
```

### 3.2 CDC 类回调函数

在 `firmware/GD32F4xx_usb_library/device/class/cdc/Source/cdc_acm_core.c` 中定义了 CDC 类回调函数结构体：

```c
/* USB CDC 设备类回调函数结构体 */
usb_class_core cdc_class =
{
    .command   = NO_CMD,
    .alter_set = 0U,
    .init      = cdc_acm_init,
    .deinit    = cdc_acm_deinit,
    .req_proc  = cdc_acm_req,
    .ctlx_out  = cdc_acm_ctlx_out,
    .data_in   = cdc_acm_in,
    .data_out  = cdc_acm_out
};
```

### 3.3 CDC 核心函数

#### 3.3.1 CDC 初始化函数
```c
static uint8_t cdc_acm_init (usb_dev *udev, uint8_t config_index)
{
    static __ALIGN_BEGIN usb_cdc_handler cdc_handler __ALIGN_END;

    // 初始化数据 Tx 端点
    usbd_ep_setup (udev, &(cdc_config_desc.cdc_in_endpoint));
    // 初始化数据 Rx 端点
    usbd_ep_setup (udev, &(cdc_config_desc.cdc_out_endpoint));
    // 初始化命令 Tx 端点
    usbd_ep_setup (udev, &(cdc_config_desc.cdc_cmd_endpoint));

    // 初始化 CDC 处理程序结构体
    cdc_handler.packet_receive = 1U;
    cdc_handler.packet_sent = 1U;
    cdc_handler.receive_length = 0U;

    cdc_handler.line_coding = (acm_line){
        .dwDTERate   = 115200U,
        .bCharFormat = 0U,
        .bParityType = 0U,
        .bDataBits   = 0x08U
    };

    udev->dev.class_data[CDC_COM_INTERFACE] = (void *)&cdc_handler;

    return USBD_OK;
}
```

#### 3.3.2 CDC 数据发送函数
```c
void cdc_acm_data_send (usb_dev *udev)
{
    usb_cdc_handler *cdc = (usb_cdc_handler *)udev->dev.class_data[CDC_COM_INTERFACE];

    if (0U != cdc->receive_length) {
        cdc->packet_sent = 0U;
        usbd_ep_send (udev, CDC_DATA_IN_EP, (uint8_t*)(cdc->data), cdc->receive_length);
        cdc->receive_length = 0U;
    }
}
```

#### 3.3.3 CDC 数据接收函数
```c
void cdc_acm_data_receive (usb_dev *udev)
{
    usb_cdc_handler *cdc = (usb_cdc_handler *)udev->dev.class_data[CDC_COM_INTERFACE];

    cdc->packet_receive = 0U;
    cdc->packet_sent = 0U;

    usbd_ep_recev (udev, CDC_DATA_OUT_EP, (uint8_t*)(cdc->data), USB_CDC_DATA_PACKET_SIZE);
}
```

#### 3.3.4 CDC 状态检查函数
```c
uint8_t cdc_acm_check_ready(usb_dev *udev)
{
    if (udev->dev.class_data[CDC_COM_INTERFACE] != NULL) {
        usb_cdc_handler *cdc = (usb_cdc_handler *)udev->dev.class_data[CDC_COM_INTERFACE];

        if ((1U == cdc->packet_receive) && (1U == cdc->packet_sent)) {
            return 0U;
        }
    }

    return 1U;
}
```

## 4. 端点配置

在 `board/usbd_conf.h` 中定义了 CDC 端点配置：

```c
#define CDC_DATA_IN_EP          EP1_IN  /* EP1 for data IN */
#define CDC_DATA_OUT_EP         EP3_OUT /* EP3 for data OUT */
#define CDC_CMD_EP              EP2_IN  /* EP2 for CDC commands */

#define USB_CDC_CMD_PACKET_SIZE 8 /* Control Endpoint Packet size */
#define USB_CDC_DATA_PACKET_SIZE 64 /* Endpoint IN & OUT Packet size */
```

## 5. 总结

GD32F4xx 的 USB CDC 实现遵循标准的 USB 设备初始化流程：

1. 配置 USB 硬件（时钟、GPIO、中断、定时器）
2. 初始化 USB 设备核心
3. 注册 CDC 类回调函数
4. 在主循环中处理数据收发

CDC 类实现了标准的 ACM (Abstract Control Model) 功能，支持串行通信的基本操作，包括数据发送、接收和线路编码设置等。