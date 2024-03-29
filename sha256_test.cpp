#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*缓冲区*/
#define BUFSIZE 55

inline unsigned int ror(unsigned int val, int size) {
	unsigned res = val >> size;
	res |= val << (32 - size);
	return res;
}

int sha256(FILE *inf, unsigned char *hash) {
	unsigned int h0 = 0x6a09e667;
	unsigned int h1 = 0xbb67ae85;
	unsigned int h2 = 0x3c6ef372;
	unsigned int h3 = 0xa54ff53a;
	unsigned int h4 = 0x510e527f;
	unsigned int h5 = 0x9b05688c;
	unsigned int h6 = 0x1f83d9ab;
	unsigned int h7 = 0x5be0cd19;//八个哈希初值
	unsigned int k[64] =
		{0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
		0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
		0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
		0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
		0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
		0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
		0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
		0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2 };
	//64个常量
	unsigned int w[64];
	unsigned char in_buf[BUFSIZE + 1];
	int read_len = 0;
	unsigned int s0 = 0, s1 = 0;
	unsigned int maj = 0, t1 = 0, t2 = 0, ch = 0;
	unsigned int a, b, c, d, e, f, g, h;
	a = b = c = d = e = f = g = h = 0;
	unsigned long long filelen;
	fseek(inf, 0L, SEEK_END);
	filelen = ftell(inf);
	filelen *= 8;//文件长度
	fseek(inf, 0L, SEEK_SET);
	while (1) {
		read_len = fread(in_buf, sizeof(unsigned char), BUFSIZE, inf);//读取文件到缓冲池
		in_buf[read_len] = (unsigned char)0x80;
		for (int i = read_len + 1; i < BUFSIZE + 1; i++)
			in_buf[i] = (unsigned char)0;//补位
		memcpy(w, in_buf, BUFSIZE + 1);//将一个哈希块放入64个32bit字的数组中
		memcpy((w + 14), &filelen, 2 * sizeof(unsigned int));
		for (int round = 16; round < 64; round++) {
			s0 = (ror(w[round - 15], 7)) ^ (ror(w[round - 15], 18)) ^ (w[round - 15] >> 3);
			s1 = (ror(w[round - 2], 17)) ^ (ror(w[round - 2], 19)) ^ (w[round - 2] >> 10);
			w[round] = w[round - 16] + s0 + w[round - 7] + s1;
		}//初始化64个32bit字的数组
		a = h0; b = h1; c = h2; d = h3; e = h4; f = h5; g = h6; h = h7;
		for (int round = 0; round < 64; round++) {
			s0 = (ror(a, 2)) ^ (ror(a, 13)) ^ (ror(a, 22));
			maj = (a&b) ^ (a&c) ^ (b&c);
			t2 = s0 + maj;
			s1 = (ror(e, 6)) ^ (ror(e, 11)) ^ (ror(e, 25));
			ch = (e&f) ^ ((~e)&g);
			t1 = h + s1 + ch + k[round] + w[round];
			h = g; g = f; f = e; e = d + t1; d = c; c = b; b = a; a = t1 + t2;
		}
		h0 += a; h1 += b; h2 += c; h3 += d; h4 += e; h5 += f; h6 += g; h7 += h;

		if (read_len < BUFSIZE)
			break;
	}
	unsigned int di[8];
	di[0] = h0; di[1] = h1; di[2] = h2; di[3] = h3; 
	di[4] = h4; di[5] = h5; di[6] = h6; di[7] = h7;
	memcpy(hash, di, 32 * sizeof(unsigned char));
	return 0;
}

int digest(FILE *inf, FILE *dig) {
	unsigned char hash[32];
	sha256(inf, (unsigned char*)hash);
	for (int round = 0; round < 32; round++) {
		printf("%02x", hash[round]);
	}
	printf("\n");
	for (int round = 0; round < 32; round++) {
		fprintf(dig, "%02x", hash[round]);
	}
	return 0;
}

int verify(FILE *inf, FILE *dig) {
	unsigned char hash[32];
	sha256(inf, (unsigned char*)hash);
	unsigned char hash_hex[64];
	unsigned char hash_file[64];
	for (int round = 0; round < 32; round++) {
		hash_hex[2 * round] = (hash[round] >> 4) & 0x0F;
		if (hash_hex[2 * round] < 10)
			hash_hex[2 * round] = (unsigned char)((int)hash_hex[2 * round] + 48);
		else
			hash_hex[2 * round] = (unsigned char)((int)hash_hex[2 * round] - 10 + 97);
		hash_hex[2 * round + 1] = (hash[round]) & 0x0F;
		if (hash_hex[2 * round + 1] < 10)
			hash_hex[2 * round + 1] = (unsigned char)((int)hash_hex[2 * round + 1] + 48);
		else
			hash_hex[2 * round + 1] = (unsigned char)((int)hash_hex[2 * round + 1] - 10 + 97);
	}
	printf("\n");
	for (int round = 0; round < 64; round++)
		printf("%c", (char)hash_hex[round]);
	printf("\n");

	fread(hash_file, sizeof(unsigned char), 64, dig);
	if (memcmp(hash_file, hash_hex, 64) == 0) {
		printf("\nVerify Success.\n");
	}
	else {
		printf("\nVerify Failure.\n");
	}

	return 0;
}

int main(int argc, char *argv[])
{
	FILE *inf, *otf;

	if (!strcmp(argv[1], "-h")) {
		printf("\n--help\n\n");
		printf("-d inputfile hashfile          对指定文件做sha-256方法的哈希摘要\n");
		printf("-v inputfile hashfile          对指定文件使用hashfile中的哈希值做sha-256方法的哈希校验\n\n");
	}
	else if (!strcmp(argv[1], "-d")) {
		inf = fopen(argv[2], "rb");
		if (!inf) {
			fprintf(stderr, "ERROR: InputFile fopen error: %s\n", strerror(errno));
			return errno;
		}//打开输入文件
		otf = fopen(argv[3], "wb");
		if (!otf) {
			fprintf(stderr, "ERROR: DigestFile fopen error: %s\n", strerror(errno));
			return errno;
		}//打开输出文件
		digest(inf, otf);
		printf("\nDigest Success.\n");
		fclose(inf);
		fclose(otf);
	}
	else if (!strcmp(argv[1], "-v")) {
		inf = fopen(argv[2], "rb");
		if (!inf) {
			fprintf(stderr, "ERROR: InputFile fopen error: %s\n", strerror(errno));
			return errno;
		}//打开输入文件
		otf = fopen(argv[3], "rb");
		if (!otf) {
			fprintf(stderr, "ERROR: DigestFile fopen error: %s\n", strerror(errno));
			return errno;
		}//打开摘要文件
		verify(inf, otf);
		fclose(inf);
		fclose(otf);
	}
	
	return 0;
}
