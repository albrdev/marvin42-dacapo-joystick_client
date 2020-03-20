To build this project with PlatformIO you need to use "git clone --recurse-submodules"

BlueSMiRF setup (master):
SN,name             # Set name
SP,pin              # Set PIN
SM,3                # Set mode to master, auto connect
SR,001122334455     # Set remote slave address
SU,115200,N         # Optional: Set baud rate and parity
SL,N                # Optional: Set only parity
SA,4                # Set authentication mode to PIN code
R,1                 # Reboot
