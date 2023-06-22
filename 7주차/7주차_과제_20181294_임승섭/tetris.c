#include "tetris.h"
#include <unistd.h>

static struct sigaction act, oact;

int main(){
	int exit=0;

	initscr();
	noecho();
	keypad(stdscr, TRUE);	

	srand((unsigned int)time(NULL));

	while(!exit){
		clear();
		switch(menu()){
		case MENU_PLAY: play(); break;
		case MENU_EXIT: exit=1; break;
		case MENU_RANK: rank(); break;
		default: break;
		}
	}

	endwin();
	system("clear");
	return 0;
}

void InitTetris(){
	int i,j;

	for(j=0;j<HEIGHT;j++)
		for(i=0;i<WIDTH;i++)
			field[j][i]=0;

	nextBlock[0]=rand()%7;
	nextBlock[1]=rand()%7;
	nextBlock[2] = rand()%7;
	blockRotate=0;
	blockY=-1;
	blockX=WIDTH/2-2;
	score=0;	
	gameOver=0;
	timed_out=0;

	DrawOutline();
	DrawField();
	DrawBlockWithFeatures(blockY,blockX,nextBlock[0],blockRotate);
	DrawNextBlock(nextBlock);
	PrintScore(score);
}

void DrawOutline(){	
	int i,j;
	/* 블럭이 떨어지는 공간의 테두리를 그린다.*/
	DrawBox(0,0,HEIGHT,WIDTH);

	/* next block을 보여주는 공간의 테두리를 그린다.*/
	move(2,WIDTH+10);
	printw("NEXT BLOCK");
	DrawBox(3,WIDTH+10,4,8);
	DrawBox(9, WIDTH+10, 4, 8); // next next block

	/* score를 보여주는 공간의 테두리를 그린다.*/
	move(15,WIDTH+10);
	printw("SCORE");
	DrawBox(16,WIDTH+10,1,8);
}

int GetCommand(){
	int command;
	command = wgetch(stdscr);
	switch(command){
	case KEY_UP:
		break;
	case KEY_DOWN:
		break;
	case KEY_LEFT:
		break;
	case KEY_RIGHT:
		break;
	case ' ':	/* space key*/
		/*fall block*/
		break;
	case 'q':
	case 'Q':
		command = QUIT;
		break;
	default:
		command = NOTHING;
		break;
	}
	return command;
}

int ProcessCommand(int command){
	int ret=1;
	int drawFlag=0;
	switch(command){
	case QUIT:
		ret = QUIT;
		break;
	case KEY_UP:
		if((drawFlag = CheckToMove(field,nextBlock[0],(blockRotate+1)%4,blockY,blockX)))
			blockRotate=(blockRotate+1)%4;
		break;
	case KEY_DOWN:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY+1,blockX)))
			blockY++;
		break;
	case KEY_RIGHT:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY,blockX+1)))
			blockX++;
		break;
	case KEY_LEFT:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY,blockX-1)))
			blockX--;
		break;
	default:
		break;
	}
	if(drawFlag) DrawChange(field,command,nextBlock[0],blockRotate,blockY,blockX);
	return ret;	
}

void DrawField(){
	int i,j;
	for(j=0;j<HEIGHT;j++){
		move(j+1,1);
		for(i=0;i<WIDTH;i++){
			if(field[j][i]==1){
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			}
			else printw(".");
		}
	}
}

void PrintScore(int score){
	move(17,WIDTH+11);
	printw("%8d",score);
}

void DrawNextBlock(int *nextBlock){
	int i, j;
	for( i = 0; i < 4; i++ ){
		move (4+i, WIDTH+13);
		for (j = 0; j < 4; j++) {
			if( block[nextBlock[1]][0][i][j] == 1 ){
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			}
			else printw(" ");
		}
		move (10+i, WIDTH + 13);
		for (j = 0; j < 4; j++) {
			if (block[nextBlock[2]][0][i][j] == 1) {
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			}
			else printw(" ");
		}
	}
}

void DrawBlock(int y, int x, int blockID,int blockRotate,char tile){
	int i,j;
	for(i=0;i<4;i++)
		for(j=0;j<4;j++){
			if(block[blockID][blockRotate][i][j]==1 && i+y>=0){
				move(i+y+1,j+x+1);
				attron(A_REVERSE);
				printw("%c",tile);
				attroff(A_REVERSE);
			}
		}

	move(HEIGHT,WIDTH+10);
}

void DrawBox(int y,int x, int height, int width){
	int i,j;
	move(y,x);
	addch(ACS_ULCORNER);
	for(i=0;i<width;i++)
		addch(ACS_HLINE);
	addch(ACS_URCORNER);
	for(j=0;j<height;j++){
		move(y+j+1,x);
		addch(ACS_VLINE);
		move(y+j+1,x+width+1);
		addch(ACS_VLINE);
	}
	move(y+j+1,x);
	addch(ACS_LLCORNER);
	for(i=0;i<width;i++)
		addch(ACS_HLINE);
	addch(ACS_LRCORNER);
}

void play(){
	int command;
	clear();
	act.sa_handler = BlockDown;
	sigaction(SIGALRM,&act,&oact);
	InitTetris();
	do{
		if(timed_out==0){
			alarm(1);
			timed_out=1;
		}

		command = GetCommand();
		if(ProcessCommand(command)==QUIT){
			alarm(0);
			DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
			move(HEIGHT/2,WIDTH/2-4);
			printw("Good-bye!!");
			refresh();
			getch();

			return;
		}
	}while(!gameOver);

	alarm(0);
	getch();
	DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
	move(HEIGHT/2,WIDTH/2-4);
	printw("GameOver!!");
	refresh();
	getch();
	newRank(score);
}

char menu(){
	printw("1. play\n");
	printw("2. rank\n");
	printw("3. recommended play\n");
	printw("4. exit\n");
	return wgetch(stdscr);
}

// 6th sil
int CheckToMove(char f[HEIGHT][WIDTH],int currentBlock,int blockRotate, int blockY, int blockX){

	for (int i = 0; i < BLOCK_HEIGHT; i++) {
		for (int j = 0; j < BLOCK_WIDTH; j++) {
			if (block[currentBlock][blockRotate][i][j] == 1) {
				if (blockY + i >= HEIGHT) // block is upper than field Height
					return 0;
				else if (blockX + j < 0) // block is futher to the left than field Width
					return 0;
				else if (blockX + j >= WIDTH) // block is futher to the right than field Width
					return 0;
				else if (f[blockY+i][blockX+j] == 1) // field is already filled with another block
					return 0;
			}
		}
	}
	return 1; // Everything okay. Block can move
}

void DrawChange(char f[HEIGHT][WIDTH],int command,int currentBlock,int blockRotate, int blockY, int blockX){
	//1. 이전 블록 정보를 찾는다. ProcessCommand의 switch문을 참조할 것
	//2. 이전 블록 정보를 지운다. DrawBlock함수 참조할 것.
	//3. 새로운 블록 정보를 그린다. 

	//1. Find the previous block information
	int c_Rotate = blockRotate;
	int c_blockY = blockY;
	int c_blockX = blockX;

	switch(command) {
		case KEY_UP:
			c_Rotate = (c_Rotate+3)%4; // before rotate
			break;
		case KEY_DOWN:
			c_blockY--; // before down
			break;
		case KEY_RIGHT:
			c_blockX--; // before right
			break;
		case KEY_LEFT:
			c_blockX++; // before left
			break;
		default:
			break;
	}

	// decide the height of previous block shadow
	int shadow_y = c_blockY;
	while (1) {
		if (CheckToMove(field, currentBlock, c_Rotate, shadow_y+1, c_blockX))
			shadow_y++;
		else
			break;
	}

	//2. Erase the previous block ( + Erase the previous block shadow too. (6th hw) )
	for (int i = 0; i < BLOCK_HEIGHT; i++) {
		for (int j = 0; j < BLOCK_WIDTH; j++) {
			if ((block[currentBlock][c_Rotate][i][j] == 1) && (i + c_blockY>=0) ){
				move(c_blockY + i + 1, c_blockX + j + 1);
				printw("."); // erase block
				move(shadow_y + i + 1, c_blockX + j + 1);
				printw("."); // erase shadow
			}
		}
	}

	//3. Draw the new block and block shadow.
	DrawBlockWithFeatures(blockY, blockX, currentBlock, blockRotate);

	// Move the cursor
	move(HEIGHT, WIDTH+10);
}

void BlockDown(int sig){

	if (CheckToMove(field, nextBlock[0], blockRotate, blockY+1, blockX) == 0) { // block cannot go down
		if (blockY == -1) // block is upper than field height
			gameOver = 1; // gameover
		else { 
			score += AddBlockToField(field, nextBlock[0], blockRotate, blockY, blockX); // add block to field and raise the score
			score += DeleteLine(field); // delete the full line and raise the score
			PrintScore(score); // print the final score on the screen

			// change the block information
			nextBlock[0] = nextBlock[1]; 
			nextBlock[1] = nextBlock[2];
			nextBlock[2] = rand()%7;
			
			// initialize the block position
			blockRotate = 0;
			blockY = -1;
			blockX = (WIDTH/2) - 2;

			// Draw the blocks on the screen
			DrawNextBlock(nextBlock);
		}
		DrawField();
	}
	else { // block can go down
		blockY++; // y_position down
		DrawChange(field, KEY_DOWN, nextBlock[0], blockRotate, blockY, blockX);
	}
	timed_out = 0;
}

int AddBlockToField(char f[HEIGHT][WIDTH],int currentBlock,int blockRotate, int blockY, int blockX){

	int touched = 0; // Number of tangent part
	for (int i = 0; i < BLOCK_HEIGHT; i++) {
		for (int j = 0; j < BLOCK_WIDTH; j++) {
			if (block[currentBlock][blockRotate][i][j] == 1) { // current block is filled
				if (f[blockY + i + 1][blockX + j] == 1) // right below the field is filled
					touched++; // raise the number of tangent part
				
				f[blockY+i][blockX+j] = 1; // change the field information
			}
		}
	}
	int score = touched * 10; // raise the score
	return score; // return score
}

int DeleteLine(char f[HEIGHT][WIDTH]) {

	int DeleteLine = 0; // Number of lines to be deleted
	bool DeleteCheck = 0; // Variable to check if the line is deleted.

	for (int i = 0; i < HEIGHT; i++) {
		DeleteCheck = 1; // change the value to true.
		for (int j = 0; j < WIDTH; j++) {
			if (f[i][j] == 0 ) { // Not filled
				DeleteCheck = 0; // change the value to false, again.
				break;
			}
		}

		if (DeleteCheck == 1) { // the line is filld completely
			DeleteLine++; // raise the number of lines to be deleted
			for (int p = i; p > 0; p--) {
				for (int q = 0; q < WIDTH; q++) {
					f[p][q] = f[p-1][q]; // lower the lines one by one
				}
			}
		}
	}

	return (DeleteLine)*(DeleteLine) * 100; // return the score
}

// 6th hw
void DrawShadow(int y, int x, int blockID,int blockRotate){
	
	// decide the position of block shadow
	int shadow_y = y;
	while(1) {
		if (CheckToMove(field, blockID, blockRotate, shadow_y+1, blockX))
			shadow_y ++;
		else
			break;
	}

	// Draw the shadow
	DrawBlock(shadow_y, blockX, blockID, blockRotate, '/');
}

void DrawBlockWithFeatures(int y, int x, int blockID, int blockRotate) {

	DrawBlock(blockY, blockX, blockID, blockRotate, ' '); // Draw block
	DrawShadow(blockY, blockX, blockID, blockRotate); // Draw shadow
}



// 7th sil
void createRankList() {
	// 목적: Input파일인 "rank.txt"에서 랭킹 정보를 읽어들임, 읽어들인 정보로 랭킹 목록 생성
	// 1. "rank.txt"열기
	// 2. 파일에서 랭킹정보 읽어오기
	// 3. LinkedList로 저장
	// 4. 파일 닫기

	HEAD = (Node*)malloc(sizeof(Node)); // allocate memory to HEAD node

	//1. 파일 열기
	FILE* fp;
	fp = fopen("rank.txt", "r"); // open file for reading
	if (fp == NULL)
		printf("File Open Error\n"); // print the error

	// 2. 정보읽어오기
	/* int fscanf(FILE* stream, const char* format, ...);
	stream:데이터를 읽어올 스트림의 FILE 객체를 가리키는 파일포인터
	format: 형식지정자 등등
	변수의 주소: 포인터
	return: 성공할 경우, fscanf 함수는 읽어들인 데이터의 수를 리턴, 실패하면 EOF리턴 */
	// EOF(End Of File): 실제로 이 값은 -1을 나타냄, EOF가 나타날때까지 입력받아오는 if문

	int num; // Number of score information in file.
	char name[NAMELEN+1]; // ranking name
	int data; // ranking score
	Node* curr = HEAD; // first, curr is HEAD node

	if (fscanf(fp, "%d", &num) != EOF) {

		if (num != 0) {
			rank_num = num; // rank_num(global variable) is num(Number of score in file)

			for (int i = 0; i < num; i++) {
				Node* newnode = (Node*)malloc(sizeof(Node)); // memory allocation for new node

				fscanf(fp, "%s %d", name, &(data)); // scan the name and score in file
				strcpy(newnode->name, name); // new node's name
				newnode->data = data; // new node's score
				newnode->next = NULL; // new node's next is NULL

				curr->next = newnode; // curr's next is newnode
				curr = curr->next; // move curr to newnode
			}
		}
	}

	// 4. 파일닫기
	fclose(fp); // close file
}

void rank() {
	
	//목적: rank 메뉴를 출력하고 점수 순으로 X부터~Y까지 출력함

	createRankList(); // read the ranking system 

	//1. 문자열 초기화
	int X=-1, Y=-1; // initialize X and Y -1
	char ch;
	clear();

	//2. printw()로 3개의 메뉴출력
	printw("1. list ranks from X to Y\n");
	printw("2. list ranks by a specific name\n");
	printw("3. delete a specific rank\n");

	//3. wgetch()를 사용하여 변수 ch에 입력받은 메뉴번호 저장
	ch = wgetch(stdscr);

	//4. 각 메뉴에 따라 입력받을 값을 변수에 저장
	//4-1. 메뉴1: X, Y를 입력받고 적절한 input인지 확인 후(X<=Y), X와 Y사이의 rank 출력
	if (ch == '1') {
		echo(); // show the input message on the screen
		printw("X : ");
		scanw("%d", &X); // get input X
		printw("Y : ");
		scanw("%d", &Y); // get input Y
		noecho(); // blind the input message on the screen
		printw("     name     |   score   \n");
		printw("--------------------------\n");

		int count = 0;
		Node* curr = HEAD; // first curr is HEAD node

		if ( X != -1 && Y != -1 && X > Y) { // if new X is greater than new Y
			printw("search failure : no rank in the list\n"); // cannot find
		}
		else if ( X > rank_num) // if X is greater than the rank_num
			printw("search failure : no rank in the list\n"); // cannot find

		// X, Y 입력하지 않은 경우 (1 ~ rank_num 출력)
		else if (X == -1 && Y == -1) {
			while (count < rank_num && curr->next != NULL) {
				curr = curr->next; // move curr node to final
				printw(" %-18s | %-10d\n", curr->name, curr->data); // print the information
				count++;
			}
		}

		// X 입력하지 않은 경우 (Y는 입력) (1 ~ Y 출력)
		else if (X == -1 && Y != -1) {
			while (count < Y && curr->next != NULL) {
				curr = curr->next; // move curr node to Yth
				printw(" %-18s | %-10d\n", curr->name, curr->data); // print the information
				count++;
			}
		}

		// Y 입력하지 않은 경우 (X는 입력) (X ~ rank_num 출력)
		else if (Y == -1 && X != -1) {
			while (count < X) {
				curr = curr->next; // first, move curr node to Xth
				count ++;
			}
			while (count <= rank_num && curr != NULL) { // 주의!! curr부터 출력!!
				printw(" %-18s | %-10d\n", curr->name, curr->data); // print the information
				count++;
				curr = curr->next; // move curr node to final
			}
		}

		// X, Y 정상적으로 입력한 경우 (X ~ Y 출력)
		else if (X != -1 && Y != -1) {
			while (count < X) {
				curr = curr->next; // first, move curr node to Xth
				count++;
			}
			while (count <= Y && curr != NULL) { // 주의!! curr부터 출력!!
				printw(" %-18s | %-10d\n", curr->name, curr->data); // print the information
				count++;
				curr = curr->next; // move curr node to Yth
			}
		}
	}
	
	
	//4-2. 메뉴2: 문자열을 받아 저장된 이름과 비교하고 이름에 해당하는 리스트를 출력
	else if (ch == '2') {
		char str[NAMELEN + 1];
		int check = 0;

		echo(); // show the input message on the screen
		printw("Input the name : ");
		scanw("%s", str); // get input the name
		noecho(); // blind the input message on the screen
		printw("     name     |   score   \n");
		printw("--------------------------\n");

		Node *curr2 = HEAD->next; // first, curr2 node is HEAD->next (the first node which has score)
		while (curr2 != NULL) {
			if (!strcmp(curr2->name, str)) { // if curr2 node's name is equal to input name
				printw(" %-18s | %-10d\n", curr2->name, curr2->data); // print the name and score
				check++; // counting the number of input name in rank
			}
			curr2 = curr2->next; // move curr2 node to next
		}

		if (check == 0) { // the number is 0 (i.e. there is no name in the rank list)
			printw("search failure : no name in the list\n"); // print the message
		}
	}

	
	//4-3. 메뉴3: rank번호를 입력받아 리스트에서 삭제
	else if (ch == '3') {
		int num;
		echo(); // show the input message on the screen
		printw("Input the rank : "); 
		scanw("%d", &num); // get input the ranking
		noecho(); // blind the input message on the screen

		if (num > rank_num || num <= 0) // if the input ranking is greater than the number of rank or lower than 0
			printw("search failure : the rank not in the list\n"); // cannot find the rank

		else {
			Node* prev3 = HEAD; // first, prev3 node is HEAD
			for (int i = 0; i < num - 1; i++) {
				prev3 = prev3->next; // move prev3 node to right before the node be deleted
			}
			Node* curr3 = prev3->next; // curr3 node is the node be deleted
			prev3->next = curr3->next; // prev3 node's next become curr3 node's next
			free(curr3); // free memory of curr3
			rank_num--; // rank_num down 1

			printw("result : the rank deleted\n"); // finish message
			writeRankFile(); // rewrite the information in "rank.txt"
		}
	}
	
	getch();
}

void writeRankFile() {
	int sn, i;

	FILE* fp = fopen("rank.txt", "w"); // open file for writing
	fscanf(fp, "%d", &sn); // read the number of score in fil

	Node *curr = HEAD->next; // first, curr node is HEAD node

	fprintf(fp, "%d\n", rank_num); // rewriting the information
	while (curr != NULL) {
		fprintf(fp, "%s %d\n", curr->name, curr->data); // rewriting the information
		curr = curr->next;	// move the curr node to final
	}

	fclose(fp); //close file
}

void newRank(int score) {
	// 목적: GameOver시 호출되어 사용자 이름을 입력받고 score와 함께 리스트의 적절한 위치에 저장
	
	createRankList(); // read the ranking system

	char str[NAMELEN + 1]; // for the new name
	clear();

	//1. 사용자 이름을 입력받음
	echo(); // show the input message on the screen
	printw("your name : ");
	scanw("%s", str); // get input new name
	noecho(); // blind the input message on the screen


	//2. 새로운 노드를 생성해 이름과 점수를 저장, score_number가
	Node *newnode = (Node*)malloc(sizeof(Node)); // memory allocation for new node
	strcpy(newnode->name, str); // new node's name
	newnode->data = score; // new node's score
	newnode->next = NULL; // new node's next is NULL

	Node *prev = HEAD; // first, prev node is HEAD node
	Node *curr = prev->next; // first, curr node is HEAD->next node

	if (curr == NULL) {
		HEAD->next = newnode; // if HEAD->next is NULL, then new node should be there
	}

	else {
		while (curr != NULL) { // find the position for new node by comparing the score of nodes.
			if (newnode->data > curr->data) { // if new node's score is greater than curr node's score, break
				prev->next = newnode; // new node is between prev node and curr node
				newnode->next = curr;
				break;
			}
			else { // move prev and curr node
				prev = prev->next;
				curr = curr->next;
			}
		}

		if (curr == NULL) { // if curr move to the final, then new node should be there
			prev->next = newnode;
		}
	}
	rank_num++;
	writeRankFile();
}







/////////////////////////////////////////////////////
/*


void DrawRecommend(int y, int x, int blockID,int blockRotate){
	// user code
}

int recommend(RecNode *root){
	int max=0; // 미리 보이는 블럭의 추천 배치까지 고려했을 때 얻을 수 있는 최대 점수

	// user code

	return max;
}

void recommendedPlay(){
	// user code
}


*/