# [ OpenPara ]
# Copyright Regan "CKDEV" Green 2022
# 
# - Mesh.cpp -
# Mesh drawing

.set noreorder

# void OpenPara_Mesh_MIMe_asm(::SVECTOR *out, const ::SVECTOR *in, uint32_t n_vert, int32_t weight);
#
# a0 = out
# a1 = in
# a2 = n_vert
# a3 = weight
#

.section .text.OpenPara_Mesh_MIMe_asm
.global OpenPara_Mesh_MIMe_asm
.type OpenPara_Mesh_MIMe_asm, @function
OpenPara_Mesh_MIMe_asm:
	mtc2  $a3, $8

.Ltrans_loop:
	lw    $t0, 0($a1)
	lw    $t2, 4($a1)
	srl   $t1, $t0, 16
	andi  $t0, 0xFFFF
	
	mtc2  $t0, $9
	mtc2  $t1, $10
	mtc2  $t2, $11
	
	lw    $t0, 0($a0)
	lw    $t2, 4($a0)
	srl   $t1, $t0, 16
	andi  $t0, 0xFFFF
	
	mtc2  $t0, $25
	mtc2  $t1, $26
	mtc2  $t2, $27
	
	addiu $a1, 8
	addiu $a2, -1

	cop2  0x01A8003E  # gte_gpl12
	
	mfc2  $t0, $25
	mfc2  $t1, $26

	andi  $t0, $t0, 0xFFFF
	sll   $t1, $t1, 16
	or    $t0, $t1
	
	mfc2  $t2, $27
	sw    $t0, 0($a0)
	sw    $t2, 4($a0)
	
	bnez  $a2, .Ltrans_loop
	addiu $a0, 8

	jr    $ra
	nop
	
