#include <stdio.h>
#include <conio.h>
#include <graphics.h>
#pragma warning(disable : 4996) //��ֹ�����ú���getche�ľ��� 
#include <stdlib.h>
#include <time.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")


IMAGE cover;//��ʼ�˵�����
IMAGE end; //�����˵�����
IMAGE background;//����
IMAGE self;//�Ի�
IMAGE bull;//�ӵ�
IMAGE enemy;//�л�

int main();

// ����PNGͼ��ȥ͸������
void drawAlpha(int  picture_x, int picture_y, IMAGE* picture) //xΪ����ͼƬ��X���꣬yΪY����
{

	// ������ʼ��
	DWORD* dst = GetImageBuffer();    // GetImageBuffer()���������ڻ�ȡ��ͼ�豸���Դ�ָ�룬EASYX�Դ�
	DWORD* draw = GetImageBuffer();
	DWORD* src = GetImageBuffer(picture); //��ȡpicture���Դ�ָ��
	int picture_width = picture->getwidth(); //��ȡpicture�Ŀ�ȣ�EASYX�Դ�
	int picture_height = picture->getheight(); //��ȡpicture�ĸ߶ȣ�EASYX�Դ�
	int graphWidth = getwidth();       //��ȡ��ͼ���Ŀ�ȣ�EASYX�Դ�
	int graphHeight = getheight();     //��ȡ��ͼ���ĸ߶ȣ�EASYX�Դ�
	int dstX = 0;    //���Դ������صĽǱ�

	// ʵ��͸����ͼ ��ʽ�� Cp=��p*FP+(1-��p)*BP �� ��Ҷ˹���������е���ɫ�ĸ��ʼ���
	for (int iy = 0; iy < picture_height; iy++)
	{
		for (int ix = 0; ix < picture_width; ix++)
		{
			int srcX = ix + iy * picture_width; //���Դ������صĽǱ�
			int sa = ((src[srcX] & 0xff000000) >> 24); //0xAArrggbb;AA��͸����
			int sr = ((src[srcX] & 0xff0000) >> 16); //��ȡRGB���R
			int sg = ((src[srcX] & 0xff00) >> 8);   //G
			int sb = src[srcX] & 0xff;              //B
			if (ix >= 0 && ix <= graphWidth && iy >= 0 && iy <= graphHeight && dstX <= graphWidth * graphHeight)
			{
				dstX = (ix + picture_x) + (iy + picture_y) * graphWidth; //���Դ������صĽǱ�
				int dr = ((dst[dstX] & 0xff0000) >> 16);
				int dg = ((dst[dstX] & 0xff00) >> 8);
				int db = dst[dstX] & 0xff;
				draw[dstX] = ((sr * sa / 255 + dr * (255 - sa) / 255) << 16)  //��ʽ�� Cp=��p*FP+(1-��p)*BP  �� ��p=sa/255 , FP=sr , BP=dr
					| ((sg * sa / 255 + dg * (255 - sa) / 255) << 8)         //��p=sa/255 , FP=sg , BP=dg
					| (sb * sa / 255 + db * (255 - sa) / 255);              //��p=sa/255 , FP=sb , BP=db
			}
		}
	}
}

//���ݲ���
typedef enum
{
	START_GAME,
	END_GAME
} MenuOption;
enum My
{
	Height = 880,
	Width = 600, //���ڴ�С
	SelfSpeed = 6, //�ҵ��ٶ�
	BulletNum = 20, //�ӵ�����
	BulletSpeed = 20, //�ӵ��ٶ�
	MeBlood = 50, //�ҵ�Ѫ��
	EnemyBlood = 10, //�л�Ѫ��
	EnemyNum = 10, //�л�����
	EnemySpeed = 1 //�л��ٶ�
};
struct Plane
{
	int x;
	int y;
	int hp;
	int score;
	bool isAlive;
}player, bullet[BulletNum],enem[EnemyNum];

//��Ϸ��ʼ��
void gameInit() {
	player.x = 250;
	player.y = 780;
	player.score = 100;
	player.isAlive = true;
	for (int i = 0;i < BulletNum;i++) {
		bullet[i].isAlive = false;
		bullet[i].x = 0;
		bullet[i].y = 0;
	}
	for (int i = 0;i < EnemyNum;i++) {
		enem[i].x = 0;
		enem[i].y = 0;
		enem[i].hp = 1;
		enem[i].isAlive = false;
	}
}

//ͼƬ��ʼ��
void ImgLoad() {
	loadimage(&background, "./resource/act1.png");
	loadimage(&self, "./resource/self.png");
	loadimage(&bull, "./resource/bullet01.png");
	loadimage(&enemy, "./resource/enemy.png");
}

//�ӵ��ƶ�
void bulletMove(int BulletSpeed) {
	for (int i = 0;i < BulletNum;i++) {
		if (bullet[i].isAlive && bullet[i].y > 21) {
			bullet[i].y -= BulletSpeed;
			if (bullet[i].y < 0) {
				bullet[i].isAlive = false;
			}
		}
	}
}

//���ű�������
void playBackgroundMusic(const char* musicFile) {
	PlaySound(musicFile, NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
}

//��ʼ�˵�
void StartMenu()
{
	// ��������
	initgraph(Width, Height);

	// ��ʼ���汳��ͼƬ
	IMAGE backgroundImage;
	loadimage(&backgroundImage, "./resource/cover.png", Width, Height);
	putimage(0, 0, &backgroundImage);

	playBackgroundMusic("./resource/ClashingBladesDuel.wav");

	MenuOption selectedOption = START_GAME; //��ʼΪ��ʼ
	bool isMenuActive = true;

	while (isMenuActive)
	{
		cleardevice();
		putimage(0, 0, &backgroundImage);

		if (selectedOption == START_GAME)
		{
			settextcolor(RGB(0, 0, 255));
			settextstyle(50, 0, "����");
			setbkmode(0);
			outtextxy(Width / 2 - 250, Height / 3, "��ʼ��Ϸ");
			settextcolor(RGB(128, 128, 128));
			settextstyle(50, 0, "����");
			setbkmode(0);
			outtextxy(Width / 2 - 250, Height / 3 + 60, "������Ϸ");
		}
		else
		{
			settextcolor(RGB(128, 128, 128));
			settextstyle(50, 0, "����");
			setbkmode(0);
			outtextxy(Width / 2 - 250, Height / 3, "��ʼ��Ϸ");
			settextcolor(YELLOW);
			settextstyle(50, 0, "����");
			setbkmode(0);
			outtextxy(Width / 2 - 250, Height / 3 + 60, "������Ϸ");
		}

		int const key = _getch();
		if (key == 72) { // ��
			selectedOption = START_GAME;
		}
		else if (key == 80) { // ��
			selectedOption = END_GAME;
		}
		else if (key == 13) { // �س�
			isMenuActive = false;
		}
	}

	if (selectedOption == START_GAME)
	{
		printf("��ʼ��Ϸѡ�ѡ�У�����������Ϸ...\n");
	}
	else if (selectedOption == END_GAME) {
		printf("������Ϸѡ�ѡ�У����򼴽��˳�...\n");
		closegraph();
		exit(0);
	}
}

//���ɵл�
void createEnemy() {
	for (int i = 0; i < EnemyNum; i++)
	{
		if (!enem[i].isAlive) {
			enem[i].isAlive = true;
			enem[i].x = rand() % (Width - 80);
			enem[i].y = 0;
			break;
		}
	}
}

//�л��ƶ�
void enemyMove(){
	for (int i = 0; i < EnemyNum; i++)
	{
		if (enem[i].isAlive && enem[i].y < Height-100) {
			enem[i].y += EnemySpeed;
			if (enem[i].y >= Height-100) {
				enem[i].isAlive = false;
				player.score -= 10;
			}
		}
	}
}

//��Ϸ����
void GameDraw() {
	ImgLoad();
	putimage(0, 0, &background);
	drawAlpha(player.x, player.y, &self);
	//�����ӵ�
	for (int i = 0;i < BulletNum;i++) {
		if (player.isAlive)
		{
			drawAlpha(bullet[i].x, bullet[i].y, &bull);
		}
		if (bullet[i].y < 21)
		{
			bullet[i].isAlive = false;
		}
	}
	//���Ƶл�
	for (int i = 0;i < EnemyNum;i++) {
		if (enem[i].isAlive)
		{
			drawAlpha(enem[i].x, enem[i].y, &enemy);
		}
	}

	settextcolor(RGB(255, 50, 60));
	settextstyle(20, 0, "����");
	setbkmode(0);
	char score[10];
	sprintf(score, "%d", player.score);
	outtextxy(20, 20, score);
}

//�����ӵ�
void createBullet() {
	for (int i = 0; i < BulletNum; i++)
	{
		if (!bullet[i].isAlive) {
			bullet[i].isAlive = true;
			bullet[i].x = player.x + 20;
			bullet[i].y = player.y - 52;
			break;
		}
	}
}

//�������ϵͳ
void playerMove(int SelfSpeed) {
	if ((GetAsyncKeyState(VK_UP) || GetAsyncKeyState('W')) && player.y > 7) {
		if (GetAsyncKeyState(VK_SHIFT) && player.y > 13) {
			player.y -= SelfSpeed * 2;
		}
		else {
			player.y -= SelfSpeed;
		}
	}
	if ((GetAsyncKeyState(VK_DOWN) || GetAsyncKeyState('S')) && player.y < Height - 100) {
		if (GetAsyncKeyState(VK_SHIFT)) {
			player.y += SelfSpeed * 2;
		}
		else {
			player.y += SelfSpeed;
		}
	}
	if ((GetAsyncKeyState(VK_LEFT) || GetAsyncKeyState('A')) && player.x > -20) {
		if (GetAsyncKeyState(VK_SHIFT)) {
			player.x -= SelfSpeed * 2;
		}
		else {
			player.x -= SelfSpeed;
		}
	}
	if ((GetAsyncKeyState(VK_RIGHT) || GetAsyncKeyState('D')) && player.x < Width - 80) {
		if (GetAsyncKeyState(VK_SHIFT)) {
			player.x += SelfSpeed * 2;
		}
		else {
			player.x += SelfSpeed;
		}
	}
	static DWORD t1 = 0, t2 = 0;
	if (GetAsyncKeyState(VK_SPACE)&& t2 - t1 > 100) {
			createBullet();
			player.score -= 5;
			t1 = t2;
	}
	t2 = GetTickCount();
}

//��ʱ��
bool timer(int time) {
	static DWORD t1 = 0, t2 = 0;
	t2 = GetTickCount();
	if (t2 - t1 > time) {
		t1 = t2;
		return true;
	}
	return false;
}

//��ײ���
void hit() {
	for (int i = 0; i < BulletNum; i++)
	{
		if (bullet[i].isAlive) {
			for (int j = 0; j < EnemyNum; j++)
			{
				if (enem[j].isAlive) {
					if (bullet[i].x > enem[j].x && bullet[i].x < enem[j].x + 80 && bullet[i].y > enem[j].y && bullet[i].y < enem[j].y + 80) {
						bullet[i].isAlive = false;
						enem[j].hp--;
						if (enem[j].hp <= 0) {
							enem[j].isAlive = false;
							player.score += 10;
						}
					}
				}
			}
		}
	}
}

//ս�ܻ���
void EndMenu()
{
	initgraph(Width, Height);
	IMAGE endImage;
	loadimage(&endImage, "./resource/end.png", Width, Height);
	putimage(0, 0, &endImage);

	playBackgroundMusic("./resource/ApocalypseKnight.wav");

	MenuOption selectedOption = START_GAME; //��ʼΪ��ʼ
	bool isMenuActive = true;

	while (isMenuActive)
	{
		cleardevice();
		putimage(0, 0, &endImage);

		if (selectedOption == START_GAME)
		{
			settextcolor(RGB(200, 40, 40));
			settextstyle(50, 0, "����");
			setbkmode(0);
			outtextxy(Width / 2-100 , Height / 3, "��ɽ����");
			settextcolor(RGB(128, 128, 128));
			settextstyle(50, 0, "����");
			setbkmode(0);
			outtextxy(Width / 2 - 100, Height / 3 + 80, "������Ϸ");
		}
		else
		{
			settextcolor(RGB(128, 128, 128));
			settextstyle(50, 0, "����");
			setbkmode(0);
			outtextxy(Width / 2 - 100, Height / 3, "��ɽ����");
			settextcolor(YELLOW);
			settextstyle(50, 0, "����");
			setbkmode(0);
			outtextxy(Width / 2 - 100, Height / 3 + 80, "������Ϸ");
		}

		int const key = _getch();
		if (key == 72) { // ��
			selectedOption = START_GAME;
		}
		else if (key == 80) { // ��
			selectedOption = END_GAME;
		}
		else if (key == 13) { // �س�
			isMenuActive = false;
		}
	}

	if (selectedOption == START_GAME)
	{
		printf("��ɽ����ѡ�ѡ�У�����������Ϸ...\n");
		main();
	}
	else if (selectedOption == END_GAME) {
		printf("������Ϸѡ�ѡ�У����򼴽��˳�...\n");
		closegraph();
		exit(0);
	}
}



int main() {
	initgraph(Width, Height, 1);
	StartMenu();
	gameInit();

	playBackgroundMusic("./resource/BlazingStrike.wav");
	BeginBatchDraw();//˫�������
	while (1){
		GameDraw();
		FlushBatchDraw();
		playerMove(SelfSpeed);
		bulletMove(BulletSpeed);
		if (timer(1000)) {
			createEnemy();
		}
		enemyMove();
		hit();
		if (player.score <= 0) {
			break;
		}
	};
	EndBatchDraw();
	EndMenu();

	return 0;
}