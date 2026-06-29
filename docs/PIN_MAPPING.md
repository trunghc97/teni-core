# Pin mapping

| Function | ESP32-S3 GPIO | Peripheral pin | Notes |
|---|---:|---|---|
| I2S bit clock | 4 | MAX98357 BCLK | Digital audio |
| I2S word select | 5 | MAX98357 LRC | Digital audio |
| I2S data out | 6 | MAX98357 DIN | Digital audio |
| Future mic data | 7 | INMP441 SD | Not used in MVP1.1 |
| RGB status LED | 48 | onboard NeoPixel DIN | Board-dependent; verify revision |
| Battery ADC | 1 | divider midpoint | Never connect a cell directly |
| Charger detect | 2 | TP4056 status interface | Use transistor/level-safe circuit |
| Play button | 14 | switch to GND | Internal pull-up; wake source |
| Next button | 15 | switch to GND | Internal pull-up |
| Previous button | 16 | switch to GND | Internal pull-up |
| Mode button | 17 | switch to GND | Internal pull-up |
| Power hold | 18 | load-switch/latch enable | Optional hardware power latch |

Pins are centralized in `firmware/include/config/BoardConfig.hpp`. Validate strapping pins and the
exact DevKit revision before producing a PCB.

GPIO 10–13 were used by MVP1 MicroSD and are free in MVP1.1.
