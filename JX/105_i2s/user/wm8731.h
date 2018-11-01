#ifndef __WM8731_H__
#define __WM8731_H__

#include "jx_i2c.h"
#include "stdint.h"

#ifndef WM_ENABLE
#define WM_ENABLE 1
#endif

#ifndef WM_DISABLE
#define WM_DISABLE 0
#endif
/* wm8731 I2C interface */
#define WM8731_I2C  I2C1_BASE

#define WM8731_ADDR 0x1A
/* line in volume control */
#define VOLUME_LINE_IN_0DB          0x17
#define VOLUME_LINE_INT_STEP_1p5DB  0x01
#define VOLUME_LINE_IN_MAX          0x1f
#define VOLUME_LINE_IN_MIN          0x00
#define VOLUME_LINE_IN_DEFAULT      VOLUME_LINE_IN_0DB

/*headphone volume control */
#define VOLUME_HEADPHONE_0DB        0x79
#define VOLUME_HEADPHONE_STEP_1DB   0x01
#define VOLUME_HEADPHONE_MAX        0x7f
#define VOLUME_HEADPHONE_MIN        0x30
#define VOLUME_HEADPHONE_DEFAULT    VOLUME_HEADPHONE_0DB

/* left line in register */
struct  L_LINE_IN_BIT          // bits  description
{
    uint16_t    linvol: 5;   /* left channel volume input volume control
                               11111:+12dB,steps:1.5dB,00000:-34.5dB */
    uint16_t    rsvd: 2;    /*reserved */
    uint16_t    linmute: 1; /* left channel line input mute to ADC */
    uint16_t    lrinboth: 1; /* left to right channel line input volume and mute data load control*/
    uint16_t    resvd: 7;
};

union L_LINE_IN_REG
{
    uint16_t             all;
    struct L_LINE_IN_BIT bit;
};

/* right line in register */
struct  R_LINE_IN_BIT          // bits  description
{
    uint16_t    rinvol: 5;   /* right channel volume input volume control
                               11111:+12dB,steps:1.5dB,00000:-34.5dB */
    uint16_t    rsvd: 2;    /*reserved */
    uint16_t    rinmute: 1; /* right channel line input mute to ADC */
    uint16_t    rlinboth: 1; /* right to left channel line input volume and mute data load control*/
    uint16_t    resvd: 7;
};

union R_LINE_IN_REG
{
    uint16_t             all;
    struct R_LINE_IN_BIT bit;
};

/* left headphone out register */
struct  L_HEADPHONE_BIT          // bits  description
{
    uint16_t    lhpvol: 7;   /* left channel  headphone output volume control
                               1111111:+6dB,steps:1dB,0110000:-73dB */
    uint16_t    lzcen: 1;   /* left channel zero cross detect */
    uint16_t    lrhpboth: 1; /* left to right channel headphone volume,mute and zero cross data load control*/
    uint16_t    resvd: 7;
};

union L_HEADPHONE_REG
{
    uint16_t             all;
    struct L_HEADPHONE_BIT bit;
};

/* right headphone out register */
struct  R_HEADPHONE_BIT          // bits  description
{
    uint16_t    rhpvol: 7;   /* right channel  headphone output volume control
                               1111111:+6dB,steps:1dB,0110000:-73dB */
    uint16_t    rzcen: 1;   /* right channel zero cross detect */
    uint16_t    rlhpboth: 1; /* right to left channel headphone volume,mute and zero cross data load control*/
    uint16_t    resvd: 7;
};

union R_HEADPHONE_REG
{
    uint16_t             all;
    struct R_HEADPHONE_BIT bit;
};

/* analogue audio path control register */
struct  A_AUDIO_PATH_CTL_BIT          // bits  description
{
    uint16_t    micboost: 1; /* microphone input level boost */
    uint16_t    mutemic: 1; /* mic input mute to ADC */
    uint16_t    insel: 1;   /* microphone/line input select to ADC */
    uint16_t    bypass: 1;  /* bypass switch */
    uint16_t    dacsel: 1;  /* DAC select */
    uint16_t    sidetone: 1; /* side tone switch */
    uint16_t    sideatt: 2; /* side tone attenuation 00:-6dB | 01:09dB | 10:-12dB | 11:-15dB */
    uint16_t    resvd: 8;
};

union A_AUDIO_PATH_CTL_REG
{
    uint16_t             all;
    struct A_AUDIO_PATH_CTL_BIT bit;
};

/* digital audio path control register */
struct  D_AUDIO_PATH_CTL_BIT          // bits  description
{
    uint16_t    adchpd: 1; /* adc high pass filter enable */
    uint16_t    deemp: 2; /* de-enphasis control */
    uint16_t    dacmu: 1; /* DAC soft mute control */
    uint16_t    hpor: 1;  /*store dc offset when high pass filter */
    uint16_t    resvd: 11;
};

union D_AUDIO_PATH_CTL_REG
{
    uint16_t             all;
    struct D_AUDIO_PATH_CTL_BIT bit;
};

/* power down control register */
struct  POWER_DOWN_CTL_BIT          // bits  description
{
    uint16_t    lineinpd: 1; /* line input power down */
    uint16_t    micpd: 1;   /* microphone input an bias power down */
    uint16_t    adcpd: 1;   /* ADC power down */
    uint16_t    dacpd: 1;   /* DAC power down */
    uint16_t    outpd: 1;   /* output power down */
    uint16_t    oscpd: 1;   /* oscillator power down */
    uint16_t    clkoutpd: 1; /* clkout power down */
    uint16_t    poweroff: 1; /* poweroff mode */
    uint16_t    resvd: 8;
};

union POWER_DOWN_CTL_REG
{
    uint16_t             all;
    struct POWER_DOWN_CTL_BIT bit;
};

/* digital audio interface format register */
struct  D_AUDIO_INTERFACE_FMT_BIT          // bits  description
{
    uint16_t    format: 2;  /* audio data format select 00:MSB-Left | 01:MSB-Right | 10:I2S | 11:DSP */
    uint16_t    iwl: 2;     /* input audio data bit length select 00:16bits | 01:20bits | 10:24bits | 11:32bits */
    uint16_t    lrp: 1;     /* DACLRC phase control */
    uint16_t    lrswap: 1;  /* DAC left right clock swap */
    uint16_t    ms: 1;      /* master slave mode control */
    uint16_t    bclkinv: 1; /* bit clock invert */
    uint16_t    resvd: 8;
};

union D_AUDIO_INTERFACE_FMT_REG
{
    uint16_t             all;
    struct D_AUDIO_INTERFACE_FMT_BIT bit;
};

/* sample control register */
struct  SAMPLING_CTL_BIT          // bits  description
{
    uint16_t    usb_normal: 1; /* mode select */
    uint16_t    bosr: 1;    /* base over-sampling rate */
    uint16_t    sr: 4;      /* ADC and DAC sample rate control */
    uint16_t    clkidiv2: 1; /* core clock divider select */
    uint16_t    clkodiv2: 1; /* clkout divider select */
    uint16_t    resvd: 8;
};

union SAMPLING_CTL_REG
{
    uint16_t             all;
    struct SAMPLING_CTL_BIT bit;
};

/* active control register */
struct  ACTIVE_CTL_BIT          // bits  description
{
    uint16_t    active: 1; /* active interface */
    uint16_t    resvd: 15;
};

union ACTIVE_CTL_REG
{
    uint16_t             all;
    struct ACTIVE_CTL_BIT bit;
};

/* reset  register */
struct  RESET_BIT          // bits  description
{
    uint16_t    reset: 9; /* active interface */
    uint16_t    resvd: 7;
};

union RESET_REG
{
    uint16_t             all;
    struct RESET_BIT bit;
};

struct WM8731_REGS
{
    union L_LINE_IN_REG         l_line_in;
    union R_LINE_IN_REG         r_line_in;
    union L_HEADPHONE_REG       l_headphone;
    union R_HEADPHONE_REG       r_headphone;
    union A_AUDIO_PATH_CTL_REG  a_audio_path_ctl;
    union D_AUDIO_PATH_CTL_REG  d_audio_path_ctl;
    union POWER_DOWN_CTL_REG    power_down_ctl;
    union D_AUDIO_INTERFACE_FMT_REG d_audio_interface_fmt;
    union SAMPLING_CTL_REG      sampling_ctl;
    union ACTIVE_CTL_REG        active_ctl;
    union RESET_REG             reset;
};

/*wm8731 register list */
enum
{
    REG_L_LINE_IN           = 0x00,
    REG_R_LINE_IN           = 0x01,
    REG_L_HEADPHONE         = 0x02,
    REG_R_HEADPHONE         = 0x03,
    REG_A_AUDIO_PATH_CTL    = 0x04,
    REG_D_AUDIO_PATH_CTL    = 0x05,
    REG_POWER_DOWN_CTL      = 0x06,
    REG_D_AUDIO_INTERFACE_FMT   = 0x07,
    REG_SAMPLING_CTL        = 0x08,
    REG_ACTIVE_CTL          = 0x09,
    REG_RESET_REG           = 0x0f
};

/*L_LINE_IN register */

typedef enum
{
    WM8731_DIR_LEFT    = 0,
    WM8731_DIR_RIGHT   = 1
} direction_t;

/* wm8731 transfer format */
typedef enum
{
    WM8731_FMT_MSB_RIGHT   = 0x0000,
    WM8731_FMT_MSB_LEFT    = 0x0001,
    WM8731_FMT_I2S         = 0x0002,
    WM8731_FMT_DSP         = 0x0003
} audio_data_fmt_t;
/* wm8731 sample data bits */
typedef enum
{
    WM8731_BIT_16 = 0x0000,
    WM8731_BIT_20 = 0x0001,
    WM8731_BIT_24 = 0x0002,
    WM8731_BIT_32 = 0x0003
} audio_data_bits_t;

/* wm8731 sample mode */
typedef enum
{
    WM8731_MODE_NORMAL = 0,
    WM8731_MODE_USB    = 1
} sample_ctl_mode_t;
/* wm8731 mclk frq in usb_mode */
typedef enum
{
    WM8731_FRQ_250FS   = 0,
    WM8731_FRQ_272FS   = 1
} sample_ctl_bosr_usb_t;
/* wm8731 mclk frq in normal_mode */
typedef enum
{
    WM8731_FRQ_256FS   = 0,
    WM8731_FRQ_384FS   = 1
} sample_ctl_bosr_normal_t;

/* wm8731 sample rate of adc and dac,SAMPLE_RATE_ADC_DAC */
typedef enum
{
    WM8731_SAMPLE_RATE_48_48   = 0x0000,
    WM8731_SAMPLE_RATE_48_8    = 0x0001,
    WM8731_SAMPLE_RATE_8_48    = 0x0002,
    WM8731_SAMPLE_RATE_8_8     = 0x0003,
    WM8731_SAMPLE_RATE_32_32   = 0x0006,
    WM8731_SAMPLE_RATE_96_96   = 0x0007,
    WM8731_SAMPLE_RATE_44_44   = 0x0008,
    WM8731_SAMPLE_RATE_44_8    = 0x0009,
    WM8731_SAMPLE_RATE_8_44    = 0x000a,
    WM8731_SAMPLE_RATE_8p_8p   = 0x000b,
    WM8731_SAMPLE_RATE_88_88   = 0x000f
} sample_ctl_sample_rate_t;

/*
dir_high volume up
dir_low  volume down
dir_level volume value
dir_silence silence
*/
typedef enum
{
    WM8731_MODE_HIGH   = 0,
    WM8731_MODE_LOW    = 1,
    WM8731_MODE_LEVER  = 2,
    WM8731_MODE_SILENCE = 3
} volume_set_mode_t;

void wm8731_init(void);
void wm8731_set_headphone_volume(direction_t dir, volume_set_mode_t mode, uint8_t volume);
uint16_t wm8731_get_headphone_volume(direction_t dir);
void wm8731_set_line_in_volume(direction_t dir, volume_set_mode_t mode, uint8_t volume);

#endif  /* __WM8731_H__ */