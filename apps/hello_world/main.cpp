#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "hardware/clocks.h"
#include "hardware/irq.h"
#include "hardware/sync.h"
#include "hardware/gpio.h"
#include "hardware/vreg.h"
#include "hardware/structs/bus_ctrl.h"
#include "hardware/structs/ssi.h"
#include "hardware/dma.h"
#include "hardware/uart.h"
#include "pico/sem.h"

extern "C" {
#include "dvi.h"
#include "dvi_serialiser.h"
#ifdef DVI_CUSTOM_CONFIG
#include "custom_dvi_pin_configs.h"
#else
#include "common_dvi_pin_configs.h"
#endif
}

#include "bsp/board.h"
#include "tusb.h"
#include <pico/printf.h>
#include "PicoWinHidKeyboard.h"
#include "PicoDisplay.h"
#include "PicoPen.h"
#include "PicoCharRenderer.h"

#define UART_ID uart0
#define BAUD_RATE 115200

// We are using pins 0 and 1, but see the GPIO function select table in the
// datasheet for information on which other pins can be used.
#define UART_TX_PIN 0
#define UART_RX_PIN 1

// DVDD 1.2V (1.1V seems ok too)
#define VREG_VSEL VREG_VOLTAGE_1_20
#ifndef DVI_TIMING
#define DVI_TIMING dvi_timing_640x480p_60hz
#endif

#define LED_PIN 25

struct dvi_inst dvi0;
struct semaphore dvi_start_sem;

static volatile uint _frames = 0;

static PicoWin picoRootWin(
  0,
  0,
  PCS_COLS,
  PCS_ROWS
);

static PicoDisplay picoDisplay(
  pcw_screen(),
  &picoRootWin
);

static PicoWinHidKeyboard picoWinHidKeyboard(
  &picoDisplay
);

void print(hid_keyboard_report_t const *report) {
	printf("HID key report modifiers %2.2X report ", report->modifier);
	for(int i = 0; i < 6; ++i) printf("%2.2X", report->keycode[i]);
	printf("\n");
}

extern "C"  void __not_in_flash_func(process_kbd_mount)(uint8_t dev_addr, uint8_t instance) {
}

extern "C"  void __not_in_flash_func(process_kbd_unmount)(uint8_t dev_addr, uint8_t instance) {
}

extern "C"  void __not_in_flash_func(process_kbd_report)(hid_keyboard_report_t const *report, hid_keyboard_report_t const *prev_report) {
#if 1
  // Some help debugging keyboard input/drivers
	printf("PREV ");print(prev_report);
	printf("CURR ");print(report);
#endif
  picoWinHidKeyboard.processHidReport(report, prev_report);
}

void __not_in_flash_func(core1_render)() {
  uint y = 0;
  uint ys = 0;
  while(true) {
    uint rs = pcw_prepare_scanline_80(&dvi0, y++, ys, _frames);
    if (0 == (y & 7)) {
      ys += rs;
    }
    if (y == (PCS_ROWS * 8)) {
      y = 0;
      ys = 0;
    }
  }
}

void __not_in_flash_func(core1_main)() {
  
  dvi_register_irqs_this_core(&dvi0, DMA_IRQ_1);
  sem_acquire_blocking(&dvi_start_sem);
  dvi_start(&dvi0);
  
  core1_render();

  while (1) 
    __wfi();
  __builtin_unreachable();
}

void __not_in_flash_func(main_loop)()
{
  uint frames = 0;

  while (1)
  {
    tuh_task();
    if (frames != _frames)
    {
      frames = _frames;
      picoDisplay.refresh();
    }
  }
}

int main()
{
  vreg_set_voltage(VREG_VSEL);
  sleep_ms(10);

    // Run system at TMDS bit clock
  set_sys_clock_khz(DVI_TIMING.bit_clk_khz, true);

  setup_default_uart();
  tusb_init();

  gpio_init(LED_PIN);
  gpio_set_dir(LED_PIN, GPIO_OUT);
  
  // Initialise the menu renderer
  pcw_init_renderer();
  
  printf("Configuring DVI\n");
  dvi0.timing = &DVI_TIMING;
  dvi0.ser_cfg = DVI_DEFAULT_SERIAL_CONFIG;
  dvi_init(&dvi0, next_striped_spin_lock_num(), next_striped_spin_lock_num());

  printf("Core 1 start\n");
  sem_init(&dvi_start_sem, 0, 1);
  hw_set_bits(&bus_ctrl_hw->priority, BUSCTRL_BUS_PRIORITY_PROC1_BITS);
  
  multicore_launch_core1(core1_main);
  
  picoRootWin.onPaint([=](PicoPen *pen) {
    pen->printAtF(3, 1, false, "Hello world!");
  });
          
  picoDisplay.refresh();
  
  sem_release(&dvi_start_sem);
  
  main_loop();
  
  __builtin_unreachable();
}
