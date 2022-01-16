.data
    n: .word 7
    
.text
.globl __start

    # Todo: Define your own function in HW1
  FUNCTION:
    # store return address and n to stack
      addi sp, sp, -8
      sw x1, 4(sp)
      sw a0, 0(sp)
    
    # if n != 1
      addi a1, x0, 1
      bne a0, a1, L1
    
    # if n == 1
      addi t4, x0, 7  # return 7
      addi sp, sp, 8  # pop stack
      jalr x0, 0(x1)  # return to calle
    
  L1:
    # get n/2
      addi t1, x0, 2
      div a0, a0, t1
    
    # call T(n/2)
      jal ra, FUNCTION
    
      lw x1, 4(sp)  # restore return address
      lw a0, 0(sp)  # restore n
      addi sp, sp, 8  # pop stack
      
      addi t2, x0, 4
      mul t2, t2, a0  # 4n
      
      addi t3, x0, 8
      mul t3, t3, t4  # 8(T(n/2))
      
      add t4, t2, t3  # save 8(T(n/2)) + 4n to t0
      
      add a0, t4, x0
      jalr x0, 0(x1)

# Do NOT modify this part!!!
__start:
    la   t0, n
    lw   x10, 0(t0)
    jal  x1,FUNCTION
    la   t0, n
    sw   x10, 4(t0)
    addi a0,x0,10
    ecall