#include<stdio.h>
#include<stdlib.h>
typedef struct Sudoku {
	int d[11][9][9];// d[0]是主盘，d[10],是标记是否入队,d[i]是数字i的可能位置盘
	int left[10]; //left[0]是整体还有几个格子没有填， left[i]记录数字i还有几个没有填。
} Sudoku;

typedef struct point {
	int x,y,v;
} Point;

typedef struct pnode {
	Point data;
	struct pnode *next;
} PNode, *PList;

typedef struct {
	PList front, rear;
} PQueue;



void InitPQ(PQueue *q) {
	q->front = q->rear = (PList)malloc(sizeof(PNode));
	q->front->next = NULL;
}
int EmptyQ(PQueue q) {
	return q.front == q.rear;
}


void EnQ(PQueue *q, Point x) {
	q->rear->next = (PList)malloc(sizeof(PNode));
	q->rear->next->data = x;
	q->rear->next->next = NULL;
	q->rear = q->rear->next;
}
void DeQ(PQueue *q, Point *x) {
	PNode *p = q->front->next;
	*x = p->data;
	q->front->next = q->front->next->next;
	if(q->front->next == NULL) {
		q->rear = q->front;
	}
	free(p);
}

void DestroyPQ(PQueue *q) {
	Point x;
	while(!EmptyQ(*q)) {
		DeQ(q, &x);
	}
	free(q->front);
	q->front = q->rear = NULL;
}

void PrintPoint(Point p) {
	printf("r = %d, c = %d, v= %d\n", p.x, p.y, p.v);
}


void ShowQ(PQueue q) {
	PNode *p = q.front->next;
	while(p) {
		PrintPoint(p->data);
		p = p->next;
	}
}



int InitSudoku(Sudoku *s) {
	int i,j,k;
	for(k = 0; k < 11; k++) {
		for(j = 0; j < 9; j++) {
			for(i = 0; i < 9; i++) {
				s->d[k][i][j] = k;
			}
		}
		s->left[k] = 9;
	}
	s->left[0] = 81;
	return 0;
}

int show(Sudoku s) {
	int i,j;
	for(j = 0; j < 9; j++) {
		for(i = 0; i < 9; i++) {
			printf("%d ",s.d[0][j][i]);
			if(i%3 == 2) {
				printf(" ");
			}
		}
		if(j%3 == 2) {
			printf("\n");
		}
		printf("\n");
	}
	printf("************************************\n");
	return 1;
}

int showall(Sudoku s) {
	int i,j,k;
	for(k = 0; k < 10; k++) {
		for(j = 0; j < 9; j++) {
			for(i = 0; i < 9; i++) {
				printf("%d ",s.d[k][j][i]);
				if(i%3 == 2) {
					printf(" ");
				}
			}
			if(j%3 == 2) {
				printf("\n");
			}
			printf("\n");
		}
	}
	return 1;
}

int clearrow(Sudoku *s, int r, int v) { // 将数字v的第r行的可能性全部清空。r从0开始数。
	int i;
	for(i = 0; i < 9; i++) {
		s->d[v][r][i] = 0;
	}
	return 1;
}

int clearcol(Sudoku *s, int c, int v) { // 将数字v的第c行的可能性全部清空。c从0开始数。
	int i;
	for(i = 0; i < 9; i++) {
		s->d[v][i][c] = 0;
	}
	return 1;
}

int cleargong(Sudoku *s, int g, int v) { // 将数字v的第g宫的可能性全部清空。g从0开始数。
	int i,j,rs = g/3*3, cs = g%3*3, re = rs + 3, ce = cs + 3;
	for(i = rs ; i < re ; i++) {
		for(j = cs; j < ce; j++) {
			s->d[v][i][j] = 0;
		}
	}
	return 1;
}

int clearpos(Sudoku *s, int r, int c) { // 将数字v的第c行的可能性全部清空。c从0开始数。
	int i;
	for(i = 1; i < 10; i++) {
		s->d[i][r][c] = 0;
	}
	return 1;
}

int onlyrow(Sudoku s, int r, int v, int *c) { // 检查数字v的第r行是否只剩下一种可能。r从0开始数。
	int i, f = 0;
	for(i = 0; i < 9; i++) {
		if(s.d[v][r][i] != 0) {
			if(f > 0) {
				return 0;
			}
			*c = i;
			f++;
		}
	}
	return f;
}

int onlycol(Sudoku s, int c, int v, int *r) { // 检查数字v的第c行是否只剩下一种可能。c从0开始数。
	int i, f = 0;
	for(i = 0; i < 9; i++) {
		if(s.d[v][i][c] != 0) {
			if(f > 0) {
				return 0;
			}
			*r = i;
			f++;
		}
	}
	return f;
}

int onlygong(Sudoku s, int g, int v, int *r, int *c) { // 检查数字v是否在第g宫唯一。g从0开始数。
	int i,j,rs = g/3*3, cs = g%3*3, re = rs + 3, ce = cs + 3,f = 0;
	for(i = rs ; i < re ; i++) {
		for(j = cs; j < ce; j++) {
			if(s.d[v][i][j] != 0) {
				if(f > 0) {
					return 0;
				}
				*r = i;
				*c = j;
				f++;
			}
		}
	}
	return f;
}

int onlypos(Sudoku s, int r, int c, int *v){//检查指定位置是否只有一种可能。
	int i, f = 0;
	for(i = 1; i < 10; i++) {
		if(s.d[i][r][c] != 0) {
			if(f > 0) {
				return 0;
			}
			*v = i;
			f++;
		}
	}
	return f; 
}
int fill(Sudoku *s, int r, int c, int v) {
	if(s->d[v][r][c] == 0) {
		return -1;//指定位置被要求填入不可能的值
	}
	if(s->d[0][r][c] != 0) {
		return -2;//指定位置已经被填入过值
	}
	s->d[0][r][c] = v;
	clearrow(s, r, v);
	clearcol(s, c, v);
	cleargong(s,r/3*3+c/3,v);
	clearpos(s, r, c);
	s->left[v]--;
	s->left[0]--;
	return 1;//填入成功
}

int find(Sudoku s, PQueue *q) {
	int i, v, r, c, rv;
	Point p;
	for (v = 1; v < 10; v++) {
		p.v = v;
		//printf("v = %d:\n",v);
		for(i = 0; i < 9; i++) {
			//printf("\ti = %d:\n",i);
			//printf("\t\tisr = %d",onlyrow(s,i,v,&c));
			//printf(", sd10 = %d:\n", s.d[10][i][c]);
			if(onlyrow(s,i,v,&c) && s.d[10][i][c]) {
				s.d[10][i][c] = 0;
				p.x = i;
				p.y = c;
				//printf("row\n");
				EnQ(q, p);
			}
			//printf("\t\tisc = %d",onlycol(s,i,v,&r));
			//printf(", sd10 = %d:\n", s.d[10][r][i]);
			if(onlycol(s,i,v,&r) && s.d[10][r][i]) {
				s.d[10][r][i] = 0;
				p.x = r;
				p.y = i;
				//printf("col\n");
				EnQ(q, p);

			}
			//printf("\t\tisg = %d",onlygong(s,i,v,&r,&c));
			//printf(", sd10 = %d:\n", s.d[10][r][c]);
			if(onlygong(s,i,v,&r,&c) && s.d[10][r][c]){
				s.d[10][r][c] = 0;
				p.x = r;
				p.y = c;
				//printf("gong\n");
				EnQ(q, p);
			}
			if(onlypos(s,v-1,i,&rv) && s.d[10][v-1][i]) {
				s.d[10][v-1][i] = 0;
				p.x = v-1;
				p.y = i;
				p.v = rv;
				//printf("position\n");
				EnQ(q, p);
			}
			
		}
	}
}



int main() {
	Sudoku a;
	PQueue q;
	FILE *fp;
	Point p;
	int r,c,v,n, t=1;
	InitSudoku(&a);
	InitPQ(&q);
	fp = fopen("quizn010.txt","r");
	fscanf(fp,"%d", &n);
	while(n--) {
		fscanf(fp,"%d %d %d", &r, &c, &v);
		fill(&a, r, c, v);
		a.d[10][r][c] = 0;
	}
	fclose(fp);
	/*n = onlypos(a,0,0,&v);
	printf("n = %d, v = %d.\n", n, v);
	n = onlygong(a,0,1,&r,&c);
	printf("n = %d, r = %d, c = %d.\n", n, r, c);
	fill(&a, 5, 7, 9);
	fill(&a, 7, 4, 9);
	fill(&a, 3, 2, 6);
	fill(&a, 7, 5, 6);
	showall(a);*/
	find(a, &q);
	while(!EmptyQ(q)) {
		while(!EmptyQ(q)) {
			DeQ(&q, &p);
			fill(&a, p.x, p.y, p.v);
			/*printf("第%d步:r = %d, c = %d, v = %d\n", t, p.x, p.y, p.v);
			t++;
			show(a);*/
		}
		find(a, &q);
		//ShowQ(q);
	}
	showall(a);
	ShowQ(q);
	return 0;
}


