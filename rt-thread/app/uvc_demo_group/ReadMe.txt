uvc demo
编译前请先make menuconfig，并打开usb device中uvc驱动选项。


以下为简要说明，详细使用方法和配置参考《RT-Thread SDK开发指南.pdf》第11.2节-UVC&UAC使用指南

目录说明
- common        dsp、isp公共函数，如初始化ISP、创建编码通道等
- usb_audio     uac mic&speaker应用demo例程
- usb_vide      uvc应用demo例程
- usb_vcom      usb cdc-vcom应用demo例程

1、kernal config
    1) FH_RT_USB_DEVICE [=y] && FH_RT_USB_DEVICE_COMPOSITE [=y]；
    2) FH_RT_USB_DEVICE_UVC [=y] 使能uvc驱动；
    3) FH_RT_USB_DEVICE_UAC [=y] 使能uac驱动；
    4) FH_RT_USB_DEVICE_CDC [=y] 使能cdc-vcom驱动；
    5) vendor_label和device name可以配置设备名称；
    6) UVC_DOUBLE_STREAM[=y] or [=n], 是否使能双码流；
    7) Choose UVC EP type, 选择ISOC or BULK方式作为UVC端点传输方式；
    8) ISOC模式下：UVC_PACKET_SIZE [=1024]isoc端点大小，ISOC_HIGH_BANDWIDTH_EP [=y]启用高带宽模式；
    9) BULK模式下：UVC_PACKET_SIZE最大为512，UVC_BULK_PAYLOAD_PACKET_NUM bulk模式每个payloay传输usb包个数，即多少个UVC_PACKET_SIZE;
    10) ENABLE_SPEAKER_DESC  [=y] or [=n], 是否使能usb speaker；


功能说明:
1 coolview使用usb-vcom方式需要usb-cdc驱动支持: FH_RT_USB_DEVICE_CDC [=y]。
2 usb-vcom设备不属于免驱usb设置，需要在PC端安装CDC串口驱动。
3 usb-vcom串口支持coolview调试和串口finsh命令行功能，使用ESC键进行切换。
4 uvc在线升级功能, 需要使用《Usbtool.exe》工具进行升级
5 flash_layout发生更改或app.img以外的如ramboot、update.img修改时，升级uvc需要使用《升级完整固件.bat》配合《Usbtool.exe》工具进行升级
6 uvc flash固件导出和sensor hex文件导入需要使用《RTT 数据关联工具.exe》工具

注意:
1 windows驱动会保留设备枚举信息，如果UVC设备端添加或取消设备选择（如取消usb-cdc）后重新插入windows端口导致失败异常的，
  可卸载windows端uvc/uac/cdc驱动并重启设备。
2 升级完整固件过程中请勿断电, 升级完整固件失败有变砖风险, 请谨慎使用。

参考flash_layout:
    如果采用烧录Flash.img方式运行，建议使用以下flash_layout布局, 更改flash_layout需要同时修改ramboot, Usbtool.exe的usbtoolcfg.ini文件参数、RTT 数据关联工具
的UsbExportToolCfg.ini文件参数和FlashBase.h中的ISP_PARAM_ADDR && UPDATE_CFG_ADDR地址。同时修改生成img文件的配置文件rtt_update_fh885x.ini。
    {"reservd1",    0x020000, BLOCK_64K, PART_RESERVED},
    {"reservd2",    0x050000, BLOCK_64K, PART_RESERVED},
    {"cp-firmware", 0x040000, BLOCK_64K, PART_ARCFIRM},
    {"reservd3",    0x020000, BLOCK_64K, PART_RESERVED},
    {"application", 0x200000, BLOCK_64K, PART_LAST| PART_APPLICATION},

    另外：由于uvc功能后续增加了mstorage功能，而mstorage申请的ramfs分区需要挂载到jffs2文件系统的ramfs目录下，因此需要正确挂载jffs2分区，上述分区方式在不使用mstorage或
不挂载mstorage的ramfs分区时无需修改，但如果需要挂载mstorage的ramfs分区，则需要修改如下：(烧写该Flash.img时需要擦除flash大小为以下分区大小之和，否则会有很多warring打印)
    {"reservd1",    0x020000, BLOCK_64K, PART_RESERVED},
    {"reservd2",    0x050000, BLOCK_64K, PART_RESERVED},
    {"cp-firmware", 0x040000, BLOCK_64K, PART_ARCFIRM},
    {"reservd3",    0x020000, BLOCK_64K, PART_RESERVED},
    {"application", 0x200000, BLOCK_64K, PART_APPLICATION},
    {"user-datum",  0x200000, BLOCK_64K, PART_ROOT | PART_LAST},

    采用TFTP方式加载程序只需按照SDK flash_layout.c中规定地址烧写arc固件即可, 无需修改flash_layout. 此时hex参数升级、img固件导出、uvc固件升级功能不可用, 可能会
破坏flash中数据.


