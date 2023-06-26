# 
# Usage: To re-create this platform project launch xsct with below options.
# xsct /home/jmakwana/workspace/one_wire_wrapper/platform.tcl
# 
# OR launch xsct and run below command.
# source /home/jmakwana/workspace/one_wire_wrapper/platform.tcl
# 
# To create the platform in a different location, modify the -out option of "platform create" command.
# -out option specifies the output directory of the platform project.

platform create -name {one_wire_wrapper}\
-hw {/home/jmakwana/onewire/one_wire_wrapper.xsa}\
-out {/home/jmakwana/workspace}

platform write
domain create -name {standalone_ps7_cortexa9_0} -display-name {standalone_ps7_cortexa9_0} -os {standalone} -proc {ps7_cortexa9_0} -runtime {cpp} -arch {32-bit} -support-app {empty_application}
platform generate -domains 
platform active {one_wire_wrapper}
domain active {zynq_fsbl}
domain active {standalone_ps7_cortexa9_0}
platform generate -quick
platform generate
platform active {one_wire_wrapper}
platform config -updatehw {/home/jmakwana/onewire/one_wire_wrapper.xsa}
platform generate -domains 
platform config -updatehw {/home/jmakwana/onewire/one_wire_wrapper.xsa}
platform generate -domains 
platform config -updatehw {/home/jmakwana/onewire/one_wire_wrapper.xsa}
platform generate -domains 
platform generate -domains standalone_ps7_cortexa9_0 
platform generate -domains standalone_ps7_cortexa9_0 
