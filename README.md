# res
## Project Goal
- Implement functionality similar to [canon_mc-g02_resetter](https://github.com/wangyu-/canon_mc-g02_resetter) for STM32F103C8T6 (Blue Pill).
- Provide clear visual feedback via LED with distinct state transitions.

## Core Hardware Configuration

### MCU Setup
- STM32F103C8T6 (Blue Pill)
- System clock: 72 MHz using HSE (8 MHz crystal)
- APB1: 36 MHz (Timer clocks at 72 MHz)

### Peripheral Pins
- USART2: PA2 (TX), PA3 (RX) at 115200 baud
- I2C2: PB10 (SCL), PB11 (SDA) at 100 kHz
- LED: PC13 (Active LOW - lights when pin LOW)
- Debug: USART2 via USB-TTL converter

## Timing System
- TIM2 as master timer (10 µs resolution)
- Prescaler: 71 (72 MHz/(71+1) = 1 MHz)
- Auto-reload: 9 (1 MHz/(9+1) = 100 kHz)
- Interrupt every 10 µs
- I2C clock: 100 kHz, rise time <1 µs
- All delays (write, verification, initialization, retries) derived from TIM2, adjustable for MC-G02 timing

## I2C EEPROM Handling
- Device address: 0x50 (7-bit)
- Page size: 16 bytes
- Total size: 2048 bytes (128 pages)
- Write cycle time: 10–15 ms per page ( 16 bytes), including ~1 ms transfer and ~3–4 ms polling (WRITE_CYCLE_TIME_MS=15 ms)
- Post-initialization delay: 20–50 ms after dummy write to stabilize EEPROM
- Retry delay: 50 ms per retry, 20 attempts for I2C read/write failures
- Hardcoded 2048-byte data array in `i2c_handler.c` starting with `{0x2B, 0x3D, 0x50, ..., 0x00}`

## LED Operation Modes

### Idle Mode
- Pattern: 100 ms ON, 100 ms OFF
- Active until chip detected (I2C acknowledgment at 0x50)
- `LED_Update` must be called frequently in `Application_Loop` to ensure toggling every 100 ms

### Transition to Writing Mode
- LED: OFF for 2000 ms upon chip detection
- Followed by I2C post-initialization delay (20–50 ms)

### Writing Mode
- Solid ON during write operations
- Maximum duration: 20 seconds
- Post-writing: LED OFF for 2000 ms before entering Success or Error Mode

### Success Mode
- Pattern: 1900 ms ON, 100 ms OFF
- Continues until chip removed, then returns to Idle
- Indicates successful programming

### Error Modes
- All errors: 500 ms ON/OFF blinking
  - Chip Damaged: 2 blinks + 3000 ms pause, repeat
  - Chip Removed During Process: 3 blinks + 3000 ms pause, repeat
- After error pattern: 2000 ms OFF before returning to Idle

## Watchdog System
- Independent Watchdog (IWDG, LSI 40 kHz)
- Timeout: (256 * 4095) / 40 kHz ≈ 26.2 seconds
- Prescaler: 256
- Reload value: 4095
- Refresh period: Every 20 seconds

## Debug System
- USART2 initialized at 115200 baud in `MX_USART2_UART_Init`, called early in `main`.
- Debug messages output via `DEBUG_PRINT` to USART2 for PuTTY logging.
- Log format: `[MODULE] State: Action (Time: Xms, Delta: Yms)`
  - `MODULE`: Module name (e.g., I2C, LED, Error).
  - `State`: Current state (e.g., Idle, Writing, Success, Error).
  - `Action`: Event/action (e.g., Starting, Complete, Failed).
  - `Time`: Timestamp in ms (from `HAL_GetTick()`).
  - `Delta`: Time difference in ms since last debug message.
- `DEBUG_PRINT` in `usart_debug.c` appends time/delta to formatted messages.
- `USART_Debug_Init` logs start/complete with time/delta.
- Functions logging entry/exit in specified format:
  - `main`, `SystemClock_Config`, `MX_GPIO_Init`, `MX_I2C2_Init`, `MX_TIM2_Init`, `MX_USART2_UART_Init`, `MX_IWDG_Init`
  - `Application_Init`, `Application_Loop`, `Error_Handler`, `Set_Error_Code`, `Get_Error_Code`, `Handle_Error`
  - `I2C_Initialize_EEPROM`, `I2C_Handler_Init`, `I2C_Poll_Chip`
  - `LED_Init`, `LED_On`, `LED_Off`, `LED_Toggle`, `LED_Set_State`, `LED_Get_State`, `LED_Update`
  - `USART_Debug_Init`, `DEBUG_PRINT`

## Code Change Tracking
- Track changes:
  - Additions: `// **A: Add**`
  - Removals: `// **A: Remove**`
  - Subsequent edits: `// **B: Add**`, `// **C: Add**`, etc.

## Modular File Layout
- `config.h`: Defines I2C address (0x50), page size (16), ROM size (2048), write cycle (15 ms)
- `i2c_handler.h/c`: Manages I2C2 read/write, includes 2048-byte hardcoded data array `{0x2B, 0x3D, 0x50, ..., 0x00}`
- `led_status.h/c`: Controls LED (PC13) behavior
- `usart_debug.h/c`: Implements `USART_Debug_Init`, `DEBUG_PRINT` for PuTTY logs
- `error_handler.h/c`: Handles error detection and verification logic

## Development Environment
- IDE: STM32CubeIDE
- Firmware: STM32Cube FW_F1 V1.8.6
- Compiler: ARM GCC
- Programmer: ST-Link or UART bootloader via FTDI232 for flashing; FTDI232 for debug

## Code Structure Notes
- User-defined functions in `main.c` declared static, excluding HAL-generated functions
- `MX_GPIO_Init` must initialize PC13 as output with initial state HIGH (OFF for active LOW)
- `Application_Loop` must call `LED_Update` in every iteration without blocking delays to ensure timely LED toggling
