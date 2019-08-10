#include <graphics.h>//easyx图像库
#include <conio.h>
#include <windows.h>
#include <time.h>
#include<stdio.h>
#include <stdlib.h>
#pragma comment(lib,"winmm.lib")//播放音频

#define  WIDTH 512//宏定义画布尺寸宽
#define  HIGH 256//宏定义画布尺寸高
#define R_H 94//定义人物高
#define R_W 68//定义人物宽
int T = 2000;//图片显示延时

//定义障碍物结构体
struct barrier
{
	int id;//代表障碍物种类,定义障碍物bar，为0代表无障碍物，1代表火球，2代表食人花，3代表上藤蔓
	int begin_i;
	int pass_i;
	int end_i;
	int img_num;
	int i;
};

//定义全局变量
IMAGE bk;	    // 定义画布
IMAGE img_bg;	// 定义背景
IMAGE img_run;  //人物跑动图
IMAGE img_run1; //跑动遮罩图
IMAGE img_jump; //人物跳跃图
IMAGE img_jump1;//跳跃遮罩图
IMAGE img_slide;//人物下滑图
IMAGE img_slide1;//下滑遮罩图
IMAGE img_trap;//火球图
IMAGE img_trap1;//火球遮罩图
IMAGE img_dowmPlant;//食人花图
IMAGE img_dowmPlant1;//食人花遮罩图
IMAGE img_upPlant;//上藤曼图
IMAGE img_upPlant1;//上藤曼遮罩图
IMAGE img_trapDeath; //碰到地刺死亡图
IMAGE img_trapDeath1;//碰到地刺死亡遮罩图
IMAGE img_plantDeath;//碰到藤曼死亡图
IMAGE img_plantDeath1;//碰到藤曼死亡遮罩图
IMAGE img_deathbk;//死亡后 游戏背景界面
int img_bg_i;//背景图片分割线
int M_X;//人物x位置
int M_Y;//人物y位置
int begin_i, end_i, pass_i;//begin_i为人物在图片上的初始像素，end_i为在图片上的当前像素，pass_i为通过图片像素
int img_num1;//显示第img_num1张跑动图片
int img_num2;//显示第img_num2张跳跃图片
int img_num3;//显示第img_num3张下滑图片
int move;//move为0代表跑，1代表跳，2代表滑
barrier barr;//障碍物
int gameStatus; // 游戏状态，0为初始菜单界面，1为正常游戏，2为死亡游戏界面
int judge; //死亡判断 1为生，2为死。
int score; // 得分
int scoreNum;//判断背景循环次数，循环一次为2048

// 根据透明度绘图
void drawAlpha(IMAGE* dstimg, int x, int y, IMAGE* srcimg);

//主界面
void change(int a, int b, IMAGE flower);

//菜单主界面
void startMenu();

//显示得分情况
void showScore();

//数据初始化
void startup();

//游戏数据重新初始化
void restartup();

// 延时函数
void delay(unsigned int xms);

//跑
void runImg(int x, int y);

//跳
void jumpImg(int w, int h, int i, int j);

//滑
void slideImg(int X, int Y, int w, int h, int i, int j);

//障碍物
void barrierImg(int id, int x, int i, int j, int w, int h);

//碰火球死亡
void trapDeathImg(int y, int i, int j, int w, int h);

//碰植物死亡
void plantDeathImg(int i, int j, int w, int h);

//人物跑图片显示
void run(int i);

//人物跳图片显示
void jump(int i);

//人物滑图片显示
void slide(int i);

//障碍物图片显示
void isBarrier(int i);

//碰火球死亡图片显示
void trapDeath();

//碰植物死亡图片显示
void plantDeath();

//显示死亡游戏背景界面
void showdeathbk();

//判断死亡
void die();

//显示画面
void show();

//与用户无关的输入
void updateWithoutInput();

//与用户有关的输入
void updateWithInput();

//游戏结束
void gameover();

//主函数
void main()
{
	startup();//游戏初始化
	while (1)
	{
		show();//显示画面
		updateWithoutInput();//与用户输入无关
		updateWithInput();//与用户输入有关
	}
	gameover();//游戏结束
}

//与用户有关的输入
void updateWithInput()
{
	char ch;
	if (kbhit())
	{
		ch = getch();
		if (ch == 'w')
		{
			mciSendString("close smusic", NULL, 0, NULL);
			mciSendString("open res\\1159.wav alias smusic", NULL, 0, NULL);//打开跳音乐
			mciSendString("play smusic ", NULL, 0, NULL);//仅播放一次
			move = 1;
			begin_i = img_bg_i;//获取按键时的img_bg_i
		}

		if (ch == 's')
		{
			mciSendString("close smusic", NULL, 0, NULL);
			mciSendString("open res\\8.wav alias smusic", NULL, 0, NULL);//打开滑音乐
			mciSendString("play smusic ", NULL, 0, NULL);//仅播放一次
			move = 2;
			begin_i = img_bg_i;//获取按键时的img_bg_i
		}
	}
}

//与用户无关的输入
void updateWithoutInput()
{
	die();//人物死亡
	if (img_bg_i < 2048)//背景循环一次，像素经过3200
		score = scoreNum * 2048 + img_bg_i;
}

// 根据透明度绘图
void drawAlpha(IMAGE* dstimg, int x, int y, IMAGE* srcimg)
{
	if (dstimg == NULL)
	{
		return;
	}
	// 变量初始化
	DWORD *dst = GetImageBuffer(dstimg);
	DWORD *src = GetImageBuffer(srcimg);
	int src_width = srcimg->getwidth();
	int src_height = srcimg->getheight();
	int dst_width = dstimg->getwidth();
	int dst_height = dstimg->getheight();

	// 实现透明贴图  可优化
	for (int iy = 0; iy < src_height; iy++)
	{
		for (int ix = 0; ix < src_width; ix++)
		{
			int srcX = ix + iy * src_width;
			int sa = ((src[srcX] & 0xff000000) >> 24);
			int sr = ((src[srcX] & 0xff0000) >> 16);
			int sg = ((src[srcX] & 0xff00) >> 8);
			int sb = src[srcX] & 0xff;
			if (x + ix >= 0 && x + ix < dst_width
				&& y + iy >= 0 && y + iy < dst_height)
			{
				int dstX = (x + ix) + (y + iy) * dst_width;
				int dr = ((dst[dstX] & 0xff0000) >> 16);
				int dg = ((dst[dstX] & 0xff00) >> 8);
				int db = dst[dstX] & 0xff;
				dst[dstX] = ((sr * sa / 255 + dr * (255 - sa) / 255) << 16)
					| ((sg * sa / 255 + dg * (255 - sa) / 255) << 8)
					| (sb * sa / 255 + db * (255 - sa) / 255);
			}
		}
	}
}

//主界面
void change(int a, int b, IMAGE flower)
{
	drawAlpha(&bk, a, b, &flower);
}

//菜单主界面
void startMenu()
{
	initgraph(480, 320);//画布大小
	MOUSEMSG m;
	IMAGE main1, main2, main3, main4, main5, main6, main7, main8, main9, main10, main11,main12;
	loadimage(&main1, "res\\main2.png");// 菜单背景
	loadimage(&main2, "res\\button_more02.png");//菜单栏more
	loadimage(&main3, "res\\button_difficuty02.png");//菜单栏difficul
	loadimage(&main4, "res\\button_play02.png");///菜单栏play
	loadimage(&main5, "res\\button_back01.png");//菜单栏back
	loadimage(&main6, "res\\morebackgound.png");//more背景
	loadimage(&main7, "res\\diffultybackgoud.png");//difficult背景
	loadimage(&main8, "res\\gameover_stage1.png");//难度选择：1
	loadimage(&main9, "res\\gameover_stage2.png");//难度选择：2
	loadimage(&main10, "res\\gameover_stage3.png");//难度选择：3
	loadimage(&main11, "res\\gameover_stage4.png");//难度选择：4
	loadimage(&main12, "res\\diffcultbackgound.png");//difficult背景
									  //游戏开始欢迎动画 ,背景音乐 
	//起始页动画
	int i;
	BeginBatchDraw();//开头动画的图片
	for (i = 1; i <= 9; i++)
	{
		char tmp[128];//定义一个字符数组
		IMAGE bk;//定义图画
		sprintf_s(tmp, "res\\%d.png", i + 1);//sprintf函数将结果送到字符串，载入图片的名称1,2,3....
		loadimage(&bk, tmp);//载入图片资源
		putimage(0, 0, &bk);//逐个输出图片
		mciSendString("open res\\WE080.wav alias kaitoumusic", NULL, 0, NULL);//打开开头音乐
		mciSendString("play kaitoumusic ", NULL, 0, NULL);//仅播放一次
		FlushBatchDraw();//刷新画面
		Sleep(70);//控制图片播放速度
	}
	Sleep(2000);
	//用于返回跳跃 
loop1:
	{
		putimage(0, 0, &main1);
		while (true)
		{
			m = GetMouseMsg();
			//选择diffulet
			if (m.x > 287 && m.x < 408 && m.y>211 && m.y < 243)
			{
				change(287, 211, main3);
				FlushBatchDraw();
				if (m.uMsg == WM_LBUTTONDOWN)
				{
					//mciSendString("play turn from 0",NULL,0,NULL);
				loop2:
					{
						putimage(0, 0, &main12);
						FlushBatchDraw();
						m = GetMouseMsg();
						if (m.x > 352 && m.x < 473 && m.y>262 && m.y < 294)
						{
						loop7:
							{
								change(352, 262, main5);
								FlushBatchDraw();
								if (m.uMsg == WM_LBUTTONDOWN) 
								{
									mciSendString("play turn from 0", NULL, 0, NULL); 
									goto loop1;
								}
								m = GetMouseMsg();
								if (m.x > 352 && m.x < 473 && m.y>262 && m.y < 294)
								{
									goto loop7;
								}
								else goto loop2;
							}
						}
						else goto loop2;
					}
				}
			}
			//选择more
			if (m.x > 287 && m.x < 408 && m.y>150 && m.y < 182)
			{
				change(287, 150, main2);
				FlushBatchDraw();
				if (m.uMsg == WM_LBUTTONDOWN)
				{
					mciSendString("play turn from 0", NULL, 0, NULL);
				loop3:
					{
						putimage(0, 0, &main6);
						FlushBatchDraw();
						m = GetMouseMsg();
						if (m.x > 352 && m.x < 473 && m.y>262 && m.y < 294)
						{
						loop6:
							{
								change(352, 262, main5);
								FlushBatchDraw();
								if (m.uMsg == WM_LBUTTONDOWN)
								{
									mciSendString("play turn from 0", NULL, 0, NULL);
									goto loop1;
								}
								m = GetMouseMsg();
								if (m.x > 352 && m.x < 473 && m.y>262 && m.y < 294)
								{
									goto loop6;
								}
								else goto loop3;
							}
						}
						else goto loop3;
					}
				}
			}
			//选择play 
			if (m.x > 287 && m.x < 408 && m.y>90 && m.y < 122)//游戏开始键位置
			{
				change(287, 90, main4);//开始游戏键变化
				FlushBatchDraw();
				if (m.uMsg == WM_LBUTTONDOWN)//点击
				{
					mciSendString("play turn from 0", NULL, 0, NULL);
				loop4:
					{
						putimage(0, 0, &main7);//游戏难度选择界面
						FlushBatchDraw();
						m = GetMouseMsg();
						if (m.x > 352 && m.x < 473 && m.y>262 && m.y < 294)//返回键位置
						{
						loop8:
							{
								change(352, 262, main5);//back键变化
								FlushBatchDraw();
								m = GetMouseMsg();
								if (m.uMsg == WM_LBUTTONDOWN)//点击
								{
									mciSendString("play turn from 0", NULL, 0, NULL);
									goto loop1;//返回主界面
								}
								if (m.x > 352 && m.x < 473 && m.y>262 && m.y < 294)//返回键位置
								{
									goto loop8;//back键变化
								}
								else goto loop4;//其它返回游戏难度选择界面
							}
						}
						//选择1，返回 1
						if (m.x > 270 && m.x < 421 && m.y>49 && m.y < 72)//难度一键位置
						{
						loop9:
							{
								change(250, 29, main8);//难度一键变化
								FlushBatchDraw();
								m = GetMouseMsg();
								if (m.uMsg == WM_LBUTTONDOWN)//点击
								{
									mciSendString("stop interfacemusic", NULL, 0, NULL);//换音乐
									mciSendString("close interfacemusic", NULL, 0, NULL);
									closegraph();//关闭当前图画
									T = 2000;
									initgraph(WIDTH, HIGH);
									gameStatus = 1;
									return;
								}
								if (m.x > 270 && m.x < 421 && m.y>49 && m.y < 72)//鼠标到这个位置进行跳转
									goto loop9;
								else goto loop4;//其它跳转到难度选择界面
							}
						}
						//选择2模式，返回  2
						if (m.x > 270 && m.x < 421 && m.y>106 && m.y < 129)
						{
						loop10:
							{
								change(250, 86, main9);
								FlushBatchDraw();
								m = GetMouseMsg();
								if (m.uMsg == WM_LBUTTONDOWN) {
									mciSendString("stop interfacemusic", NULL, 0, NULL);
									mciSendString("close interfacemusic", NULL, 0, NULL);
									closegraph();
									T = 1500;
									initgraph(WIDTH, HIGH);
									gameStatus = 1;
									return;
								}
								if (m.x > 270 && m.x < 421 && m.y>106 && m.y < 129)
									goto loop10;
								else goto loop4;
							}
						}
						//选择3模式，返回3
						if (m.x > 270 && m.x < 421 && m.y>165 && m.y < 188)
						{
						loop11:
							{
								change(150, 145, main10);
								FlushBatchDraw();
								m = GetMouseMsg();
								if (m.uMsg == WM_LBUTTONDOWN) {
									mciSendString("stop interfacemusic", NULL, 0, NULL);
									mciSendString("close interfacemusic", NULL, 0, NULL);
									T = 500;
									initgraph(WIDTH, HIGH);
									gameStatus = 1;
									return;
								}
								if (m.x > 270 && m.x < 421 && m.y>165 && m.y < 188)
									goto loop11;
								else goto loop4;
							}
						}
						//选择4模式，返回4
						if (m.x > 270 && m.x < 421 && m.y>222 && m.y < 245)
						{
						loop12:
							{
								change(250, 202, main11);
								FlushBatchDraw();
								m = GetMouseMsg();
								if (m.uMsg == WM_LBUTTONDOWN) {
									mciSendString("stop interfacemusic", NULL, 0, NULL);
									mciSendString("close interfacemusic", NULL, 0, NULL);
									closegraph();
									T = 100;
									initgraph(WIDTH, HIGH);
									gameStatus = 1;
									return;
								}
								if (m.x > 270 && m.x < 421 && m.y>222 && m.y < 245)
									goto loop12;
								else goto loop4;
							}

						}
						if ((m.x > 352 && m.x < 473 && m.y>262 && m.y < 294 || m.x > 270 && m.x < 421 && m.y>49 && m.y < 72 && m.uMsg == WM_LBUTTONDOWN || m.x > 270 && m.x < 421 && m.y>106 && m.y < 129 && m.uMsg == WM_LBUTTONDOWN || m.x > 270 && m.x < 421 && m.y>165 && m.y < 188 && m.uMsg == WM_LBUTTONDOWN || m.x > 270 && m.x < 421 && m.y>222 && m.y < 245 && m.uMsg == WM_LBUTTONDOWN) == 0)
							goto loop4;//这些按键位置跳转到难度选择菜单
					}
				}
			}
			//选择退出游戏
			if (m.x > 287 && m.x < 408 && m.y>271 && m.y < 303)//退出游戏按位置
			{
			loop5: {
				change(287, 271, main5);//退出游戏按键变化
				FlushBatchDraw();
				m = GetMouseMsg();
				if (m.uMsg == WM_LBUTTONDOWN)//点击
				{
					mciSendString("stop interfacemusic", NULL, 0, NULL);
					mciSendString("close interfacemusic", NULL, 0, NULL);
					closegraph();
					exit(0);
				}
				if (m.x > 287 && m.x < 408 && m.y>271 && m.y < 303)
					goto loop5;
				else goto loop1;//不选择退出继续返回主界面
				}
			}
			if ((m.x > 287 && m.x < 408 && m.y>90 && m.y < 122 || m.x > 287 && m.x < 408 && m.y>150 && m.y < 182 || m.x > 287 && m.x < 408 && m.y>211 && m.y < 243 || m.x > 287 && m.x < 408 && m.y>271 && m.y < 303) == 0)
			{
				putimage(0, 0, &main1);//主界面图片
				FlushBatchDraw();
			}
		}
		EndBatchDraw();
	}
}

//显示得分情况
void showScore()
{
	settextcolor(RED);//字体颜色
	settextstyle(30, 0, _T("黑体"));//字体类型
	outtextxy(WIDTH*0.4, HIGH*0.5, _T("得分："));//显示位置
	TCHAR s[500];
	_stprintf(s, _T("%d"), score / 10);
	outtextxy(WIDTH*0.6, HIGH*0.5, s);
}

//数据初始化
void startup()
{
	initgraph(WIDTH, HIGH);
	BeginBatchDraw();//开头动画
	loadimage(&img_bg, _T("res\\back1.png"));//背景图片
	loadimage(&img_run, _T("res\\run.png"));//跑步图片
	loadimage(&img_run1, _T("res\\run1.png"));
	loadimage(&img_jump, _T("res\\jump.png"));//跳图片
	loadimage(&img_jump1, _T("res\\jump1.png"));
	loadimage(&img_slide, _T("res\\slide.png"));//滑图片
	loadimage(&img_slide1, _T("res\\slide1.png"));
	loadimage(&img_trap, _T("res\\fire.png"));//火焰
	loadimage(&img_trap1, _T("res\\fire1.png"));
	loadimage(&img_dowmPlant, _T("res\\plantGreen.png"));//食人花
	loadimage(&img_dowmPlant1, _T("res\\plantGreen1.png"));
	loadimage(&img_upPlant, _T("res\\ivyAnim1.jpg"));//上藤曼
	loadimage(&img_upPlant1, _T("res\\ivyAnim1_1.jpg"));
	loadimage(&img_trapDeath, _T("res\\deathTrapRunInto.png"));//碰火球死亡
	loadimage(&img_trapDeath1, _T("res\\deathTrapRunInto1.png"));
	loadimage(&img_plantDeath, _T("res\\plantDeath.png"));//碰植物死亡
	loadimage(&img_plantDeath1, _T("res\\plantDeath1.png"));
	loadimage(&img_deathbk, _T("res\\death.jpg"));//死亡背景
	mciSendString("open res\\0441.mp3 alias bkmusic", NULL, 0, NULL);//打开背景音乐
	mciSendString("play bkmusic repeat", NULL, 0, NULL);//循环播放
	img_bg_i = 0;//初始化为0
	M_X = 100;//人物当前x位置
	M_Y = 110;//人物当前y位置
	begin_i = 0;//初始像素
	barr.begin_i = 512;//障碍物
	img_num1 = 0;//初始化显 示第0张跑动图片
	img_num2 = 0;//初始化显示第0张跳跃图片
	img_num3 = 0;//初始化显示第0张下滑图片
	barr.img_num = 0;//初始化显示第0张障碍物图片
	score = 0;//得分
	gameStatus = 0;//初始菜单界面
	scoreNum = 0;
	srand((unsigned)time(NULL));//计时器
	BeginBatchDraw();//开头动画
	while (gameStatus == 0)
		startMenu(); // 初始菜单界面
}

//游戏数据重新初始化
void restartup()
{
	img_bg_i = 0;//初始化为0
	M_X = 100;
	M_Y = 110;//定义人物当前位置
	begin_i = 0;
	barr.begin_i = 512;
	img_num1 = 0;//初始化显示第0张跑动图片
	img_num2 = 0;//初始化显示第0张跳跃图片
	img_num3 = 0;//初始化显示第0张下滑图片
	barr.img_num = 0;//初始化显示第0张障碍物图片
	score = 0;//得分为0
	gameStatus = 0;//初始在菜单界面
	scoreNum = 0;//背景循环次数初始为0
}

// 延时函数
void delay(unsigned int xms) // xms代表需要延时的毫秒数
{
	unsigned int x, y;
	for (x = xms; x > 0; x--)
		for (y = 1000; y > 0; y--);
}

//跑
void runImg(int x, int y)
{
	putimage(M_X, M_Y, R_W, R_H, &img_run1, x, y, NOTSRCERASE);
	putimage(M_X, M_Y, R_W, R_H, &img_run, x, y, SRCINVERT);
	FlushBatchDraw();
}

//跳
void jumpImg(int w, int h, int i, int j)
{
	putimage(M_X, M_Y, w, h, &img_jump1, i, j, NOTSRCERASE);
	putimage(M_X, M_Y, w, h, &img_jump, i, j, SRCINVERT);
	FlushBatchDraw();
}

//滑
void slideImg(int X, int Y, int w, int h, int i, int j)
{
	putimage(X, Y, w, h, &img_slide1, i, j, NOTSRCERASE);
	putimage(X, Y, w, h, &img_slide, i, j, SRCINVERT);
	FlushBatchDraw();
}

//障碍物
void barrierImg(int id, int x, int i, int j, int w, int h)//图片中的x，y的位置，图片高度
{
	if (id == 1)//火球
	{
		putimage(x, 145, w, h, &img_trap1, i, j, NOTSRCERASE);//图片高度，图片中x，y的位置
		putimage(x, 145, w, h, &img_trap, i, j, SRCINVERT);
	}
	else if (id == 2)//食人花
	{
		putimage(x, 110, w, h, &img_dowmPlant1, i, j, NOTSRCERASE);
		putimage(x, 110, w, h, &img_dowmPlant, i, j, SRCINVERT);
	}
	else if (id == 3)//上藤条
	{
		putimage(x, -165, w, h, &img_upPlant1, i, j, NOTSRCERASE);
		putimage(x, -165, w, h, &img_upPlant, i, j, SRCINVERT);
	}
}

//碰火球死亡
void trapDeathImg(int y, int i, int j, int w, int h)//人物位置，人物在图片中x，y位置，人物宽度x高度y。
{
	putimage(M_X, y - y + 110, w, h, &img_trapDeath1, i, j, NOTSRCERASE);
	putimage(M_X, y - y + 110, w, h, &img_trapDeath, i, j, SRCINVERT);
}

//碰植物死亡
void plantDeathImg(int i, int j, int w, int h)//人物在图片中x，y位置，人物宽度x高度y
{
	putimage(M_X + 20, 110, w, h, &img_plantDeath1, i, j, NOTSRCERASE);
	putimage(M_X + 20, 110, w, h, &img_plantDeath, i, j, SRCINVERT);
}

//人物跑图片显示
void run(int i)
{
	end_i = i;//当前像素
	pass_i = end_i - begin_i;//当前像素-初始像素
	if (pass_i == 8)
	{
		img_num1++;//显示下一张跑动图片，播放图片
		begin_i = end_i;
	}
	if (img_num1 == 9)//当等于9时，显示第0张
		img_num1 = 0;

	switch (img_num1)
	{
	case 0:runImg(R_W * 1, 0); break;//人物动作在图片中的x，y位置
	case 1:runImg(R_W * 2, 0); break;
	case 2:runImg(R_W * 3, 0); break;
	case 3:runImg(R_W * 4, 0); break;
	case 4:runImg(R_W * 5, 0); break;
	case 5:runImg(R_W * 6, 0); break;
	case 6:runImg(R_W * 7, 0); break;
	case 7:runImg(R_W * 8, 0); break;
	case 8:runImg(R_W * 9, 0); break;
	case 9:runImg(R_W * 10,0); break;
	}
}

//人物跳图片显示
void jump(int i)
{
	end_i = i;
	pass_i = end_i - begin_i;
	if (pass_i == 20)
	{
		img_num2++;							//显示下一张跳跃图片
		if (img_num2 < 8 && img_num2>0)		//计算显示跳跃图片的位置
			M_Y -= 20;
		else if (img_num2 > 8 && img_num2 < 16)
			M_Y += 20;

		begin_i = end_i;
	}

	switch (img_num2)
	{
	case 0:jumpImg(68, 94, 68, 0); break;//人物的宽和高，人物在图片中的x,y位置
	case 1:jumpImg(68, 94, 136, 0); break;
	case 2:jumpImg(68, 94, 204, 0); break;
	case 3:jumpImg(68, 94, 272, 0); break;
	case 4:jumpImg(68, 94, 340, 0); break;
	case 5:jumpImg(68, 94, 408, 0); break;
	case 6:jumpImg(68, 94, 408, 0); break;
	case 7:jumpImg(68, 94, 340, 0); break;
	case 8:jumpImg(68, 94, 272, 0); break;
	case 9:jumpImg(68, 94, 204, 0); break;
	case 10:jumpImg(68, 94,136, 0); break;
	case 11:jumpImg(68, 94, 68, 0); break;
	case 12:jumpImg(68, 94, 68, 0); break;
	case 13:jumpImg(68, 94, 68, 0); break;
	case 14:jumpImg(68, 94, 68, 0); break;
	case 15:jumpImg(68, 94, 68, 0); break;
	}
}

//人物滑图片显示
void slide(int i)
{
	end_i = i;
	pass_i = end_i - begin_i;
	if (pass_i == 20)
	{
		img_num3++;//显示下一张下滑图片
		begin_i = end_i;
	}

	switch (img_num3)
	{

	case 0:slideImg(M_X, 110, 68, 94, 0, 0); break;
	case 1:slideImg(M_X, 110, 45, 94, 68, 0); break;
	case 2:slideImg(M_X, 125, 80, 94, 112, 0); break;
	case 3:slideImg(M_X, 125, 80, 94, 192, 0); break;
	case 4:slideImg(M_X, 125, 80, 94, 272, 0); break;
	case 5:slideImg(M_X, 125, 80, 94, 112, 0); break;
	case 6:slideImg(M_X, 125, 80, 94, 192, 0); break;
	case 7:slideImg(M_X, 125, 80, 94, 272, 0); break;
	case 8:slideImg(M_X, 125, 80, 94, 192, 0); break;
	case 9:slideImg(M_X, 125, 80, 94, 272, 0); break;
	case 10:slideImg(M_X, 110, 68, 94, 0, 0); break;
	case 11:slideImg(M_X, 110, 45, 94, 68, 0); break;

	}
}

//障碍物图片显示
void isBarrier(int i)
{
	int imgCount;//不同障碍物图片的数量

	if (i % 512 == 0)
	{
		barr.id = rand() % 3 + 1;//产生随机种类的障碍物
		barr.img_num = 0;
	}

	if (barr.id == 1)//火球
		imgCount = 7;//7张
	else if (barr.id == 2)//食人花
		imgCount = 6;//6张
	else if (barr.id == 3)//上藤曼
		imgCount = 6;//6张

	barr.end_i = i;
	barr.pass_i = barr.end_i - barr.begin_i;
	if (barr.pass_i == -20)
	{
		barr.img_num++;//显示下一张图片
		barr.begin_i = barr.end_i;
	}
	if (barr.img_num == imgCount)//循环显示
		barr.img_num = 0;

	if (barr.id == 1)
	{
		switch (barr.img_num)
		{
		case 0:barrierImg(barr.id, i, 58 * 2, 0, 58, 82); break;//图片中的x，y的位置，图片高度
		case 1:barrierImg(barr.id, i, 58 * 3, 0, 58, 82); break;
		case 2:barrierImg(barr.id, i, 58 * 4, 0, 58, 82); break;
		case 3:barrierImg(barr.id, i, 58 * 5, 0, 58, 82); break;
		case 4:barrierImg(barr.id, i, 58 * 6, 0, 58, 82); break;
		case 5:barrierImg(barr.id, i, 58 * 7, 0, 58, 82); break;
		case 6:barrierImg(barr.id, i, 58 * 8, 0, 58, 82); break;
		}
	}
	else if (barr.id == 2)
	{
		switch (barr.img_num)
		{
		case 0:barrierImg(barr.id, i, 94 * 1, 0, 94, 100); break;//图片中的x，y的位置，图片高度
		case 1:barrierImg(barr.id, i, 94 * 2, 0, 94, 100); break;
		case 2:barrierImg(barr.id, i, 94 * 3, 0, 94, 100); break;
		case 3:barrierImg(barr.id, i, 94 * 4, 0, 94, 100); break;
		case 4:barrierImg(barr.id, i, 94 * 5, 0, 94, 100); break;
		case 5:barrierImg(barr.id, i, 94 * 6, 0, 94, 100); break;
		}
	}
	else
	{
		switch (barr.img_num)
		{
		case 0:barrierImg(barr.id, i, 0, 25, 265, 310); break;
		case 1:barrierImg(barr.id, i, 265, 25, 265, 310); break;
		case 2:barrierImg(barr.id, i, 2 * 265, 25, 265, 310); break;
		case 3:barrierImg(barr.id, i, 0, 372, 265, 315); break;
		case 4:barrierImg(barr.id, i, 265, 372, 265, 315); break;
		case 5:barrierImg(barr.id, i, 2 * 265, 372, 265, 315); break;
		}
	}
}

//碰火球死亡图片显示
void trapDeath()
{
	int num = 0;//显示第num张图片
	mciSendString("close swumusic", NULL, 0, NULL);
	mciSendString("open res\\4.wav alias swumusic", NULL, 0, NULL);//打开死亡音乐
	mciSendString("play swumusic ", NULL, 0, NULL);//仅播放一次
	while (num < 8)
	{
		//背景障碍覆盖，为了图片能逐张显示
		if (img_bg_i < 3 * WIDTH)
			putimage(0, 0, WIDTH, HIGH, &img_bg, img_bg_i, 0);
		else if (img_bg_i >= 3 * WIDTH&&img_bg_i < 4 * WIDTH)//背景循环
		{
			putimage(0, 0, 4 * WIDTH - img_bg_i, HIGH, &img_bg, img_bg_i, 0);
			putimage(4 * WIDTH - img_bg_i, 0, img_bg_i - 3 * WIDTH, HIGH, &img_bg, 0, 0);
		}
		isBarrier(barr.i);

		switch (num)
		{
		case 0:trapDeathImg(0, 76 * 1, 0, 76, 92); break;//人物位置，人物在图片中x，y位置，人物宽度x高度y。
		case 1:trapDeathImg(0, 76 * 2, 0, 76, 92); break;
		case 2:trapDeathImg(0, 76 * 3, 0, 76, 92); break;
		case 3:trapDeathImg(0, 76 * 4, 0, 76, 92); break;
		case 4:trapDeathImg(0, 76 * 5, 0, 76, 92); break;
		case 5:trapDeathImg(0, 76 * 6, 0, 76, 92); break;
		case 6:trapDeathImg(0, 76 * 7, 0, 76, 92); break;
		case 7:trapDeathImg(0, 0, 92, 76, 92); break;
		}
		FlushBatchDraw();
		num++;
		Sleep(150);
	}
}

//碰植物死亡图片显示
void plantDeath()
{
	int num = 0;//显示第num张图片
	mciSendString("close sumusic", NULL, 0, NULL);
	mciSendString("open res\\1158.wav alias sumusic", NULL, 0, NULL);//打开死亡音乐
	mciSendString("play sumusic ", NULL, 0, NULL);//仅播放一次
	while (num < 6)
	{
		//背景障碍覆盖，为了图片能逐张显示
		if (img_bg_i < 3 * WIDTH)
			putimage(0, 0, WIDTH, HIGH, &img_bg, img_bg_i, 0);
		else if (img_bg_i >= 3 * WIDTH&&img_bg_i < 4 * WIDTH)//背景循环
		{
			putimage(0, 0, 4 * WIDTH - img_bg_i, HIGH, &img_bg, img_bg_i, 0);
			putimage(4 * WIDTH - img_bg_i, 0, img_bg_i - 3 * WIDTH, HIGH, &img_bg, 0, 0);
		}
		isBarrier(barr.i);

		switch (num)
		{
		case 0:plantDeathImg(100, 0, 100, 94); break;//人物在图片中x，y位置，人物宽度x高度y。
		case 1:plantDeathImg(200, 0, 100, 94); break;
		case 2:plantDeathImg(300, 0, 100, 94); break;
		case 3:plantDeathImg(400, 0, 100, 94); break;
		case 4:plantDeathImg(500, 0, 100, 94); break;
		case 5:plantDeathImg(600, 0, 100, 94); break;
		}
		FlushBatchDraw();
		num++;
		Sleep(150);
	}
}

//显示死亡游戏背景界面
void showdeathbk()
{
	mciSendString("stop  bkmusic", NULL, 0, NULL);//停止背景音乐
	//mciSendString("close swmusic", NULL, 0, NULL);//关闭死亡音乐，调试
	mciSendString("open res\\2.wav alias swmusic", NULL, 0, NULL);//打开死亡背景音乐
	mciSendString("play swmusic ", NULL, 0, NULL);//播放死亡音乐
	putimage(0, 0, &img_deathbk);//游戏背景界面
	setbkmode(TRANSPARENT);//背景显示透明，使得分显示
	showScore();//显示得分情况
	//显示游戏界面文字
	FlushBatchDraw();
	Sleep(2);
	MOUSEMSG m;		// 定义鼠标消息
	while (MouseHit())  //这个函数用于检测当前是否有鼠标消息
	{
		m = GetMouseMsg();
		if (m.uMsg == WM_LBUTTONDOWN && m.x >= 330 && m.x <= 450 && m.y >= 70 && m.y <= 100)//鼠标点击“NEW GAME”重新开始游戏
		{
			restartup();//数据初始化
			mciSendString("stop  swmusic", NULL, 0, NULL);//停止死亡背景音乐
			mciSendString("play  bkmusic repeat", NULL, 0, NULL);//播放背景音乐
			gameStatus = 1;//游戏状态变成1
		}
		else if (m.uMsg == WM_LBUTTONDOWN && m.x >= 330 && m.x <= 450 && m.y >= 190 && m.y <= 220)//鼠标点击“MENU”进入游戏规则界面
		{
			restartup();//数据初始化
			mciSendString("stop  swmusic", NULL, 0, NULL);//停止死亡背景音乐
			mciSendString("play  bkmusic repeat", NULL, 0, NULL);//播放背景音乐
			gameStatus = 0;//游戏状态变成0，菜单界面
			while (gameStatus == 0)
			{
				startMenu();//返回主菜单
			}
		}
		else if (m.uMsg == WM_LBUTTONDOWN && m.x >= 330 && m.x <= 450 && m.y >= 300 && m.y <= 330)//鼠标点击“EXIT”退出雨欣
		{
			restartup();//初始化数据
			exit(0);//退出游戏
			closegraph();
		}
	}
	FlushBatchDraw();

}

//判断死亡
void die()
{
	//如果遇到的障碍物是火球，在火球区域内跑和滑都会死亡
	if (barr.id == 1)
	{
		if (move == 0 || move == 2)
			if (M_X + 30 >= barr.i && M_X <= barr.i + 88)
			{
				trapDeath();
				gameStatus = 2;
			}
	}
	//如果遇到的障碍物是植物，在植物内跑和滑都会死亡
	else if (barr.id == 2)
	{
		if (move == 0 || move == 2)
			if (M_X + 20 >= barr.i && M_X <= barr.i + 78)
			{
				plantDeath();
				gameStatus = 2;
			}
	}
	//如果遇到的障碍物是上藤曼，在藤曼区域内跑和跳都会死亡
	else
	{
		if (move == 0 || move == 1)
			if (M_X + 20 >= barr.i && M_X <= barr.i + 256)
			{
				plantDeath();
				gameStatus = 2;
			}
	}
}

//显示画面
void show()//显示
{
	while (gameStatus == 0)//游戏状态为0，显示菜单界面
	{
		startMenu();
	}
	while (gameStatus == 2)//游戏状态为2，显示死亡背景界面
	{
		showdeathbk();
	}
	//背景播放
	if (img_bg_i < 3 * WIDTH)
	{
		putimage(0, 0, WIDTH, HIGH, &img_bg, img_bg_i, 0);

		if (img_bg_i < WIDTH)//获得一个0-512的位置
			barr.i = WIDTH - img_bg_i;
		else if (img_bg_i >= WIDTH && img_bg_i < 2 * WIDTH)
			barr.i = 2 * WIDTH - img_bg_i;
		else if (img_bg_i >= 2 * WIDTH&&img_bg_i < 3 * WIDTH)
			barr.i = 3 * WIDTH - img_bg_i;

		isBarrier(barr.i);

		//判断执行动作
		if (move == 0)
			run(img_bg_i);
		else if (move == 1)
		{
			jump(img_bg_i);
			if (img_num2 == 15)
			{
				move = 0;//一次完整的跳跃后跑
				img_num2 = 0;
				img_num1 = 0;//跳完后开始跑
			}
		}
		else if (move == 2)
		{
			slide(img_bg_i);
			if (img_num3 == 11)
			{
				move = 0;//一次完整的下滑后跑
				img_num3 = 0;
				img_num1 = 0;//滑完后开始跑
			}
		}
		FlushBatchDraw();
	}
	else if (img_bg_i >= 3 * WIDTH&&img_bg_i < 4 * WIDTH)//背景循环
	{
		putimage(0, 0, 4 * WIDTH - img_bg_i, HIGH, &img_bg, img_bg_i, 0);
		putimage(4 * WIDTH - img_bg_i, 0, img_bg_i - 3 * WIDTH, HIGH, &img_bg, 0, 0);

		barr.i = 4 * WIDTH - img_bg_i;//获得一个0-512的位置
		isBarrier(barr.i);

		if (move == 0)
			run(img_bg_i);
		else if (move == 1)
		{
			jump(img_bg_i);
			if (img_num2 == 15)
			{
				move = 0;
				img_num2 = 0;
				img_num1 = 0;
			}
		}
		else if (move == 2)
		{
			slide(img_bg_i);
			if (img_num3 == 12)
			{
				move = 0;//一次完整的下滑后跑
				img_num3 = 0;
				img_num1 = 0;//滑完后开始跑
			}
		}
		FlushBatchDraw();
	}

	img_bg_i++;//分割线右移，图片移动
	if (img_bg_i == 4 * WIDTH)
	{
		img_bg_i = 0;
		begin_i = 0;
	}

	if (img_bg_i%WIDTH == 0)
		barr.begin_i = 800;
	delay(T);
}

//游戏结束
void gameover()
{
	EndBatchDraw();
	getch();
	closegraph();
}
