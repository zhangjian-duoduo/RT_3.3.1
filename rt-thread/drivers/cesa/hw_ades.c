/********************************************************************************************/
/* Fullhan Technology (Shanghai) Co., Ltd.                                                  */
/* Fullhan Proprietary and Confidential                                                     */
/* Copyright (c) 2017 Fullhan Technology Group Limited and its affiliated companies         */
/********************************************************************************************/
#include "hw_cesa_if.h"


#define EncryptControl        (0x0)
#define FIFOStatus            (0x8)
#define PErrStatus            (0xC)
#define ClearKey0            (0x10)
#define ClearKey1            (0x14)
#define ClearKey2            (0x18)
#define ClearKey3            (0x1C)
#define ClearKey4            (0x20)
#define ClearKey5            (0x24)
#define ClearKey6            (0x28)
#define ClearKey7            (0x2c)
#define InitIV0                (0x30)
#define InitIV1                (0x34)
#define InitIV2                (0x38)
#define InitIV3                (0x3C)
#define DMASrc0                (0x48)
#define DMADes0                (0x4c)
#define DMATrasSize            (0x50)
#define DMACtrl                (0x54)
#define FIFOThold              (0x58)
#define IntrEnable             (0x5c)
#define IntrSrc                (0x60)
#define MaskedIntrStatus       (0x64)
#define IntrClear              (0x68)
#define LastIV0                (0x80)
#define LastIV1                (0x84)
#define LastIV2                (0x88)
#define LastIV3                (0x8c)

typedef union _HW_CIPHER_CTRL_REG
{
  uint32_t all;
  struct {
  uint32_t OPER_MODE:1;
  uint32_t ALGO_MODE:3;
  uint32_t WORK_MODE:3;
  uint32_t FIRST_BLOCK:1;
  uint32_t DES_PCHK:1;
  uint32_t SELF_KEY_GEN_EN:1;
  uint32_t:22;
  } bitc;
} HW_CIPHER_CTRL_REG;

static void biglittle_swap(uint8_t *buf)
{
    uint8_t tmp, tmp1;

    tmp = buf[0];
    tmp1 = buf[1];
    buf[0] = buf[3];
    buf[1] = buf[2];
    buf[2] = tmp1;
    buf[3] = tmp;
}

static rt_bool_t use_efuse_key(HW_CESA_ADES_CTRL_S *hw_ctrl, uint32_t key_size)
{
    if (hw_ctrl->efuse_para.mode & CRYPTO_EX_MEM_SET_KEY)
    {
#ifdef FH_USING_EFUSE
        extern void efuse_trans_key(void *efuse_para);
        efuse_trans_key(&hw_ctrl->efuse_para);
#else
        rt_kprintf("Failed to use efuse key due to FH_USING_EFUSE is off\n");
#endif
        return RT_TRUE;
    } else
        return RT_FALSE;
}

int32_t hw_cesa_ades_config(HW_CESA_ADES_CTRL_S *hw_ctrl)
{
    HW_CIPHER_CTRL_REG ctrl_reg;
    uint32_t temp_key_buf[8];
    uint32_t temp_iv_buf[4];
    uint32_t i, key_size, iv_size;


    ctrl_reg.all = ADES_REG32(EncryptControl);
    ctrl_reg.bitc.OPER_MODE = hw_ctrl->oper_mode;
    ctrl_reg.bitc.ALGO_MODE = hw_ctrl->algo_mode;
    ctrl_reg.bitc.WORK_MODE = hw_ctrl->work_mode;
    ctrl_reg.bitc.FIRST_BLOCK = 1;
    ADES_REG32(EncryptControl) = ctrl_reg.all;

    /*** KEY ***/
    switch (hw_ctrl->algo_mode)
    {
    case HW_CESA_ADES_ALGO_MODE_DES:
        key_size = 8;
        break;
    case HW_CESA_ADES_ALGO_MODE_TDES:
        key_size = 24;
        break;
    case HW_CESA_ADES_ALGO_MODE_AES128:
        key_size = 16;
        break;
    case HW_CESA_ADES_ALGO_MODE_AES192:
        key_size = 24;
        break;
    case HW_CESA_ADES_ALGO_MODE_AES256:
        key_size = 32;
        break;
    default:
        rt_kprintf("cipher error algo_mode!!\n");
        return CESA_ADES_ALGO_MODE_ERROR;
    }

    if (use_efuse_key(hw_ctrl, key_size) == RT_FALSE)
    {
        memcpy((uint8_t *) &temp_key_buf[0], hw_ctrl->key, key_size);
        for (i = 0; i < key_size / sizeof(uint32_t); i++)
        {
            biglittle_swap((uint8_t *) (temp_key_buf + i));
            ADES_REG32(ClearKey0 + 4*i) = temp_key_buf[i];
        }
    }

    /*** IV ***/
    switch (hw_ctrl->work_mode)
    {
    case HW_CESA_ADES_WORK_MODE_CBC:
    case HW_CESA_ADES_WORK_MODE_CTR:
    case HW_CESA_ADES_WORK_MODE_CFB:
    case HW_CESA_ADES_WORK_MODE_OFB:
        if (hw_ctrl->algo_mode >= HW_CESA_ADES_ALGO_MODE_AES128)
            iv_size = 16;
        else
            iv_size = 8;
        break;
    case HW_CESA_ADES_WORK_MODE_ECB:
        iv_size = 0;
        break;
    default:
        rt_kprintf("cipher error work_mode!!\n");
        return CESA_ADES_WORK_MODE_ERROR;
    }


    for (i = 0; i < iv_size; i++)
    {
        if (hw_ctrl->iv_last[i] != 0)
        {
            uint32_t *temp_iv_ptr = (uint32_t *)hw_ctrl->iv_last;

            for (i = 0; i < iv_size / sizeof(uint32_t); i++)
                ADES_REG32(InitIV0 + 4*i) = temp_iv_ptr[i];

            break;
        }
    }

    if (i >= iv_size)
    {
        memcpy((uint8_t *) &temp_iv_buf[0], hw_ctrl->iv_init, iv_size);
        for (i = 0; i < iv_size / sizeof(uint32_t); i++)
        {
            biglittle_swap((uint8_t *) (temp_iv_buf + i));
            ADES_REG32(InitIV0 + 4*i) = temp_iv_buf[i];
        }
    }

    /* algo_mode & work_mode conflict check */
    if (hw_ctrl->algo_mode <= HW_CESA_ADES_ALGO_MODE_TDES
        && hw_ctrl->work_mode == HW_CESA_ADES_WORK_MODE_CTR)
        return CESA_ADES_WORK_ALGO_MODE_CONFLICT;

    return CESA_SUCCESS;
}

int32_t hw_cesa_ades_process(HW_CESA_ADES_CTRL_S *hw_ctrl,
                                    uint32_t phy_src_addr,
                                    uint32_t phy_dst_addr,
                                    uint32_t length)
{
    uint32_t outfifo_thold = 0x40;
    uint32_t infifo_thold = 0x40;

    if ((phy_src_addr & 0x00000003) || (phy_dst_addr & 0x00000003))
    {
        rt_kprintf("ades at dma mode: input or output address is not 4bytes multiple\n");
        return CESA_ADES_DMA_ADDR_ERROR;
    }

    /* length check */
    /* AES/DES CFB data length 1 byte align is permitted */
    if (hw_ctrl->algo_mode >= HW_CESA_ADES_ALGO_MODE_AES128)
    {
        if (hw_ctrl->work_mode != HW_CESA_ADES_WORK_MODE_CFB)
        {
            if (length & 0xF)
            {
                rt_kprintf("error: AES ECB/CBC/OFB/CTR data length should be 16 byte align!\n");
                return CESA_ADES_DATA_LENGTH_ERROR;
            }
        }
    }
    else
    {
        if (hw_ctrl->work_mode != HW_CESA_ADES_WORK_MODE_CFB)
        {
            if (length & 0x7)
            {
                rt_kprintf("error: DES ECB/CBC/OFB data length should be 8 byte align!\n");
                return CESA_ADES_DATA_LENGTH_ERROR;
            }
        }
    }

    ADES_REG32(DMASrc0) = phy_src_addr;
    ADES_REG32(DMADes0) = phy_dst_addr;
    ADES_REG32(DMATrasSize) = length;
    ADES_REG32(FIFOThold) = (outfifo_thold << 8) | infifo_thold;
    ADES_REG32(IntrEnable) = 1;
    ADES_REG32(DMACtrl) = 1;

    return CESA_SUCCESS;
}

uint32_t hw_cesa_ades_intr_src(void)
{
    uint32_t intr_src;

    intr_src = ADES_REG32(IntrSrc);
    ADES_REG32(IntrClear) = 1;

    return intr_src;
}

void hw_cesa_ades_save_lastiv(HW_CESA_ADES_CTRL_S *hw_ctrl)
{
    uint32_t temp_iv_buf[4];

    temp_iv_buf[0] = ADES_REG32(LastIV0);
    temp_iv_buf[1] = ADES_REG32(LastIV1);
    temp_iv_buf[2] = ADES_REG32(LastIV2);
    temp_iv_buf[3] = ADES_REG32(LastIV3);
    memcpy(hw_ctrl->iv_last, temp_iv_buf, 16);
}

void hw_cesa_ades_restore_lastiv(HW_CESA_ADES_CTRL_S *hw_ctrl)
{
    memset(hw_ctrl->iv_last, 0, IV_MAX_SIZE);
}

