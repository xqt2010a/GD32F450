#include "wm8731.h"
#include "string.h"

static struct WM8731_REGS s_wm8731_regs;
static uint16_t s_currect_line_in_volume = VOLUME_LINE_IN_DEFAULT;
static uint16_t s_currect_headphone_volume = VOLUME_HEADPHONE_DEFAULT - 15;

void i2c_write_wm8731(uint8_t Reg_Addr, uint8_t *buf, uint8_t len)
{
    I2C_Write(I2C1, buf,len);
}

static void wm8731_write_reg(uint8_t reg, uint16_t data)
{
    uint32_t j = 0;
    uint8_t write_buff[2] = {0};
    write_buff[0] = (reg << 1) | (data >> 8);
    write_buff[1] = (uint8_t)data;
    i2c_write_wm8731(WM8731_ADDR, write_buff, 2);  //reset
    for (j = 0; j < 0xFFFF; j++);
}

static void wm8731_write_regs(void)
{
    wm8731_write_reg(REG_RESET_REG, s_wm8731_regs.reset.all);
    wm8731_write_reg(REG_L_LINE_IN, s_wm8731_regs.l_line_in.all);
    wm8731_write_reg(REG_R_LINE_IN, s_wm8731_regs.r_line_in.all);
    wm8731_write_reg(REG_L_HEADPHONE, s_wm8731_regs.l_headphone.all);
    wm8731_write_reg(REG_R_HEADPHONE, s_wm8731_regs.r_headphone.all);
    wm8731_write_reg(REG_A_AUDIO_PATH_CTL, s_wm8731_regs.a_audio_path_ctl.all);
    wm8731_write_reg(REG_D_AUDIO_PATH_CTL, s_wm8731_regs.d_audio_path_ctl.all);
    wm8731_write_reg(REG_POWER_DOWN_CTL, s_wm8731_regs.power_down_ctl.all);
    wm8731_write_reg(REG_D_AUDIO_INTERFACE_FMT, s_wm8731_regs.d_audio_interface_fmt.all);
    wm8731_write_reg(REG_SAMPLING_CTL, s_wm8731_regs.sampling_ctl.all);
    wm8731_write_reg(REG_ACTIVE_CTL, s_wm8731_regs.active_ctl.all);
}

void wm8731_init(void)
{
    I2C_InitTypeDef I2C_InitS;

    I2C_InitS.I2C_Mode = I2C_MASTER;  //Master
    I2C_InitS.I2C_AddrMode = I2C_7BIT;  //7bit
    I2C_InitS.I2C_Speed = I2C_STANDARD;
    I2C_InitS.I2C_OwnAddress = WM8731_ADDR;
    
    I2C_Init(I2C1, &I2C_InitS);

    memset(&s_wm8731_regs, 0, sizeof(struct WM8731_REGS));
    /* wm8731 reset */
    s_wm8731_regs.reset.all = 0x0000;
    /* left line in volume */
    s_wm8731_regs.l_line_in.bit.linvol = s_currect_line_in_volume;/* set line in volume */
    s_wm8731_regs.l_line_in.bit.linmute = 0;    //1:enable mute  0:disable mute
    s_wm8731_regs.l_line_in.bit.lrinboth = 0;
    /* right line in volume */
    s_wm8731_regs.r_line_in.bit.rinvol = s_currect_line_in_volume;
    s_wm8731_regs.r_line_in.bit.rinmute = 0;
    s_wm8731_regs.r_line_in.bit.rlinboth = 0;
    /* left headphone out volume */
    s_wm8731_regs.l_headphone.bit.lhpvol = s_currect_headphone_volume;/* set headphone volume */
    s_wm8731_regs.l_headphone.bit.lzcen = 1;
    s_wm8731_regs.l_headphone.bit.lrhpboth = 1;
    /* right headphone out volume */
    s_wm8731_regs.r_headphone.bit.rhpvol = s_currect_headphone_volume;
    s_wm8731_regs.r_headphone.bit.rzcen = 1;
    s_wm8731_regs.r_headphone.bit.rlhpboth = 1;
    /* analogue audio path control */
    s_wm8731_regs.a_audio_path_ctl.bit.micboost = 0;    //1
    s_wm8731_regs.a_audio_path_ctl.bit.mutemic = 0;     //0
    s_wm8731_regs.a_audio_path_ctl.bit.insel = 0;       //1 1:microphone input 0:line input
    s_wm8731_regs.a_audio_path_ctl.bit.bypass = 0;      //0
    s_wm8731_regs.a_audio_path_ctl.bit.dacsel = 0;      //1
    s_wm8731_regs.a_audio_path_ctl.bit.sidetone = 0;    //0
    s_wm8731_regs.a_audio_path_ctl.bit.sideatt = 0;     //0
    /* digital audio path control */
    s_wm8731_regs.d_audio_path_ctl.bit.adchpd = 1;  /* disable high pass filter */
    s_wm8731_regs.d_audio_path_ctl.bit.deemp = 0;
    s_wm8731_regs.d_audio_path_ctl.bit.dacmu = 0;
    s_wm8731_regs.d_audio_path_ctl.bit.hpor = 0;
    /* power down control */
    s_wm8731_regs.power_down_ctl.bit.lineinpd = 1;
    s_wm8731_regs.power_down_ctl.bit.micpd = 1;
    s_wm8731_regs.power_down_ctl.bit.adcpd = 1;
    s_wm8731_regs.power_down_ctl.bit.dacpd = 0;
    s_wm8731_regs.power_down_ctl.bit.outpd = 0;
    s_wm8731_regs.power_down_ctl.bit.oscpd = 1;
    s_wm8731_regs.power_down_ctl.bit.clkoutpd = 1;
    s_wm8731_regs.power_down_ctl.bit.poweroff = 0;
    /*digital audio interface format:i2s format,bit 16 */
    s_wm8731_regs.d_audio_interface_fmt.bit.format = WM8731_FMT_I2S;
    s_wm8731_regs.d_audio_interface_fmt.bit.iwl = WM8731_BIT_16;
    s_wm8731_regs.d_audio_interface_fmt.bit.lrp = 0;
    s_wm8731_regs.d_audio_interface_fmt.bit.lrswap = 0;
    s_wm8731_regs.d_audio_interface_fmt.bit.ms = 0;
    s_wm8731_regs.d_audio_interface_fmt.bit.bclkinv = 0;
    /* sampling control:normal_mode,256fs,adc:44.1k,dac:44.1k */
    s_wm8731_regs.sampling_ctl.bit.usb_normal = WM8731_MODE_NORMAL;
    s_wm8731_regs.sampling_ctl.bit.bosr = WM8731_FRQ_256FS;
    s_wm8731_regs.sampling_ctl.bit.sr = WM8731_SAMPLE_RATE_44_44;
    s_wm8731_regs.sampling_ctl.bit.clkidiv2 = 0;
    s_wm8731_regs.sampling_ctl.bit.clkodiv2 = 0;
    /* active wm8731 */
    s_wm8731_regs.active_ctl.bit.active = 1;
    //logd("wm8731 init ok");
    wm8731_write_regs();
}

void WM8731_Reset(void)
{
    uint8_t temp[2];

    temp[0] = 0x1E;
    temp[1] = 0x00;
    i2c_write_wm8731(WM8731_ADDR, temp, 2);  //reset
}

void wm8731_set_headphone_volume(direction_t dir, volume_set_mode_t mode, uint8_t volume)
{
    if (volume >= VOLUME_HEADPHONE_MAX)
    {
        //loge("%s line(%d):volume is out of range:%d\r\n",__func__,__LINE__,volume);
        return;
    }

    switch (mode)
    {
    case WM8731_MODE_HIGH:
        s_currect_headphone_volume += volume;
        if (s_currect_headphone_volume >= VOLUME_HEADPHONE_MAX)
            s_currect_headphone_volume = VOLUME_HEADPHONE_MAX;
        break;
    case WM8731_MODE_LOW:
        if (s_currect_headphone_volume <= volume + VOLUME_HEADPHONE_MIN)
            s_currect_headphone_volume = VOLUME_HEADPHONE_MIN;
        else
            s_currect_headphone_volume -= volume;
        break;
    case WM8731_MODE_LEVER:
        s_currect_headphone_volume = volume;
        if (s_currect_headphone_volume >= VOLUME_HEADPHONE_MAX)
            s_currect_headphone_volume = VOLUME_HEADPHONE_MAX;
        break;
    case WM8731_MODE_SILENCE:
        s_currect_headphone_volume = VOLUME_HEADPHONE_MIN;
        break;
    default:
        break;
    }

    switch (dir)
    {
    case WM8731_DIR_LEFT:
        s_wm8731_regs.l_headphone.bit.lhpvol = s_currect_headphone_volume;
        wm8731_write_reg(REG_L_HEADPHONE, s_wm8731_regs.l_headphone.all);
        break;
    case WM8731_DIR_RIGHT:
        s_wm8731_regs.r_headphone.bit.rhpvol = s_currect_headphone_volume;
        wm8731_write_reg(REG_R_HEADPHONE, s_wm8731_regs.l_headphone.all);
        break;
    default:
        //loge("%s line(%d):error direction:%d\r\n",__func__,__LINE__,dir);
        break;
    }
}

uint16_t wm8731_get_headphone_volume(direction_t dir)
{
    return s_currect_headphone_volume;
}

void wm8731_set_line_in_volume(direction_t dir, volume_set_mode_t mode, uint8_t volume)
{
    if (volume >= VOLUME_HEADPHONE_MAX)
    {
        //loge("volume is out of range:%d\r\n",volume);
        return;
    }

    switch (mode)
    {
    case WM8731_MODE_HIGH:
        s_currect_line_in_volume += volume;
        if (s_currect_line_in_volume >= VOLUME_LINE_IN_MAX)
            s_currect_line_in_volume = VOLUME_LINE_IN_MAX;
        break;
    case WM8731_MODE_LOW:
        if (s_currect_line_in_volume <= volume)
            s_currect_line_in_volume = 0;
        else
            s_currect_line_in_volume -= volume;
        break;
    case WM8731_MODE_LEVER:
        s_currect_line_in_volume = volume;
        if (s_currect_line_in_volume >= VOLUME_LINE_IN_MAX)
            s_currect_line_in_volume = VOLUME_LINE_IN_MAX;
        break;
    case WM8731_MODE_SILENCE:
        s_currect_line_in_volume = 0;
        break;
    default:
        break;
    }

    switch (dir)
    {
    case WM8731_DIR_LEFT:
        s_wm8731_regs.l_line_in.bit.linvol = s_currect_line_in_volume;
        wm8731_write_reg(REG_L_LINE_IN, s_wm8731_regs.l_line_in.all);
        break;
    case WM8731_DIR_RIGHT:
        s_wm8731_regs.r_line_in.bit.rinvol = s_currect_line_in_volume;
        wm8731_write_reg(REG_R_LINE_IN, s_wm8731_regs.r_line_in.all);
        break;
    default:
        //loge("%s line(%d):error direction:%d\r\n",__func__,__LINE__,dir);
        break;
    }
}