.globl __start

.rodata
    msg0: .string "This is HW1-2: \n"
    msg1: .string "Plaintext:  "
    msg2: .string "Ciphertext: "
.text

################################################################################
  # print_char function
  # Usage: 
  #     1. Store the beginning address in x20
  #     2. Use "j print_char"
  #     The function will print the string stored from x20 
  #     When finish, the whole program with return value 0

print_char:
    addi a0, x0, 4
    la a1, msg2
    ecall
    
    add a1,x0,x20
    ecall

  # Ends the program with status code 0
    addi a0,x0,10
    ecall
    
################################################################################

__start:
  # Prints msg
    addi a0, x0, 4
    la a1, msg0
    ecall

    la a1, msg1
    ecall
    
    addi a0,x0,8
    li a1, 0x10130
    addi a2,x0,2047
    ecall
    
  # Load address of the input string into a0
    add a0,x0,a1

################################################################################ 
  # Write your main function here. 
  # a0 stores the begining Plaintext
  # Do store 66048(0x10200) into x20 
  # ex. j print_char

caesar:
    li x20, 0x10200  # starting point to store output string
    addi x18, x0, -1 # initialize space incrementation=-1
    add x19, x0, x0  # initialize i=0

L1:
    add x5, a0, x19  # address of text[i] in x5
    lbu x6, 0(x5)    # load text[i] into x6
  
  # if char = '\n', terminate loop
    addi x16, x0, 10
    beq x6, x16, terminate

  # if text[i] = space
    addi x17, x0, 32
    beq x6, x17, space
  
  # if text[i] = alphabet
    addi x6, x6, -97
    addi x6, x6, 3    # shift by 3
  
    # if incrementation < 0
    blt x18, x0, no_incrementation
  
    # if incrementation >= 0
    add x6, x6, x18   # shift by incrementation
  
    addi x7, x0, 26
    rem x6, x6, x7
    addi x6, x6, 97
  
    jal x0, L2

space:
    addi x18, x18, 1  # shift increment by 1
    addi x6, x18, 48  # store incrementation (int to char)

    jal x0, L2

no_incrementation:
    addi x7, x0, 26
    rem x6, x6, x7
    addi x6, x6, 97
  
    jal x0, L2

L2:
    sb x6, 0(x20)     # save cipher
    addi x20, x20, 1
  
    addi x19, x19, 1  # increment i
    jal x0, L1        # keep looping

terminate:
    li x20, 0x10200
    jal x0, print_char
  
################################################################################

