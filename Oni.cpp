#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<limits.h>
#include<windows.h>

/* �����̓���̒�` */
#define YOMIKOMI 1   // 0�FQ�l�������� 1�FQ�l���O���t�@�C������ǂݍ���

/* �G�[�W�F���g�֌W�̒�` */
#define     N       7    /* �t�B�[���h�̍ő�}�X�� */
#define    NN       5    /* �G�[�W�F���g�̎��� �����𒆐S�� NN * NN  */
#define  SEED   32767    /* �����̃V�[�h */
#define  stepmax   50 /* �ڕW�ɓ��B���Ȃ��ꍇ��step��1�p�^�[�����I�����邩 */

/* �w�K�֌W�̒�` */
#define move         5     /* �ړ���Q�l�̐� 1:��A2:���A3:���A4:�E�A5:���܂� ��5��ނ��� */
#define GENMAX     100     /* �w�K�̌J��Ԃ��� */
#define ALPHA      0.1     /* �w�K�W�� */
#define GAMMA      0.5     /* ������ */
#define REWARD    1000     /* ��V */
#define PENALTY    -50     /* �y�i���e�B */
#define YABAI    -1000     /* �傫�߂̃y�i���e�B */
#define EPSILON    0.1     /* �s���I���̃����_���������� */

int A[N][N];   /* �t�B�[���h�̔z�u��z��ŕۊ� */
int B[NN][NN]; /* AGENT�̎�����̏���ۊ� */
int v = 0, w = 0;

void updateA(int s);
void dispA(void);                                                                                 /*  �z��A�̒��g��\�� */
void printqvalue(int qvalue[N][N][N][N][move]);
void FIELDini(void);                                                                              /* �t�B�[���h���D���Ȍ`�œ��͂��� */
void VIEW(int x, int y);                                                                          /* �G�[�W�F���g�̎��E */
void MOVE(int s);                                                                                /*�G�[�W�F���g1�𓮂��� */
void SET();
void Qini( int qvalue[N][N][N][N][move] );
void READ( int qvalue[N][N][N][N][move] );     /* �O���t�@�C������ǂݍ��� */
void WRITE( int qvalue[N][N][N][N][move] );    /* �O���t�@�C���֏������� */
int HANTEI( int s );  /* �����̔��� */
int updateq( int s, int qvalue[N][N][N][N][move] );                 /* Q�l�̍X�V */
int selecta( int qvalue[N][N][N][N][move] );                        /* �s���I�� */
int rand03(void);                                                                                 /* 0,1,2,3�̒����烉���_���ŏo�� */
int rand04(void);                                                                                 /* 0,1,2,3,4�̒����烉���_���ŏo�� */
int rand100(void);                                                                                /* 0�`100�܂ł̐����������_���ŏo�� */
int rand06(void);
double rand1(void);    


struct AGE{
	int X;   /* X���W A[Y][X] */
	int Y;   /* Y���W A[Y][X] */
	int C;   /* �������^��ł��邩 0:��̏��  1:�l���^��ł�����  2:�K���L���^��ł����� */
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
	int M; /* ����̕ϐ� */
	int s;
	int qvalue[N][N][N][N][move];   /* Q�l [Y][X]�ɑΉ�����[move]��Q�l */
	
	
   	srand( SEED );
	/* QQ�̒l�ɂ���Ĕ��f(�蓮) */
	#if YOMIKOMI 
	       READ( qvalue );
	#else 
	       Qini( qvalue );
	
	#endif
	
	for(i=1; i <= GENMAX; i++){   /* GENMAX�͊w�K�� */
	    	
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
				s = selecta( qvalue );    /* �Ԃ��ꂽ�l�ɂ���ď������ς�� 0:�� 1:�� 2:�E 3:�� 4:���܂� */				
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
			if( AGENT.X == OBJ.X && AGENT.Y == OBJ.Y ){ printf("�݂���\n"); break;}
		}
		
	}
	
	
	printf("\nfinish");
	
	/* �O���t�@�C����Q�l���������� */
	WRITE( qvalue );
		
	return(0);
}

                     /*  S 0:�� 1:�� 2:�E 3:�� 4:���܂�  */
int HANTEI(int s){   /* �Ԃ�M�̒l�ɂ���Đ��䂷�� 0 or 1 */
                     /* 2�F�s���I������   1:�s���I���p�� */
	
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
	
	    if( AGENT.Y == OBJ.Y && AGENT.X == OBJ.X ){ qv = qvalue[OBJ.Y][OBJ.X][OLD.Y][OLD.X][s] + REWARD; }  /* ��V�̕t�^ */ 
		
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
	
	/* ��-greedy�@�ɂ���čs����I�� */
	/*    �����_���������s���I��   */
	if( rand1() < EPSILON ){ s = rand04();}
	
	else{
		
		/* �ʂ̔z��ɐ��l������� */
		for(k = 0; k < move; k++){
			posi[k] = qvalue[OBJ.Y][OBJ.X][AGENT.Y][AGENT.X][k];
//			printf("%d,%d\n",k,posi[k]);
		}
		
		/* �ő�l��posi[0]�Ɏ����Ă��� */
		for(k = move-1; k >0; k--){
			
			if( posi[k] >  posi[k-1]){
				
				tmp =  posi[k-1];
				posi[k-1] =  posi[k];
				posi[k] = tmp;
			}
		}
		
		/* Q�l�̍ő�l�ɑΉ�����s���I���̒l�������� */
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
		
		case 0 : AGENT.Y = AGENT.Y - 1; break;      /* ��Ɉړ� */
		case 1 : AGENT.Y = AGENT.Y + 1; break;      /* ���Ɉړ� */
		case 2 : AGENT.X = AGENT.X + 1; break;      /* �E�Ɉړ� */
		case 3 : AGENT.X = AGENT.X - 1; break;      /* ���Ɉړ� */
		case 4 : break;                             /* ���̏�ɂƂǂ܂� */
	}
	
	
}

void Qini( int qvalue[N][N][N][N][move] )
{
	int i,j,k,l,m;
	   	srand( SEED );
	
		/* Q�l�̏����� */
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
void FIELDini(){ /* 0 = ����   1 = ����Q��     2 = �~�ڕW     3 = �Z �G�[�W�F���g  4 = ���ו��^�����̃G�[�W�F���g */
    
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
			
			if( A[t][u] == 0 ){ printf("��"); }
			
			else if( A[t][u] == 1 ){ printf("��"); }
			
			else if( A[t][u] == 2 ){ printf("�~"); }
			
			else if( A[t][u] == 3 ){ printf("�Z"); }
			
			else if( A[t][u] == 4 ){ printf("��"); }
			
		}
		printf("\n");
	}
}

void updateA(int s){
	
	//printf("�������� X = %d  Y = %d\n",AGENT.X, AGENT.Y);
	
		switch(s){
		
		case 0 : A[AGENT.Y][AGENT.X] = 3;  A[OLD.Y][OLD.X] = 0; break;     /* ��Ɉړ� */
		case 1 : A[AGENT.Y][AGENT.X] = 3;  A[OLD.Y][OLD.X] = 0; break;     /* ���Ɉړ� */
		case 2 : A[AGENT.Y][AGENT.X] = 3;  A[OLD.Y][OLD.X] = 0; break;     /* �E�Ɉړ� */
		case 3 : A[AGENT.Y][AGENT.X] = 3;  A[OLD.Y][OLD.X] = 0; break;     /* ���Ɉړ� */
		case 4 : break;                                                    /* ���̏�ɂƂǂ܂� */
	}
	
}

void SET(){
	int i,j;
	
	while(1){ /* �G�[�W�F���g�ƖڕW�̈ʒu�������_���ɔz�u */
			
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
	
		/* �O���t�@�C����Q�l���������� */
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
	if( fp == NULL ){ printf("\n�ǂݍ��ݎ��s\n");}
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
	
		

/* �������烉���_���̊֐����� */

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

/* 0�`1�̎�����Ԃ������֐� */
double rand1(){
	/* �����̌v�Z */
	return (double)rand() / RAND_MAX;
}