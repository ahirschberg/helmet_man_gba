#define REG_BASE 0x4000000
// 0300:7ff[y] is mirrored at 03ff:fff[y], which is why this works out:
#define REG_IFBIOS			*(vu16*)(REG_BASE-0x0008)	//!< IRQ ack for IntrWait functions
#define REG_RESET_DST		*(vu16*)(REG_BASE-0x0006)	//!< Destination for after SoftReset
#define REG_ISR_MAIN		*(fnptr*)(REG_BASE-0x0004)	//!< IRQ handler address

#define REG_DISPCTL         *(vu16*)(REG_BASE+0x0)
#define REG_DISPSTAT        *(vu16*)(REG_BASE+0x4)
#define SCANLINECOUNTER     *(vu16*)(REG_BASE+0x6)
#define REG_IE				*(vu16*)(REG_BASE+0x0200)	//!< IRQ enable
#define REG_IF				*(vu16*)(REG_BASE+0x0202)	//!< IRQ status/acknowledge
#define REG_WAITCNT			*(vu16*)(REG_BASE+0x0204)	//!< Waitstate control
#define REG_IME				*(vu16*)(REG_BASE+0x0208)	//!< IRQ master enable
#define REG_PAUSE			*(vu16*)(REG_BASE+0x0300)	//!< Pause system (?)

#define REG_DMA             ((volatile DMA_REC*)0x040000B0)
#define VIDEOBUFF           ((uint16_t*)0x6000000)
