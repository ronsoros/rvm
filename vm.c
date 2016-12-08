// this is a probably not-so-good vm and bytecode implementation

#include <inttypes.h>
#include <stdio.h>
#define LITTLE_ENDIAN
#ifdef LITTLE_ENDIAN
#define SWAP_UINT16(x) (((x) >> 8) | ((x) << 8))
#else
#define SWAP_UINT16(x) x
#endif
#define auint(q) SWAP_UINT16(*(uint16_t*)&q)
#define readbyte(a) data[a]
#define writebyte(a,b) data[a] = b
#define jmp(a) ip = a;
#define mkw(a,b) ((a*256)+b)
#define io_out(p, d) putchar(d)
#define io_in(p) getchar()
uint8_t data[4096];
uint16_t regs[16];
uint16_t ip;
uint16_t lip[16];
uint8_t lipp;
#define incip ip+=4;
typedef enum { MPUT = 1, MGET = 2 , MOV = 3 , JMP = 4, OUT = 5, ADD = 6, RET = 7, CALL = 8, JEQ = 9, JNE = 10, LDW = 11, STW = 12, TIMER = 13, JGT = 14, JLT = 15, MUL = 16, SUB = 17, DIV = 18, IN = 19 } opcodes;
typedef struct {
	uint8_t op;
	uint8_t p1;
	uint8_t p2;
	uint8_t p3;
} opcode;
uint16_t timer = 0;
uint8_t cycles = 0;
void run() {
	uint8_t tjmp[4];
	tjmp[0] = CALL;
	ip = 0;
	lipp = 0;
	opcode* nexop = NULL;
	while ( 1 ) {
		cycles++;
		if ( cycles == 0 && timer != 0 ) {
			tjmp[1] = ((uint8_t*)&timer)[1];
tjmp[2] = ((uint8_t*)&timer)[0];
			//fprintf(stderr, "TIMER: going to 0x%04x\n", timer);
			nexop = tjmp;
			ip = ip - 4;
		} else {
		//fprintf(stderr,"RAW: %x %x %x %x\n", data[ip], data[ip+1], data[ip+2], data[ip+3]);
		nexop = &data[ip];
		
		}
//fprintf(stderr,"IP: 0x%04x %d - OPCODE: %d, params: %x %x %x\n", ip, ip, nexop->op, nexop->p1, nexop->p2, nexop->p3);
		switch(nexop->op) {
			case 0: exit(0); break;
			case MPUT: incip
				uint16_t qq = auint(nexop->p1);
				writebyte(regs[qq],regs[nexop->p3]);
				break;
			case MGET: incip
//				printf("%d %d", ip, auint(nexop->p1));
				uint16_t q = auint(nexop->p1);
				//printf("%d", q);
				regs[nexop->p3] = readbyte(regs[q]);
				break;
			case STW: incip
				uint16_t qx = auint(nexop->p1);
				writebyte(regs[qx],regs[nexop->p3]);
				writebyte(regs[qx+1],regs[nexop->p3]<<8);
				break;
			case LDW: incip
//				printf("%d %d", ip, auint(nexop->p1));
				uint16_t qy = auint(nexop->p1);
				//printf("%d", q);
				regs[nexop->p3] = (readbyte(regs[qy]) | readbyte(regs[qy+1]) >> 8);
				break;
			case MOV: incip
				regs[nexop->p3] = auint(nexop->p1); break;
			case JMP:
				ip = auint(nexop->p1); break;
			case OUT: incip
				io_out(nexop->p1, regs[nexop->p2]); break;
			case IN:
				incip
				regs[nexop->p2] = io_in(nexop->p1); break;
			#define EMIT_MATH(a, o) case a: incip \
				regs[nexop->p3] = regs[nexop->p1] o regs[nexop->p2]; break;
			EMIT_MATH(MUL, *)
			EMIT_MATH(SUB, -)
			EMIT_MATH(DIV, /)	
			case ADD: incip
				regs[nexop->p3] = regs[nexop->p1] + regs[nexop->p2]; 
				//fprintf(stderr, "now: %d\n", regs[nexop->p3]);
				break;
			#define EMIT_JMP(a, o) case a: incip \
				if ( regs[nexop->p1] o regs[nexop->p2] ) ip = regs[nexop->p3]; break;
			EMIT_JMP(JLT, <)
			EMIT_JMP(JGT, >)
			case TIMER: incip
				timer = auint(nexop->p1); break;
			case JEQ: incip
				if ( regs[nexop->p1] == regs[nexop->p2] ) ip = regs[nexop->p3]; break;
			case JNE:
				  incip
				if ( regs[nexop->p1] != regs[nexop->p2] ) ip = regs[nexop->p3]; break;
			case CALL:
				lip[lipp] = ip+4;
				ip = auint(nexop->p1);
				lipp++;
				break;
			case RET:
				lipp--;
				ip = lip[lipp];
				break;
				
		}
	}
					
}

int main(int argc, char **argv) {
	//assert(argc > 1);
	FILE *fp = fopen(argv[1], "r");
	//assert(fp != NULL);
	fread(data, 4096, 1, fp);
	fclose(fp);
	run();
}