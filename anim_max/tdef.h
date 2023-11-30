#include <math.h>
#include <stdio.h>
#include <windows.h>

#define S_DIMS 3

// --- ������ ----------------------
 // �������� ������� (����)
#define TIMER_INTERVAL 10
// �����  ���������  ����������
#define ALL_STEP_TIME  200       
#define TIMER_MAX  150  
 // �������������
#define TIMER_ID 1000

 // ����������
struct CVect {
	double x, y;
};



// ������� �������������� 
struct CMatr {
	double m[S_DIMS][S_DIMS];
};

// ��������� �������
void MatrZero(CMatr& m_);

// ��������� ��������� �������
void MatrIdentity(CMatr& m_);

// ��������� ������� ���������������
void MatrScale(CMatr& m_, double kx, double ky);

// ��������� ������� �����������
void MatrTransl(CMatr& m_,
	const CVect& v_);

// ��������� ������� ��������
void MatrRot(CMatr& m_,
	const double& dAng_);

// ��������� ������
CMatr operator * (const CMatr& m1_,
	const CMatr& m2_);

// ��������� ������� �� �������
CVect operator * (const CVect& v_,
	const CMatr& m_);

// ������ � ������
void MatrPrint(const CMatr& m1_, LPSTR pcBuff);
