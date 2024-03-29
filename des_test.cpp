// des_test.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

/*缓冲池*/
#define BUFSIZE 1024

int encrypt(unsigned char *cipher, unsigned char *key, int c_len) {
	int loc_char = 0;
	int loc_bit = 0;

	unsigned char block_o[8];
	unsigned char block[8];
	unsigned char block_ep2[4];
	unsigned char block_ep1[4];
	unsigned char block_epf[6];
	unsigned char block_cipher[8];

	unsigned char subkey[17][7];
	unsigned char subkey_2[16][6];

	int temp = 1;
	unsigned char temp1;

	unsigned char *c_pt;


	int ip[] = { 58,50,42,34,26,18,10,2,
		60,52,44,36,28,20,12,4,
		62,54,46,38,30,22,14,6,
		64,56,48,40,32,24,16,8,
		57,49,41,33,25,17,9,1,
		59,51,43,35,27,19,11,3,
		61,53,45,37,29,21,13,5,
		63,55,47,39,31,23,15,7 };
	//IP置换

	int ip_1[64];
	for (int round = 0; round < 64; round++) {
		for (int round1 = 0; round1 < 64; round1++) {
			if (ip[round1] == round + 1) {
				ip_1[round] = round1 + 1;
			}
		}
	}
	//IP逆置换

	int pc_1[] = { 57,49,41,33,25,17,9,
		1,58,50,42,34,26,18,
		10,2,59,51,43,35,27,
		19,11,3,60,52,44,36,
		63,55,47,39,31,23,15,
		7,62,54,46,38,30,22,
		14,6,61,53,45,37,29,
		21,13,5,28,20,12,4 };
	//密钥压缩置换1

	int pc_2[] = { 14,17,11,24,1,5,
		3,28,15,6,21,10,
		23,19,12,4,26,8,
		16,7,27,20,13,2,
		41,52,31,37,47,55,
		30,40,51,45,33,48,
		44,49,39,56,34,53,
		46,42,50,36,29,32 };
	//子密钥压缩置换2

	int lcs[] = { 1,1,2,2,2,2,2,2,1,2,2,2,2,2,2,1 };
	//每轮加密左移的位数
	int lc_1[56], lc_2[56];
	lc_1[27] = 1; lc_1[55] = 29;
	lc_2[26] = 1; lc_2[27] = 2;
	lc_2[54] = 29; lc_2[55] = 30;
	for (int round = 54; round >= 0; round--) {
		if (round == 27)
			continue;
		lc_1[round] = round + 2;
	}
	for (int round = 53; round >= 0; round--) {
		if (round == 27) {
			round--;
			continue;
		}
		lc_2[round] = round + 3;
	}
	//制作移位的置换数组


	int e_p[] = { 32,1,2,3,4,5,
		4,5,6,7,8,9,
		8,9,10,11,12,13,
		12,13,14,15,16,17,
		16,17,18,19,20,21,
		20,21,22,23,24,25,
		24,25,26,27,28,29,
		28,29,30,31,32,1 };
	//加密函数中的扩展置换

	int s_box[8][4][16] = {                     //8个S盒   三维数组  

	// S1   

	14, 4,  13,  1,  2, 15, 11,  8,  3, 10,  6, 12,  5,  9,  0,  7,

	0, 15,  7,  4, 14,  2, 13,  1, 10,  6, 12, 11,  9,  5,  3,  8,

	4,  1, 14,  8, 13,  6,  2, 11, 15, 12,  9,  7,  3, 10,  5,  0,

	15, 12,  8,  2,  4,  9,  1,  7,  5, 11,  3, 14, 10,  0,  6, 13,

	// S2   

	15,  1,  8, 14,  6, 11,  3,  4,  9,  7,  2, 13, 12,  0,  5, 10,

	3, 13,  4,  7, 15,  2,  8, 14, 12,  0,  1, 10,  6,  9, 11,  5,

	0, 14,  7, 11, 10,  4, 13,  1,  5,  8, 12,  6,  9,  3,  2, 15,

	13,  8, 10,  1,  3, 15,  4,  2, 11,  6,  7, 12,  0,  5, 14,  9,

	// S3   

	10,  0,  9, 14,  6,  3, 15,  5,  1, 13, 12,  7, 11,  4,  2,  8,

	13,  7,  0,  9,  3,  4,  6, 10,  2,  8,  5, 14, 12, 11, 15,  1,

	13,  6,  4,  9,  8, 15,  3,  0, 11,  1,  2, 12,  5, 10, 14,  7,

	1, 10, 13,  0,  6,  9,  8,  7,  4, 15, 14,  3, 11,  5,  2, 12,

	// S4   

	7, 13, 14,  3,  0,  6,  9, 10,  1,  2,  8,  5, 11, 12,  4, 15,

	13,  8, 11,  5,  6, 15,  0,  3,  4,  7,  2, 12,  1, 10, 14,  9,

	10,  6,  9,  0, 12, 11,  7, 13, 15,  1,  3, 14,  5,  2,  8,  4,

	3, 15,  0,  6, 10,  1, 13,  8,  9,  4,  5, 11, 12,  7,  2, 14,

	// S5   

	2, 12,  4,  1,  7, 10, 11,  6,  8,  5,  3, 15, 13,  0, 14,  9,

	14, 11,  2, 12,  4,  7, 13,  1,  5,  0, 15, 10,  3,  9,  8,  6,

	4,  2,  1, 11, 10, 13,  7,  8, 15,  9, 12,  5,  6,  3,  0, 14,

	11,  8, 12,  7,  1, 14,  2, 13,  6, 15,  0,  9, 10,  4,  5,  3,

	// S6   

	12,  1, 10, 15,  9,  2,  6,  8,  0, 13,  3,  4, 14,  7,  5, 11,

	10, 15,  4,  2,  7, 12,  9,  5,  6,  1, 13, 14,  0, 11,  3,  8,

	9, 14, 15,  5,  2,  8, 12,  3,  7,  0,  4, 10,  1, 13, 11,  6,

	4,  3,  2, 12,  9,  5, 15, 10, 11, 14,  1,  7,  6,  0,  8, 13,

	// S7   

	4, 11,  2, 14, 15,  0,  8, 13,  3, 12,  9,  7,  5, 10,  6,  1,

	13,  0, 11,  7,  4,  9,  1, 10, 14,  3,  5, 12,  2, 15,  8,  6,

	1,  4, 11, 13, 12,  3,  7, 14, 10, 15,  6,  8,  0,  5,  9,  2,

	6, 11, 13,  8,  1,  4, 10,  7,  9,  5,  0, 15, 14,  2,  3, 12,

	// S8   

	13,  2,  8,  4,  6, 15, 11,  1, 10,  9,  3, 14,  5,  0, 12,  7,

	1, 15, 13,  8, 10,  3,  7,  4, 12,  5,  6, 11,  0, 14,  9,  2,

	7, 11,  4,  1,  9, 12, 14,  2,  0,  6, 10, 13, 15,  3,  5,  8,

	2,  1, 14,  7,  4, 10,  8, 13, 15, 12,  9,  0,  3,  5,  6, 11

	};
	//S盒

	unsigned char test_2, test_3;

	int p[] = { 16,7,20,21,29,12,28,17,
		1,15,23,26,5,18,31,10,
		2,8,24,14,32,27,3,9,
		19,13,30,6,22,11,4,25 };
	//S盒选择后的置换P

	unsigned char test_1[8];//探测unsigned char每一位（bit）的取值
	test_1[0] = (unsigned char)128;
	for (int round = 1; round < 8; round++) {
		test_1[round] = test_1[round - 1] >> 1;
	}

	for (int round1 = 0; round1 < 17; round1++)
		for (int round = 0; round < 7; round++)
			subkey[round1][round] = (unsigned char)0;
	for (int round = 0; round < 7; round++) {
		for (int round1 = 0; round1 < 8; round1++) {
			loc_char = (pc_1[round * 8 + round1] - 1) / 8;
			loc_bit = (pc_1[round * 8 + round1] - 1) % 8;
			if (round1 > loc_bit)
				subkey[0][round] |= (test_1[loc_bit] & key[loc_char]) >> (round1 - loc_bit);
			else
				subkey[0][round] |= (test_1[loc_bit] & key[loc_char]) << (loc_bit - round1);
		}
	}//置换选择1生成subkey1
	for (int r = 1; r < 17; r++) {
		if (lcs[r - 1] == 1) {
			for (int round = 0; round < 7; round++) {
				for (int round1 = 0; round1 < 8; round1++) {
					loc_char = (lc_1[round * 8 + round1] - 1) / 8;
					loc_bit = (lc_1[round * 8 + round1] - 1) % 8;
					if (round1 > loc_bit)
						subkey[r][round] |= (test_1[loc_bit] & subkey[r - 1][loc_char]) >> (round1 - loc_bit);
					else
						subkey[r][round] |= (test_1[loc_bit] & subkey[r - 1][loc_char]) << (loc_bit - round1);
				}
			}
		}//移位得到本轮子密钥
		else {
			for (int round = 0; round < 7; round++) {
				for (int round1 = 0; round1 < 8; round1++) {
					loc_char = (lc_2[round * 8 + round1] - 1) / 8;
					loc_bit = (lc_2[round * 8 + round1] - 1) % 8;
					if (round1 > loc_bit)
						subkey[r][round] |= (test_1[loc_bit] & subkey[r - 1][loc_char]) >> (round1 - loc_bit);
					else
						subkey[r][round] |= (test_1[loc_bit] & subkey[r - 1][loc_char]) << (loc_bit - round1);
				}
			}
		}//移位得到本轮的子密钥
		for (int round = 0; round < 6; round++)
			subkey_2[r - 1][round] = (unsigned char)0;
		for (int round = 0; round < 6; round++) {
			for (int round1 = 0; round1 < 8; round1++) {
				loc_char = (pc_2[round * 8 + round1] - 1) / 8;
				loc_bit = (pc_2[round * 8 + round1] - 1) % 8;
				if (round1 > loc_bit)
					subkey_2[r - 1][round] |= (test_1[loc_bit] & subkey[r][loc_char]) >> (round1 - loc_bit);
				else
					subkey_2[r - 1][round] |= (test_1[loc_bit] & subkey[r][loc_char]) << (loc_bit - round1);
			}
		}//压缩置换得到48bit的密钥
	}
	
	c_pt = cipher;
	for (int _round = 0; _round < c_len; _round++, c_pt = c_pt + 8) {
		for (int round = 0; round < 8; round++) {
			block_o[round] = *(c_pt + round);
		}//将缓冲池中一个块的大小放入算法加密
		for (int round = 0; round < 8; round++)
			block[round] = (unsigned char)0;

		for (int round = 0; round < 8; round++) {
			for (int round1 = 0; round1 < 8; round1++) {
				loc_char = (ip[round * 8 + round1] - 1) / 8;
				loc_bit = (ip[round * 8 + round1] - 1) % 8;
				if (round1 > loc_bit)
					block[round] |= (test_1[loc_bit] & block_o[loc_char]) >> (round1 - loc_bit);
				else
					block[round] |= (test_1[loc_bit] & block_o[loc_char]) << (loc_bit - round1);
			}
		}//初始IP置换

		for (int r = 0; r < 16; r++) {
			for (int round = 0; round < 4; round++)
				block_ep2[round] = block[4 + round];
			for (int round = 0; round < 6; round++)
				block_epf[round] = (unsigned char)0;
			for (int round = 0; round < 6; round++) {
				for (int round1 = 0; round1 < 8; round1++) {
					loc_char = (e_p[round * 8 + round1] - 1) / 8;
					loc_bit = (e_p[round * 8 + round1] - 1) % 8;
					if (round1 > loc_bit)
						block_epf[round] |= (test_1[loc_bit] & block_ep2[loc_char]) >> (round1 - loc_bit);
					else
						block_epf[round] |= (test_1[loc_bit] & block_ep2[loc_char]) << (loc_bit - round1);
				}
			}//e/p扩展
			
			for (int round = 0; round < 6; round++) {
				block_epf[round] ^= subkey_2[r][round];
			}//48bit子密钥和扩展置换后的数据进行异或

			for (int round = 0; round < 4; round++) {
				loc_char = (2 * 6 * round) / 8;
				loc_bit = (2 * 6 * round) % 8;
				test_2 = (unsigned char)0;
				test_2 |= (test_1[loc_bit] & block_epf[loc_char]) >> (7 - loc_bit - 1);
				loc_char = (2 * 6 * round + 5) / 8;
				loc_bit = (2 * 6 * round + 5) % 8;
				test_2 |= (test_1[loc_bit] & block_epf[loc_char]) >> (7 - loc_bit);
				test_3 = (unsigned char)0;
				temp = 1;
				for (int round1 = 4; round1 > 0; round1--) {
					loc_char = (2 * 6 * round + round1) / 8;
					loc_bit = (2 * 6 * round + round1) % 8;
					if ((test_1[loc_bit] & block_epf[loc_char]) > 0)
						test_3 = (unsigned char)((int)test_3 + temp);
					temp *= 2;
				}
				block_ep2[round] = (unsigned char)s_box[2 * round][(int)test_2][(int)test_3];
				block_ep2[round] = block_ep2[round] << 4;

				loc_char = ((2 * round + 1) * 6) / 8;
				loc_bit = ((2 * round + 1) * 6) % 8;
				test_2 = (unsigned char)0;
				test_2 |= (test_1[loc_bit] & block_epf[loc_char]) >> (7 - loc_bit - 1);
				loc_char = ((2 * round + 1) * 6 + 5) / 8;
				loc_bit = ((2 * round + 1) * 6 + 5) % 8;
				test_2 |= (test_1[loc_bit] & block_epf[loc_char]) >> (7 - loc_bit);
				test_3 = (unsigned char)0;
				temp = 1;
				for (int round1 = 4; round1 > 0; round1--) {
					loc_char = ((2 * round + 1) * 6 + round1) / 8;
					loc_bit = ((2 * round + 1) * 6 + round1) % 8;
					if ((test_1[loc_bit] & block_epf[loc_char]) > 0)
						test_3 = (unsigned char)((int)test_3 + temp);
					temp *= 2;
				}
				block_ep2[round] = (unsigned char)((int)block_ep2[round] + s_box[2 * round + 1][(int)test_2][(int)test_3]);
			}//s盒压缩数据
		
			for (int round = 0; round < 4; round++)
				block_ep1[round] = (unsigned char)0;
			for (int round = 0; round < 4; round++) {
				for (int round1 = 0; round1 < 8; round1++) {
					loc_char = (p[round * 8 + round1] - 1) / 8;
					loc_bit = (p[round * 8 + round1] - 1) % 8;
					if (round1 > loc_bit)
						block_ep1[round] |= (test_1[loc_bit] & block_ep2[loc_char]) >> (round1 - loc_bit);
					else
						block_ep1[round] |= (test_1[loc_bit] & block_ep2[loc_char]) << (loc_bit - round1);
				}
			}//S盒之后的置换P

			for (int round = 0; round < 4; round++) {
				block_ep1[round] ^= block[round];
			}//和左边块进行异或
			for (int round = 0; round < 4; round++) {
				block[round] = block[4 + round];
				block[4 + round] = block_ep1[round];
			}//交换左右两块
		}//16轮迭代加密
		for (int round = 0; round < 4; round++) {
			temp1 = block[round];
			block[round] = block[round + 4];
			block[round + 4] = temp1;
		}
		
		for (int round = 0; round < 8; round++)
			block_cipher[round] = (unsigned char)0;
		for (int round = 0; round < 8; round++) {
			for (int round1 = 0; round1 < 8; round1++) {
				loc_char = (ip_1[round * 8 + round1] - 1) / 8;
				loc_bit = (ip_1[round * 8 + round1] - 1) % 8;
				if (round1 > loc_bit)
					block_cipher[round] |= (test_1[loc_bit] & block[loc_char]) >> (round1 - loc_bit);
				else
					block_cipher[round] |= (test_1[loc_bit] & block[loc_char]) << (loc_bit - round1);
			}
		}//IP逆置换
		
		for (int round = 0; round < 8; round++)
			*(c_pt + round) = block_cipher[round];
	}
	return 0;
}

int decrypt(unsigned char *cipher, unsigned char *key, int c_len) {
	int loc_char = 0;
	int loc_bit = 0;

	unsigned char block_o[8];
	unsigned char block[8];
	unsigned char block_ep2[4];
	unsigned char block_ep1[4];
	unsigned char block_epf[6];
	unsigned char block_cipher[8];

	unsigned char subkey[17][7];
	unsigned char subkey_2[16][6];

	int temp = 1;
	unsigned char temp1;

	unsigned char *c_pt;

	int ip[] = { 58,50,42,34,26,18,10,2,
		60,52,44,36,28,20,12,4,
		62,54,46,38,30,22,14,6,
		64,56,48,40,32,24,16,8,
		57,49,41,33,25,17,9,1,
		59,51,43,35,27,19,11,3,
		61,53,45,37,29,21,13,5,
		63,55,47,39,31,23,15,7 };

	int ip_1[64];
	for (int round = 0; round < 64; round++) {
		for (int round1 = 0; round1 < 64; round1++) {
			if (ip[round1] == round + 1) {
				ip_1[round] = round1 + 1;
			}
		}
	}//IP逆置换
	/*for (int round = 0; round < 64; round++) {
		printf("%d ", ip_1[round]);
		if (round % 8 == 7) {
			printf("\n");
		}
	}*/

	int pc_1[] = { 57,49,41,33,25,17,9,
		1,58,50,42,34,26,18,
		10,2,59,51,43,35,27,
		19,11,3,60,52,44,36,
		63,55,47,39,31,23,15,
		7,62,54,46,38,30,22,
		14,6,61,53,45,37,29,
		21,13,5,28,20,12,4 };

	int pc_2[] = { 14,17,11,24,1,5,
		3,28,15,6,21,10,
		23,19,12,4,26,8,
		16,7,27,20,13,2,
		41,52,31,37,47,55,
		30,40,51,45,33,48,
		44,49,39,56,34,53,
		46,42,50,36,29,32 };

	int lcs[] = { 0,1,2,2,2,2,2,2,1,2,2,2,2,2,2,1 };
	int lc_1[56], lc_2[56];
	lc_1[0] = 28; lc_1[28] = 56;
	lc_2[0] = 27; lc_2[1] = 28;
	lc_2[28] = 55; lc_2[29] = 56;
	for (int round = 55; round > 0; round--) {
		if (round == 28)
			continue;
		lc_1[round] = round;
	}
	for (int round = 55; round > 1; round--) {
		if (round == 29) {
			round--;
			continue;
		}
		lc_2[round] = round - 1;
	}


	int e_p[] = { 32,1,2,3,4,5,
		4,5,6,7,8,9,
		8,9,10,11,12,13,
		12,13,14,15,16,17,
		16,17,18,19,20,21,
		20,21,22,23,24,25,
		24,25,26,27,28,29,
		28,29,30,31,32,1 };

	int s_box[8][4][16] = {                     //8个S盒   三维数组  

	// S1   

	14, 4,  13,  1,  2, 15, 11,  8,  3, 10,  6, 12,  5,  9,  0,  7,

	0, 15,  7,  4, 14,  2, 13,  1, 10,  6, 12, 11,  9,  5,  3,  8,

	4,  1, 14,  8, 13,  6,  2, 11, 15, 12,  9,  7,  3, 10,  5,  0,

	15, 12,  8,  2,  4,  9,  1,  7,  5, 11,  3, 14, 10,  0,  6, 13,

	// S2   

	15,  1,  8, 14,  6, 11,  3,  4,  9,  7,  2, 13, 12,  0,  5, 10,

	3, 13,  4,  7, 15,  2,  8, 14, 12,  0,  1, 10,  6,  9, 11,  5,

	0, 14,  7, 11, 10,  4, 13,  1,  5,  8, 12,  6,  9,  3,  2, 15,

	13,  8, 10,  1,  3, 15,  4,  2, 11,  6,  7, 12,  0,  5, 14,  9,

	// S3   

	10,  0,  9, 14,  6,  3, 15,  5,  1, 13, 12,  7, 11,  4,  2,  8,

	13,  7,  0,  9,  3,  4,  6, 10,  2,  8,  5, 14, 12, 11, 15,  1,

	13,  6,  4,  9,  8, 15,  3,  0, 11,  1,  2, 12,  5, 10, 14,  7,

	1, 10, 13,  0,  6,  9,  8,  7,  4, 15, 14,  3, 11,  5,  2, 12,

	// S4   

	7, 13, 14,  3,  0,  6,  9, 10,  1,  2,  8,  5, 11, 12,  4, 15,

	13,  8, 11,  5,  6, 15,  0,  3,  4,  7,  2, 12,  1, 10, 14,  9,

	10,  6,  9,  0, 12, 11,  7, 13, 15,  1,  3, 14,  5,  2,  8,  4,

	3, 15,  0,  6, 10,  1, 13,  8,  9,  4,  5, 11, 12,  7,  2, 14,

	// S5   

	2, 12,  4,  1,  7, 10, 11,  6,  8,  5,  3, 15, 13,  0, 14,  9,

	14, 11,  2, 12,  4,  7, 13,  1,  5,  0, 15, 10,  3,  9,  8,  6,

	4,  2,  1, 11, 10, 13,  7,  8, 15,  9, 12,  5,  6,  3,  0, 14,

	11,  8, 12,  7,  1, 14,  2, 13,  6, 15,  0,  9, 10,  4,  5,  3,

	// S6   

	12,  1, 10, 15,  9,  2,  6,  8,  0, 13,  3,  4, 14,  7,  5, 11,

	10, 15,  4,  2,  7, 12,  9,  5,  6,  1, 13, 14,  0, 11,  3,  8,

	9, 14, 15,  5,  2,  8, 12,  3,  7,  0,  4, 10,  1, 13, 11,  6,

	4,  3,  2, 12,  9,  5, 15, 10, 11, 14,  1,  7,  6,  0,  8, 13,

	// S7   

	4, 11,  2, 14, 15,  0,  8, 13,  3, 12,  9,  7,  5, 10,  6,  1,

	13,  0, 11,  7,  4,  9,  1, 10, 14,  3,  5, 12,  2, 15,  8,  6,

	1,  4, 11, 13, 12,  3,  7, 14, 10, 15,  6,  8,  0,  5,  9,  2,

	6, 11, 13,  8,  1,  4, 10,  7,  9,  5,  0, 15, 14,  2,  3, 12,

	// S8   

	13,  2,  8,  4,  6, 15, 11,  1, 10,  9,  3, 14,  5,  0, 12,  7,

	1, 15, 13,  8, 10,  3,  7,  4, 12,  5,  6, 11,  0, 14,  9,  2,

	7, 11,  4,  1,  9, 12, 14,  2,  0,  6, 10, 13, 15,  3,  5,  8,

	2,  1, 14,  7,  4, 10,  8, 13, 15, 12,  9,  0,  3,  5,  6, 11

	};

	unsigned char test_2, test_3;

	int p[] = { 16,7,20,21,29,12,28,17,
		1,15,23,26,5,18,31,10,
		2,8,24,14,32,27,3,9,
		19,13,30,6,22,11,4,25 };

	unsigned char test_1[8];
	test_1[0] = (unsigned char)128;
	for (int round = 1; round < 8; round++) {
		test_1[round] = test_1[round - 1] >> 1;
	}
	/*
	for (int round = 0; round < 8; round++) {
		printf("%d ", (int)test_1[round]);
	}
	printf("\n");
	for (int round1 = 0; round1 < 8; round1++) {
		printf("%d ", (int)((test_1[round1] & '1') << (round1 - 8)));
	}*/

	for (int round1 = 0; round1 < 17; round1++)
		for (int round = 0; round < 7; round++)
			subkey[round1][round] = (unsigned char)0;
	for (int round = 0; round < 7; round++) {
		for (int round1 = 0; round1 < 8; round1++) {
			loc_char = (pc_1[round * 8 + round1] - 1) / 8;
			loc_bit = (pc_1[round * 8 + round1] - 1) % 8;
			if (round1 > loc_bit)
				subkey[0][round] |= (test_1[loc_bit] & key[loc_char]) >> (round1 - loc_bit);
			else
				subkey[0][round] |= (test_1[loc_bit] & key[loc_char]) << (loc_bit - round1);
		}
	}//置换选择1生成subkey1
	/*
	for (int round = 0; round < 7; round++) {
		for (int round1 = 0; round1 < 8; round1++) {
			printf("%d ", (int)((test_1[round1] & subkey[0][round]) >> (7 - round1)));
		}
		printf("\n");
	}*/
	for (int r = 1; r < 17; r++) {
		if (lcs[r - 1] == 1) {
			for (int round = 0; round < 7; round++) {
				for (int round1 = 0; round1 < 8; round1++) {
					loc_char = (lc_1[round * 8 + round1] - 1) / 8;
					loc_bit = (lc_1[round * 8 + round1] - 1) % 8;
					if (round1 > loc_bit)
						subkey[r][round] |= (test_1[loc_bit] & subkey[r - 1][loc_char]) >> (round1 - loc_bit);
					else
						subkey[r][round] |= (test_1[loc_bit] & subkey[r - 1][loc_char]) << (loc_bit - round1);
				}
			}
		}//移位得到本轮子密钥
		else if (lcs[r - 1] == 2) {
			for (int round = 0; round < 7; round++) {
				for (int round1 = 0; round1 < 8; round1++) {
					loc_char = (lc_2[round * 8 + round1] - 1) / 8;
					loc_bit = (lc_2[round * 8 + round1] - 1) % 8;
					if (round1 > loc_bit)
						subkey[r][round] |= (test_1[loc_bit] & subkey[r - 1][loc_char]) >> (round1 - loc_bit);
					else
						subkey[r][round] |= (test_1[loc_bit] & subkey[r - 1][loc_char]) << (loc_bit - round1);
				}
			}
		}
		else {
			for (int round = 0; round < 7; round++) {
				subkey[r][round] = subkey[r - 1][round];
			}
		}//移位得到本轮的子密钥
		/*
		for (int round = 0; round < 7; round++) {
			for (int round1 = 0; round1 < 8; round1++) {
				printf("%d ", (int)((test_1[round1] & subkey[r][round]) >> (7 - round1)));
			}
			printf("\n");
		}
		printf("\n");*/
		for (int round = 0; round < 6; round++)
			subkey_2[r - 1][round] = (unsigned char)0;
		for (int round = 0; round < 6; round++) {
			for (int round1 = 0; round1 < 8; round1++) {
				loc_char = (pc_2[round * 8 + round1] - 1) / 8;
				loc_bit = (pc_2[round * 8 + round1] - 1) % 8;
				if (round1 > loc_bit)
					subkey_2[r - 1][round] |= (test_1[loc_bit] & subkey[r][loc_char]) >> (round1 - loc_bit);
				else
					subkey_2[r - 1][round] |= (test_1[loc_bit] & subkey[r][loc_char]) << (loc_bit - round1);
			}
		}//压缩置换得到48bit的密钥
		/*
		for (int round = 0; round < 6; round++) {
			for (int round1 = 0; round1 < 8; round1++) {
				printf("%d ", (int)((test_1[round1] & subkey_2[r - 1][round]) >> (7 - round1)));
			}
			printf("\n");
		}
		printf("\n");*/
	}

	c_pt = cipher;
	for (int _round = 0; _round < c_len; _round++, c_pt = c_pt + 8) {

		for (int round = 0; round < 8; round++) {
			block_o[round] = *(c_pt + round);
		}//将缓冲池中一个块的大小放入算法加密
		for (int round = 0; round < 8; round++)
			block[round] = (unsigned char)0;


		for (int round = 0; round < 8; round++) {
			for (int round1 = 0; round1 < 8; round1++) {
				loc_char = (ip[round * 8 + round1] - 1) / 8;
				loc_bit = (ip[round * 8 + round1] - 1) % 8;
				if (round1 > loc_bit)
					block[round] |= (test_1[loc_bit] & block_o[loc_char]) >> (round1 - loc_bit);
				else
					block[round] |= (test_1[loc_bit] & block_o[loc_char]) << (loc_bit - round1);
			}
		}//初始IP置换
		/*
		for (int round = 0; round < 8; round++) {
			for (int round1 = 0; round1 < 8; round1++) {
				printf("%d ", (int)((test_1[round1] & block[round]) >> (7 - round1)));
			}
			printf("\n");
		}*/

		for (int r = 0; r < 16; r++) {
			for (int round = 0; round < 4; round++)
				block_ep2[round] = block[4 + round];
			for (int round = 0; round < 6; round++)
				block_epf[round] = (unsigned char)0;
			for (int round = 0; round < 6; round++) {
				for (int round1 = 0; round1 < 8; round1++) {
					loc_char = (e_p[round * 8 + round1] - 1) / 8;
					loc_bit = (e_p[round * 8 + round1] - 1) % 8;
					if (round1 > loc_bit)
						block_epf[round] |= (test_1[loc_bit] & block_ep2[loc_char]) >> (round1 - loc_bit);
					else
						block_epf[round] |= (test_1[loc_bit] & block_ep2[loc_char]) << (loc_bit - round1);
				}
			}//e/p扩展
			/*
			for (int round = 0; round < 6; round++) {
				for (int round1 = 0; round1 < 8; round1++) {
					printf("%d ", (int)((test_1[round1] & block_epf[round]) >> (7 - round1)));
				}
				printf("\n");
			}
			printf("\n");*/

			for (int round = 0; round < 6; round++) {
				block_epf[round] ^= subkey_2[r][round];
			}//48bit子密钥和扩展置换后的数据进行异或
			/*
			for (int round = 0; round < 6; round++) {
				for (int round1 = 0; round1 < 8; round1++) {
					printf("%d ", (int)((test_1[round1] & block_epf[round]) >> (7 - round1)));
				}
				printf("\n");
			}
			printf("\n");*/

			for (int round = 0; round < 4; round++) {
				loc_char = (2 * 6 * round) / 8;
				loc_bit = (2 * 6 * round) % 8;
				test_2 = (unsigned char)0;
				test_2 |= (test_1[loc_bit] & block_epf[loc_char]) >> (7 - loc_bit - 1);
				loc_char = (2 * 6 * round + 5) / 8;
				loc_bit = (2 * 6 * round + 5) % 8;
				test_2 |= (test_1[loc_bit] & block_epf[loc_char]) >> (7 - loc_bit);
				test_3 = (unsigned char)0;
				temp = 1;
				for (int round1 = 4; round1 > 0; round1--) {
					loc_char = (2 * 6 * round + round1) / 8;
					loc_bit = (2 * 6 * round + round1) % 8;
					if ((test_1[loc_bit] & block_epf[loc_char]) > 0)
						test_3 = (unsigned char)((int)test_3 + temp);
					temp *= 2;
				}
				block_ep2[round] = (unsigned char)s_box[2 * round][(int)test_2][(int)test_3];
				block_ep2[round] = block_ep2[round] << 4;

				loc_char = ((2 * round + 1) * 6) / 8;
				loc_bit = ((2 * round + 1) * 6) % 8;
				test_2 = (unsigned char)0;
				test_2 |= (test_1[loc_bit] & block_epf[loc_char]) >> (7 - loc_bit - 1);
				loc_char = ((2 * round + 1) * 6 + 5) / 8;
				loc_bit = ((2 * round + 1) * 6 + 5) % 8;
				test_2 |= (test_1[loc_bit] & block_epf[loc_char]) >> (7 - loc_bit);
				test_3 = (unsigned char)0;
				temp = 1;
				for (int round1 = 4; round1 > 0; round1--) {
					loc_char = ((2 * round + 1) * 6 + round1) / 8;
					loc_bit = ((2 * round + 1) * 6 + round1) % 8;
					if ((test_1[loc_bit] & block_epf[loc_char]) > 0)
						test_3 = (unsigned char)((int)test_3 + temp);
					temp *= 2;
				}
				block_ep2[round] = (unsigned char)((int)block_ep2[round] + s_box[2 * round + 1][(int)test_2][(int)test_3]);
			}//s盒压缩数据
			/*
			for (int round = 0; round < 4; round++) {
				for (int round1 = 0; round1 < 8; round1++) {
					printf("%d ", (int)((test_1[round1] & block_ep2[round]) >> (7 - round1)));
				}
				printf("\n");
			}
			printf("\n");*/

			for (int round = 0; round < 4; round++)
				block_ep1[round] = (unsigned char)0;
			for (int round = 0; round < 4; round++) {
				for (int round1 = 0; round1 < 8; round1++) {
					loc_char = (p[round * 8 + round1] - 1) / 8;
					loc_bit = (p[round * 8 + round1] - 1) % 8;
					if (round1 > loc_bit)
						block_ep1[round] |= (test_1[loc_bit] & block_ep2[loc_char]) >> (round1 - loc_bit);
					else
						block_ep1[round] |= (test_1[loc_bit] & block_ep2[loc_char]) << (loc_bit - round1);
				}
			}//S盒之后的置换P
			/*
			for (int round = 0; round < 4; round++) {
				for (int round1 = 0; round1 < 8; round1++) {
					printf("%d ", (int)((test_1[round1] & block_ep1[round]) >> (7 - round1)));
				}
				printf("\n");
			}
			printf("\n");*/

			/*
			for (int round = 0; round < 4; round++) {
				for (int round1 = 0; round1 < 8; round1++) {
					printf("%d ", (int)((test_1[round1] & block[round]) >> (7 - round1)));
				}
				printf("\n");
			}
			printf("\n");*/

			for (int round = 0; round < 4; round++) {
				block_ep1[round] ^= block[round];
			}//和左边块进行异或
			for (int round = 0; round < 4; round++) {
				block[round] = block[4 + round];
				block[4 + round] = block_ep1[round];
			}//交换左右两块
			/*
			for (int round = 0; round < 4; round++) {
				for (int round1 = 0; round1 < 8; round1++) {
					printf("%d ", (int)((test_1[round1] & block_ep1[round]) >> (7 - round1)));
				}
				printf("\n");
			}
			printf("\n");*/
		}//16轮迭代加密
		for (int round = 0; round < 4; round++) {
			temp1 = block[round];
			block[round] = block[round + 4];
			block[round + 4] = temp1;
		}
		/*
		for (int round = 0; round < 8; round++) {
			for (int round1 = 0; round1 < 8; round1++) {
				printf("%d ", (int)((test_1[round1] & block[round]) >> (7 - round1)));
			}
			printf("\n");
		}
		printf("\n");*/

		for (int round = 0; round < 8; round++)
			block_cipher[round] = (unsigned char)0;
		for (int round = 0; round < 8; round++) {
			for (int round1 = 0; round1 < 8; round1++) {
				loc_char = (ip_1[round * 8 + round1] - 1) / 8;
				loc_bit = (ip_1[round * 8 + round1] - 1) % 8;
				if (round1 > loc_bit)
					block_cipher[round] |= (test_1[loc_bit] & block[loc_char]) >> (round1 - loc_bit);
				else
					block_cipher[round] |= (test_1[loc_bit] & block[loc_char]) << (loc_bit - round1);
			}
		}//IP逆置换
		/*
		for (int round = 0; round < 8; round++) {
			for (int round1 = 0; round1 < 8; round1++) {
				printf("%d ", (int)((test_1[round1] & block_cipher[round]) >> (7 - round1)));
			}
			printf("\n");
		}
		printf("\n");*/
		
		for (int round = 0; round < 8; round++)
			*(c_pt + round) = block_cipher[round];
	}

	return 0;
}

int loop_en_de(FILE *inf, FILE *otf, unsigned char *key, int flag) {
	int read_len;//读取的长度
	int block_num;//读取出的满足块大小的块的个数
	unsigned char *in_buf, *out_buf;//设定缓冲池的大小
	in_buf = (unsigned char*)malloc(BUFSIZE * sizeof(unsigned char));
	out_buf = (unsigned char*)malloc(BUFSIZE * sizeof(unsigned char));
	if (flag == 1) {
		//flag=1，加密
		while (1) {
			read_len = fread(in_buf, sizeof(unsigned char), BUFSIZE, inf);
			//读取文件内容到缓冲池，按unsigned char读取

			block_num = read_len / 8;
			encrypt(in_buf, key, block_num);

			fwrite(in_buf, sizeof(unsigned char), read_len, otf);//加密后的数据写入文件

			if (read_len < BUFSIZE)
				break;
		}
	}

	if (flag == 0) {
		//flag=0，解密
		while (1) {
			read_len = fread(in_buf, sizeof(unsigned char), BUFSIZE, inf);
			//读取文件内容到缓冲池，按unsigned char读取

			block_num = read_len / 8;
			decrypt(in_buf, key, block_num);

			fwrite(in_buf, sizeof(unsigned char), read_len, otf);//加密后的数据写入文件

			if (read_len < BUFSIZE)
				break;
		}
	}

	free(in_buf);
	free(out_buf);

	return 0;
}

int main(int argc, char *argv[])
{
	unsigned char *key;
	FILE *inf, *otf;
	key = (unsigned char*)malloc(9 * sizeof(unsigned char));

	if (!strcmp(argv[1], "-h")) {
		printf("--help\n\n");
		printf("Command : [-e] [infile] [outfile]      对文件加密\n");
		printf("Command : [-d] [infile] [outfile]      对文件解密\n");
		return 0;
	}
	if (!strcmp(argv[1], "-e")) {
		inf = fopen(argv[2], "rb");
		if (!inf) {
			fprintf(stderr, "ERROR: InputFile fopen error: %s\n", strerror(errno));
			return errno;
		}
		otf = fopen(argv[3], "wb");
		if (!inf) {
			fprintf(stderr, "ERROR: OutputFile fopen error: %s\n", strerror(errno));
			return errno;
		}
		printf("Input 8 bytes key:\n");
		gets_s((char*)key, 9);
		loop_en_de(inf, otf, key, 1);
	}
	else if (!strcmp(argv[1], "-d")) {
		inf = fopen(argv[2], "rb");
		if (!inf) {
			fprintf(stderr, "ERROR: InputFile fopen error: %s\n", strerror(errno));
			return errno;
		}
		otf = fopen(argv[3], "wb");
		if (!inf) {
			fprintf(stderr, "ERROR: OutputFile fopen error: %s\n", strerror(errno));
			return errno;
		}
		printf("Input 8 bytes key:\n");
		gets_s((char*)key, 9);
		loop_en_de(inf, otf, key, 0);
	}

	return 0;
}
