// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/04/Mult.asm

// Multiplies R0 and R1 and stores the result in R2.
// (R0, R1, R2 refer to RAM[0], RAM[1], and RAM[2], respectively.)
//
// This program only needs to handle arguments that satisfy
// R0 >= 0, R1 >= 0, and R0*R1 < 32768.

// Put your code here.

// ------------------------
// Pseudocode: Mult(R0, R1)
//  res := 0, i := 0
//  while (i < R0) do
//      res := res + R1
//      i := i + 1
//  end do
//  return res
// ------------------------

@res
M = 0 // res = 0

@i
M = 0 // i = 0

(LOOP)
	@i
	D = M // D = i
	@R0
	D = M - D // D = i - R0
	@END
	D; JEQ // if i = R0 goto END
        
  // If not, this mean i < R0
	@R1
	D = M // D = R1
	@res
	M = D + M // res := res + R1

	@i
	M = M + 1 // i := i + 1

	@LOOP
	0; JMP // goto LOOP

(END)
@res
D = M // D = res
@R2
M = D // R2 = res

(INFINITE_LOOP)
@INFINITE_LOOP
0; JMP // return R2 = res
