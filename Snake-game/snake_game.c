#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include <termios.h>
#include<sys/select.h>
struct coordinates{
	int x;
	int y;
};

struct boarder{	
	char **grid;
};
struct snake{
	struct coordinates body[100];
	int length;
	int direction;
};

void grid(struct boarder *gr);
void x_y(struct boarder *gr);
void print_grid(struct boarder *gr);
void food(int *x ,int *y);
void non_block_mode(int Enable);
int kbhit();
void change_direction(int direction,struct coordinates *co);
int update_score(int n);
void game_over(struct boarder *gr);
void move_snake(struct snake *s);

int main(){

	struct boarder gr;
	grid(&gr);
	print_grid(&gr);

	x_y(&gr);

	return 0;

}


void grid(struct boarder *gr){
	
	//struct boarder gr;
	gr->grid = (char**)malloc(10 * sizeof(char*));

	for(int i = 0; i<10; i++){
		gr->grid[i] = (char*)malloc(40);
	}
	for(int i = 0; i<10;i++){
		for(int j = 0; j<40; j++){
			if(j == 0 || j == 39){

				gr->grid[i][j] = '|';
			}
			else if(i == 0 || i == 9){
				gr->grid[i][j] = '-';
			}
			else{
				gr->grid[i][j] =' ';
			}
		}
		printf("\n");

	}

}
			
void x_y(struct boarder *gr){
//	int hit = 0,direction;
	char ch;
/*	char ch;
	int food_x = 0,food_y =0;
	struct coordinates co;
	co.x = 5;
	co.y = 20;
	*/

	struct snake s;
	s.length = 1;
	s.body[0].x = 5;
	s.body[0].y = 20;
	s.direction = 3;

	int food_x = 0, food_y = 0;
	food(&food_x, &food_y);
	non_block_mode(1);
	
	while(1){
//		system("clear");
		if(kbhit()){
			ch = getchar();
			if(ch == 'w')
				s.direction = 0; // up 
			else if(ch == 's')
				s.direction = 1; // down 
			else if(ch == 'a')
				s.direction = 2; // left
			else if(ch == 'd')
				s.direction = 3;
			//change_direction(direction ,&co);
		}
		move_snake(&s);

		// if food eaten
		if(s.body[0].x == food_x && s.body[0].y == food_y){
			update_score(1);
//			if(s.length < 100){
				//s.length++;
//				int old_tail = s.length - 1;                 
        			s.length++;                                   
  //      			s.body[s.length - 1] = s.body[old_tail];
//			}
			food(&food_x,&food_y);
		}

		if(s.body[0].x <= 0 || s.body[0].x >= 9 || s.body[0].y <= 0 || s.body[0].y >= 39){
			game_over(gr);
			non_block_mode(0);
			exit(0);
		}

		for(int i = 1; i<s.length; i++){
			if(s.body[0].x == s.body[i].x && s.body[0].y == s.body[i].y){
				game_over(gr);
				non_block_mode(0);
				exit(0);
			}
		}
		
		for(int i = 1; i<9; i++){
			for(int j = 1; j<39; j++)
				gr->grid[i][j] = ' ';
		}

		gr->grid[food_x][food_y] = '#';

		for(int i = 0; i<s.length; i++){
			int x = s.body[i].x;
			int y = s.body[i].y;
			if(x < 1 || x >8 || y < 1 || y > 38)
				continue;

			if(i == 0){
				gr->grid[x][y] = '0';
			}
			else{
				gr->grid[x][y] = 'o';
			}
		}

		print_grid(gr);

		sleep(1);
	}
	game_over(gr);
	non_block_mode(0);








/*		gr->grid[co.x][co.y] = ' ';
		gr->grid[co.x][co.y] = '0';
		gr->grid[food_x][food_y] = '#';
		print_grid(gr);
		sleep(1);
		if(gr->grind[co.x][co.y] == '#'){
			update_score();
			get_food(&food_x,&food_y);
		}
		if(gr->grid[co.x][co.y] == '|'){
			game_over();
		}
	}
*/

}






void print_grid(struct boarder *gr){
	system("clear");
	for(int i = 0; i<10; i++){
		for(int j = 0; j<40; j++){
			printf("%c",gr->grid[i][j]);
		}
		printf("\n");
	}

}

	
void food(int *x , int *y){
	
	*y = (rand() % 38) + 1;
	*x = (rand() % 8) + 1;
	
//	printf("%d %d\n",random_x,random_y);
	
//	f->grid[random_x][random_y] = '#';

//	printf("%c",f->grid[random_x][random_y]);

}
	
void non_block_mode(int Enable){
	struct termios oldState,newState;
	tcgetattr(0,&oldState);
	newState = oldState;
	
	if(Enable){
		newState.c_lflag &= ~ICANON;// disable buffered info
		newState.c_lflag &= ~ECHO; //  disbale Echo (Do not print in the terminal)
		newState.c_cc[VMIN] = 0;  // take minimum time
		newState.c_cc[VTIME] = 0;
		tcsetattr(0,TCSANOW,&newState);
	}else{
		tcsetattr(0,TCSANOW,&oldState);
	}
}

int kbhit(){
	struct timeval tv = {0L,0L};
	fd_set fds;
	FD_ZERO(&fds);
	FD_SET(0,&fds);
	return select(STDIN_FILENO + 1,&fds,NULL,NULL,&tv);
}

void change_direction(int direction,struct coordinates *co){

	if(direction == 0){
		if(co->x>0){
		    co->x-1;
		}
	}else if(direction == 1){
		if(co->x < 10){
			co->x++;
		}
	}else if(direction == 2){
		if(co->y > 0){
			co->y-1;
		}
	}else if(direction == 3){
		if(co->y < 39){
			co->y++;
		}
	}
}

int update_score(int n){

	static int score = 0;
	score += n;

	return score;
	
}

void game_over(struct boarder *gr){
	
	for(int i = 0; i<10;i++){
		free(gr->grid[i]);
	}
	free(gr->grid);
	printf("Game OVER!!!\n");
	printf("The final score is %d\n",update_score(0));

}

void move_snake(struct snake *sn){

	for(int i = sn->length-1; i>0; i--)
		sn->body[i] = sn->body[i-1];

	
	if(sn->direction == 0)
		sn->body[0].x -= 1;
	else if(sn->direction == 1)
		sn->body[0].x += 1;
	else if(sn->direction == 2)
		sn->body[0].y -= 1;
	else if(sn->direction == 3)
		sn->body[0].y += 1;
	
}

	
	


