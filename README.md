To build this project with PlatformIO you need to use "git clone --recurse-submodules"

## BlueSMiRF setup (master)
| Command          | Description                                               |
| ---------------- | ----------------------------------------------------      |
| SR,001122334455  | Set the remote slave MAC address                          |
| SN,<em>name</em> | Set name (optional)                                       |
| SP,<em>pin</em>  | Set PIN code (must match with slave)                      |
| SU,115200,N      | Set baud rate and disable parity (must match with slave)  |
| SL,N             | Only disable parity (must match with slave)               |
| SM,3             | Set mode to <em>master, auto connect</em>                 |
| SA,4             | Set authentication mode to <em>PIN code</em>              |
| R,1              | Reboot the device                                         |
