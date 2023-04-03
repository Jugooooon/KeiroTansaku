#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<limits.h>
#include<windows.h>

/* 処理の動作の定義 */
#define YOMIKOMI 1   // 0：Q値を初期化 1：Q値を外部ファイルから読み込み

/* エージェント関係の定義 */
#define     N       7    /* フィールドの最大マス数 */
#define    NN       5    /* エージェントの視野 自分を中心に NN * NN  */
#define  SEED   32767    /* 乱数のシード */
#define  stepmax   50 /* 目標に到達しない場合何stepで1パターンを終了するか */

/* 学習関係の定義 */
#define move         5     /* 移動のQ値の数 1:上、2:下、3:左、4:右、5:留まる の5種類ある */
#define GENMAX     100     /* 学習の繰り返し回数 */
#define ALPHA      0.1     /* 学習係数 */
#define GAMMA      0.5     /* 割引率 */
#define REWARD    1000     /* 報酬 */
#define PENALTY    -50     /* ペナルティ */
#define YABAI    -1000     /* 大きめのペナルティ */
#define EPSILON    0.1     /* 行動選択のランダム性を決定 */

int A[N][N];   /* フィールドの配置を配列で保管 */
int B[NN][NN]; /* AGENTの視野内の情報を保管 */
int v = 0, w = 0;

void updateA(int s);
void dispA(void);                                                                                 /*  配列Aの中身を表示 */
void printqvalue(int qvalue[N][N][N][N][move]);
void FIELDini(void);                                                                              /* フィールドを好きな形で入力する */
void VIEW(int x, int y);                                                                          /* エージェントの視界 */
void MOVE(int s);                                                                                /*エージェント1を動かす */
void SET();
void Qini( int qvalue[N][N][N][N][move] );
void READ( int qvalue[N][N][N][N][move] );     /* 外部ファイルから読み込み */
void WRITE( int qvalue[N][N][N][N][move] );    /* 外部ファイルへ書き込み */
int HANTEI( int s );  /* 動きの判定 */
int updateq( int s, int qvalue[N][N][N][N][move] );                 /* Q値の更新 */
int selecta( int qvalue[N][N][N][N][move] );                        /* 行動選択 */
int rand03(void);                                                                                 /* 0,1,2,3の中からランダムで出す */
int rand04(void);                                                                                 /* 0,1,2,3,4の中からランダムで出す */
int rand100(void);                                                                                /* 0～100までの整数をランダムで出す */
int rand06(void);
double rand1(void);    


struct AGE{
	int X;   /* X座標 A[Y][X] */
	int Y;   /* Y座標 A[Y][X] */
	int C;   /* 何かを運んでいるか 0:空の状態  1:人を運んでいる状態  2:ガレキを運んでいる状態 */
};

AGE AGENT;

struct COR{
	int X;
	int Y;
};

COR OBJ;  COR OLD;

int main(void)
{
	int i,j,k,l,m,n;
	int M; /* 判定の変数 */
	int s;
	int qvalue[N][N][N][N][move];   /* Q値 [Y][X]に対応する[move]のQ値 */
	
	
   	srand( SEED );
	/* QQの値によって判断(手動) */
	#if YOMIKOMI 
	       READ( qvalue );
	#else 
	       Qini( qvalue );
	
	#endif
	
	for(i=1; i <= GENMAX; i++){   /* GENMAXは学習回数 */
	    	
	    printf("step0\n");
		FIELDini();  SET();
	    
		dispA();
	//	Sleep(0.5*1000);
	    
		j = 1;
		while(j <= stepmax){
			system("cls");
			printf("set%d\n",i);
			printf("%d / %d\n",j, stepmax);
			
			while(1){
				s = selecta( qvalue );    /* 返された値によって処理が変わる 0:上 1:下 2:右 3:左 4:留まる */				
				MOVE( s );
				M = HANTEI( s );
	//			if( M == 2 ) break;
				
				if( AGENT.Y >= 0 && AGENT.Y < N && AGENT.X >= 0 && AGENT.X < N && M == 2) break;
				else{ AGENT.X = OLD.X; AGENT.Y = OLD.Y; }
			}
			
			qvalue[OBJ.Y][OBJ.X][OLD.Y][OLD.X][s] = updateq( s, qvalue );
			
			
			updateA( s );
			
			dispA();
	//		Sleep(0.5*4000);
			printf("\n");
			
			j = j+1;
			if( AGENT.X == OBJ.X && AGENT.Y == OBJ.Y ){ printf("みつけた\n"); break;}
		}
		
	}
	
	
	printf("\nfinish");
	
	/* 外部ファイルにQ値を書き込む */
	WRITE( qvalue );
		
	return(0);
}

                     /*  S 0:上 1:下 2:右 3:左 4:留まる  */
int HANTEI(int s){   /* 返すMの値によって制御する 0 or 1 */
                     /* 2：行動選択完了   1:行動選択継続 */
	
	int M = 0;
	
	switch(s){
		case 0:if( AGENT.Y < 0 ){ M = 1; break;}
		       else if( A[AGENT.Y][AGENT.X] == 1 ){ M = 1; break;}
			   else{ M = 2; break; }
		
		case 1:if( AGENT.Y > N ){ M = 1; break;}
		       else if( A[AGENT.Y][AGENT.X] == 1 ){ M = 1; break;}
			   else{ M = 2; break; }
		
		case 2:if( AGENT.X < 0 ){ M = 1; break;}
		       else if( A[AGENT.Y][AGENT.X] == 1 ){ M = 1; break;}
			   else{ M = 2; break; }
		
		case 3:if( AGENT.X > N ){ M = 1; break;}
		       else if( A[AGENT.Y][AGENT.X] == 1 ){ M = 1; break;}
			   else{ M = 2; break; }
		
		case 4:M = 2; break;
		
	}
	
	return M;
	
}

int updateq(int s, int qvalue[N][N][N][N][move] ){

	int qv, qmax;
	int qs,tmp;	
	int NX, NY, OX, OY;
	int k;
	
	
	NY = abs(OBJ.Y - AGENT.Y); NX = abs(OBJ.X - AGENT.X);
	OY = abs(OBJ.Y - OLD.Y);   OX = abs(OBJ.X - OLD.X);
	
	    if( AGENT.Y == OBJ.Y && AGENT.X == OBJ.X ){ qv = qvalue[OBJ.Y][OBJ.X][OLD.Y][OLD.X][s] + REWARD; }  /* 報酬の付与 */ 
		
		else if( NY <= OY && NX < OX ){  qv = qvalue[OBJ.Y][OBJ.X][OLD.Y][OLD.X][s] + ALPHA * (GAMMA * REWARD - qvalue[OBJ.Y][OBJ.X][OLD.Y][OLD.X][s]); }
		else if( NY < OY && NX <= OX ){  qv = qvalue[OBJ.Y][OBJ.X][OLD.Y][OLD.X][s] + ALPHA * (GAMMA * REWARD - qvalue[OBJ.Y][OBJ.X][OLD.Y][OLD.X][s]); }
		
		else{ qv = qvalue[OBJ.Y][OBJ.X][OLD.Y][OLD.X][s] + ALPHA * (GAMMA * PENALTY - qvalue[OBJ.Y][OBJ.X][OLD.Y][OLD.X][s]); }
		
		printf("%d\nqvalue[%d][%d][%d][%d][%d] = %d\nqv = %d\n",s ,OBJ.Y, OBJ.X, OLD.Y, OLD.X, s, qvalue[OBJ.Y][OBJ.X][OLD.Y][OLD.X][s], qv);
		
		return qv;
}


int selecta(int qvalue[N][N][N][N][move] ){
	
	int s;
	int k;
	int posi[ move ];
	int tmp;
	
	/* ε-greedy法によって行動を選択 */
	/*    ランダム性を持つ行動選択   */
	if( rand1() < EPSILON ){ s = rand04();}
	
	else{
		
		/* 別の配列に数値をいれる */
		for(k = 0; k < move; k++){
			posi[k] = qvalue[OBJ.Y][OBJ.X][AGENT.Y][AGENT.X][k];
//			printf("%d,%d\n",k,posi[k]);
		}
		
		/* 最大値をposi[0]に持ってくる */
		for(k = move-1; k >0; k--){
			
			if( posi[k] >  posi[k-1]){
				
				tmp =  posi[k-1];
				posi[k-1] =  posi[k];
				posi[k] = tmp;
			}
		}
		
		/* Q値の最大値に対応する行動選択の値を見つける */
		for(k = 0 ; k < move; k++){
			
			if( posi[0] ==  qvalue[OBJ.Y][OBJ.X][AGENT.Y][AGENT.X][k] ){ s = k; }
		}
	}
//	printf("%d",s);
	return s;
}


void  MOVE(int s){
	
	OLD.X = AGENT.X; OLD.Y = AGENT.Y;
	
	switch(s){
		
		case 0 : AGENT.Y = AGENT.Y - 1; break;      /* 上に移動 */
		case 1 : AGENT.Y = AGENT.Y + 1; break;      /* 下に移動 */
		case 2 : AGENT.X = AGENT.X + 1; break;      /* 右に移動 */
		case 3 : AGENT.X = AGENT.X - 1; break;      /* 左に移動 */
		case 4 : break;                             /* その場にとどまる */
	}
	
	
}

void Qini( int qvalue[N][N][N][N][move] )
{
	int i,j,k,l,m;
	   	srand( SEED );
	
		/* Q値の初期化 */
	for(i=0; i<N; i++){
		for(j=0; j<N; j++){
			for(k=0; k<N; k++){
				for(l=0; l<N; l++){
					for(m=0; m<move; m++){
						
						qvalue[i][j][k][l][m] = rand100();
					}
				}
			}
		}
	}
	
}
void FIELDini(){ /* 0 = □空白   1 = ■障害物     2 = ×目標     3 = 〇 エージェント  4 = ●荷物運搬中のエージェント */
    
	  A[0][0] = 0;  A[0][1] = 0;  A[0][2] = 0;  A[0][3] = 0;  A[0][4] = 0;  A[0][5] = 0;  A[0][6] = 0;  
	  A[1][0] = 0;  A[1][1] = 1;  A[1][2] = 1;  A[1][3] = 1;  A[1][4] = 0;  A[1][5] = 0;  A[1][6] = 0;  
	  A[2][0] = 0;  A[2][1] = 1;  A[2][2] = 0;  A[2][3] = 0;  A[2][4] = 0;  A[2][5] = 0;  A[2][6] = 0;  
	  A[3][0] = 0;  A[3][1] = 1;  A[3][2] = 0;  A[3][3] = 0;  A[3][4] = 0;  A[3][5] = 1;  A[3][6] = 0;  
  	  A[4][0] = 0;  A[4][1] = 0;  A[4][2] = 0;  A[4][3] = 0;  A[4][4] = 0;  A[4][5] = 1;  A[4][6] = 0;  
	  A[5][0] = 0;  A[5][1] = 0;  A[5][2] = 0;  A[5][3] = 1;  A[5][4] = 1;  A[5][5] = 1;  A[5][6] = 0;  
	  A[6][0] = 0;  A[6][1] = 0;  A[6][2] = 0;  A[6][3] = 0;  A[6][4] = 0;  A[6][5] = 0;  A[6][6] = 0;  
   
}

			


void dispA(){
	int t,u;
	for(t=0; t<N; t++){
		for(u=0; u<N; u++){
			
			if( A[t][u] == 0 ){ printf("□"); }
			
			else if( A[t][u] == 1 ){ printf("■"); }
			
			else if( A[t][u] == 2 ){ printf("×"); }
			
			else if( A[t][u] == 3 ){ printf("〇"); }
			
			else if( A[t][u] == 4 ){ printf("●"); }
			
		}
		printf("\n");
	}
}

void updateA(int s){
	
	//printf("たしかめ X = %d  Y = %d\n",AGENT.X, AGENT.Y);
	
		switch(s){
		
		case 0 : A[AGENT.Y][AGENT.X] = 3;  A[OLD.Y][OLD.X] = 0; break;     /* 上に移動 */
		case 1 : A[AGENT.Y][AGENT.X] = 3;  A[OLD.Y][OLD.X] = 0; break;     /* 下に移動 */
		case 2 : A[AGENT.Y][AGENT.X] = 3;  A[OLD.Y][OLD.X] = 0; break;     /* 右に移動 */
		case 3 : A[AGENT.Y][AGENT.X] = 3;  A[OLD.Y][OLD.X] = 0; break;     /* 左に移動 */
		case 4 : break;                                                    /* その場にとどまる */
	}
	
}

void SET(){
	int i,j;
	
	while(1){ /* エージェントと目標の位置をランダムに配置 */
			
			AGENT.X = rand06(); AGENT.Y = rand06();
			OBJ.X = rand06();   OBJ.Y = rand06();
			
			if( AGENT.X != OBJ.X || AGENT.Y == OBJ.Y ){ if( A[AGENT.Y][AGENT.X] != 1 && A[OBJ.Y][OBJ.X] != 1 )break; }
	}
	A[AGENT.Y][AGENT.X] = 3;
	A[OBJ.Y][OBJ.X] = 2;
	
	OLD.X = AGENT.X; OLD.Y = AGENT.Y;
}

void WRITE( int qvalue[N][N][N][N][move] )
{
	int i,j,k,l,m;
	
		/* 外部ファイルにQ値を書き込む */
	FILE *fp;
	fp = fopen("Qvalue.txt","w");
	
	for(i=0; i<N; i++){
		for(j=0; j<N; j++){
			for(k=0; k<N; k++){
				for(l=0; l<N; l++){
					for(m=0; m<move; m++){
						
						fprintf(fp,"%d\n",qvalue[i][j][k][l][m] );
						
					}
					fprintf(fp,"\n");
				}
			}
		}
	}
	fclose(fp);
}

void READ( int qvalue[N][N][N][N][move] )
{
	int i,j,k,l,m;
	FILE *fp;
	
	fp = fopen("Qvalue.txt","r");
	if( fp == NULL ){ printf("\n読み込み失敗\n");}
	else{
		for(i=0; i<N; i++){
			for(j=0; j<N; j++){
				for(k=0; k<N; k++){
					for(l=0; l<N; l++){
						for(m=0; m<move; m++){
							fscanf(fp, "%d",&qvalue[i][j][k][l][m]);
						}
					}
				}
			}
		}
	}
	fclose(fp);
}
	
		

/* こっからランダムの関数だけ */

int  rand03(){
	int rnd;
	rnd = rand() % 4;
	return rnd;
}

int rand04(){
	int rnd;
	rnd = rand() % 5;
	return rnd;
}

int rand06(){
	int rnd;
	rnd = rand() % 7;
	return rnd;
}

int  rand100(){
	int rnd;
	rnd = rand() % 101;
	return rnd;
}

/* 0～1の実数を返す乱数関数 */
double rand1(){
	/* 乱数の計算 */
	return (double)rand() / RAND_MAX;
}