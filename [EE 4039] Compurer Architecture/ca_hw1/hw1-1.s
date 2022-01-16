.globl __start

.rodata
    msg0: .string "This is HW1-1: T(n) = 8T(n/2) + 4n, T(1) = 7\n"
    msg1: .string "Enter a number: "
    msg2: .string "The result is: "

.text


__start:
  # Prints msg0
    addi a0, x0, 4
    la a1, msg0
    ecall

  # Prints msg1
    addi a0, x0, 4
    la a1, msg1
    ecall

  # Reads an int
    addi a0, x0, 5
    ecall

################################################################################ 
  # Write your main function here. 
  # Input n is in a0. You should store the result T(n) into t0
  # HW1-1 T(n) = 8T(n/2) + 4n, T(1) = 7, round down the result of division
  # ex. addi t0, a0, 1
  T:
    # store return address and n to stack
      addi sp, sp, -8
      sw x1, 4(sp)
      sw a0, 0(sp)
    
    # if n != 1
      addi a1, x0, 1
      bne a0, a1, L1
    
    # if n == 1
      addi t0, x0, 7  # return 7
      addi sp, sp, 8  # pop stack
      jalr x0, 0(x1)  # return to calle
    
  L1:
    # get n/2
      addi t1, x0, 2
      div a0, a0, t1
    
    # call T(n/2)
      jal ra, T
    
      lw x1, 4(sp)  # restore return address
      lw a0, 0(sp)  # restore n
      addi sp, sp, 8  # pop stack
      
      addi t2, x0, 4
      mul t2, t2, a0  # 4n
      
      addi t3, x0, 8
      mul t3, t3, t0  # 8(T(n/2))
      
      add t0, t2, t3  # save 8(T(n/2)) + 4n to t0
      
      beq x1, x0, result
      jalr x0, 0(x1)

################################################################################

result:
  # Prints msg2
    addi a0, x0, 4
    la a1, msg2
    ecall

  # Prints the result in t0
    addi a0, x0, 1
    add a1, x0, t0
    ecall
    
  # Ends the program with status code 0
    addi a0, x0, 10
    ecall