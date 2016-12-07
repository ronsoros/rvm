#include <stdio.h>
#include <string.h>
char *outdata; char *outcode;
#define apd(n, q, ...) sprintf(n, "%s" q, n, __VA_ARGS__)
char* dtrs(char *in) {
	char ret[512];
	long long tmp = time(NULL);
	// find if its a symbol, or whatever
	if ( in[0] == '\'' ) {
		sprintf(ret, "#%d", in[1]);
		return ret;
	}
	if ( in[0] == '"' ) {
		in[strlen(in)-1]=0;
		apd(outdata, ".label %lld\n.ds %s\n.db #0\n", tmp, &in[1]);
		sprintf(ret, "@%d", tmp);
		return ret;
	}
	if ( isdigit(in[0]) ) {
		sprintf(ret, "#%d", atoi(in));
		return ret;
	}
	sprintf(ret, "@%s", in);
	return ret;
}
int main(int argc, char **argv) {
	char cmd[64];
	char nsn[64];
	char val[512];
	char t = 0;
	outdata = malloc(16384 + atoi(argc > 2 ? argv[2] : "16384"));
	outcode = malloc(16384 + atoi(argc > 2 ? argv[2] : "16384"));
	outcode[0] = 0;
	outdata[0] = 0;
	
	FILE *fp = fopen(argv[1], "r");
	while(!feof(fp)) {
		fscanf(fp, "%s ", cmd);
		if (!strcmp(cmd, "//" )) {
		fscanf(fp, "%[^\n]", cmd);
		} else 
		if (!strcmp(cmd, "int")||!strcmp(cmd, "char*")) {
			fscanf(fp, "%s ", nsn);
			//printf("%s\n", nsn);
			fscanf(fp, "%c ", &t);
			if ( t == '=' || t == ';' ) {
				apd(outdata, ".label %s\n", nsn);
				if ( t != ';' ) {
				fscanf(fp, "%[^;]; ", val);
				if ( val[0] == '"' ) {
					val[strlen(val)-1] = '\0';
					apd(outdata, ".ds %s\n.db #0\n", &val[1]);
				} else if ( val[0] == '\'' ) {
					apd(outdata, ".ds %c\n", val[1]);
				} else {
					apd(outdata, ".db #%d\n", atoi(val));
				}
				} else {
					apd(outdata, ".db #0\n", NULL);
				}
			} else {
			if ( t == '(' ) {
				fgetc(fp);
				apd(outcode, ".label %s\n", nsn);
			}
			}
		} else if (!strcmp(cmd, "{")) {
		} else
		if (!strcmp(cmd, "}") ) {
			apd(outcode, "ret\n", NULL);
		} else
		if (!strcmp(cmd, "return") ) {
			fscanf(fp, "%[^;]; ", nsn);
			apd(outcode, "mov #%s #0\n", nsn); 
		} else if (!strcmp(cmd, "//_asm")) {
			fscanf(fp, "%[^;]; ", nsn);
			apd(outcode, "%s\n", nsn);
		} else {
			nsn[0] = 0;
			fscanf(fp, "(%[^)]); ", nsn);
			if ( nsn[0] != 0 ) {
			apd(outcode, "mov %s #10\n", dtrs(nsn)); 
			}
			apd(outcode, "call @%s\n", cmd);
		}
	}
	puts(outcode);
	puts(outdata);
}