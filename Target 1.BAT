SET PATH=C:\Keil_v5\ARM\ARMCLANG\Bin;C:\WINDOWS\system32;C:\WINDOWS;C:\WINDOWS\System32\Wbem;C:\WINDOWS\System32\WindowsPowerShell\v1.0\;C:\WINDOWS\System32\OpenSSH\;C:\Program Files\Git\cmd;C:\Program Files (x86)\STMicroelectronics\STM32 ST-LINK Utility\ST-LINK Utility;C:\MinGW\bin;C:\Users\Kamil\AppData\Local\Microsoft\WindowsApps;C:\MinGW\bin;
SET CPU_TYPE=STM32F303VCTx
SET CPU_VENDOR=STMicroelectronics
SET UV2_TARGET=Target 1
SET CPU_CLOCK=0x03D09000
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @".\objects\main.__i"
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @".\objects\startup_stm32f303xc._ac"
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmClang.exe" @".\objects\system_stm32f3xx.__i"
"C:\Keil_v5\ARM\ARMCLANG\Bin\ArmLink" --Via ".\Objects\SwiecenieDioda.lnp"
"C:\Keil_v5\ARM\ARMCLANG\Bin\fromelf.exe" ".\Objects\SwiecenieDioda.axf" --i32combined --output ".\Objects\SwiecenieDioda.hex"
