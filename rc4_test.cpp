#include<stdio.h>
#include<string.h>
#include<stdlib.h>

/*缓冲池*/
#define BUFSIZE 1024 * 10

int rc4_en_de(unsigned char *key, int keylen, unsigned char *stream, int slen) {
	unsigned char s[256];
	unsigned char t[256];
	int j = 0;
	int k = 0;
	int i = 0;
	for (int round = 0; round < 256; round++) {
		s[round] = (unsigned char)round;
		t[round] = key[round%keylen];
	}//初始化S和T

	for (int round = 0; round < 256; round++) {
		j = (j + (int)s[round] + (int)t[round]) % 256;
		s[round] = (s[round]) ^ (s[j]);
		s[j] = (s[j]) ^ (s[round]);
		s[round] = (s[j]) ^ (s[round]);
	}//交换S[i]和S[j]

	j = 0;
	for (int round = 0; round < slen; round++) {
		i = round % 256;
		j = (j + (int)s[i]) % 256;
		s[i] = (s[i]) ^ (s[j]);
		s[j] = (s[j]) ^ (s[i]);
		s[i] = (s[j]) ^ (s[i]);
		k = ((int)s[i] + (int)s[j]) % 256;
		stream[round] = (stream[round]) ^ (s[k]);
	}//由S[i]定位S[j],交换S[i]和S[j]，并相加得到k，s[k]用于异或加解密

	return 0;
}

int file_en_de(FILE *inf, FILE *otf, FILE *keyfile, int start, int keylen) {
	unsigned char stream[BUFSIZE];
	unsigned char *key;
	int read_len = 0, read_key = 0;
	key = (unsigned char*)malloc(keylen * sizeof(unsigned char));
	fseek(inf, 1L * start, 1);
	read_key = fread(key, sizeof(unsigned char), keylen, keyfile);
	if (read_key < keylen) {
		printf("Error: Can Not Read %d Bytes From KeyFile.");
		return 0;
	}
	fseek(inf, 0L, 0);
	while (1) {
		read_len = fread(stream, sizeof(unsigned char), BUFSIZE, inf);
		rc4_en_de(key, keylen, stream, read_len);
		fwrite(stream, sizeof(unsigned char), read_len, otf);
		if (read_len < BUFSIZE)
			break;
	}
	free(key);
	return 0;
}

int main(int argc,char *argv[])
{
	FILE *inf, *otf, *keyfile;
	int start = 0, keylen = 0;
	if (!strcmp(argv[1], "-h")) {
		printf("--help\n\n");
		printf("Encryption: [-e] [keyfile] [infile] [outfile]\n");
		printf("Decryption: [-e] [keyfile] [infile] [outfile]\n");
		return 0;
	}
	if ((!strcmp(argv[1], "-e")) || (!strcmp(argv[1], "-d"))) {
		printf("input Key start location at KeyFile: ");
		scanf("%d", &start);
		printf("input Key length: ");
		scanf("%d", &keylen);
		if (keylen < 1 || keylen>256) {
			printf("Error: Wrong Key Length.\n");
			return 1;
		}
		keyfile = fopen(argv[2], "rb");
		if (!keyfile) {
			fprintf(stderr, "ERROR: KeyFile fopen error: %s\n", strerror(errno));
			return errno;
		}//打开密钥文件
		inf = fopen(argv[3], "rb");
		if (!inf) {
			fprintf(stderr, "ERROR: InputFile fopen error: %s\n", strerror(errno));
			return errno;
		}//打开输入文件
		otf = fopen(argv[4], "wb");
		if (!otf) {
			fprintf(stderr, "ERROR: OutputFile fopen error: %s\n", strerror(errno));
			return errno;
		}//打开输出文件
		file_en_de(inf, otf, keyfile, start, keylen);
		printf("Success.\n");
	}
	return 0;
}