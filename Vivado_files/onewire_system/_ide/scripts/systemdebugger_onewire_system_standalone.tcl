# Usage with Vitis IDE:
# In Vitis IDE create a Single Application Debug launch configuration,
# change the debug type to 'Attach to running target' and provide this 
# tcl script in 'Execute Script' option.
# Path of this script: /home/jmakwana/workspace/onewire_system/_ide/scripts/systemdebugger_onewire_system_standalone.tcl
# 
# 
# Usage with xsct:
# To debug using xsct, launch xsct and run below command
# source /home/jmakwana/workspace/onewire_system/_ide/scripts/systemdebugger_onewire_system_standalone.tcl
# 
connect -url tcp:127.0.0.1:3121
targets -set -nocase -filter {name =~"APU*"}
rst -system
after 3000
targets -set -filter {jtag_cable_name =~ "Digilent Zybo Z7 210351B481FAA" && level==0 && jtag_device_ctx=="jsn-Zybo Z7-210351B481FAA-23727093-0"}
fpga -file /home/jmakwana/workspace/onewire/_ide/bitstream/one_wire_wrapper.bit
targets -set -nocase -filter {name =~"APU*"}
loadhw -hw /home/jmakwana/workspace/one_wire_wrapper/export/one_wire_wrapper/hw/one_wire_wrapper.xsa -mem-ranges [list {0x40000000 0xbfffffff}] -regs
configparams force-mem-access 1
targets -set -nocase -filter {name =~"APU*"}
source /home/jmakwana/workspace/onewire/_ide/psinit/ps7_init.tcl
ps7_init
ps7_post_config
targets -set -nocase -filter {name =~ "*A9*#0"}
dow /home/jmakwana/workspace/onewire/Debug/onewire.elf
configparams force-mem-access 0
targets -set -nocase -filter {name =~ "*A9*#0"}
con
