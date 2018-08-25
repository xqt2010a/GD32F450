#ifndef __JX_DMA_H__
#define __JX_DMA_H__

#define DMA_BASE        0x0190E000
#define DMA0_BASE       (DMA_BASE)
#define DMA1_BASE       (DMA_BASE + 0x1000)

#define DMA_ID(x)                   (DMA_BASE + 0x1000*(x) + 0x000)
#define DMA_COMPVER(x)              (DMA_BASE + 0x1000*(x) + 0x008)
#define DMA_CFG(x)                  (DMA_BASE + 0x1000*(x) + 0x010)
#define DMA_CHEN(x)                 (DMA_BASE + 0x1000*(x) + 0x018)
#define DMA_INTSTATUS(x)            (DMA_BASE + 0x1000*(x) + 0x030)
#define DMA_COM_INTCLR(x)           (DMA_BASE + 0x1000*(x) + 0x038)
#define DMA_COM_INTSTATUS_EN(x)     (DMA_BASE + 0x1000*(x) + 0x040)
#define DMA_COM_INTSIGNAL_EN(x)     (DMA_BASE + 0x1000*(x) + 0x048)
#define DMA_COM_INTSTATUS(x)        (DMA_BASE + 0x1000*(x) + 0x050)
#define DMA_RST(x)                  (DMA_BASE + 0x1000*(x) + 0x058)
#define DMA_CHy_SAR(x,y)            (DMA_BASE + 0x1000*(x) + 0x100*(y) + 0x00) // y = 1 to 8
#define DMA_CHy_DAR(x,y)            (DMA_BASE + 0x1000*(x) + 0x100*(y) + 0x08)
#define DMA_CHy_BLK_TS(x,y)         (DMA_BASE + 0x1000*(x) + 0x100*(y) + 0x10)
#define DMA_CHy_CTL(x,y)            (DMA_BASE + 0x1000*(x) + 0x100*(y) + 0x18)
#define DMA_CHy_CFG(x,y)            (DMA_BASE + 0x1000*(x) + 0x100*(y) + 0x20)
#define DMA_CHy_LLP(x,y)            (DMA_BASE + 0x1000*(x) + 0x100*(y) + 0x28)
#define DMA_CHy_STATUS(x,y)         (DMA_BASE + 0x1000*(x) + 0x100*(y) + 0x30)
#define DMA_CHy_SWHSSRC(x,y)        (DMA_BASE + 0x1000*(x) + 0x100*(y) + 0x38)
#define DMA_CHy_SWHSDST(x,y)        (DMA_BASE + 0x1000*(x) + 0x100*(y) + 0x40)
#define DMA_CHy_BLK_TFR(x,y)        (DMA_BASE + 0x1000*(x) + 0x100*(y) + 0x48)
#define DMA_CHy_AXI_ID(x,y)         (DMA_BASE + 0x1000*(x) + 0x100*(y) + 0x50)
#define DMA_CHy_QOS(x,y)            (DMA_BASE + 0x1000*(x) + 0x100*(y) + 0x58)
#define DMA_CHy_SSTAT(x,y)          (DMA_BASE + 0x1000*(x) + 0x100*(y) + 0x60)
#define DMA_CHy_DSTAT(x,y)          (DMA_BASE + 0x1000*(x) + 0x100*(y) + 0x68)
#define DMA_CHy_SSTATAR(x,y)        (DMA_BASE + 0x1000*(x) + 0x100*(y) + 0x70)
#define DMA_CHy_DSTATAR(x,y)        (DMA_BASE + 0x1000*(x) + 0x100*(y) + 0x78)
#define DMA_CHy_INTSTATUS_EN(x,y)   (DMA_BASE + 0x1000*(x) + 0x100*(y) + 0x80)
#define DMA_CHy_INTSTATUS(x,y)      (DMA_BASE + 0x1000*(x) + 0x100*(y) + 0x88)
#define DMA_CHy_INTSIGNAL_EN(x,y)   (DMA_BASE + 0x1000*(x) + 0x100*(y) + 0x90)
#define DMA_CHy_INTCLR(x,y)         (DMA_BASE + 0x1000*(x) + 0x100*(y) + 0x98)


#endif  /* __JX_DMA_H__ */