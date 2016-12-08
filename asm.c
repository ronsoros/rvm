#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#define LITTLE_ENDIAN
#ifdef LITTLE_ENDIAN
#define SWAP_UINT16(x) (((x) >> 8) | ((x) << 8))
#else
#define SWAP_UINT16(x) x
#endif
int firstpass = 0;
uint16_t getsymbol(char *name) ;
#define emit_pass if(firstpass != 1)
char pp;
char symbname[128];
uint16_t oip = 0;
typedef struct {
	char *name;
	uint16_t ptr;
} symbol;
symbol symbols[256];
int nsymbols = 0;
uint16_t ips = 0;
uint16_t ip = 0;
int pqx = 0; 
FILE *in;
#define addsymbol(n, p) symbols[nsymbols].name=n;symbols[nsymbols].ptr=p;nsymbols++
#define emit_inline(o) oip=ip;ip+=4+strlen(symbname)+1;emit_pass { fputc(4, stdout); }; ips = SWAP_UINT16(ip); \
		emit_pass { fwrite(&ips, 2, 1, stdout); fputc(0, stdout); \
		fwrite(symbname, strlen(symbname), 1, stdout); fputc(0, stdout); }\
		o=oip+4; emit_pass { addsymbol("~inline_begin", oip+4); addsymbol("~inline_end", ip); }
#define getpar(o) pp=fgetc(in);if(pp=='#'){fscanf(in, "%d ",&o);}else if(pp=='@'){fscanf(in, "%s ", symbname);o=getsymbol(symbname);}else if(pp=='"'){fscanf(in, "%[^\"]\" ", symbname);emit_inline(o);}else if(pp=='*'){emit_reg_inline(&o);}else{fscanf(in,"%x ",&o);}
int emitted_reg_add = 0;
int emit_reg_inline(int* o) {
	int tmpo = 0;
	getpar(tmpo);
	uint16_t ras = tmpo;
	emit_pass { fputc(3, stdout); }
	uint16_t lol = SWAP_UINT16(ras);
	emit_pass { fwrite(&lol, 2, 1, stdout); }
	emit_pass { fputc(14 + emitted_reg_add, stdout); }
	ip+=4;
	*o = 14 + emitted_reg_add;
	emitted_reg_add += 1;
}
typedef struct {
	char *name;
	uint8_t opcode;
	uint8_t tb1;
	uint8_t tb2;
	int operands;
} optable;
optable ops[] = {
	{ "mov", 3, 1, 0, 2 },
	{ "out", 5, 0, 0, 2 },
	{ "stb", 1, 1, 0, 2 },
	{ "ldb", 2, 1, 0, 2 },
	{ "ldw", 11, 1, 0, 2 },
	{ "stw", 12, 1, 0, 2 },
	{ "call", 8, 1, 0, 1 },
	{ "jmp", 4, 1, 0, 1 },
	{ "ret", 7, 0, 0, 0 },
	{ "jeq", 9, 0, 0, 3 },
	{ "jne", 10, 0, 0, 3 },
	{ "jgt", 14, 0, 0, 3 },
	{ "jlt", 15, 0, 0, 3 },
	{ "add", 6, 0, 0, 3 },
	{ "mul", 16, 0, 0, 3 },
	{ "sub", 17, 0, 0, 3 },
	{ "div", 18, 0, 0, 3 },
	{ "timer", 13, 1, 0, 1 },
	{ "in", 19, 0, 0, 1 }
};


uint16_t getsymbol(char *name) {
	for ( int s = 0; s < nsymbols; s++ ) {
		if ( !strcmp(name, symbols[s].name) ) {
			return symbols[s].ptr;
		}
	}
	emit_pass {
	fprintf(stderr, "Warning: symbol not found: %s\n", name);
	}
	return 0;
}


int tops = sizeof(ops)/sizeof(optable);



void preproc(int firstpassx, char *infile) {
	in = fopen(infile, "r");
	firstpass = firstpassx;
	ip = 0;
	char op[8];
	uint8_t outbuf[4];
	int p1, p2, p3;
	while (!feof(in)) {
		emitted_reg_add = 0;
		p1 = 0; p2 = 0; p3 = 0;
		fscanf(in, "%s ", op);
		if ( !strcmp(op, ".base" ) ) {
			fscanf(in, "%d ", &ip);
		}
		if ( !strcmp(op, ".db" ) ) {
			getpar(p1);
			emit_pass {
			uint8_t ob = p1;
			fwrite(&p1, 1, 1, stdout);
			}
			ip+=1;
		}
		if ( !strcmp(op, ".label") && firstpass == 1 ) {
			char symbname[512];
			fscanf(in, "%s ", symbname);
			addsymbol(strdup(symbname), ip);
		}
		if ( !strcmp(op, ".dw" ) ) {
			getpar(p1);
			emit_pass {
			uint16_t ow = p1;
			ow = SWAP_UINT16(ow);
			fwrite(&ow, 2, 1, stdout);
			}
			ip+=2;
		}
		if ( !strcmp(op, ".ds" ) ) {
			char tmpbuf[4096];
			fscanf(in, "%[^\n]", tmpbuf);
			emit_pass { fwrite(tmpbuf, strlen(tmpbuf), 1, stdout); }
			ip+=strlen(tmpbuf);
		}
	if ( !strcmp(op, ".dsz" ) ) {
			char tmpbuf[4096];
			fscanf(in, "%[^\n]", tmpbuf);
			emit_pass { fwrite(tmpbuf, strlen(tmpbuf)+1, 1, stdout); }
			ip+=strlen(tmpbuf)+1;
		}
		if ( !strcmp(op, ".#" ) ) {
			char blah[256];
			fscanf(in, "%[^\n]", blah);
		}
		if ( op[0] != '.' ) {
		
		//fprintf(stderr, "OPS: %d %d %d", p1, p2, p3);
		for ( int q = 0; q < tops; q++ ) {
			if (!strcmp(op, ops[q].name) ) {
				outbuf[0] = ops[q].opcode;
				outbuf[1] = 0; outbuf[2] = 0; outbuf[3] = 0;
				if ( ops[q].operands > 0 ) {
				getpar(p1);
				if ( ops[q].tb1 == 0 ) {
				
				outbuf[1] = p1;
				} else {
				*(uint16_t*)&outbuf[1] = SWAP_UINT16((uint16_t)p1);
				}
				if ( ops[q].operands > 1 ) {
				getpar(p2);
				if ( ops[q].tb2 == 0 && ops[q].tb1 == 0) {
				outbuf[2] = p2;
				if ( ops[q].operands > 2 ) {
				getpar(p3);
				outbuf[3] = p3;
				}
				} else if ( ops[q].tb1 == 1 ) {
				outbuf[3] = p2;
				} else {
				*(uint16_t*)&outbuf[2] = SWAP_UINT16((uint16_t)p2);
				}
				}
				}
				emit_pass { fwrite(outbuf, 4, 1, stdout); }
				ip+=4;
			}
		}	
		}	
	}
	fclose(in);
}
int main(int argc, char **argv) {
	preproc(1, argv[1]);
	preproc(0, argv[1]);
	fprintf(stderr, "%15s %4s %5s\n", "Symbols", "Hex", "Dec");
	for(int q = 0; q < nsymbols; q++ ) {
		fprintf(stderr, "%15s 0x%04x %05d\n", symbols[q].name, symbols[q].ptr, symbols[q].ptr);
	}
}