/********************************************************************/
/*                           anim_proc_plus                         */
/*                           ==============                         */
/*    Функции для анимации  произвольного набора плоских объектов   */
/*        ( с примером - полёт квадрата с машущими крыльями         */
/*     с поворотом и увеличением  масштаба с середине траектории )  */
/********************************************************************/
#include "tdef.h"

#include "resource.h"

// #define ALL_STEP_TIME  400        // число  временных  интервалов

#define ALL_PT         4         // число  точек 
#define X_SPACE_ANIM   100.0     // Размер области вывода по X
#define Y_SPACE_ANIM    70.0     // Размер области вывода по Y

#define  K_SCALE  0.1           // коэффициент пропорциональности
#define  K_GAB    0.1           // Габаритный  коэффициент отступа
								//  выводимого изображение от края границы вывода

#define  PI       3.14159265

int pr_start = 0, znak = 1;
double ang_wing = 0.0;
/*-------------------------------------*/
/*  Координаты  квадрата               */
/*-------------------------------------*/
CVect  kvadro[ALL_PT] =
{ {  0.5,   0.5  },
	{ -0.5,   0.5 },
	{ -0.5,  -0.5 },
	{  0.5,  -0.5 }
};
CVect  line[ALL_PT] =
{ {  0.6,   0.6  },
	{ -0.6,   0.6 },
	{ -5,  -0.6 },
	{  5,  -0.6 }
};
/*-------------------------------------*/
/*  Координаты крыла                   */
/*-------------------------------------*/
CVect  wing[ALL_PT] =
{ {  0.0,   0.0  },
	{  0.5,   0.15 },
	{  1.0,   0.0 },
	{  0.5,   0.3 }
};

void trans_coor(CVect* pvOld, CVect* pvNew, int all_pt, CMatr mpr);
CMatr  make_matr_pr(RECT& cp);
void draw_polygon(HDC hDc, CVect* pVect, int nPoints);
BOOL chek = TRUE, chek2 = TRUE;
static int tm;
static double color = 0;
static double color2 = 0;

/********************************************************************/
/*                        anim_main                                 */
/*                        ==========                                */
/*      Управляющая  процедура  отрисовки  полёта  квадрата         */
/********************************************************************/
BOOL  anim_main(HWND hwnd)
{
	int    R = 80;


	extern int timer;

	double ang, dk, d_ang = PI / 32;
	CVect  vt, kvadro_win[ALL_PT], kvadro_real[ALL_PT], vt_wing;

	CMatr  ms, mr, mt, mpr, ms_wing, mr_wing, mt_wing, mreal;
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(hwnd, &ps);

	if (pr_start == 0)
	{
		pr_start = 1;
		EndPaint(hwnd, &ps);
		return TRUE;
	}

	HPEN hpen_prv = (HPEN)SelectObject(hdc, GetStockObject(NULL_PEN));
	HPEN pen_white = CreatePen(PS_SOLID, NULL, RGB(255, 255, 255));

	HBRUSH
		hbr_red = CreateSolidBrush(RGB(color2, color, 0)),
		hbr_green = CreateSolidBrush(RGB(0, 255, 0)),
		hbr_prv = (HBRUSH)SelectObject(hdc, hbr_red);

	// определить размеры клиентской области окна
	RECT rc;
	GetClientRect(hwnd, &rc);

	mpr = make_matr_pr(rc);

	double k_sc = X_SPACE_ANIM * K_SCALE, k_sc_wing = X_SPACE_ANIM * K_SCALE;

	/*---------------------------------------*/
	/*      Перемещение по траектории        */
	/*---------------------------------------*/
	ang = 0;
	dk = double(timer) / (ALL_STEP_TIME);

	vt.x = X_SPACE_ANIM / 2;
	vt.y = Y_SPACE_ANIM - (Y_SPACE_ANIM * dk);



	MatrTransl(mt, vt);


	if (dk >= 0.5)
	{
		if (chek) {
			tm = timer;
			chek = FALSE;
		}
		dk = double(timer + (tm)) / (ALL_STEP_TIME);
		vt.y = (Y_SPACE_ANIM / 2) * dk;
		MatrTransl(mt, vt);
		color -= 3;
		color2 -= 3;
	}
	color2 += 1;
	color += 1;
	MatrRot(mr, ang);
	MatrScale(ms, k_sc, k_sc);
	mreal = ms * mr * mt;
	trans_coor(kvadro, kvadro_real, ALL_PT, mreal);
	trans_coor(kvadro, kvadro_win, ALL_PT, mreal * mpr);

	Ellipse(hdc, kvadro_win[1].x, kvadro_win[1].y, kvadro_win[1].x + R, kvadro_win[1].y + R);

	/*---------------------------------------*/
	SelectObject(hdc, pen_white);
	vt.x = X_SPACE_ANIM / 2;
	vt.y = Y_SPACE_ANIM / 2;
	MatrTransl(mt, vt);
	MatrRot(mr, ang);
	MatrScale(ms, k_sc, k_sc);
	mreal = ms * mr * mt;

	trans_coor(line, kvadro_win, ALL_PT, mreal * mpr);



	MoveToEx(hdc, kvadro_win[3].x, kvadro_win[3].y, NULL);
	LineTo(hdc, kvadro_win[2].x, kvadro_win[2].y);



	SelectObject(hdc, hpen_prv);
	SelectObject(hdc, hbr_prv);
	DeleteObject(hbr_red);
	DeleteObject(hbr_green);
	DeleteObject(pen_white);

	EndPaint(hwnd, &ps);

	return TRUE;
}

/********************************************************************/
/*                        draw_polygon                              */
/*                        ============                              */
/*      Рисование многоугольника  на экране по  вещественным коорд  */
/********************************************************************/
void draw_polygon(HDC hDc, CVect* pVect, int nPoints)
{
	POINT* pPnt = new POINT[nPoints];
	for (int i = 0; i < nPoints; i++) {
		pPnt[i].x = LONG(pVect[i].x);
		pPnt[i].y = LONG(pVect[i].y);
	}
	Polygon(hDc, pPnt, nPoints);
	delete[] pPnt;
}

/********************************************************************/
/*                        make_matr_pr                              */
/*                        ============                              */
/*              Формирование  матрицы  проекции                     */
/********************************************************************/
CMatr  make_matr_pr(RECT& cp)
{
	int a, b, gab_a, gab_b, xc_w, yc_w;
	double x_max, y_max, x_min, y_min, k_scale, k1, k2,
		dx, dy, xc, yc, l_snar, l_pushka;

	CMatr  mt1, ms, mr, mt2;

	CVect  vt;

	/*--------------------------------*/
	/*  Параметры  окна  вывода       */
	/*--------------------------------*/
	a = abs(cp.right - cp.left);
	b = abs(cp.top - cp.bottom);

	gab_a = (int)((double)a * K_GAB);  // отступ по ширине
	gab_b = (int)((double)b * K_GAB);  // отступ по высоте

	a -= gab_a;
	b -= gab_b;

	xc_w = (cp.right + cp.left) / 2;
	yc_w = (cp.top + cp.bottom) / 2;

	/*-----------------------------------------*/
	/*  Определение  масштабного коэффициента  */
	/*-----------------------------------------*/
	x_min = 0;
	y_min = 0;

	x_max = X_SPACE_ANIM;
	y_max = Y_SPACE_ANIM;

	dx = fabs(x_max - x_min);
	dy = fabs(y_max - y_min);

	xc = (x_max + x_min) / 2;
	yc = (y_max + y_min) / 2;

	k1 = a / dx;
	k2 = b / dy;

	k_scale = (k1 < k2) ? k1 : k2;

	vt.x = -xc;
	vt.y = -yc;
	MatrTransl(mt1, vt);
	MatrScale(ms, k_scale, k_scale);
	// Зеркальное отображение
	MatrScale(mr, 1, -1);

	vt.x = xc_w;
	vt.y = yc_w;
	MatrTransl(mt2, vt);

	return (mt1 * ms * mr * mt2);
}

/********************************************************************/
/*                           trans_coor                             */
/*                           ==========                             */
/*  Изменение массива координат  по  матрице  преобразований        */
/********************************************************************/

void trans_coor(CVect* pvOld, CVect* pvNew, int all_pt, CMatr mpr)
{
	for (int i = 0; i < all_pt; i++)
		pvNew[i] = pvOld[i] * mpr;
}


