#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <time.h>

#include <sys/select.h>
#include <termios.h>
#include <unistd.h>

#define WIDTH		79
#define HEIGHT		24
#define SPAWN		39

#define KEY_A		97
#define KEY_D		100
#define KEY_W		119
#define KEY_S		115
#define KEY_ENTER	13

#define MENU_PLAY	0
#define MENU_STATS  1
#define MENU_HELP   2
#define MENU_EXIT   3

#define ENEMY_X		0
#define ENEMY_Y		1

void initialize();
void key_control(int key);
void menuSelecting();
void gotoxy(int column, int line);
void showHelp();
void showStats();
void playGame();
void showScore();
void enemy();
void clearScreen();
void saveData(int coins);
int getData();
int kbhit();
int getch();

char menu[4][7] = 
{
	"> Play", 
	"  Stats",
	"  Help",
	"  Exit"
};

int enemyPos[2] = 
{
	0,
	0
};

bool ingame = false;
bool instuff = false;
int menu_position = 0;
int last_position = 0;
int coins;
int x = SPAWN;
int timer;

int main(int argc, char *argv[]) {
	initialize();
	srand(time(NULL));
	
	while(1)
	{
		if(kbhit()) key_control(getch());
		sleep(50 - (coins * 5) > 10 ? 50/1000 - (coins * 5)/1000 : 10/1000);
		if(ingame){
			showScore();
			enemy();
		}
	}
	return 0;
}

void initialize(){
	int i, j;
	for(i = 0; i < 4; i++){
		for(j = 0; j < 7; j++){
			printf("%c", menu[i][j]);
		}
		printf("\n");
	}
}

void key_control(int key){
	if(!ingame){
		switch(key){
			case KEY_W: {
				menu_position = menu_position > 0 ? menu_position - 1 : 3;
				menuSelecting();
				break;
			}
			case KEY_S: {
				menu_position = menu_position < 3 ? menu_position + 1 : 0;
				menuSelecting();
				break;
			}
			case KEY_ENTER: {
				switch(menu_position){
					case MENU_PLAY: playGame();
					break;
					
					case MENU_STATS: showStats();
					break;
					
					case MENU_HELP: showHelp();
					break;
					case MENU_EXIT: exit(1);
				}
				break;
			}
		}
	}
	else {
		switch(key){
			case KEY_A:{
				x = x > 0 ? x - 1 : x;
				gotoxy(x, HEIGHT-3);
				putchar('U');
				gotoxy(x+1, HEIGHT-3);
				putchar(' ');
				break;
			}
			case KEY_D:{
				x = x < WIDTH ? x + 1 : x;
				gotoxy(x, HEIGHT-3);
				putchar('U');
				gotoxy(x-1, HEIGHT-3);
				putchar(' ');
				break;
			}
		}
	}
}

void menuSelecting(){
	menu[last_position][0] = ' ';
	menu[menu_position][0] = '>';
	last_position = menu_position;
	
	if(instuff) 
		clearScreen();
	else 
		gotoxy(0,0);
	initialize();
}

void gotoxy(int x,int y)
{
 	printf("%c[%d;%df",0x1B,y,x);
}

void clearScreen(){
	gotoxy(0,0);
	int i;
	for(i = 0; i < WIDTH*HEIGHT; i++){
		putchar(' ');
	}
	gotoxy(0,0);
	instuff = false;
}

void showHelp(){
	clearScreen();
	printf("/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*\n");
	printf("/*/*/*/*/*/*/*GameCoin/*/*/*/*/*/*/*\n");
	printf("/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*\n\n\n");
	printf("Gamae is controlled by A and S. \nYou have to gain as much coins as possible.");
	instuff = true;
}

void showStats(){
	clearScreen();
	printf("------------------------------------\n");
	printf("----------------Stats---------------\n");
	printf("------------------------------------\n\n\n");
	printf("Number of collected coins: %d", getData());
	instuff = true;
}

void playGame(){
	ingame = true;
	clearScreen();
	
	gotoxy(0, HEIGHT-2);
	int i;
	for(i = 0; i < WIDTH; i++){
		putchar('-');
	}
	
	showScore();
}

void showScore(){
	gotoxy(WIDTH/2-5, HEIGHT-1);
	printf("Score: %d", coins);
}

void enemy(){
	if(enemyPos[ENEMY_Y] == 0){
		enemyPos[ENEMY_X] = rand() % WIDTH;
		enemyPos[ENEMY_Y] = 1;
	}
	timer++;
	if(timer == 5) {
		timer = 0;
		gotoxy(enemyPos[ENEMY_X], enemyPos[ENEMY_Y]);
		putchar('O');
		gotoxy(enemyPos[ENEMY_X], enemyPos[ENEMY_Y]-1);
		putchar(' ');
		
		if(enemyPos[ENEMY_X] == x && enemyPos[ENEMY_Y] == HEIGHT-3)
		{
			coins++;
			saveData(coins);
			enemyPos[ENEMY_Y] = 0;
		}
		else{
			if(enemyPos[ENEMY_Y] == HEIGHT-2)
				enemyPos[ENEMY_Y] = 0;
			else enemyPos[ENEMY_Y] += 1;
		}
	}
}

void saveData(int coins){
	FILE *fp;
	if(getData() < coins){
		fp = fopen("stats.dat", "w");
		fprintf(fp, "%d", coins);
		fclose(fp);
	}
}

int getData(){
	FILE *fp;
	fp = fopen("stats.dat", "r");
	int tmp;
	fscanf(fp, "%d", &tmp);
	fclose(fp);
	
	return tmp;
}

int kbhit() {
    static const int STDIN = 0;
    static short initialized = 0;
    static struct termios term;
    static struct termios original;
    
    
    if (! initialized) {
        // Use termios to turn off line buffering
        tcgetattr(STDIN, &term);
        tcgetattr(STDIN, &original);
        
        term.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN, TCSANOW, &term);
        
        setbuf(stdin, NULL);
        initialized = 1;
    }
    
    int bytesWaiting;
    ioctl(STDIN, FIONREAD, &bytesWaiting);
    
    if (bytesWaiting) {
        tcsetattr(STDIN, TCSANOW, &original);
        return bytesWaiting;
    }
    
    return bytesWaiting;
}

int getch() {
	static struct termios mySettings, original;
	char input;

	tcgetattr(STDIN_FILENO, &original);
	mySettings = original;

	mySettings.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &mySettings);

	input = getchar();

	tcsetattr(STDIN_FILENO, TCSANOW, &original);

	return input;
}
