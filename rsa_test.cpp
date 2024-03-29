// rsa_test.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<math.h>
#include<time.h>

#define BUFSIZE 1024

/*随机寻找两个大于256，小于65536的质数，作为p，q返回*/
void create(unsigned _int64 &p, unsigned _int64 &q) {
	unsigned _int64 set[65536];
	int size = 0;
	_int64 k = 0;
	_int64 i = 0;
	for (int round = 256; round < 65536; round++) {
		k = (int)sqrt((double)round);
		for (i = 2; i <= k; i++)
			if (round%i == 0)
				break;
		if (i > k) {
			set[size] = round;
			size++;
		}
	}
	/*
	printf("size: %d\n", size);
	for (_int64 round = 0; round < size; round++) {
		printf("%d ", set[round]);
		if (round % 10 == 1) {
			printf("\n");
		}
	}
	*/
	srand((unsigned)(time(NULL)));
	p = set[rand() % size];
	srand(rand());
	q = set[rand() % size];
}

/*判断是否互质*/
int gcd(unsigned _int64 N, unsigned _int64 e) {
	if (e == 0)
		return N;
	else
		return gcd(e, N%e);
}

/*通过E和L使用欧几里得方法找到满足条件的D*/
int findD(unsigned _int64 E, unsigned _int64 L, unsigned _int64 &D, unsigned _int64 &y) {
	if (L == 0 || E == 0) {
		D = 1;
		y = 0;
		return 0;
	}
	if (E < L) {
		findD(E, L%E, D, y);
		D = (unsigned _int64)(L*y + 1) / E;
	}
	else {
		findD(E%L, L, D, y);
		y = (unsigned _int64)(E*D - 1) / L;
	}
}

/*随机生成E，判断符合互质后，在去计算D*/
void createE_D(unsigned _int64 p, unsigned _int64 q, unsigned _int64 &E, unsigned _int64 &D) {
	unsigned _int64 e = 0, d = 0, y = 0;
	unsigned _int64 L = (p - 1) * (q - 1);
	srand((unsigned)time(NULL));
	//e = 2;
	e = (unsigned _int64)rand();
	while (1) {
		if (e == 0)
			continue;
		if (gcd(L, e) == 1)
			break;
		e++;
		if (e > L) {
			e = 2;
		}
	}
	findD(e, L, d, y);
	E = e;
	D = d;
}

/*单字节进行rsa加密*/
unsigned _int64 rsa(unsigned char plain, unsigned _int64 E, unsigned _int64 N) {
	unsigned _int64 cipher = 0;
	cipher = 1;
	for (_int64 round = 1; round <= E; round++) {
		cipher = (cipher * ((unsigned _int64)plain)) % N;
	}
	return cipher;
}

/*读取文件，对制定文件进行rsa加密*/
void encrypt(FILE* inf, FILE* otf, unsigned _int64 E, unsigned _int64 N) {

	unsigned char in_buf[BUFSIZE];
	unsigned _int64 out_buf[BUFSIZE];
	int read_len = 0;

	while (1) {
		read_len = fread(in_buf, sizeof(unsigned char), BUFSIZE, inf);
		for (int round = 0; round < read_len; round++) {
			out_buf[round] = rsa(in_buf[round], E, N);
		}
		fwrite(out_buf, sizeof(unsigned _int64), read_len, otf);

		if (read_len < BUFSIZE)
			break;
	}
	
}

/*对单个64bit数据进行rsa解密，得到单字节原数据*/
unsigned char rsa_1(unsigned _int64 cipher, unsigned _int64 D, unsigned _int64 N) {
	unsigned _int64 plain1 = 1;
	unsigned char plain = (unsigned char)0;
	for (_int64 round = 1; round <= D; round++) {
		plain1 = (plain1 * cipher) % N;
	}
	plain = (unsigned char)plain1;
	return plain;
}

/*对指定密文文件进行rsa解密*/
void decrypt(FILE* inf, FILE* otf, unsigned _int64 D, unsigned _int64 N) {
	unsigned _int64 in_buf[BUFSIZE];
	unsigned char out_buf[BUFSIZE];
	int read_len = 0;
	while (1) {
		read_len = fread(in_buf, sizeof(unsigned _int64), BUFSIZE, inf);
		for (int round = 0; round < read_len; round++) {
			out_buf[round] = rsa_1(in_buf[round], D, N);
		}
		fwrite(out_buf, sizeof(unsigned _int64), BUFSIZE, otf);

		if (read_len < BUFSIZE)
			break;
	}
}

int main(int argc, char *argv[])
{

	FILE *inf, *otf;

	if (!strcmp(argv[1], "-c")) {
		unsigned _int64 p = 0, q = 0, N = 0;
		unsigned _int64 E = 0, D = 0;

		create(p, q);
		N = p * q;
		createE_D(p, q, E, D);
		printf("P: %I64u  Q: %I64u  N: %I64u  L: %I64u\n", p, q, (p)*(q), (p - 1)*(q - 1));
		printf("E: %I64u  D: %I64u\n", E, D);

		FILE *pub, *pri;
		pub = fopen(argv[2], "wb");
		if (!pub) {
			fprintf(stderr, "ERROR: PublicKeyFile fopen error: %s\n", strerror(errno));
			return errno;
		}//公钥文件
		pri = fopen(argv[3], "wb");
		if (!pri) {
			fprintf(stderr, "ERROR: PrivateKeyFile fopen error: %s\n", strerror(errno));
			return errno;
		}//私钥文件

		fwrite(&E, sizeof(unsigned _int64), 1, pub);
		fwrite(&N, sizeof(unsigned _int64), 1, pub);//将公钥写入公钥文件
		fwrite(&D, sizeof(unsigned _int64), 1, pri);
		fwrite(&N, sizeof(unsigned _int64), 1, pri);//将私钥写入私钥文件

		printf("Generate Key Pair Success.\n");
		fclose(pub); fclose(pri);
	}

	if (!strcmp(argv[1], "-e")) {
		inf = fopen(argv[3], "rb");
		if (!inf) {
			fprintf(stderr, "ERROR: Infile fopen error: %s\n", strerror(errno));
			return errno;
		}
		otf = fopen(argv[4], "wb");
		if (!otf) {
			fprintf(stderr, "ERROR: Outfile fopen error: %s\n", strerror(errno));
			return errno;
		}
		FILE *pub;
		pub = fopen(argv[2], "rb");
		if (!pub) {
			fprintf(stderr, "ERROR: PublicKeyFile fopen error: %s\n", strerror(errno));
			return errno;
		}
		unsigned _int64 E, N;
		fread(&E, sizeof(unsigned _int64), 1, pub);
		fread(&N, sizeof(unsigned _int64), 1, pub);
		printf("E: %I64u  N: %I64u\n", E, N);

		encrypt(inf, otf, E, N);
		printf("Encryption Success.\n");
		fclose(inf); fclose(otf); fclose(pub);

	}
	
	if (!strcmp(argv[1], "-d")) {
		inf = fopen(argv[3], "rb");
		if (!inf) {
			fprintf(stderr, "ERROR: Infile fopen error: %s\n", strerror(errno));
			return errno;
		}
		otf = fopen(argv[4], "wb");
		if (!otf) {
			fprintf(stderr, "ERROR: Outfile fopen error: %s\n", strerror(errno));
			return errno;
		}
		FILE *pri;
		pri = fopen(argv[2], "rb");
		if (!pri) {
			fprintf(stderr, "ERROR: PrivateKeyFile fopen error: %s\n", strerror(errno));
			return errno;
		}
		unsigned _int64 D, N;
		fread(&D, sizeof(unsigned _int64), 1, pri);
		fread(&N, sizeof(unsigned _int64), 1, pri);
		printf("D: %I64u  N: %I64u\n", D, N);

		decrypt(inf, otf, D, N);
		printf("Decryption Success.\n");
		fclose(inf); fclose(otf); fclose(pri);
	}

	
	return 0;
}
