#include <stdio.h>//C语言标准库
#include <math.h>//数学函数需要
#include <time.h>//time()函数需要
#include <conio.h>//_kbhit()函数需要
#include <string.h>//字符串处理需要
#include <graphics.h>//easyx图形库
#include <mmsystem.h>//包含多媒体设备头文件
#pragma comment (lib,"winmm.lib")//加载静态库

//对按钮创建函数进行声明
struct Button* CreatButton(int x, int y, int width, int height,
	const char* text, int dx, int dy, COLORREF buttoncolor, COLORREF textcolor);

#define WIDTH 640//窗口宽度
#define HEIGHT 720//窗口高读
#define PHOTO_NUM 8//图片数量
#define PI 3.1415926//定义π
#define SMALL_GOLD_NUM 5//小金子数量
#define BIG_GOLD_NUM 3//大金子数量
#define STONE_NUM 3//石头数量
#define SACK_NUM 4//袋子数量

struct Button{//按钮
	int x;//左上角x坐标
	int y;//左上角y坐标
	int width;//按钮宽度
	int height;//按钮高度
	char* text;//按钮内文字
	int dx;//文字相对于按钮左上角x坐标的偏移值
	int dy;//文字相对于按钮左上角y坐标的偏移值
	COLORREF buttoncolor;//按钮颜色
	COLORREF textcolor;//按钮内文字颜色
};

struct Role{//人物
	int x;//人物x坐标
	int y;//人物y坐标
	int width;//人物图片宽度
	int height;//人物图片高度
}role;

struct Mine{//物品
	double x;//物品x坐标
	double y;//物品y坐标
	int width;//物品宽度
	int height;//物品高度
	double weight;//物品重量
	bool flag;//判断物品是否还存在
	int value;//物品价值
}smallgold[SMALL_GOLD_NUM], biggold[BIG_GOLD_NUM], stone[STONE_NUM], sack[SACK_NUM];

struct Hook{//钩子
	double start_x;//钩子起始位置x坐标
	double start_y;//钩子起始位置y坐标
	double len;//钩子长度
	double end_x;//钩子结束位置x坐标
	double end_y;//钩子结束位置y坐标
	double radian;//钩子与水平面弧度
	int dir;//钩子移动方向，左，右
	bool rockflag;//状态标识，用于判断钩子是否移动
	bool flexflag;//状态标识，用于判断钩子是否伸缩
}hook;

int cas;//控制人物动画
double tempcas;//动画的控制
int aim;//记录目标得分
int coin;//记录金币得分
IMAGE bk;//存放背景图片
IMAGE ka;//存放文字图片
IMAGE kb;//存放文字图片
IMAGE rolen[2][2];//存放人物图片
IMAGE img[PHOTO_NUM];//存放游戏图片
int imgIndex[PHOTO_NUM] = { 0, 1, 2, 3, 4, 5, 6, 7 };//记录图片地址下标
struct Button* beginGame = CreatButton(80, 500, 150, 50, "START", 38, 12, RGB(210, 152, 64), RGB(240, 240, 240));
struct Button* endGame = CreatButton(420, 500, 150, 50, "END", 52, 12, RGB(210, 152, 64), RGB(240, 240, 240));

//加载资源
void LoadResource(){
	//批量加载图片
	for (int i = 0; i < PHOTO_NUM; i++){
		char fileName[25];
		sprintf(fileName, "./picture/%d.jpg", imgIndex[i]);
		loadimage(img + i, fileName);
	}
	//加载背景图片
	loadimage(&bk, "./picture/bk.jpg", WIDTH*1, HEIGHT*0.85);
	//加载人物图片
	loadimage(rolen[0], "./picture/role1.jpg");
	loadimage(rolen[0] + 1, "./picture/roley1.jpg");
	loadimage(rolen[1], "./picture/role2.jpg");
	loadimage(rolen[1] + 1, "./picture/roley2.jpg");
	//加载文字图片
	loadimage(&ka, "./picture/ka.png");
	loadimage(&kb, "./picture/kb.png");
}

//加载音乐
void BGM(){
	mciSendString("open ./music/bk.mp3 alias BGM", NULL, 0, NULL);
	mciSendString("play BGM repeat", NULL, 0, NULL);
}

// 载入PNG图并去透明部分
void drawAlpha(IMAGE* picture, int  picture_x, int picture_y) //x为载入图片的X坐标，y为Y坐标
{

	// 变量初始化
	DWORD *dst = GetImageBuffer();    // GetImageBuffer()函数，用于获取绘图设备的显存指针，EASYX自带
	DWORD *draw = GetImageBuffer();
	DWORD *src = GetImageBuffer(picture); //获取picture的显存指针
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

//游戏数据初始化
void GameInit(){
	//设置随机数种子
	srand((unsigned)time(NULL)*rand());

	//初始情况下，人物正常状态
	cas = 0;
	tempcas = 0;

	//初始情况下，金币数量为0
	coin = 0;
	aim = 650;

	//初始化人物
	role.width = 140;
	role.height = 120;
	role.x = WIDTH / 2 - role.width / 2;
	role.y = 4;

	//初始化钩子
	hook.start_x = WIDTH / 2 - 20;
	hook.start_y = role.height - 20;
	hook.len = 50;
	hook.radian = PI / 2;
	hook.end_x = hook.start_x + cos(hook.radian)*hook.len;
	hook.end_y = hook.start_y + sin(hook.radian)*hook.len;
	hook.dir = 1;//初始情况下，钩子向右移动
	hook.rockflag = true;//初始情况下，钩子移动

	//初始化物品
	for (int i = 0; i < BIG_GOLD_NUM; i++){
		biggold[i].value = 120;
		biggold[i].width = 110;
		biggold[i].height = 97;
		biggold[i].weight = 0.8;
		biggold[i].flag = true;//初始情况下金子存在
	}
	biggold[0].x = 30; biggold[0].y = 510;
	biggold[1].x = 265; biggold[1].y = 585;
	biggold[2].x = 500; biggold[2].y = 445;
	for (int i = 0; i < SMALL_GOLD_NUM; i++){
		smallgold[i].value = 50;
		smallgold[i].width = 50;
		smallgold[i].height = 44;
		smallgold[i].weight = 0.4;
		smallgold[i].flag = true;//初始情况下金子存在
	}
	smallgold[0].x = 100; smallgold[0].y = 350;
	smallgold[1].x = 260; smallgold[1].y = 370;
	smallgold[2].x = 520; smallgold[2].y = 345;
	smallgold[3].x = 320; smallgold[3].y = 200;
	smallgold[4].x = 480; smallgold[4].y = 580;
	for (int i = 0; i < STONE_NUM; i++){
		stone[i].value = 20;
		stone[i].width = 80;
		stone[i].height = 67;
		stone[i].weight = 1.0;
		stone[i].flag = true;//初始情况下石头存在
	}
	stone[0].x = 200; stone[0].y = 465;
	stone[1].x = 395; stone[1].y = 380;
	stone[2].x = 40; stone[2].y = 220;
	for (int i = 0; i < SACK_NUM; i++){
		sack[i].value = (rand() % 11 + 2) * 10;
		sack[i].width = 60;
		sack[i].height = 50;
		sack[i].weight = 0.2;
		sack[i].flag = true;//初始情况下袋子存在
	}
	sack[0].x = 320; sack[0].y = 480;
	sack[1].x = 540; sack[1].y = 180;
	sack[2].x = 220; sack[2].y = 220;
	sack[3].x = 420; sack[3].y = 240;
}

//创建按钮
struct Button* CreatButton(int x, int y, int width, int height, 
	const char* text, int dx, int dy, COLORREF buttoncolor, COLORREF textcolor){
	struct Button* pButton = (struct Button*)malloc(sizeof(struct Button));
	pButton->x = x;
	pButton->y = y;
	pButton->width = width;
	pButton->height = height;
	pButton->text = (char*)malloc(strlen(text) + 1);
	strcpy(pButton->text, text);
	pButton->dx = dx;
	pButton->dy = dy;
	pButton->buttoncolor = buttoncolor;
	pButton->textcolor = textcolor;
	return pButton;
}

//绘制按钮
void DrawButton(struct Button* pButton){
	setbkmode(TRANSPARENT);
	setlinecolor(RGB(180, 124, 41));
	setlinestyle(PS_SOLID, 5);
	setfillcolor(pButton->buttoncolor);
	fillrectangle(pButton->x, pButton->y, pButton->x + pButton->width, pButton->y + pButton->height);
	settextcolor(pButton->textcolor);
	settextstyle(28, 0, "楷体", 0, 0, 1000, 0, 0, 0);
	outtextxy(pButton->x + pButton->dx, pButton->y + pButton->dy, pButton->text);
}

//绘制钩子
void DrawHook(struct Hook* hook){
	setlinestyle(PS_SOLID, 3);
	setlinecolor(RGB(83, 83, 83));
	line(hook->start_x, hook->start_y, hook->end_x, hook->end_y);

	int length1 = 20;
	int Rend_x = hook->end_x + length1 * cos(PI / 5.0 - PI / 2 + hook->radian);
	int Rend_y = hook->end_y + length1 * sin(PI / 5.0 - PI / 2 + hook->radian);
	int Lend_x = hook->end_x - length1 * cos(PI / 5.0 + PI / 2 - hook->radian);
	int Lend_y = hook->end_y + length1 * sin(PI / 5.0 + PI / 2 - hook->radian);
	setlinestyle(PS_SOLID, 4);
	setlinecolor(RGB(134, 134, 134));
	line(hook->end_x, hook->end_y + 1, Rend_x, Rend_y);
	line(hook->end_x, hook->end_y + 1, Lend_x, Lend_y);

	int length2 = 10;
	int Rend_xx = Rend_x - length2*cos(-PI / 5.0 - PI / 2 + hook->radian);
	int Rend_yy = Rend_y - length2 * sin(-PI / 5.0 - PI / 2 + hook->radian);
	int Lend_xx = Lend_x + length2 * cos(-PI / 5.0 + PI / 2 - hook->radian);
	int Lend_yy = Lend_y - length2 * sin(-PI / 5.0 + PI / 2 - hook->radian);
	setlinestyle(PS_SOLID, 3);
	setlinecolor(RGB(134, 134, 134));
	line(Rend_x, Rend_y, Rend_xx, Rend_yy);
	line(Lend_x, Lend_y, Lend_xx, Lend_yy);
}

//贴矿工图片
void DrawRole(struct Role* role){
	//掩码图：SRCAND格式去贴图
	putimage(role->x, role->y, rolen[cas] + 1, SRCAND);
	//源码图：SRCPAINT格式去贴图
	putimage(role->x, role->y, rolen[cas], SRCPAINT);
}

//贴大金子图片
void DrawBigGold(struct Mine* biggold){
	//掩码图：SRCAND格式去贴图
	putimage(biggold->x, biggold->y, img, SRCAND);
	//源码图：SRCPAINT格式去贴图
	putimage(biggold->x, biggold->y, img + 1, SRCPAINT);
}

//贴小金子图片
void DrawSmallGold(struct Mine* smallgold){
	//掩码图：SRCAND格式去贴图
	putimage(smallgold->x, smallgold->y, img + 2, SRCAND);
	//源码图：SRCPAINT格式去贴图
	putimage(smallgold->x, smallgold->y, img + 3, SRCPAINT);
}

//贴石头图片
void DrawStone(struct Mine* stone){
	//掩码图：SRCAND格式去贴图
	putimage(stone->x, stone->y, img + 4, SRCAND);
	//源码图：SRCPAINT格式去贴图
	putimage(stone->x, stone->y, img + 5, SRCPAINT);
}

//贴袋子图片
void DrawSack(struct Mine* sack){
	//掩码图：SRCAND格式去贴图
	putimage(sack->x, sack->y, img + 6, SRCAND);
	//源码图：SRCPAINT格式去贴图
	putimage(sack->x, sack->y, img + 7, SRCPAINT);
}

//开始界面绘制
void Welcome(){
	setbkcolor(RGB(223, 172, 11));
	cleardevice();//清屏

	setfillcolor(RGB(42, 63, 156));
	solidcircle(WIDTH / 2, role.height + 5, role.height);
	putimage(0, role.height + 5, &bk);
	DrawRole(&role);
	for (int i = 0; i < BIG_GOLD_NUM; i++){
		DrawBigGold(&biggold[i]);
	}
	for (int i = 0; i < SMALL_GOLD_NUM; i++){
		DrawSmallGold(&smallgold[i]);
	}
	for (int i = 0; i < STONE_NUM; i++){
		DrawStone(&stone[i]);
	}
	for (int i = 0; i < SACK_NUM; i++){
		DrawSack(&sack[i]);
	}
	DrawButton(beginGame);
	DrawButton(endGame);
	DrawHook(&hook);
	drawAlpha(&ka, 20, 160);
	drawAlpha(&kb, 160, 320);

	//显示金币数量
	settextstyle(26, 0, "楷体", 0, 0, 1000, 0, 0, 0);
	settextcolor(RGB(240, 240, 240));
	char coinName[25];
	sprintf(coinName, "金钱：$%d", coin);
	outtextxy(7, 23, coinName);
	char aimName[25];
	sprintf(aimName, "目标钱数：$%d", aim);
	outtextxy(5, 69, aimName);
}

//游戏界面绘制
void GameDraw(){
	cleardevice();//清屏
	setfillcolor(RGB(42, 63, 156));
	solidcircle(WIDTH / 2, role.height + 5, role.height);
	putimage(0, role.height + 5, &bk);
	DrawRole(&role);
	for (int i = 0; i < BIG_GOLD_NUM; i++){
		if (biggold[i].flag == true){
			DrawBigGold(&biggold[i]);
		}
	}
	for (int i = 0; i < SMALL_GOLD_NUM; i++){
		if (smallgold[i].flag == true){
			DrawSmallGold(&smallgold[i]);
		}
	}
	for (int i = 0; i < STONE_NUM; i++){
		if (stone[i].flag == true){
			DrawStone(&stone[i]);
		}
	}
	for (int i = 0; i < SACK_NUM; i++){
		if (sack[i].flag == true){
			DrawSack(&sack[i]);
		}
	}
	DrawHook(&hook);

	//显示金币数量
	settextstyle(26, 0, "楷体", 0, 0, 1000, 0, 0, 0);
	settextcolor(RGB(240, 240, 240));
	char coinName[25];
	sprintf(coinName, "金钱：$%d", coin);
	outtextxy(7, 23, coinName);
	char aimName[25];
	sprintf(aimName, "目标钱数：$%d", aim);
	outtextxy(5, 69, aimName);
}

//钩子摆动
void HookRock(){
	if (hook.rockflag == true){
		//防止移动出现越界
		if (hook.radian > PI){//防止向左越界
			hook.radian = PI;
			hook.radian -= 0.0044;
			hook.dir = 1;//越界后开始向右移动
		}
		else if (hook.radian < 0){//防止向右越界
			hook.radian = 0;
			hook.radian += 0.0044;
			hook.dir = 0;//越界后开始向左移动
		}

		//控制钩子移动方向
		if (hook.dir == 1){//向右移动
			hook.radian -= 0.0044;
		}
		else if (hook.dir == 0){//向左移动
			hook.radian += 0.0044;
		}
	}
	else{
		//人物收钩子动画
		tempcas += 0.022;
		if (tempcas >= 2){
			tempcas = 0;
		}
		cas = (int)tempcas;
		//钩子的伸缩
		if (hook.flexflag == true){//伸
			hook.len += 1.2;
			if (hook.len >= 630){
				hook.len = 630;
				hook.flexflag = false;
			}
		}
		else if (hook.flexflag == false){//缩
			hook.len -= 1.2;
			if (hook.len <= 50){
				hook.len = 50;
				hook.rockflag = true;
			}
		}
	}
	hook.end_x = hook.start_x + cos(hook.radian)*hook.len;
	hook.end_y = hook.start_y + sin(hook.radian)*hook.len;
}

//钩子抓取物品
void HookGrab(){
	//设置x，y坐标
	int length = 25;
	double x = hook.end_x + length * cos(hook.radian);
	double y = hook.end_y + length * sin(hook.radian);

	//钩子与物品接触
	for (int i = 0; i < BIG_GOLD_NUM; i++){
		if (biggold[i].flag == true &&
			x >= biggold[i].x + 5 && x <= biggold[i].x + 5 + biggold[i].width&&
			y >= biggold[i].y + 5 && y <= biggold[i].y + 5 + biggold[i].height){
			biggold[i].x = x - 48;
			biggold[i].y = y - 15;
			hook.flexflag = false;
			hook.len += biggold[i].weight;
			if (hook.rockflag==true){
				biggold[i].flag = false;
				coin += biggold[i].value;
			}
		}
	}
	for (int i = 0; i < SMALL_GOLD_NUM; i++){
		if (smallgold[i].flag == true && 
			x >= smallgold[i].x&&x <= smallgold[i].x + smallgold[i].width&&
			y >= smallgold[i].y&&y <= smallgold[i].y + smallgold[i].height){
			smallgold[i].x = x - 23;
			smallgold[i].y = y - 10;
			hook.flexflag = false;
			hook.len += smallgold[i].weight;
			if (hook.rockflag == true){
				smallgold[i].flag = false;
				coin += smallgold[i].value;
			}
		}
	}
	for (int i = 0; i < STONE_NUM; i++){
		if (stone[i].flag == true &&
			x >= stone[i].x&&x <= stone[i].x + stone[i].width&&
			y >= stone[i].y&&y <= stone[i].y + stone[i].height){
			stone[i].x = x - 48;
			stone[i].y = y - 10;
			hook.flexflag = false;
			hook.len += stone[i].weight;
			if (hook.rockflag == true){
				stone[i].flag = false;
				coin += stone[i].value;
			}
		}
	}
	for (int i = 0; i < SACK_NUM; i++){
		if (sack[i].flag == true && 
			x >= sack[i].x&&x <= sack[i].x + sack[i].width&&
			y >= sack[i].y&&y <= sack[i].y + sack[i].height){
			sack[i].x = x - 20;
			sack[i].y = y - 10;
			hook.flexflag = false;
			hook.len += sack[i].weight;
			if (hook.rockflag == true){
				sack[i].flag = false;
				coin += sack[i].value;
			}
		}
	}
}

//判断鼠标是否在按钮中
int MouseInButton(struct Button* pButton, MOUSEMSG* msg){
	if (msg->x >= pButton->x&&msg->x <= pButton->x + pButton->width&&
		msg->y >= pButton->y&&msg->y <= pButton->y + pButton->height){
		pButton->buttoncolor = RGB(200, 138, 47);
		pButton->textcolor = RGB(135, 135, 135);
		return 1;//返回值为1，表明鼠标在按钮中
	}
	pButton->buttoncolor = RGB(210, 152, 64);
	pButton->textcolor = RGB(240, 240, 240);
	return 0;//返回值为0，说明不在按钮中
}

//鼠标控制
int MouseControl(struct Button* pButton1, struct Button* pButton2){
	//判断是否有鼠标消息
	if (MouseHit()){
		MOUSEMSG msg = GetMouseMsg();
		//判断用户是否点击按钮
		if (MouseInButton(pButton1, &msg) == 1 && msg.uMsg == WM_LBUTTONDOWN){
			mciSendString("close click", NULL, 0, NULL);
			mciSendString("open ./music/click.wav alias click", NULL, 0, NULL);
			mciSendString("play click", NULL, 0, NULL);
			return 1;//返回值为1，进入游戏
		}
		else if (MouseInButton(pButton2, &msg) == 1 && msg.uMsg == WM_LBUTTONDOWN){
			mciSendString("close click", NULL, 0, NULL);
			mciSendString("open ./music/click.wav alias click", NULL, 0, NULL);
			mciSendString("play click", NULL, 0, NULL);
			exit(0);//游戏结束
		}
	}
	return 0;
}

//按键响应（同步按键响应）
void KeyDown(){
	//判断是否有按键消息
	while (_kbhit()){
		//用于接收用户信息
		char userKey = _getch();
		fflush(stdin);//清空键盘消息缓冲区

		//对按键消息进行分发
		switch (userKey){
		case ' ':
			hook.rockflag = false;
			hook.flexflag = true;
			break;
		}
	}
}

//游戏结束条件判断
int GameOver(){
	if (coin >= 650){
		return 1;//返回值为1时游戏结束
	}
	return 0;//返回值为0时游戏继续
}

//主函数，唯一的入口函数
int main(){
	LoadResource();
	initgraph(WIDTH, HEIGHT);
	BGM();
	GameInit();

	BeginBatchDraw();//双缓冲
	while (1){
		Welcome();
		FlushBatchDraw();
		if (MouseControl(beginGame, endGame) == 1){
			break;
		}
	}
	fflush(stdin);//清空键盘消息缓冲区
	FlushMouseMsgBuffer();//清空鼠标消息缓冲区

	while (1){
		KeyDown();
		HookRock();
		HookGrab();
		GameDraw();
		FlushBatchDraw();
		if (GameOver() == 1){
			mciSendString("close win", NULL, 0, NULL);
			mciSendString("open ./music/win.wav alias win", NULL, 0, NULL);
			mciSendString("play win", NULL, 0, NULL);
			MessageBox(GetHWnd(), "You Win!", "Game Over!", MB_OK);
			break;
		}
	}
	EndBatchDraw();

	closegraph();
	return 0;
}