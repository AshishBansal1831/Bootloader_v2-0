# STM32F4 Custom C++ Bootloader & Host Updater

A robust, production-grade bare-metal C++ bootloader and host-side serial flashing utility designed for STM32F4 microcontrollers (ARM Cortex-M4 @ 168MHz). This project features a custom binary framing protocol, zero-copy buffer management, Intel HEX file parsing, and strict bare-metal C++ constraints.

---

## 🚀 Key Features

* **Strict Bare-Metal C++ Architecture:** Compiled with `-fno-exceptions`, `-fno-rtti`, and zero dynamic heap allocation (`new`/`delete`), ensuring deterministic real-time execution and zero memory fragmentation.
* **Custom Binary Protocol & Framing:** Implements reliable packet serialization, command routing (`CONFIRM_ERASE`, `WRITE_DATA`, `JUMP_TO_APP`), and CRC-32 (IEEE 802.3) integrity checks.
* **Dual-Format Host Parser:** Supports both raw binary files and Intel HEX files with Extended Linear Address (ELA, Record Type `04`) support for addressing beyond 64KB boundaries.
* **Safe Application Jump Sequence:** Validates stack pointer bounds in RAM and reset handler addresses in Flash, disables core interrupts, clears the NVIC and SysTick, updates `SCB->VTOR`, and executes an absolute assembly branch (`bx`) without returning.
* **Software-Triggered Bootloader Entry:** Utilizes RTC backup registers to pass a magic token from the running application, forcing a jump back to the bootloader via `NVIC_SystemReset()`.

---

## ⚙️ Memory Map & Linker Configuration

To accommodate the bootloader while leaving the rest of the Flash available for user applications, the memory layout is explicitly divided:
|Region | Memory Address Range	| Size	| Description |
|---|---|---|---|
|Bootloader	| 0x08000000 - 0x08007FFF	| 32 KB |	Sectors 0 & 1 (Bootloader firmware & vector table) |
|User Application	| 0x08008000 - 0x080FFFFF |	992 KB	| Sector 2 onwards (User application entry point) |

In the application's linker script (.ld), the flash origin must be offset accordingly:
```
MEMORY
{
  FLASH (rx)      : ORIGIN = 0x08008000, LENGTH = 992K
  RAM (xrw)       : ORIGIN = 0x20000000, LENGTH = 128K
}
```

