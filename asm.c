#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#define LITTLE_ENDIAN
#ifdef LITTLE_ENDIAN
#define SWAP_UINT16(x) (((x) >> 8) | ((x) << 8))
#else
#define SWAP_UINT16(x) x
#endif
char pp;
char symbname[128];
#define getpar(o) pp=fgetc(in);if(pp=='#'){fscanf(in, "%d ",&o);}else if(pp=='@'){fscanf(in, "%s ", symbname);o=getsymbol(symbname);}else{fscanf(in,"%x ",&o);}
typedef struct {
	char *name;
	uint8_t opcode;
	uint8_t tb1;
	uint8_t tb2;
	int operands;
} optable;
optable ops[16] = {
	{ "mov", 3, 1, 0, 2 },
	{ "out", 5, 0, 0, 2 },
	{ "mput", 1, 1, 0, 2 },
	{ "mget", 2, 1, 0, 2 },
	{ "call", 8, 1, 0, 1 },
	{ "jmp", 4, 1, 0, 1 },
	{ "ret", 7, 0, 0, 0 },
	{ "jeq", 9, 0, 0, 3 },
	{ "add", 6, 0, 0, 3 }
};
typedef struct {
	char *name;
	uint16_t ptr;
} symbol;
symbol symbols[256];
int nsymbols = 0;
uint16_t getsymbol(char *name) {
	for ( int s = 0; s < nsymbols; s++ ) {
		if ( !strcmp(name, symbols[s].name) ) {
			return symbols[s].ptr;
		}
	}
	return 0;
}


int tops = 9;
#define addsymbol(n, p) symbols[nsymbols].name=n;symbols[nsymbols].ptr=p;nsymbols++
#define emit_pass if(firstpass != 1)
void preproc(int firstpass, char *infile) {
	FILE *in = fopen(infile, "r");
	
	int ip = 0;
	char op[8];
	uint8_t outbuf[4];
	int p1, p2, p3;
	while (!feof(in)) {
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