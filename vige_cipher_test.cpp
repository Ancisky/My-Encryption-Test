#include<stdio.h>
#include<string.h>
#include<stdlib.h>

/*缓冲池*/
#define BUFSIZE 1024

/*TestKey用于检验字符类型的密钥是否规范（在大小写范围内），并且将大写字母转换为小写*/
int TestKey(char *key) {
	for (int round = 1; round < strlen(key); round++) {
		if ((int)key[round]<'A' || ((int)key[round] > 'Z' && (int)key[round] < 'a') || (int)key[round]>'z')
			//判定是否在大小写字母范围内
			return 1;
		if (key[round] >= 'A'&&key[round] <= 'Z') {
			//大写转小写
			key[round] = (char)((int)key[round] + ((int)'a' - (int)'A'));
		}
	}
	return 0;
}

/*TextOnConsole_en用于将接收到的明文用密钥进行维吉尼亚密码方法的加密，并且将密文写入参数接收的字符串中*/
int TextOnConsole_en(char *plain, char *key, char *cipher, int str_len) {
	for (int round=0; round < str_len; round++) {
		if ((int)plain[round] >= (int)'A' && (int)plain[round] <= (int)'Z') {
			plain[round] = (char)((int)plain[round] + ((int)'a'-(int)'A'));
			//大写转小写
			if (((int)plain[round] + ((int)key[round%strlen(key)] - (int)'a')) > (int)'z') {
				//如果移位后小写字母超过了'z'，就需要再从'a'开始移动
				plain[round] = (char)((int)'a' + (((int)plain[round] + ((int)key[round%strlen(key)] - (int)'a')) - (int)'z'));
			}else
				plain[round] = (char)((int)plain[round] + ((int)key[round%strlen(key)] - (int)'a'));
			//使用密钥当前位置对应的移位密码表加密
			cipher[round] = plain[round];
		}
		else if ((int)plain[round] >= (int)'a' && (int)plain[round] <= (int)'z') {
			plain[round] = (char)((int)plain[round] - ((int)'a' - (int)'A'));
			//小写转大写
			plain[round] = (char)((int)plain[round] + ((int)key[round%strlen(key)] - (int)'a'));
			if (plain[round] > 'Z') {
				plain[round] = (char)((int)'A' + ((int)plain[round] - (int)'Z'));
			}
			cipher[round] = plain[round];
		}
		else 
			cipher[round] = plain[round];
		//使用密钥当前位置对应的移位密码表加密
	}
	return 0;
}

/*TextOnConsole_de用于将接收到的明文用密钥进行维吉尼亚密码方法的解密，并且将明文写入参数接收的字符串中*/
int TextOnConsole_de(char *cipher, char *key, char *plain, int str_len) {

	for (int round = 0; round < str_len; round++) {
		if ((int)cipher[round] >= (int)'A' && (int)cipher[round] <= (int)'Z') {
			cipher[round] = (char)((int)cipher[round] + ((int)'a' - (int)'A'));
			//大写转小写
			cipher[round] = (char)((int)cipher[round] - ((int)key[round%strlen(key)] - (int)'a'));
			//减去密钥对应字母当前的移位表中的移位值
			if (cipher[round] < 'a') {
				//小于‘a’的从‘z’接着减
				cipher[round] = (char)((int)'z' - ((int)'a' - (int)cipher[round]));
			}
			plain[round] = cipher[round];
		}
		else if (cipher[round] >= 'a' && cipher[round] <= 'z') {
			//小写转大写
			cipher[round] = (char)((int)cipher[round] - ((int)'a' - (int)'A'));
			//减去密钥对应字母当前的移位表中的移位值
			cipher[round] = (char)((int)cipher[round] - ((int)key[round%strlen(key)] - (int)'a'));
			if (cipher[round] < 'A') {
				cipher[round] = (char)((int)'Z' - ((int)'A' - (int)cipher[round]));
			}
			plain[round] = cipher[round];
		}
		else {
			plain[round] = cipher[round];
		}
	}
	return 0;
}

/*replace_table用于将接收到的密钥字符串key制作替换密码表table并返回*/
int replace_table(char *key, char *table) {
	char *kp, *tp, lp;
	int flag = 0;
	int num = 0;
	lp = 'a';
	kp = key;
	tp = table;
	for (int round1 = 0; round1 < strlen(key); round1++) {
		flag = 0;
		for (int round2 = 0; round2 < num; round2++) {
			//如果key中检索到的字母已经排列在对照表中了，就不再排列
			if(table[round2]==key[round1]){
				flag = 1;
				break;
			}
		}
		if (flag == 1)
			continue;

		table[num] = key[round1];//没有排列的添加到对照表中
		num++;
	}
	for (int round = 0; round < 26; round++) {
		flag = 0;
		for (int round2 = 0; round2 < strlen(key); round2++) {
			if ((char)((int)lp + round) == key[round2]) {
				//按照顺序检索字母表，只要字母表中的字母没有在key中出现，则必定没有在对照表中排列过
				flag = 1;
				break;
			}
		}
		if (flag == 1)
			continue;
		table[num] = (char)((int)lp + round);//没有在key中出现的字母排列进对照表中
		num++;
	}
	return 0;
}

/*replace_detable根据接收到的密钥字符串制作替换表，并且使用该替换表制作解密替换表*/
int replace_detable(char *table, char *detable) {
	char mid;
	for (int round = 0; round < 26; round++) {
		detable[round] = (char)((int)'a' + round);
	}
	//起泡排序，根据加密对照表的加密行排序，并且加密行移动，明文对照行也跟着移动
	for (int round = 25; round >= 0; round--) {
		for (int round1 = 0; round1 < round; round1++) {
			if (table[round1] > table[round1 + 1]) {
				mid = table[round1];
				table[round1] = table[round1 + 1];
				table[round1 + 1] = mid;

				mid = detable[round1];
				detable[round1] = detable[round1 + 1];
				detable[round1 + 1] = mid;
			}
		}
	}

	printf("\nReplace DeTable:\n\n");
	for (int r = 0; r < 26; r++) {
		printf("%c ", table[r]);
	}
	printf("\n");
	for (int r = 0; r < 26; r++) {
		printf("%c ", detable[r]);
	}
	printf("\n");

	return 0;
}

/*单表替换加密算法*/
int replace_en(char *plain, char *cipher, char *key) {
	char table[26];//对照表
	replace_table(key, table);//生成对照表
	printf("\nReplace Table:\n\n");
	for (int r = 0; r < 26; r++) {
		printf("%c ", 'a'+r);
	}
	printf("\n");
	for (int r = 0; r < 26; r++) {
		printf("%c ", table[r]);
	}
	printf("\n");
	//输出对照表

	for (int round = 0; round < strlen(plain); round++) {
		//大小写互换，并根据对照表进行加密
		if (plain[round] >= 'A' && plain[round] <= 'Z') {
			plain[round] = (char)((int)plain[round] + ((int)'a' - (int)'A'));
			cipher[round] = table[(int)plain[round] - (int)'a'];
		}
		else if ((int)plain[round] >= (int)'a' && (int)plain[round] <= (int)'z') {
			plain[round] = (char)((int)plain[round] - ((int)'a' - (int)'A'));
			cipher[round] = (char)((int)'A' + ((int)table[(int)plain[round] - (int)'A'] - (int)'a'));
		}
		else
			cipher[round] = plain[round];
	}
	printf("\nCipher:\n");
	puts(cipher);
	return 0;
}

/*单表替换解密算法*/
int replace_de(char *cipher, char *decipher, char *key) {
	char table[26], tablede[26];//加密和解密对照表

	replace_table(key, table);//生成加密对照表

	printf("\nReplace Table:\n\n");
	for (int r = 0; r < 26; r++) {
		printf("%c ", 'a' + r);
	}
	printf("\n");
	for (int r = 0; r < 26; r++) {
		printf("%c ", table[r]);
	}
	printf("\n");

	replace_detable(table, tablede);//生成解密对照表

	for (int round = 0; round < strlen(cipher); round++) {
		//大小写互换，并且根据解密对照表解密
		if (cipher[round] >= 'A' && cipher[round] <= 'Z') {
			cipher[round] = (char)((int)cipher[round] + ((int)'a' - (int)'A'));
			decipher[round] = tablede[(int)cipher[round] - (int)'a'];
		}
		else if ((int)cipher[round] >= (int)'a' && (int)cipher[round] <= (int)'z') {
			cipher[round] = (char)((int)cipher[round] - ((int)'a' - (int)'A'));
			decipher[round] = (char)((int)'A' + ((int)tablede[(int)cipher[round] - (int)'A'] - (int)'a'));
		}
		else
			decipher[round] = cipher[round];
	}
	return 0;
}

/*对纯英文文本文件的加解密函数*/
int text_en_de(FILE *inf, FILE *otf, char *key, int flag) {
	int read_len;//读取到的文件的长度
	char in_buf[BUFSIZE],out_buf[BUFSIZE];//设置读取和输出缓冲池
	if (flag == 1) {
		//flag为1是加密
		while (1) {
			read_len = fread(in_buf, sizeof(char), BUFSIZE, inf);
			//读取最多为缓冲池大小的文件内容
			TextOnConsole_en(in_buf, key, out_buf, read_len);
			//使用维吉尼亚密码加密函数对缓冲池内容加密
			fwrite(out_buf, sizeof(char), read_len, otf);
			//加密后写入文件
			if (read_len < BUFSIZE) {
				//本次读取的内容不足缓冲池大小（文件读取完毕）退出
				break;
			}
		}
	}
	if (flag == 0) {
		//flag为0是解密
		while (1) {
			//解密步骤同加密步骤
			read_len = fread(in_buf, sizeof(char), BUFSIZE, inf);
			TextOnConsole_de(in_buf, key, out_buf, read_len);
			fwrite(out_buf, sizeof(char), read_len, otf);
			if (read_len < BUFSIZE) {
				break;
			}
		}
	}
	return 0;
}

/*对文件的加解密函数*/
int file_en_de(FILE *inf, FILE *otf, int *key, int key_len,int flag) {
	int read_len;//读取的长度
	int *kp;//指向密钥key的指针
	kp = key;
	unsigned char in_buf[BUFSIZE], out_buf[BUFSIZE];//设定缓冲池的大小
	if (flag == 1) {
		//flag=1，加密
		while (1) {
			read_len = fread(in_buf, sizeof(unsigned char), BUFSIZE, inf);
			//读取文件内容到缓冲池，按unsigned char读取
			for (int round = 0; round < read_len; round++) {
				/*由于是按unsigned char读取任意文件，读取的char大小的字符也不会局限于ASCII大小的范围内，
				键盘的字符输入就存在局限性，所以将密钥改为整型数组，将密钥空间升至256整型大小，密钥数组
				的每一个元素应该符合0~255范围，移位时直接按照密钥数组对应数字的大小进行移位*/
				if (((int)in_buf[round] + *kp) > 255)
					in_buf[round] = (unsigned char)((int)in_buf[round] + *kp - 256);
				else
					in_buf[round] = (unsigned char)((int)in_buf[round] + *kp);

				out_buf[round] = in_buf[round];
				kp = kp + 1;
				if ((kp - key) == key_len)
					kp = key;
			}

			fwrite(out_buf, sizeof(unsigned char), read_len, otf);//加密后的数据写入文件

			if (read_len < BUFSIZE)
				break;
		}
	}

	if (flag == 0) {
		//flag=0，解密
		while (1) {
			read_len = fread(in_buf, sizeof(unsigned char), BUFSIZE, inf);
			//读取文件至缓冲区
			for (int round = 0; round < read_len; round++) {
				if (((int)in_buf[round] - *kp) < 0)
					in_buf[round] = (unsigned char)((int)in_buf[round] + 256 - *kp );
				else
					in_buf[round] = (unsigned char)((int)in_buf[round] - *kp);
				out_buf[round] = in_buf[round];
				kp = kp + 1;
				if ((kp - key) == key_len)
					kp = key;
			}
			fwrite(out_buf, sizeof(unsigned char), read_len, otf);
			if (read_len < BUFSIZE)
				break;
		}
	}
	return 0;
}

int main(int argc,char *argv[])
{
	int flag = 1;
	char check1;
	int check2=0;
	int _key[20],_key_len=0;//读取任意文件的
	char plain[1024], cipher[1024], key[20], decipher[1024];//明文，密文，密钥，解密后的密文的空间（仅在控制台加解密时使用）
	FILE *inf, *otf;//文件输入输出指针

	if (!strcmp(argv[1], "-h")) {
		//帮助文档
		printf("--help\n\n");
		printf("Command Format: [-e]/[-d] [console]/[txt]/[file] [InputFilePath] [OutputFilePath]\n\n");
		printf("[-e]/[-d]                                     加密/解密\n");
		printf("[console]/[replace]/[text]/[file]             在控制台输入/单表替换密码/通过文本文件输入/通过任意类型大小文件输入\n");
		printf("[InputFilePath]                               填写输入文件路径（在[text]/[file]指令后）\n");
		printf("[OutputFilePath]                              填写输出文件路径（在[text]/[file]指令后）\n");
		return 0;
	}
	
	if (!strcmp(argv[1], "-e")) {
		//加密选项
		if (!strcmp(argv[2], "console")) {
			//控制台上的维吉尼亚密码加密
			printf("Input Plain:\n");
			gets_s(plain, 1024);
			printf("\nInput Key:\n");
			gets_s(key, 20);
			flag = TestKey(key);
			if (flag == 1) {
				printf("Error: Wrong Format Key.\n");
				return 0;
			}
			cipher[strlen(plain)] = '\0';
			TextOnConsole_en(plain, key, cipher,strlen(plain));
			printf("\nCipher:\n");
			puts(cipher);
			return 0;
		}

		if (!strcmp(argv[2], "replace")) {
			//控制台的单表替换密码加密
			printf("Input Plain:\n");
			gets_s(plain, 1024);
			printf("\nInput Key:\n");
			gets_s(key, 20);
			flag = TestKey(key);
			if (flag == 1) {
				printf("Error: Wrong Format Key.\n");
				return 0;
			}
			cipher[strlen(plain)] = '\0';
			replace_en(plain, cipher, key);
		}

		if (!strcmp(argv[2], "text")|| !strcmp(argv[2], "file")) {
			//需要文件操作的加密
			inf = fopen(argv[3], "rb");
			if (!inf) {
				fprintf(stderr, "ERROR: InputFile fopen error: %s\n", strerror(errno));
				return errno;
			}
			otf = fopen(argv[4], "wb");
			if (!inf) {
				fprintf(stderr, "ERROR: OutputFile fopen error: %s\n", strerror(errno));
				return errno;
			}

			if (!strcmp(argv[2], "text")) {
				//英文文本文件的加密
				printf("Input Key:\n");
				gets_s(key, 20);
				flag = TestKey(key);
				if (flag == 1) {
				printf("Error: Wrong Format Key.\n");
				return 0;
				}
				text_en_de(inf, otf, key, 1);
			}

			if (!strcmp(argv[2], "file")) {
				//任意类型大小文件的加密
				printf("\nInput Key Number Array:\n");
				for (_key_len = 0; _key_len < 20; _key_len++) {
					scanf("%d", &check2);
					if (check2 < 0 || check2>255) {
						printf("Error: Wrong Key Number\n");
						return 0;
					}
					_key[_key_len] = check2;
					check1 = getchar();
					if (check1 == '\n') {
						_key_len++;
						break;
					}
				}
				file_en_de(inf, otf, _key, _key_len, 1);
			}

			fclose(inf);
			fclose(otf);
		}
	}

	if (!strcmp(argv[1], "-d")) {
		//解密操作，指令对应加密中的指令
		if (!strcmp(argv[2], "console")) {
			printf("Input Cipher:\n");
			gets_s(cipher, 1024);
			printf("\nInput Key:\n");
			gets_s(key, 20);
			flag = TestKey(key);
			if (flag == 1) {
				printf("Error: Wrong Format Key.\n");
				return 0;
			}
			decipher[strlen(cipher)] = '\0';
			TextOnConsole_de(cipher, key, decipher,strlen(cipher));
			printf("\nDe_Cipher:\n");
			puts(decipher);
			return 0;
		}

		if (!strcmp(argv[2], "replace")) {
			printf("Input Cipher:\n");
			gets_s(cipher, 1024);
			printf("\nInput Key:\n");
			gets_s(key, 20);
			flag = TestKey(key);
			if (flag == 1) {
				printf("Error: Wrong Format Key.\n");
				return 0;
			}
			decipher[strlen(cipher)] = '\0';
			replace_de(cipher, decipher, key);
			printf("\nDeCipher:\n");
			puts(decipher);
		}

		if (!strcmp(argv[2], "text") || !strcmp(argv[2], "file")) {

			inf = fopen(argv[3], "rb");
			if (!inf) {
				fprintf(stderr, "ERROR: InputFile fopen error: %s\n", strerror(errno));
				return errno;
			}
			otf = fopen(argv[4], "wb");
			if (!inf) {
				fprintf(stderr, "ERROR: OutputFile fopen error: %s\n", strerror(errno));
				return errno;
			}

			if (!strcmp(argv[2], "text")) {
				printf("Input Key:\n");
				gets_s(key, 20);
				flag = TestKey(key);
				text_en_de(inf, otf, key, 0);
			}

			if (!strcmp(argv[2], "file")) {
				printf("\nInput Key Number Array:\n");
				for (_key_len = 0; _key_len < 20; _key_len++) {
					scanf("%d", &check2);
					if (check2 < 0 || check2>255) {
						printf("Error: Wrong Key Number\n");
						return 0;
					}
					_key[_key_len] = check2;
					check1 = getchar();
					if (check1 == '\n') {
						_key_len++;
						break;
					}
				}
				file_en_de(inf, otf, _key, _key_len, 0);
			}

			fclose(inf);
			fclose(otf);
		}
	}

	return 0;
}