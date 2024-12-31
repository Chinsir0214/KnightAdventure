#include <stdio.h>
#include <conio.h>
#include <graphics.h>
#pragma warning(disable : 4996) //防止对弃用函数getche的警告 
#include <stdlib.h>
#include <time.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")


IMAGE cover;//开始菜单背景
IMAGE end; //结束菜单背景
IMAGE background;//背景
IMAGE self;//自机
IMAGE bull;//子弹
IMAGE enemy;//敌机

int main();

// 载入PNG图并去透明部分
void drawAlpha(int  picture_x, int picture_y, IMAGE* picture) //x为载入图片的X坐标，y为Y坐标
{

	// 变量初始化
	DWORD* dst = GetImageBuffer();    // GetImageBuffer()函数，用于获取绘图设备的显存指针，EASYX自带
	DWORD* draw = GetImageBuffer();
	DWORD* src = GetImageBuffer(picture); //获取picture的显存指针
	int picture_width = picture->getwidth(); //获取picture的宽度，EASYX自带
	int picture_height = picture->getheight(); //获取picture的高度，EASYX自带
	int graphWidth = getwidth();       //获取绘图区的宽度，EASYX自带
	int graphHeight = getheight();     //获取绘图区的高度，EASYX自带
	int dstX = 0;    //在显存里像素的角标

	// 实现透明贴图 公式： Cp=αp*FP+(1-αp)*BP ， 贝叶斯定理来进行点颜色的概率计算
	for (int iy = 0; iy < picture_height; iy++)
	{
		for (int ix = 0; ix < picture_width; ix++)
		{
			int srcX = ix + iy * picture_width; //在显存里像素的角标
			int sa = ((src[srcX] & 0xff000000) >> 24); //0xAArrggbb;AA是透明度
			int sr = ((src[srcX] & 0xff0000) >> 16); //获取RGB里的R
			int sg = ((src[srcX] & 0xff00) >> 8);   //G
			int sb = src[srcX] & 0xff;              //B
			if (ix >= 0 && ix <= graphWidth && iy >= 0 && iy <= graphHeight && dstX <= graphWidth * graphHeight)
			{
				dstX = (ix + picture_x) + (iy + picture_y) * graphWidth; //在显存里像素的角标
				int dr = ((dst[dstX] & 0xff0000) >> 16);
				int dg = ((dst[dstX] & 0xff00) >> 8);
				int db = dst[dstX] & 0xff;
				draw[dstX] = ((sr * sa / 255 + dr * (255 - sa) / 255) << 16)  //公式： Cp=αp*FP+(1-αp)*BP  ； αp=sa/255 , FP=sr , BP=dr
					| ((sg * sa / 255 + dg * (255 - sa) / 255) << 8)         //αp=sa/255 , FP=sg , BP=dg
					| (sb * sa / 255 + db * (255 - sa) / 255);              //αp=sa/255 , FP=sb , BP=db
			}
		}
	}
}

//数据部分
typedef enum
{
	START_GAME,
	END_GAME
} MenuOption;
enum My
{
	Height = 880,
	Width = 600, //窗口大小
	SelfSpeed = 6, //我的速度
	BulletNum = 20, //子弹数量
	BulletSpeed = 20, //子弹速度
	MeBlood = 50, //我的血量
	EnemyBlood = 10, //敌机血量
	EnemyNum = 10, //敌机数量
	EnemySpeed = 1 //敌机速度
};
struct Plane
{
	int x;
	int y;
	int hp;
	int score;
	bool isAlive;
}player, bullet[BulletNum],enem[EnemyNum];

//游戏初始化
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

//图片初始化
void ImgLoad() {
	loadimage(&background, "./resource/act1.png");
	loadimage(&self, "./resource/self.png");
	loadimage(&bull, "./resource/bullet01.png");
	loadimage(&enemy, "./resource/enemy.png");
}

//子弹移动
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

//播放背景音乐
void playBackgroundMusic(const char* musicFile) {
	PlaySound(musicFile, NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
}

//开始菜单
void StartMenu()
{
	// 创建窗口
	initgraph(Width, Height);

	// 开始界面背景图片
	IMAGE backgroundImage;
	loadimage(&backgroundImage, "./resource/cover.png", Width, Height);
	putimage(0, 0, &backgroundImage);

	playBackgroundMusic("./resource/ClashingBladesDuel.wav");

	MenuOption selectedOption = START_GAME; //初始为开始
	bool isMenuActive = true;

	while (isMenuActive)
	{
		cleardevice();
		putimage(0, 0, &backgroundImage);

		if (selectedOption == START_GAME)
		{
			settextcolor(RGB(0, 0, 255));
			settextstyle(50, 0, "楷体");
			setbkmode(0);
			outtextxy(Width / 2 - 250, Height / 3, "开始游戏");
			settextcolor(RGB(128, 128, 128));
			settextstyle(50, 0, "楷体");
			setbkmode(0);
			outtextxy(Width / 2 - 250, Height / 3 + 60, "结束游戏");
		}
		else
		{
			settextcolor(RGB(128, 128, 128));
			settextstyle(50, 0, "楷体");
			setbkmode(0);
			outtextxy(Width / 2 - 250, Height / 3, "开始游戏");
			settextcolor(YELLOW);
			settextstyle(50, 0, "楷体");
			setbkmode(0);
			outtextxy(Width / 2 - 250, Height / 3 + 60, "结束游戏");
		}

		int const key = _getch();
		if (key == 72) { // 上
			selectedOption = START_GAME;
		}
		else if (key == 80) { // 下
			selectedOption = END_GAME;
		}
		else if (key == 13) { // 回车
			isMenuActive = false;
		}
	}

	if (selectedOption == START_GAME)
	{
		printf("开始游戏选项被选中，即将进入游戏...\n");
	}
	else if (selectedOption == END_GAME) {
		printf("结束游戏选项被选中，程序即将退出...\n");
		closegraph();
		exit(0);
	}
}

//生成敌机
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

//敌机移动
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

//游戏绘制
void GameDraw() {
	ImgLoad();
	putimage(0, 0, &background);
	drawAlpha(player.x, player.y, &self);
	//绘制子弹
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
	//绘制敌机
	for (int i = 0;i < EnemyNum;i++) {
		if (enem[i].isAlive)
		{
			drawAlpha(enem[i].x, enem[i].y, &enemy);
		}
	}

	settextcolor(RGB(255, 50, 60));
	settextstyle(20, 0, "黑体");
	setbkmode(0);
	char score[10];
	sprintf(score, "%d", player.score);
	outtextxy(20, 20, score);
}

//生成子弹
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

//万象操作系统
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

//定时器
bool timer(int time) {
	static DWORD t1 = 0, t2 = 0;
	t2 = GetTickCount();
	if (t2 - t1 > time) {
		t1 = t2;
		return true;
	}
	return false;
}

//碰撞检测
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

//战败画面
void EndMenu()
{
	initgraph(Width, Height);
	IMAGE endImage;
	loadimage(&endImage, "./resource/end.png", Width, Height);
	putimage(0, 0, &endImage);

	playBackgroundMusic("./resource/ApocalypseKnight.wav");

	MenuOption selectedOption = START_GAME; //初始为开始
	bool isMenuActive = true;

	while (isMenuActive)
	{
		cleardevice();
		putimage(0, 0, &endImage);

		if (selectedOption == START_GAME)
		{
			settextcolor(RGB(200, 40, 40));
			settextstyle(50, 0, "楷体");
			setbkmode(0);
			outtextxy(Width / 2-100 , Height / 3, "东山再起");
			settextcolor(RGB(128, 128, 128));
			settextstyle(50, 0, "楷体");
			setbkmode(0);
			outtextxy(Width / 2 - 100, Height / 3 + 80, "结束游戏");
		}
		else
		{
			settextcolor(RGB(128, 128, 128));
			settextstyle(50, 0, "楷体");
			setbkmode(0);
			outtextxy(Width / 2 - 100, Height / 3, "东山再起");
			settextcolor(YELLOW);
			settextstyle(50, 0, "楷体");
			setbkmode(0);
			outtextxy(Width / 2 - 100, Height / 3 + 80, "结束游戏");
		}

		int const key = _getch();
		if (key == 72) { // 上
			selectedOption = START_GAME;
		}
		else if (key == 80) { // 下
			selectedOption = END_GAME;
		}
		else if (key == 13) { // 回车
			isMenuActive = false;
		}
	}

	if (selectedOption == START_GAME)
	{
		printf("东山再起选项被选中，即将进入游戏...\n");
		main();
	}
	else if (selectedOption == END_GAME) {
		printf("结束游戏选项被选中，程序即将退出...\n");
		closegraph();
		exit(0);
	}
}



int main() {
	initgraph(Width, Height, 1);
	StartMenu();
	gameInit();

	playBackgroundMusic("./resource/BlazingStrike.wav");
	BeginBatchDraw();//双缓冲机制
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