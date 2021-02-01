#include <stdio.h>//C���Ա�׼��
#include <math.h>//��ѧ������Ҫ
#include <time.h>//time()������Ҫ
#include <conio.h>//_kbhit()������Ҫ
#include <string.h>//�ַ���������Ҫ
#include <graphics.h>//easyxͼ�ο�
#include <mmsystem.h>//������ý���豸ͷ�ļ�
#pragma comment (lib,"winmm.lib")//���ؾ�̬��

//�԰�ť����������������
struct Button* CreatButton(int x, int y, int width, int height,
	const char* text, int dx, int dy, COLORREF buttoncolor, COLORREF textcolor);

#define WIDTH 640//���ڿ��
#define HEIGHT 720//���ڸ߶�
#define PHOTO_NUM 8//ͼƬ����
#define PI 3.1415926//�����
#define SMALL_GOLD_NUM 5//С��������
#define BIG_GOLD_NUM 3//���������
#define STONE_NUM 3//ʯͷ����
#define SACK_NUM 4//��������

struct Button{//��ť
	int x;//���Ͻ�x����
	int y;//���Ͻ�y����
	int width;//��ť���
	int height;//��ť�߶�
	char* text;//��ť������
	int dx;//��������ڰ�ť���Ͻ�x�����ƫ��ֵ
	int dy;//��������ڰ�ť���Ͻ�y�����ƫ��ֵ
	COLORREF buttoncolor;//��ť��ɫ
	COLORREF textcolor;//��ť��������ɫ
};

struct Role{//����
	int x;//����x����
	int y;//����y����
	int width;//����ͼƬ���
	int height;//����ͼƬ�߶�
}role;

struct Mine{//��Ʒ
	double x;//��Ʒx����
	double y;//��Ʒy����
	int width;//��Ʒ���
	int height;//��Ʒ�߶�
	double weight;//��Ʒ����
	bool flag;//�ж���Ʒ�Ƿ񻹴���
	int value;//��Ʒ��ֵ
}smallgold[SMALL_GOLD_NUM], biggold[BIG_GOLD_NUM], stone[STONE_NUM], sack[SACK_NUM];

struct Hook{//����
	double start_x;//������ʼλ��x����
	double start_y;//������ʼλ��y����
	double len;//���ӳ���
	double end_x;//���ӽ���λ��x����
	double end_y;//���ӽ���λ��y����
	double radian;//������ˮƽ�满��
	int dir;//�����ƶ���������
	bool rockflag;//״̬��ʶ�������жϹ����Ƿ��ƶ�
	bool flexflag;//״̬��ʶ�������жϹ����Ƿ�����
}hook;

int cas;//�������ﶯ��
double tempcas;//�����Ŀ���
int aim;//��¼Ŀ��÷�
int coin;//��¼��ҵ÷�
IMAGE bk;//��ű���ͼƬ
IMAGE ka;//�������ͼƬ
IMAGE kb;//�������ͼƬ
IMAGE rolen[2][2];//�������ͼƬ
IMAGE img[PHOTO_NUM];//�����ϷͼƬ
int imgIndex[PHOTO_NUM] = { 0, 1, 2, 3, 4, 5, 6, 7 };//��¼ͼƬ��ַ�±�
struct Button* beginGame = CreatButton(80, 500, 150, 50, "START", 38, 12, RGB(210, 152, 64), RGB(240, 240, 240));
struct Button* endGame = CreatButton(420, 500, 150, 50, "END", 52, 12, RGB(210, 152, 64), RGB(240, 240, 240));

//������Դ
void LoadResource(){
	//��������ͼƬ
	for (int i = 0; i < PHOTO_NUM; i++){
		char fileName[25];
		sprintf(fileName, "./picture/%d.jpg", imgIndex[i]);
		loadimage(img + i, fileName);
	}
	//���ر���ͼƬ
	loadimage(&bk, "./picture/bk.jpg", WIDTH*1, HEIGHT*0.85);
	//��������ͼƬ
	loadimage(rolen[0], "./picture/role1.jpg");
	loadimage(rolen[0] + 1, "./picture/roley1.jpg");
	loadimage(rolen[1], "./picture/role2.jpg");
	loadimage(rolen[1] + 1, "./picture/roley2.jpg");
	//��������ͼƬ
	loadimage(&ka, "./picture/ka.png");
	loadimage(&kb, "./picture/kb.png");
}

//��������
void BGM(){
	mciSendString("open ./music/bk.mp3 alias BGM", NULL, 0, NULL);
	mciSendString("play BGM repeat", NULL, 0, NULL);
}

// ����PNGͼ��ȥ͸������
void drawAlpha(IMAGE* picture, int  picture_x, int picture_y) //xΪ����ͼƬ��X���꣬yΪY����
{

	// ������ʼ��
	DWORD *dst = GetImageBuffer();    // GetImageBuffer()���������ڻ�ȡ��ͼ�豸���Դ�ָ�룬EASYX�Դ�
	DWORD *draw = GetImageBuffer();
	DWORD *src = GetImageBuffer(picture); //��ȡpicture���Դ�ָ��
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

//��Ϸ���ݳ�ʼ��
void GameInit(){
	//�������������
	srand((unsigned)time(NULL)*rand());

	//��ʼ����£���������״̬
	cas = 0;
	tempcas = 0;

	//��ʼ����£��������Ϊ0
	coin = 0;
	aim = 650;

	//��ʼ������
	role.width = 140;
	role.height = 120;
	role.x = WIDTH / 2 - role.width / 2;
	role.y = 4;

	//��ʼ������
	hook.start_x = WIDTH / 2 - 20;
	hook.start_y = role.height - 20;
	hook.len = 50;
	hook.radian = PI / 2;
	hook.end_x = hook.start_x + cos(hook.radian)*hook.len;
	hook.end_y = hook.start_y + sin(hook.radian)*hook.len;
	hook.dir = 1;//��ʼ����£����������ƶ�
	hook.rockflag = true;//��ʼ����£������ƶ�

	//��ʼ����Ʒ
	for (int i = 0; i < BIG_GOLD_NUM; i++){
		biggold[i].value = 120;
		biggold[i].width = 110;
		biggold[i].height = 97;
		biggold[i].weight = 0.8;
		biggold[i].flag = true;//��ʼ����½��Ӵ���
	}
	biggold[0].x = 30; biggold[0].y = 510;
	biggold[1].x = 265; biggold[1].y = 585;
	biggold[2].x = 500; biggold[2].y = 445;
	for (int i = 0; i < SMALL_GOLD_NUM; i++){
		smallgold[i].value = 50;
		smallgold[i].width = 50;
		smallgold[i].height = 44;
		smallgold[i].weight = 0.4;
		smallgold[i].flag = true;//��ʼ����½��Ӵ���
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
		stone[i].flag = true;//��ʼ�����ʯͷ����
	}
	stone[0].x = 200; stone[0].y = 465;
	stone[1].x = 395; stone[1].y = 380;
	stone[2].x = 40; stone[2].y = 220;
	for (int i = 0; i < SACK_NUM; i++){
		sack[i].value = (rand() % 11 + 2) * 10;
		sack[i].width = 60;
		sack[i].height = 50;
		sack[i].weight = 0.2;
		sack[i].flag = true;//��ʼ����´��Ӵ���
	}
	sack[0].x = 320; sack[0].y = 480;
	sack[1].x = 540; sack[1].y = 180;
	sack[2].x = 220; sack[2].y = 220;
	sack[3].x = 420; sack[3].y = 240;
}

//������ť
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

//���ư�ť
void DrawButton(struct Button* pButton){
	setbkmode(TRANSPARENT);
	setlinecolor(RGB(180, 124, 41));
	setlinestyle(PS_SOLID, 5);
	setfillcolor(pButton->buttoncolor);
	fillrectangle(pButton->x, pButton->y, pButton->x + pButton->width, pButton->y + pButton->height);
	settextcolor(pButton->textcolor);
	settextstyle(28, 0, "����", 0, 0, 1000, 0, 0, 0);
	outtextxy(pButton->x + pButton->dx, pButton->y + pButton->dy, pButton->text);
}

//���ƹ���
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

//����ͼƬ
void DrawRole(struct Role* role){
	//����ͼ��SRCAND��ʽȥ��ͼ
	putimage(role->x, role->y, rolen[cas] + 1, SRCAND);
	//Դ��ͼ��SRCPAINT��ʽȥ��ͼ
	putimage(role->x, role->y, rolen[cas], SRCPAINT);
}

//�������ͼƬ
void DrawBigGold(struct Mine* biggold){
	//����ͼ��SRCAND��ʽȥ��ͼ
	putimage(biggold->x, biggold->y, img, SRCAND);
	//Դ��ͼ��SRCPAINT��ʽȥ��ͼ
	putimage(biggold->x, biggold->y, img + 1, SRCPAINT);
}

//��С����ͼƬ
void DrawSmallGold(struct Mine* smallgold){
	//����ͼ��SRCAND��ʽȥ��ͼ
	putimage(smallgold->x, smallgold->y, img + 2, SRCAND);
	//Դ��ͼ��SRCPAINT��ʽȥ��ͼ
	putimage(smallgold->x, smallgold->y, img + 3, SRCPAINT);
}

//��ʯͷͼƬ
void DrawStone(struct Mine* stone){
	//����ͼ��SRCAND��ʽȥ��ͼ
	putimage(stone->x, stone->y, img + 4, SRCAND);
	//Դ��ͼ��SRCPAINT��ʽȥ��ͼ
	putimage(stone->x, stone->y, img + 5, SRCPAINT);
}

//������ͼƬ
void DrawSack(struct Mine* sack){
	//����ͼ��SRCAND��ʽȥ��ͼ
	putimage(sack->x, sack->y, img + 6, SRCAND);
	//Դ��ͼ��SRCPAINT��ʽȥ��ͼ
	putimage(sack->x, sack->y, img + 7, SRCPAINT);
}

//��ʼ�������
void Welcome(){
	setbkcolor(RGB(223, 172, 11));
	cleardevice();//����

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

	//��ʾ�������
	settextstyle(26, 0, "����", 0, 0, 1000, 0, 0, 0);
	settextcolor(RGB(240, 240, 240));
	char coinName[25];
	sprintf(coinName, "��Ǯ��$%d", coin);
	outtextxy(7, 23, coinName);
	char aimName[25];
	sprintf(aimName, "Ŀ��Ǯ����$%d", aim);
	outtextxy(5, 69, aimName);
}

//��Ϸ�������
void GameDraw(){
	cleardevice();//����
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

	//��ʾ�������
	settextstyle(26, 0, "����", 0, 0, 1000, 0, 0, 0);
	settextcolor(RGB(240, 240, 240));
	char coinName[25];
	sprintf(coinName, "��Ǯ��$%d", coin);
	outtextxy(7, 23, coinName);
	char aimName[25];
	sprintf(aimName, "Ŀ��Ǯ����$%d", aim);
	outtextxy(5, 69, aimName);
}

//���Ӱڶ�
void HookRock(){
	if (hook.rockflag == true){
		//��ֹ�ƶ�����Խ��
		if (hook.radian > PI){//��ֹ����Խ��
			hook.radian = PI;
			hook.radian -= 0.0044;
			hook.dir = 1;//Խ���ʼ�����ƶ�
		}
		else if (hook.radian < 0){//��ֹ����Խ��
			hook.radian = 0;
			hook.radian += 0.0044;
			hook.dir = 0;//Խ���ʼ�����ƶ�
		}

		//���ƹ����ƶ�����
		if (hook.dir == 1){//�����ƶ�
			hook.radian -= 0.0044;
		}
		else if (hook.dir == 0){//�����ƶ�
			hook.radian += 0.0044;
		}
	}
	else{
		//�����չ��Ӷ���
		tempcas += 0.022;
		if (tempcas >= 2){
			tempcas = 0;
		}
		cas = (int)tempcas;
		//���ӵ�����
		if (hook.flexflag == true){//��
			hook.len += 1.2;
			if (hook.len >= 630){
				hook.len = 630;
				hook.flexflag = false;
			}
		}
		else if (hook.flexflag == false){//��
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

//����ץȡ��Ʒ
void HookGrab(){
	//����x��y����
	int length = 25;
	double x = hook.end_x + length * cos(hook.radian);
	double y = hook.end_y + length * sin(hook.radian);

	//��������Ʒ�Ӵ�
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

//�ж�����Ƿ��ڰ�ť��
int MouseInButton(struct Button* pButton, MOUSEMSG* msg){
	if (msg->x >= pButton->x&&msg->x <= pButton->x + pButton->width&&
		msg->y >= pButton->y&&msg->y <= pButton->y + pButton->height){
		pButton->buttoncolor = RGB(200, 138, 47);
		pButton->textcolor = RGB(135, 135, 135);
		return 1;//����ֵΪ1����������ڰ�ť��
	}
	pButton->buttoncolor = RGB(210, 152, 64);
	pButton->textcolor = RGB(240, 240, 240);
	return 0;//����ֵΪ0��˵�����ڰ�ť��
}

//������
int MouseControl(struct Button* pButton1, struct Button* pButton2){
	//�ж��Ƿ��������Ϣ
	if (MouseHit()){
		MOUSEMSG msg = GetMouseMsg();
		//�ж��û��Ƿ�����ť
		if (MouseInButton(pButton1, &msg) == 1 && msg.uMsg == WM_LBUTTONDOWN){
			mciSendString("close click", NULL, 0, NULL);
			mciSendString("open ./music/click.wav alias click", NULL, 0, NULL);
			mciSendString("play click", NULL, 0, NULL);
			return 1;//����ֵΪ1��������Ϸ
		}
		else if (MouseInButton(pButton2, &msg) == 1 && msg.uMsg == WM_LBUTTONDOWN){
			mciSendString("close click", NULL, 0, NULL);
			mciSendString("open ./music/click.wav alias click", NULL, 0, NULL);
			mciSendString("play click", NULL, 0, NULL);
			exit(0);//��Ϸ����
		}
	}
	return 0;
}

//������Ӧ��ͬ��������Ӧ��
void KeyDown(){
	//�ж��Ƿ��а�����Ϣ
	while (_kbhit()){
		//���ڽ����û���Ϣ
		char userKey = _getch();
		fflush(stdin);//��ռ�����Ϣ������

		//�԰�����Ϣ���зַ�
		switch (userKey){
		case ' ':
			hook.rockflag = false;
			hook.flexflag = true;
			break;
		}
	}
}

//��Ϸ���������ж�
int GameOver(){
	if (coin >= 650){
		return 1;//����ֵΪ1ʱ��Ϸ����
	}
	return 0;//����ֵΪ0ʱ��Ϸ����
}

//��������Ψһ����ں���
int main(){
	LoadResource();
	initgraph(WIDTH, HEIGHT);
	BGM();
	GameInit();

	BeginBatchDraw();//˫����
	while (1){
		Welcome();
		FlushBatchDraw();
		if (MouseControl(beginGame, endGame) == 1){
			break;
		}
	}
	fflush(stdin);//��ռ�����Ϣ������
	FlushMouseMsgBuffer();//��������Ϣ������

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