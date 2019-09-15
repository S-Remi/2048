#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "MT.h"
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define TWO_P 0.9
#define FIRST_DEPTH 2
#define MAX_DEPTH   14
#define SIZE 4
#define SIZE2 SIZE * SIZE
const int size2 = SIZE2;
typedef enum {U, D, L, R} dir;
unsigned long mt_rand(int min, int max){
	return genrand_int32()%(max-min+1)+min;
}
int P_select(double epsilon){
	return epsilon > genrand_real1();
}
int max(int *list, int size){
	int i, max = list[0];
	for(i = 1;i < size;i++)if(max < list[i])max = list[i];
	return max;
}
int c_max(char *list, int size){
	int i, max = list[0];
	for(i = 1;i < size;i++)if(max < list[i])max = list[i];
	return max;
}
int c_argmax(char *list, int size){
	int i, max = list[0], index = 0;
	for(i = 1;i < size;i++)
		if(max < list[i]){
			max = list[i];
			index = i;
		}
	return index;
}
int c_argmax_sp(char *list){
	int index[5] = {0,SIZE-1,SIZE*3,SIZE*4-1,-1};
	int i, max = c_max(list, size2);
	for(i = 0;i < SIZE;i++)
		if(max == list[index[i]])
			break;
	return index[i];
}
int c_zerocount(char *list, int size){
	int i, count = 0;
	for(i = 0;i < size;i++)if(list[i] == 0)count++;
	return count;
}
void Combine(char* board, dir d){
	int i, j, now, next = (d / 2 ? 1 : SIZE) * (d % 2 ? -1 : 1);
	char *buff;
	for(i = 0;i < SIZE;i++){
		buff = NULL;
		for(j = 0;j < SIZE;j++){
			switch(d){
				case U:now = i + j * SIZE;break;
				case D:now = SIZE - 1 - i + j * SIZE;break;
				case L:now = j + i * SIZE;break;
				case R:now = SIZE - 1 - j + i * SIZE;break;
			}
			if(board[now] == 0)continue;
			if(buff == NULL || board[now] != *buff){
				buff = &board[now];
				continue;
			}
			*buff = 0;
			board[now]++;
			buff = NULL;
		}
	}
}
void Gravity(char* board, dir d){
	int i, j, buff, now, next = (d / 2 ? 1 : SIZE) * (d % 2 ? -1 : 1);
	for(i = 0;i < SIZE;i++){
		for(j = 0;j < SIZE-1;j++){
			switch(d){
				case U:now = i + j * SIZE;break;
				case D:now = SIZE * 2 - 1 - i + j * SIZE;break;
				case L:now = j + i * SIZE;break;
				case R:now = SIZE - 1 - j + i * SIZE;break;
			}
			if(board[now] != 0 || (board[now] == 0 && board[now+next] == 0))continue;
			buff = board[now];
			board[now] = board[now+next];
			board[now+next] = buff;
			j = -1;
		}
	}
}
int can_Combine(char* board, dir d){
	int i, j, now, next = (d / 2 ? 1 : SIZE) * (d % 2 ? -1 : 1);
	char *buff;
	for(i = 0;i < SIZE;i++){
		buff = NULL;
		for(j = 0;j < SIZE;j++){
			switch(d){
				case U:now = i + j * SIZE;break;
				case D:now = SIZE - 1 - i + j * SIZE;break;
				case L:now = j + i * SIZE;break;
				case R:now = SIZE - 1 - j + i * SIZE;break;
			}
			if(board[now] == 0)continue;
			if(buff == NULL || board[now] != *buff){
				buff = &board[now];
				continue;
			}
			return 1;
		}
	}
	return 0;
}
int can_Gravity(char* board, dir d){
	int i, j, buff, now, next = (d / 2 ? 1 : SIZE) * (d % 2 ? -1 : 1);
	for(i = 0;i < SIZE;i++){
		for(j = 0;j < SIZE-1;j++){
			switch(d){
				case U:now = i + j * SIZE;break;
				case D:now = SIZE * 2 - 1 - i + j * SIZE;break;
				case L:now = j + i * SIZE;break;
				case R:now = SIZE - 1 - j + i * SIZE;break;
			}
			if(board[now] != 0 || (board[now] == 0 && board[now+next] == 0))continue;
			return 1;
		}
	}
	return 0;
}
void can_move_board(char *board, char* move_list){
	int i, j = 0;
	for(i = 0;i < 4;i++){
		if(can_Gravity(board, i) || can_Combine(board, i)){
			move_list[j] = i;
			j++;
		}
	}
	move_list[j] = -1;
}
void can_puts_board(char *board, char* move_list){
	int i, j = 0;
	for(i = 0;i < size2;i++){
		if(board[i] == 0){
			move_list[j] = i;
			j++;
		}
	}
	move_list[j] = -1;
}
int move_len(char *move){
	int i;
	for(i = 0;move[i] != -1;i++);
	return i;
}
int match(char *board){
	int pos = c_argmax_sp(board), i, a = 0,b = 0,a_next,b_next, a_now, b_now;
	if(pos==0 || pos==SIZE-1 || pos==SIZE*(SIZE-1) || pos==size2-1){
		a_next = pos%SIZE == 0 ? 1 : -1;
		b_next = pos/SIZE == 0 ? SIZE : -SIZE;
		for(i = 0;i < SIZE-1;i++){
			a_now = pos+i*a_next;
			b_now = pos+i*b_next;
			if(a <= 0 && board[a_now + a_next] != 0 && board[a_now] >= board[a_now + a_next])a-=board[a_now + a_next]*board[a_now + a_next]*((SIZE-i)*(SIZE-i))/2;
			else a*=-1;
			if(b <= 0 && board[b_now + b_next] != 0 && board[b_now] >= board[b_now + b_next])b-=board[b_now + b_next]*board[b_now + b_next]*((SIZE-i)*(SIZE-i))/2;
			else b*=-1;
		}
		a *= a < 0 ? -1 : 1;
		b *= b < 0 ? -1 : 1;
	} else {
		a = -ipow(2,board[pos]);
		b = a;
	}
	return (MAX(a, b)+1) * board[pos];
}
int evaluation(char *board, int player, int end_flag){
	int i, j;
	return match(board) * player + c_max(board, size2) * c_zerocount(board,size2) * player - 99999 * player * end_flag;
}
void print_board(char* board);
int ab(char *node,int player,int depth, int a, int b){
	char move[SIZE2] = {0} ,len;
	if(depth == 0)return evaluation(node, player, 0);
	if(player == 1) can_move_board(node, move);
	else can_puts_board(node, move);
	len = move_len(move);
	int re_ab;
	if(len == 0 && player == 1)return evaluation(node, player, 1);
	else if(move_len(move) == 0 && player == -1){
		re_ab = -ab(node, -player, depth - 1, -b, -a);
		a = MAX(a, re_ab);
		if(a >= b)return a;
	}
	int i;
	char c_node[SIZE2] = {0};
	for(i = 0;move[i] != -1;i++){
		memcpy(c_node, node, size2);
		if(player == 1){
			Combine(c_node, move[i]);
			Gravity(c_node, move[i]);
		} else {
			c_node[move[i]] = 1;
			re_ab = -ab(c_node, -player, depth - 1, -b, -a);
			a = MAX(a, re_ab);
			if(a >= b)return a;
			memcpy(c_node, node, size2);
			c_node[move[i]] = 2;
		}
		re_ab = -ab(c_node, -player, depth - 1, -b, -a);
		a = MAX(a, re_ab);
		if(a >= b)return a;
	}
	return a;
}

int ab_move(char *node,int depth, int a, int b){
	char move[SIZE2] = {0} ,len;
	can_move_board(node, move);
	len = move_len(move);
	int re_ab;
	if(len == 0)return evaluation(node, 1, 1);
	int i, buf, best_move;
	char c_node[SIZE2] = {0};
	for(i = 0;move[i] != -1;i++){
		memcpy(c_node, node, size2);
		Combine(c_node, move[i]);
		Gravity(c_node, move[i]);
		buf = -ab(c_node, -1, depth - 1, -b, -a);
		if(a < buf){
			a = buf;
			best_move = move[i];
		}
		if(a >= b)break;
	}
	printf("value : %d\n", a);
	return best_move;
}
// CUI UI ---------------------------------------------
// ※ n^0 = 0
int ipow(int a, int n) {
	if (n == 0) return 0;
	if (n == 1) return a;
	int i, val = 1;
	for(i = 0; i < n; i++)val *= a;
	return val;
}
void print_board(char* board){
	int i;
	for(i = 0;i < size2;i++){
		printf("%7d", ipow(2,board[i]));
		if(i % SIZE == SIZE - 1)puts("");
	}
}
void random_puts(char *board){
	int len;
	char move_list[SIZE2];
	can_puts_board(board, move_list);
	len = move_len(move_list);
	if(len == 0)return;
	board[(int)move_list[(int)mt_rand(0,len-1)]] = P_select(TWO_P) ? 1 : 2;
}
void input_puts(char *board){
	int d, v;
	printf("input pos >>> ");
	scanf("%d", &d);
	printf("input value >>> ");
	scanf("%d", &v);
	board[d] = v;
}
void input_dir(char *board){
	int d;
	printf("input 8,2,4,6 (↑↓←→) >>> ");
	scanf("%d", &d);
	switch(d){
		case 2:d = D;break;
		case 4:d = L;break;
		case 6:d = R;break;
		case 8:d = U;break;
	}
	Combine(board,d);
	Gravity(board,d);
}
void AI_dir(char *board){
	char move_list[SIZE2];
	can_puts_board(board, move_list);
	int len = move_len(move_list);
	int depth = FIRST_DEPTH + c_max(board, size2);
	depth = depth > MAX_DEPTH ? MAX_DEPTH : depth;
	printf("depth : %d\n", depth);
	int d = ab_move(board, depth, -300000, 300000);
	Combine(board,d);
	Gravity(board,d);
	switch(d){
		case D:d = 2;break;
		case L:d = 4;break;
		case R:d = 6;break;
		case U:d = 8;break;
	}
	printf("input 8,2,4,6 (↑↓←→) >>> %d\n", d);
}
void AI_dir_command(char *board){
	char move_list[SIZE2];
	can_puts_board(board, move_list);
	int len = move_len(move_list);
	int depth = FIRST_DEPTH + c_max(board, size2);
	depth = depth > MAX_DEPTH ? MAX_DEPTH : depth;
	int d = ab_move(board, depth, -300000, 300000);
	Combine(board,d);
	Gravity(board,d);
	switch(d){
		case D:printf("d");break;
		case L:printf("l");break;
		case R:printf("r");break;
		case U:printf("u");break;
	}
}
int main(int argc,char *argv[])
{
	char move[SIZE2];
	char board[SIZE2] = {
		0,0,0,0,
		0,0,0,0,
		0,0,0,0,
		0,0,0,0,
	};
	if(argc == 1){
		init_genrand((unsigned)time(NULL));
		random_puts(board);random_puts(board);
		//input_puts(board);input_puts(board);
		printf("2048\n");
		while(1){
			print_board(board);
			can_move_board(board, move);
			if(move_len(move) == 0)break;
			AI_dir(board);
			print_board(board);
			puts("");
			random_puts(board);
		}
		printf("result max_num : %d\n", ipow(2, c_max(board, size2)));
		printf("終了するにはエンターキーを押してください . . .\n");
		getchar();
	} else if(argc == size2+1) {
		int i;
		for(i = 1;i < argc;i++)board[i-1] = atoi(argv[i]);
		AI_dir_command(board);
	}
	return 0;
}