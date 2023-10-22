#ifndef _POOLGAME_H_
#define _POOLGMAE_H_

/****************************************************************
* INCLUDES
****************************************************************/
#define WIN32_LEAN_AND_MEAN
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

#include "resource.h"
#include <d3d9.h>
#include <d3dx9.h>

#include <Windows.h>
#include <commctrl.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <mmsystem.h>

//#include <assert.h>
#include <vector>


/****************************************************************
* CONSTANTS
****************************************************************/
#define CLASSNAME "POOLGAME"
#define WINDOWTITLE "POOLGAME"
#define SCREENWIDTH 640
#define SCREENHEIGHT 480

#define RELEASE(o)				if (o){o->Release();o=0;}
#define FREE_MEM(p)				if (p!=NULL){delete p;p=0;}
#define FREE_ARRAY_MEM(p)		if (p!=NULL){delete [] p;p=0;}

// 스프라이트 파일
#define	MAIN_SPRITE				"main.bmp"
#define	INITPLAY_SPRITE			"init_play.bmp"

// 모델 파일
#define	BALL_FILE				"Data\\ball.ASE"
#define	TABLE_FILE				"Data\\table.ASE"
#define	QUE_FILE				"Data\\que.ASE"
#define	ROOM_FILE				"Data\\room.ASE"

// GAME FLOW
enum GAMEFLOW { GAMELOGO, GAMEMAINMENU, INITPLAY, PLAYGAME, EXITGAME };
enum PLAYFLOW { SELECT_ORDER,READY_QUE,READY_SHOT,WAIT,WAIT_REFEREE};

// 충돌 타입
enum COLLISION_TYPE { WITH_CUSHION, WITH_BALL };


// 테이블 관련 상수 - 파일로 읽어서 사용할 수 있도록 수정
#define NUM_TABLE_PLANE 4
#define UP		0
#define DOWN	1
#define LEFT	2
#define RIGHT	3

// 변하는 시간 단위
#define DELTA_TIME			0.1f

#define DEPTH_EPSILON		0.001f

// 수학 관련 
#define M_PI        3.14159265358979323846f

// 최대최소
//#define MAXMIN(max,min,tmp) \
//		if(max < tmp) max = tmp;	\
//		else if(min > tmp) min = tmp;

// 모델 정점 포맷 정보
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1)

// 스프라이트 정점 포맷 정보
#define D3DFVF_SPRITE_VERTEX (D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1)

// 전반사 벡터 계산
#define REFLEXION_VECTOR(d,n) ((2*(-1)*D3DXVec3Dot(&d,&n)) * n) + d

// 볼 환경맵 구현을 위한 정보
struct BALLVERTEX
{
    D3DXVECTOR3 p;
    D3DXVECTOR3 n;
    float       tu1, tv1;
    float       tu2, tv2;

	static const DWORD FVF;
};


/////////////////AseParser Part BEGIN
#include <list>

using namespace std;

#define BUFFERSIZE 256

#define T_ASE					"*3DSMAX_ASCIIEXPORT"
#define T_MATERIAL_LIST			"*MATERIAL_LIST"
#define T_MATERIAL_COUNT		"*MATERIAL_COUNT"
#define T_MATERIAL				"*MATERIAL"
#define T_NUMSUBMTLS			"*NUMSUBMTLS"
#define T_SUBMATERIAL			"*SUBMATERIAL"
#define T_MATERIAL_AMBIENT		"*MATERIAL_AMBIENT"
#define T_MATERIAL_DIFFUSE		"*MATERIAL_DIFFUSE"
#define T_MATERIAL_SPECULAR		"*MATERIAL_SPECULAR"
#define T_MAP_DIFFUSE			"*MAP_DIFFUSE"

#define T_MAP_AMBIENT			"*MAP_AMBIENT"
#define T_MAP_SHINESTRENGTH		"*MAP_SHINESTRENGTH"
#define T_MAP_BUMP				"*MAP_BUMP"
#define T_MAP_SPECULAR			"*MAP_SPECULAR"
#define T_MAP_REFLECT			"*MAP_REFLECT"
#define T_MAP_REFRACT			"*MAP_REFRACT"

#define T_BITMAP				"*BITMAP"
#define T_GEOMOBJECT			"*GEOMOBJECT"
#define T_NODE_NAME				"*NODE_NAME"
#define T_NODE_PARENT			"*NODE_PARENT"
#define T_NODE_TM				"*NODE_TM"
#define T_TM_ROW0				"*TM_ROW0"
#define T_TM_ROW1				"*TM_ROW1"
#define T_TM_ROW2				"*TM_ROW2"
#define T_TM_ROW3				"*TM_ROW3"
#define T_MESH					"*MESH"
#define T_MESH_NUMVERTEX		"*MESH_NUMVERTEX"
#define T_MESH_NUMFACE			"*MESH_NUMFACES"
#define T_MESH_VERTEX_LIST		"*MESH_VERTEX_LIST"
#define T_MESH_FACE_LIST		"*MESH_FACE_LIST"
#define T_MESH_VERTEX			"*MESH_VERTEX"
#define T_MESH_FACE				"*MESH_FACE"
#define T_MESH_MTLID			"*MESH_MTLID"
#define T_MESH_NUMTVERTEX		"*MESH_NUMTVERTEX"
#define T_MESH_TVERTLIST		"*MESH_TVERTLIST"
#define T_MESH_TVERT			"*MESH_TVERT"
#define T_MESH_NUMTVFACES		"*MESH_NUMTVFACES"
#define T_MESH_TFACELIST		"*MESH_TFACELIST"
#define T_MESH_TFACE			"*MESH_TFACE"
#define T_MESH_NORMALS			"*MESH_NORMALS"
#define T_MESH_FACENORMAL		"*MESH_FACENORMAL"
#define T_MESH_VERTEXNORMAL		"*MESH_VERTEXNORMAL"
#define T_MATERIAL_REF			"*MATERIAL_REF"

#define T_TM_ANIMATION			"*TM_ANIMATION"
#define T_CONTROL_ROT_TCB		"*CONTROL_ROT_TCB"
#define T_CONTROL_POS_TRACK		"*CONTROL_POS_TRACK"
#define T_CONTROL_ROT_TRACK		"*CONTROL_ROT_TRACK"

/////////////////AseParser Part END

/****************************************************************
* STRUCTURES
****************************************************************/
// 텍스쳐 UV 좌표 구조체
struct TUV {
	float				fU,fV;
};

// 사용자 정의 vertex 구조체
struct UVERTEX {
	D3DXVECTOR3			vP;		// 위치 좌표
	D3DXVECTOR3			vN;
	DWORD				dwDiffuse;
	TUV					uv;

	UVERTEX()
	{
		vN = D3DXVECTOR3(0.0f,0.0f,0.0f);
		dwDiffuse=0xffffffff;
		uv.fU=0.0f;
		uv.fV=0.0f;
	}
};

// OBJECT 구조체
struct OBJECT{
	D3DXMATRIXA16		matTm;				// tm 행렬
	D3DXMATRIXA16		matInvTm;			// inverse tm 행렬

	int					nNumFace;
	UVERTEX*			pVertex;
	OBJECT*				pNext;

	OBJECT()
	{
		D3DXMatrixIdentity(&matTm);
		D3DXMatrixIdentity(&matInvTm);

		nNumFace=0;
		pVertex=0;
		pNext=0;
	}

	~OBJECT()
	{
		FREE_ARRAY_MEM(pVertex)
		FREE_MEM(pNext)
	}

};

// material 기준에 따라 나눈 mesh 리스트 구조체
struct MODEL {
	D3DMATERIAL9		Mtrl;
	LPDIRECT3DTEXTURE9	pTexture;

	OBJECT*				pObjList;
	MODEL*				pNext;

	MODEL()
	{
		pNext=0;
		pObjList=0;
		pTexture=0;
		ZeroMemory( &Mtrl, sizeof(D3DMATERIAL9) );
		Mtrl.Diffuse.r = Mtrl.Ambient.r = 1.0f;
		Mtrl.Diffuse.g = Mtrl.Ambient.g = 1.0f;
		Mtrl.Diffuse.b = Mtrl.Ambient.b = 1.0f;
		Mtrl.Diffuse.a = Mtrl.Ambient.a = 1.0f;
	}

	~MODEL()
	{
		FREE_MEM(pObjList)
		FREE_MEM(pNext)
	}
};

struct SPRITE_VERTEX {
	D3DXVECTOR3	vP;
	D3DCOLOR dwColor;
	float fU,fV;
};



struct SPRITE {
	LPDIRECT3DTEXTURE9			pTexture;
	LPDIRECT3DVERTEXBUFFER9		pVB;

	SPRITE()
	{
		pVB=0;
		pTexture=0;
	}
	~SPRITE()
	{
		RELEASE(pTexture)
		RELEASE(pVB)
	}
};


//############### AseParser Part 구조체 정의 BEGIN

struct MYINDEX {

#ifdef INDEX32
	DWORD				_0,_1,_2;
#else
	WORD				_0,_1,_2;
#endif
};

// 메시의 면 구조체
struct FACE {
	MYINDEX				Index;			// 메시 face 인덱스
	MYINDEX				TIndex;			// 텍스쳐 uv 좌표 인덱스 
	D3DXVECTOR3			vN;				// 면의 normal 벡터
	int					nSubMtrlID;		// 해당 면의 submaterial의 ID

	FACE()
	{
		Index._0=0;
		Index._1=0;
		Index._2=0;

		TIndex._0=0;
		TIndex._1=0;
		TIndex._2=0;

		vN=D3DXVECTOR3(0.0f,0.0f,0.0f);

		nSubMtrlID=0;
	}

	~FACE()
	{
	}
};

// MESH 구조체
struct MESH {
	char*				pNodeName;			// 노드 이름 - 계층관계로 이루어질 때 사용되는 인덱스
	char*				pParentName;		// 하위 계층일 때 부모 노드 이름
	UVERTEX*			pVertex;			// 정점 배열 포인터
	FACE*				pFace;				// 면 배열 data 포인터
	TUV*				pUV;				// 텍스쳐 uv 좌표 배열 포인터

	D3DXMATRIXA16		matTm;				// tm 행렬
	D3DXMATRIXA16		matInvTm;			// inverse tm 행렬

	int					nNumVertex;
	int					nNumFace;
	int					nNumTVertex;
	int					nNumTVFace;

	int					nMtrlRef;

	MESH()
	{
		pNodeName=0;
		pParentName=0;
		pVertex=0;
		pFace=0;
		pUV=0;

		nNumVertex=0;
		nNumFace=0;
		nNumTVertex=0;
		nNumTVFace=0;

		nMtrlRef=0;

		D3DXMatrixIdentity(&matTm);
		D3DXMatrixIdentity(&matInvTm);
	}

	~MESH()
	{
		FREE_ARRAY_MEM(pNodeName)
		FREE_ARRAY_MEM(pParentName)
		FREE_ARRAY_MEM(pVertex)
		FREE_ARRAY_MEM(pFace)
		FREE_ARRAY_MEM(pUV)
	}
};

// MATERIAL 구조체
struct MATERIAL {
	int					nID;
	D3DMATERIAL9		Mtrl;
	char*				sTexFile;
	MATERIAL*			pSubMtrl;
	int					nNumSubMtrl;

	MATERIAL()
	{
		nID=0;
		sTexFile=0;
		pSubMtrl=0;
		nNumSubMtrl=0;
		ZeroMemory( &Mtrl, sizeof(D3DMATERIAL9) );
		Mtrl.Diffuse.r = Mtrl.Ambient.r = 1.0f;
		Mtrl.Diffuse.g = Mtrl.Ambient.g = 1.0f;
		Mtrl.Diffuse.b = Mtrl.Ambient.b = 1.0f;
		Mtrl.Diffuse.a = Mtrl.Ambient.a = 1.0f;
	}

	~MATERIAL()
	{
		FREE_ARRAY_MEM(sTexFile)

		if(nNumSubMtrl>1)
            FREE_ARRAY_MEM(pSubMtrl)
		else if(nNumSubMtrl==1)
			FREE_MEM(pSubMtrl)
	}
};


template<typename TContainer>
void stl_wipe(TContainer& container)
{
	for(TContainer::iterator i=container.begin();i!=container.end();++i)
		delete *i;

	container.clear();
}
//############### AseParser Part 구조체 정의 END

/****************************************************************
* CLASSES
****************************************************************/
class CPlane;
class CIntersectionTest;
class CMyFont;
class CCamera;
class CCameraControl;
class CCameraMng;
class CAseParser;
class CModel;
class CPoolTable;
class CBall;
class CQue;
class CGameEnv;
class CSpriteEffect;
class CSprite;
class CSpriteButton;
class CSpriteFade;
class CSpriteScale;
class CVideo;
class CPlayer;
class CReferee;
class CPoolGame;
class CPlayEngine;
class CShadowVolume;

//################### 충돌 데이타 구조체

// 테이블 쿠션 구조체
struct CUSHION {
	D3DXVECTOR3		vNormal;		// 평면의 NORMAL
	float			fD;				// ax + by + cz + d = 0
};

struct BALL_AND_CUSHION{
	CBall* pBall;
	CPlane* pPlane;
};
struct BALL_AND_BALL{
	CBall* pBall;
	CBall* pBall2;
};
union COLLISION_COUPLE{
	struct BALL_AND_CUSHION BallNcushion;
	struct BALL_AND_BALL BallNball;
};
struct COLLISION{
	COLLISION_TYPE Type;
	union COLLISION_COUPLE Cc;
	float fCollisionTime;
};

/****************************************************************
* class CMyFont
****************************************************************/
class CMyFont
{

private:

	LPD3DXFONT	m_lpFont;

public:

	CMyFont(){ m_lpFont = NULL; }
	~CMyFont(){}

	virtual void Init(LPDIRECT3DDEVICE9 lpD3DDEV, int w, int h);
	virtual void Print(char *str, int x, int y, D3DCOLOR color=0xffffffff);
	virtual void Clean();
};

/****************************************************************
* class CAseParser
****************************************************************/
class CAseParser
{
private:

	FILE*					m_fpFile;
	MATERIAL*				m_pMtrl;			// material 배열 포인터
	int						m_nNumMtrl;			// material 갯수
	MESH*					m_pMesh;
	list<MESH *>			m_pMeshList;		// 메시 리스트
	MODEL*					m_pMdEntry;

	char					m_sBuffer[BUFFERSIZE];
	char					m_sHeader[BUFFERSIZE];


	inline void ClearBuffer();

	void Parse_MATERIAL_LIST();
	void Parse_MATERIAL(MATERIAL *);
	void Parse_MAP_DIFFUSE(MATERIAL *);
	void Parse_GEOMOBJECT();
	void Parse_NODE_TM();
	void Parse_MESH();
	void Parse_MESH_VERTEX_LIST();
	void Parse_MESH_FACE_LIST();
	void Parse_MESH_TVERTLIST();
	void Parse_MESH_TFACELIST();
	void Parse_MESH_NORMALS();
	void TransformToLocal();

	void Parse_ONLY_PASS();

	void Parse_TM_ANIMATION();
	void Parse_CONTROL_ROT_TCB();
	void Parse_CONTROL_POS_TRACK();
	void Parse_CONTROL_ROT_TRACK();

	void ProcessSubMaterial(MATERIAL*,MESH*);
	void MakeModel(MODEL**,MATERIAL*,MESH*);
	MODEL*	MakeModelList();

public:
	CAseParser(){ Clear(); }
	~CAseParser(){ Destroy(); }

	MODEL *ReadAse(const char *);
	bool Clear();
	void Destroy();
};

/****************************************************************
* class CGameEnv (singleton)
* 게임 기본 환경 - directx 9 초기화 
****************************************************************/
class CGameEnv
{

private:
	// singleton - 생성자 함수 private
	CGameEnv(){ Clear(); }
	bool InitWindow();

	LPDIRECT3D9				m_pD3D;
	LPDIRECT3DDEVICE9		m_pd3dDevice;

    D3DPRESENT_PARAMETERS	m_d3dpp;

	D3DXMATRIXA16			m_matProj;

	bool					m_bFullScreen;
	HINSTANCE				m_hInst;
	HWND					m_hWnd;

	LPSTR					m_lpszClassName;
	LPSTR					m_lpszWindowTitle;

	int						m_nScreenWidth;
	int						m_nScreenHeight;

	RECT					m_rcWindowBounds;
	RECT					m_rcWindowClient;

	float					m_fFps;

	D3DLIGHT9				m_Light;

	// 정적 선언
	static CGameEnv*		m_pObj;
public:
	CMyFont					m_Font;

	// 클래스 내의 정적 변수를 사용하기 위해서는 아래 정적 함수를 사용
	static CGameEnv* GetObj();
	~CGameEnv(){ Destroy(); }

	void Clear();
	void Destroy();
	
	bool Create(HINSTANCE hInst,LPSTR lpszClassName, LPSTR lpszWindowName,
					int nScreenWidth, int nScreenHeight);

	LPDIRECT3D9	GetD3D(){ return m_pD3D; }
	LPDIRECT3DDEVICE9 GetDevice(){ return m_pd3dDevice; }
	D3DPRESENT_PARAMETERS* GetD3dpp(){ return &m_d3dpp; }
	HWND GetHwnd(){ return m_hWnd; }
	int GetScreenWidth(){ return m_nScreenWidth; }
	int GetScreenHeight(){ return m_nScreenHeight; }
	D3DXMATRIXA16& GetMatProject(){ return m_matProj; }
	D3DLIGHT9& GetLight(){ return m_Light; }

	void DetermineFPS();
	float GetFPS(){ return m_fFps; }

	void SetupLights(bool);
	void SetupProjMatrices();
	void RenderBegin();
	void RenderEnd();
	void Flip();
};

LRESULT CALLBACK MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );


/****************************************************************
* class CCamera
****************************************************************/
class CCamera
{
	D3DXVECTOR3		m_vEyePt;
    D3DXVECTOR3		m_vLookatPt;
    D3DXVECTOR3		m_vUpVec;

    D3DXVECTOR3		m_vDirection;
    D3DXVECTOR3		m_vCross;

	D3DXMATRIXA16	m_matView;

public:

	friend class CCameraControl;

	CCamera(){ Clear(); }
	~CCamera(){ Destroy(); }

	void Clear();
	void Destroy();

	void Init(D3DXVECTOR3&, D3DXVECTOR3&, D3DXVECTOR3&);
	void Update();

	void SetEyePt(D3DXVECTOR3& vEyePt){ m_vEyePt = vEyePt; }
	void SetLookatPt(D3DXVECTOR3& vLookatPt){ m_vLookatPt = vLookatPt; }
	void SetUpVec(D3DXVECTOR3& vUpVec){ m_vUpVec = vUpVec; }

	D3DXVECTOR3& GetEyePt() { return m_vEyePt; }
	D3DXVECTOR3& GetDirection() { return m_vDirection; }
	D3DXVECTOR3& GetCross() { return m_vCross; }
	D3DXVECTOR3& GetUpVec() { return m_vUpVec; }
	D3DXVECTOR3& GetLookatPt() { return m_vLookatPt; }

	D3DXMATRIXA16& GetViewMatrix()
	{
		D3DXMatrixLookAtLH( &m_matView, &m_vEyePt, &m_vLookatPt, &m_vUpVec );
		return m_matView;
	}
};

/****************************************************************
* class CCameraControl
****************************************************************/
class CCameraControl
{
protected:
	CCamera*					m_pCamera;
	D3DXQUATERNION				m_qRotate;			// 쿼터니언 값

	D3DXMATRIXA16				m_matWorld;
public:
	enum AXIS {X,Y,Z};

	CCameraControl(){ Clear(); }
	~CCameraControl(){ Destroy(); }

	void Clear();
	void Destroy();

	virtual void Init(D3DXVECTOR3&, D3DXVECTOR3&, D3DXVECTOR3&);
	D3DXMATRIXA16& Update(){ return m_pCamera->GetViewMatrix(); }

	D3DXVECTOR3& GetDirection(){ return m_pCamera->GetDirection(); }
	D3DXVECTOR3& GetUp(){ return m_pCamera->GetUpVec(); }
	D3DXVECTOR3& GetCross(){ return m_pCamera->GetCross(); }
	D3DXQUATERNION& GetQuaternion() { return m_qRotate; }
	CCamera* GetCamera() { return m_pCamera; }

	void FixLookatPt(D3DXVECTOR3& vLookatPt);

	void MoveCamera(AXIS, float, bool);		// 카메라 이동
	void RotateCamera(AXIS, float);			// 카메라 회전

	void CircleAxis(D3DXVECTOR3&,D3DXVECTOR3&, float, float);	// 지정된 축 주위로 카메라를 이동시킨다.
};

class CCameraMng : public CCameraControl
{
public:
	CCameraMng(){ Clear(); }
	~CCameraMng(){ Destroy(); }

	void Clear();
	void Destroy();

	void MoveCameraX(float);
	void MoveCameraY(float);
	void MoveCameraZ(float);

	void AdjustCameraY();
	float AdjustCameraZ(float);

	bool TakeBasePos(D3DXVECTOR3& vLookatPt);
};


/****************************************************************
* class CModel
****************************************************************/
class CModel
{
protected:
	MODEL*						m_p3dData;
	LPDIRECT3DDEVICE9			m_pd3dDevice;

	D3DXVECTOR3					m_vPos;
	D3DXMATRIXA16				m_matWorld;

public:
	CModel(){ Clear(); }
	virtual ~CModel(){ Destroy(); }

	void Clear();
	void Destroy();

	virtual bool Create(MODEL*, D3DXVECTOR3&);

	// 3d 데이타
	MODEL* Get3dData() { return m_p3dData; }
	D3DXMATRIXA16& GetMatWorld(){ return m_matWorld; }
	D3DXVECTOR3& GetPosition(){ return m_vPos; }

	void SetMatrixByPos()
	{
			D3DXMatrixTranslation( &m_matWorld, m_vPos.x, m_vPos.y, m_vPos.z );
	}
	void SetPosition(D3DXVECTOR3& vPos){ m_vPos = vPos; }
	void Set3dData(MODEL* p3dData) { m_p3dData = p3dData; }

	virtual void Primitive();
};

/****************************************************************
* class CBall
****************************************************************/
class CBall : public CModel
{	

	D3DXVECTOR3				m_vVelocity;		// 볼의 속도
	D3DXVECTOR3				m_vAccel;			// 볼의 가속도
	D3DXVECTOR3				m_vForce;			// 볼이 받는 마찰력

	D3DXVECTOR3				m_vAngVelocity;		// 각속도
	D3DXVECTOR3				m_vAngAccel;		// 각가속도

	D3DXQUATERNION			m_qR;
	D3DXMATRIXA16			m_matR;

	// 충돌 처리시 필요한 이전 값들
	D3DXVECTOR3				m_vOldPos;
	D3DXVECTOR3				m_vOldVelocity;
	D3DXMATRIXA16			m_matOldR;

	float					m_fMass;			// 볼의 질량
	float					m_fRadius;			// 볼의 반지름

	bool					m_bActivity;		// 볼이 운동중인지?

    LPDIRECT3DTEXTURE9		m_pBallTexture;
    LPDIRECT3DTEXTURE9		m_pEnvMapTexture;    // The environment map

    vector<BALLVERTEX>		m_BallVerts;		// 수정필..
public:
	CBall(){ Clear(); }
	~CBall(){ Destroy(); }

	void Clear();
	void Destroy();

	bool Create(char*, D3DXVECTOR3&);
	void Update(float);

	vector<BALLVERTEX> GetBallVerts(){ return m_BallVerts; }
	float GetMass(){ return m_fMass; }
	float GetRadius(){ return m_fRadius; }
	D3DXVECTOR3& GetOldPosition(){ return m_vOldPos; }
	D3DXVECTOR3& GetForce(){ return m_vForce; }
	D3DXVECTOR3& GetOldVelocity(){ return m_vOldVelocity; }
	D3DXVECTOR3& GetVelocity(){ return m_vVelocity; }
	D3DXVECTOR3& GetAccel(){ return m_vAccel; }
	bool GetActivity(){ return m_bActivity; }

	void SetForce(D3DXVECTOR3*,bool bIsCollision = false);
	void SetVelocity(D3DXVECTOR3& vVelocity){ m_vVelocity = vVelocity; }

	void Move(float);	
	void Roll(float fYawAngle,float fPitchAngle,float fRollAngle);

	void ClearThisTime();

	void Primitive();

	// 환경 맵 구현을 위한 메서드
	void InitEnvMap(char*);
	void CreateSphereVertices( float,vector<BALLVERTEX>*,float*);
	void DisplayBaseMap();
	void DisplayEnvMap();
	void ReleaseRenderState();
};

/****************************************************************
* class CPoolTable
****************************************************************/
class CPoolTable : public CModel
{
	CPlane**					m_ppPlane;	// up,down,left,right
	int							m_nNumPlane;

public:
	CPoolTable(){ Clear(); }
	~CPoolTable(){ Destroy(); }

	void Clear();
	void Destroy();

	bool Create(MODEL* p3dData, D3DXVECTOR3& vPos,int nNumPlane);

	CPlane** GetPlane(){ return m_ppPlane; }
	int GetNumPlane(){ return m_nNumPlane; }

	// 쿠션 설정
	void SetCushion(int nNumPlane);
};

/****************************************************************
* class CQue
****************************************************************/
class CQue : public CModel
{
	D3DXMATRIXA16		m_matAdjust;		// 모델 위치 조정 행렬
	D3DXMATRIXA16		m_matInvDir;		// 초기화시 필요한 역행렬 - 방향전환에 사용
	D3DXMATRIXA16		m_matRotate;

	D3DXVECTOR3			m_vDirection;		// 방향
	D3DXVECTOR3			m_vMovPos;			// 이동 위치

	float				m_fMass;			// 질량

public:
	CQue(){ Clear(); }
	~CQue(){ Destroy(); }

	void Clear();							// 초기화
	void Destroy();

	D3DXVECTOR3& GetMovPos(){ return m_vMovPos; }

	void SetMovPos(){ m_vMovPos.x = m_vMovPos.y = m_vMovPos.z = 0.0f; }
	void SetQueMatrix();

	D3DXVECTOR3& GetDirection(){ return m_vDirection; }

	void Init(D3DXVECTOR3&,D3DXVECTOR3&,D3DXVECTOR3&);
	void Hide(){ Ready( D3DXVECTOR3(0.0f,-10000.0f,0.0f) ); }
	bool Ready(D3DXVECTOR3&);					// 큐를 준비했는지 bool 값으로 넘긴다.
	void TurnOnYaxis(D3DXVECTOR3&,D3DXVECTOR3&,D3DXVECTOR3&);
	const D3DXVECTOR3 Shot(float);
	float Move(float);
};


/****************************************************************
* class CRoom
****************************************************************/
class CRoom : public CModel
{
public:
	CRoom(){ Clear(); }
	~CRoom(){ Destroy(); }

	void Clear();							// 초기화
	void Destroy();

	void Primitive();
};

/****************************************************************
* class CPlayer
* 플레이어 클래스
****************************************************************/
class CPlayer
{
	CBall*					m_pMyBall;
	int						m_nRestBead;
public:
	CPlayer(){ Clear(); }
	~CPlayer(){ Destroy(); }

	void Clear();
	void Destroy();

	CBall* GetMyBall(){ return m_pMyBall; }
	int GetMyRestBead(){ return m_nRestBead; }

	void Init(CBall* pBall,int nRestBead);
	void AddBead(){ if(m_nRestBead<=15) m_nRestBead++; }
	void SubtractBead(){ if(m_nRestBead>0) m_nRestBead--; }
};

/****************************************************************
* class CReferee
* 판정 클래스 - ( A:수구, B:상대방볼, C:타겟볼1, D:타겟볼2 )
****************************************************************/
class CReferee
{
public:
	enum RESULT{NOSHOT,ONESHOT,SHOT,NICESHOT,FOUL};

private:
	RESULT						m_Result;
	bool						m_bAB;						// AB 충돌 여부
	bool						m_bAC;						// AC 충돌 여부
	bool						m_bAD;						// AD 충돌 여부
	UINT						m_nCollisionCushion;		// 쿠션에 충돌한 수
	bool						m_bEnd;						// 판정이 끝났는지 여부

public:
	CReferee(){ Clear(); }
	~CReferee(){ Destroy(); }

	void Clear();
	void Destroy();

	RESULT GetResult(){ return m_Result; }
	bool GetEndReferee(){ return m_bEnd; }

	void Update(int nShotOrder,CBall* pBall,list<COLLISION*>* pCollisionList);
};

/****************************************************************
* class CPlayEngine
* 게임 플레이 엔진
****************************************************************/
class CPlayEngine
{
	LPDIRECT3DDEVICE9			m_pd3dDevice;

	// 임시 - 모델 구조체를 임의의 포맷으로 만들어 파일로 저장하고
	// 파일로 읽어올 수 있도록 수정필
	CAseParser					m_Ase;

	CReferee					m_Referee;
	CPlayer*					m_pPlayer;
	int							m_nNumPlayer;
	int							m_nShotOrder;

	// 게임에서 사용할 테이블, 볼, 큐
	CRoom						m_Room;
	CPoolTable					m_Table;			// 테이블 - 게임에서는 단 하나만 필요하다.

	CBall*						m_pBall;			// 볼이 여러개 일 수 있다.
	int							m_nNumBall;			// 볼의 갯수

	CQue*						m_pQue;				// 큐의 종류가 여러개 일 수 있다.

	CSprite**					m_ppSprite;
	CSpriteEffect*				m_pSpriteEffect;

	CCameraMng*					m_pCameraMng;		// 카메라 관리자

	PLAYFLOW					m_PlayFlow;			// 게임 플레이 흐름

	DWORD						m_dwMouseX;			// 마우스의 좌표
	DWORD						m_dwMouseY;			// 마우스의 좌표

	float						m_fDeltaTime;		// 1 tick

	list<COLLISION*>			m_CollisionList;	// 충돌 리스트

public:
	CPlayEngine(){ Clear(); }
	~CPlayEngine(){ Destroy(); }

	void Clear();
	void Destroy();

	bool Create();
	void Update();

	void DetectCollision();
	void ResolveCollision();
	void ResolveReflexion(BALL_AND_CUSHION& BnC);
	void ResolveReflexion(BALL_AND_BALL& BnB);

	void RenderScene();

	void ProcessMouse(HWND hWnd);
	void ProcessKey();
};

/****************************************************************
* class CPoolGame
* 게임 흐름 클래스
****************************************************************/
class CPoolGame
{
	LPDIRECT3DDEVICE9			m_pd3dDevice;
	CPlayEngine					m_Engine;		// 게임 동작의 주요 엔진 - 당구 엔진
	GAMEFLOW					m_nGameFlow;	// 본 게임 프로그램의 전체적인 흐름
	CSprite**					m_ppSprite;
	CSpriteButton**				m_ppSpriteButton;
	CSpriteEffect*				m_pSpriteEffect;
	
public:
	CPoolGame(){ Clear(); }
	~CPoolGame(){ Destroy(); }

	void Clear();
	void Destroy();

	bool Create();
	void Update();

	void OutputGameLogo();
	void MainStage();
	void InitPlay();
	void Play();
};

/****************************************************************
* class CSprite
* 스프라이트 클래스
****************************************************************/
class CSprite
{
	LPDIRECT3DDEVICE9			m_pd3dDevice;
	LPDIRECT3DTEXTURE9			m_pTexture;
	LPDIRECT3DVERTEXBUFFER9		m_pVB;

	D3DXMATRIXA16				m_matWorld;
	D3DXMATRIXA16				m_matView;
	D3DXMATRIXA16				m_matProj;



	int							m_nScreenWidth;
	int							m_nScreenHeight;

	bool						m_bColorKey;
	bool						m_bAnimation;
	bool						m_bCutWidth;
	bool						m_bEndAnimation;
	int							m_nMaxAnimationFrame;
	int							m_nAnimationFramecounter;

	float						m_fCellWidth_InTexture;
	float						m_fCellHeight_InTexture;
	float						m_fTotalWidth;
	float						m_fTotalHeight;
	float						m_fDelayTime;
	float						m_fCurDelayTime;
	float						m_fFps;

protected:

	SPRITE_VERTEX				m_SpriteVertex[4];

	float						m_fWidth;			// 스프라이트 가로 크기
	float						m_fHeight;
	float						m_fCellWidth;
	float						m_fCellHeight;
public:

	CSprite(LPDIRECT3DDEVICE9 pd3dDevice,int nScreenWidth,int nScreenHeight);
	~CSprite(){ Destroy(); }

	void Clear();
	void Destroy();

	LPDIRECT3DVERTEXBUFFER9	GetVB(){ return m_pVB; }
	LPDIRECT3DDEVICE9 GetDevice(){ return m_pd3dDevice; }

	void SetTexture(LPDIRECT3DTEXTURE9 pTexture){ m_pTexture = pTexture; }
	void SetPosition(float fX,float fY);
	bool SetVertexColor(D3DCOLOR Color);
	void SetFps(float fFps){ m_fFps = fFps; }
	void DirectFrame(int nFrame);

	bool CreateVertex(float fX,float fY,float fWidth,float fHeight);
	void LoadTexture(const char *pImagePath,
		D3DCOLOR ColorKey=0,
        bool bAnimation=false,
		bool bCutWidth=true,
        int nMaxAnimationFrame=0,
        float fCellWidth=0,
        float fCellHeight=0,
		float fTotalWidth=0.0f,
		float fTotalHeight=0.0f,
		float fDelayTime=0.0f);
	void OnceWhenRender();
	void ApplyColorKey();
	void ApplyAnimation();
	void Output();
};


/****************************************************************
* class CSpriteButton
* 스프라이트 버튼 클래스
****************************************************************/
class CSpriteButton : public CSprite
{
public:
	CSpriteButton(LPDIRECT3DDEVICE9 pd3dDevice,int nScreenWidth,int nScreenHeight):
	  CSprite(pd3dDevice,nScreenWidth,nScreenHeight)
  {
		  Clear();
  }
		
	~CSpriteButton(){ Destroy(); }

	void Clear();
	void Destroy();

	bool IsMouseUp();
	bool IsMouseClick();

	bool Output();
};

/****************************************************************
* class CSpriteEffect
* 스프라이트 효과 클래스
****************************************************************/
class CSpriteEffect
{
protected:
	LPDIRECT3DDEVICE9			m_pd3dDevice;
	CSprite*					m_pSprite;
	float						m_fEffectTime;
	float						m_fTotalEffectTime;
	bool						m_bEndEffect;
public:
	CSpriteEffect();
	~CSpriteEffect(){}

	virtual bool Output(float fEffectTime)=0;
};

class CSpriteFade : public CSpriteEffect
{
public:
	enum FADETYPE{FADEIN, FADEOUT};
private:
	FADETYPE					m_FadeType;
	D3DXCOLOR					m_FadeColor;
	float						m_fAlphaRate;	// 0.0f~1.0f까지 제한 알파비- 0.0f : 0%, 1.0f : 100%

	void ComputeToFade(float fFadeTime);

public:
	CSpriteFade(FADETYPE FadeType,CSprite* pTargetSprite,D3DXCOLOR FadeColor,float fTotalEffectTime,float fAlphaRate);
	~CSpriteFade(){ Destroy(); }

	void Clear();
	void Destroy();

	FADETYPE GetFadeType(){ return m_FadeType; }
	void SetToCotinue(FADETYPE FadeType){
		m_fEffectTime = 0.001f;
		m_bEndEffect = false;
		m_FadeType = FadeType;
	}

	bool Output(float fFadeTime);
};


class CSpriteScale : public CSpriteEffect
{
public:
	CSpriteScale(CSprite* pTargetSprite,float fTotalEffectTime);
	~CSpriteScale(){ Destroy(); }

	void Clear();
	void Destroy();

	bool Output(float fFadeTime);
};


/****************************************************************
* class CPlane
* 평면 클래스
****************************************************************/
class CPlane
{
public:
	D3DXVECTOR3			m_vN; //unit normal
	float				m_fD; //distance from the plane to the origin from a normal and a point
    
	CPlane( const D3DXVECTOR3& vP0, const D3DXVECTOR3& vN ): m_vN(vN), m_fD(-D3DXVec3Dot(&m_vN,&vP0)){} //from 3 points
	CPlane( const D3DXVECTOR3& vP0, const D3DXVECTOR3& vP1, const D3DXVECTOR3& vP2 )
	{
		D3DXVec3Cross(&m_vN,&(vP1-vP0),&(vP2-vP0));
		D3DXVec3Normalize(&m_vN,&m_vN);
		m_vN = -m_vN;
		m_fD = D3DXVec3Dot(&m_vN,&vP0);
	}
	//signed distance from the plane to point 'p' along 
	//the unit normal

	const float distanceToPoint( const D3DXVECTOR3& vP ) const
	{
		return ( D3DXVec3Dot(&m_vN,&vP) + m_fD );
    }
};


/****************************************************************
* class CIntersectionTest
* 교차 판정 클래스
****************************************************************/
class CIntersectionTest
{
public:
	CIntersectionTest(){ Clear(); }
	~CIntersectionTest(){ Destroy(); }

	void Clear();
	void Destroy();

	const bool SpherePlaneSweep(
							const float fRadius, //sphere radius
                            const D3DXVECTOR3& vC0, //previous position of sphere
                            const D3DXVECTOR3& vC1, //current position of sphere
                            const CPlane& plane, //the plane
                            D3DXVECTOR3& vCi, //position of sphere when it first touched the plane
                            float& fCollisionTime //normalized time of collision
							);

	inline bool CIntersectionTest::QuadraticFormula( const float fA,
                             const float fB,
                             const float fC,
                             float& fR1, //first
                             float& fR2 //and second roots
                             );

	const bool CIntersectionTest::SphereSphereSweep( const float fRa, //radius of sphere A
                             const D3DXVECTOR3& vA0, //previous position of sphere A
                             const D3DXVECTOR3& vA1, //current position of sphere A
                             const float fRb, //radius of sphere B
                             const D3DXVECTOR3& vB0, //previous position of sphere B
                             const D3DXVECTOR3& vB1, //current position of sphere B
                             float& fU0, //normalized time of first collision
                             float& fU1 //normalized time of second collision
                             );
};

/****************************************************************
* class CVideo
* 비디오 출력 클래스
****************************************************************/
//
// Macros
//
/*
#define JIF(x) if (FAILED(hr=(x))) \
    {Msg(TEXT("FAILED(hr=0x%x) in ") TEXT(#x) TEXT("\n\0"), hr); return hr;}

#define LIF(x) if (FAILED(hr=(x))) \
    {Msg(TEXT("FAILED(hr=0x%x) in ") TEXT(#x) TEXT("\n\0"), hr);}

class CVideo
{
public:
	enum PLAYSTATE {Stopped, Paused, Running, Init};

private:
	HWND					m_hWnd;

	// DirectShow interfaces
	IGraphBuilder*			m_pGB;
	IMediaControl*			m_pMC;
	IMediaEventEx*			m_pME;
	IVideoWindow*			m_pVW;
	IBasicAudio*			m_pBA;
	IBasicVideo*			m_pBV;
	IMediaSeeking*			m_pMS;
	IMediaPosition*			m_pMP;
	IVideoFrameStep*		m_pFS;

	TCHAR					m_szFileName[260];
	PLAYSTATE				m_psCurrent;

	LONG					m_lX;
	LONG					m_lY;

	BOOL					m_bAudioOnly;
	DWORD					m_dwGraphRegister;

public:
	CVideo(HWND hWnd,LPCTSTR szFileName);
	~CVideo(){ Destroy(); }

	void Clear();
	void Destroy();

	void SetVideoPos(LONG lX, LONG lY){ m_lX=lX; m_lY=lY; }
	void CloseInterfaces();
	HRESULT InitVideoWindow(int nMultiplier, int nDivider);
	HRESULT PlayMovie();

};

*/

#define g_sGameEnv		CGameEnv::GetObj()


#endif