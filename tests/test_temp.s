# nekjsnkldan 
.global nesto, nesto2
.section sekira
.word 1, 2
ja:
.extern nesto1, nesto3, nesto4
.section neka_sekcija
.word 0x12345, nesto, 0, nesto2, nesdsadto_dsad
.skip 0x4
int 
call 1000
call nesto
nesdsadto_dsad: jmp 0x1000
beq %sp, %r2, nesto
bgt %pc, %r2, nesto2
bne %sp, %pc, 2000
push %r1
pop %r2
xchg %r1, %sp
add %r1, %sp
sub %r1, %sp
mul %r1, %sp
div %r1, %sp
not %r1
and %r1, %sp
or %r1, %sp
xor %r1, %sp
shl %r1, %sp
shr %r1, %sp
ld  0x1000, %r10
ld  1000, %r1
ld  2000, %r1
ld  nesto2, %r2
ld  $0x1000, %r3
ld  $nesto, %r4
ld  %r5, %r5
ld  [ %r2 ], %r6
ld  [ %r10 + nesto2 ], %r7
ld  [ %r3 + 500 ], %r8
st %r10, 0x1000
st %r1, 1000
st %r1, 2000
st %r2, nesto2
st %r3, $0x1000
st %r4, $nesto
st %r5, %r5
st %r6, [ %r2 ]
st %r7, [ %r10 + nesto2 ]
st %r8, [ %r3 + 500 ]
nesto: csrrd %handler, %r3  
nesto2:
csrwr %sp, %status  
# komentar
# dsa das
ret
iret
halt
# nesto
.end # dsa dsa das
# jdsai dopsa