#
# idt.s
#
#  Created on: Sept 24, 2015
#      Author: Bo Feng, Qingqing Cao
#

.text

######
# load a new IDT
#  parameter 1: address of idtr
.global _x86_64_asm_lidt
_x86_64_asm_lidt:

	lidt (%rdi)
	retq
