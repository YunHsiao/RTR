
//-----------Windows------------
#ifndef WINDOWS_USEFUL_TOOLS_
#define WINDOWS_USEFUL_TOOLS_
#define _CRT_RAND_S
#include <stdlib.h>
#include <Windows.h>
#include <MMSystem.h>
#pragma comment(lib,"Winmm.lib")
#include <wchar.h>
#include <time.h>
#include <assert.h>
#include <zmouse.h>
#include "vld.h"
#endif

//-----------D3D头、库文件------------
#ifndef D3D_USEFUL_TOOLS_
#define D3D_USEFUL_TOOLS_
#include <d3d9.h>
#pragma comment(lib,"d3d9.lib")
#include <d3dx9.h>
#pragma comment(lib,"d3dx9.lib")
#include <rmxfguid.h>
#pragma comment(lib,"dxguid.lib")
#include <dxfile.h>
#endif

//-----------安全释放宏------------
#ifndef SAFE_CLEANUP_
#define SAFE_CLEANUP_
template<typename T>inline void Safe_Delete( T* & p )
{
	if(p) { delete p ; p = NULL; }
}
template<typename T>inline void Safe_Release( T* & p )
{
	if(p) { p->Release() ; p = NULL; }
}
template<typename T>inline void Safe_Delete_Array( T* & p )
{
	if(p) { delete[] p; p = NULL; }
}
#endif

//-----------STL------------
#ifndef STL_TOOLS_
#define STL_TOOLS_
#include <string>
#include <vector>
#include <map>
#endif

//----------Game Tools----------
#ifndef GAME_TOOLS_
#define GAME_TOOLS_

struct SVertexT
{  
	float x, y, z;
	float u, v;
	static const int FVF = D3DFVF_XYZ | D3DFVF_TEX1;
	SVertexT() {}
	SVertexT(float _x, float _y, float _z, float _u, float _v) 
		:x(_x), y(_y), z(_z), u(_u), v(_v) {}
	SVertexT(float _x, float _y, float _z, float fRadius)
		:x(_x), y(_y), z(_z) { u = x / (fRadius*2) + .5f; v = z / (fRadius*2) + .5f; }
}; 

struct SVertexD {
	float x, y, z;
	DWORD c;
	static const int FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE;
	SVertexD() {}
	SVertexD(float _x, float _y, float _z, DWORD _c) 
		:x(_x), y(_y), z(_z), c(_c) {}
	SVertexD& operator *= ( float m )
	{ x*=m; y*=m; z*=m; return *this; }
	SVertexD operator * ( float m ) const
	{ SVertexD t(x*m, 0.f, z*m, c); return t; }
	SVertexD& operator += ( SVertexD v )
	{ x+=v.x; z+=v.y; return *this; }
	SVertexD operator + ( SVertexD v ) const
	{ SVertexD t(x+v.x, y+v.y, z+v.z, c); return t; }
};

struct SVertexRHWD {
	float x, y, z, rhw;
	DWORD c;
	static const DWORD FVF = D3DFVF_XYZRHW | D3DFVF_DIFFUSE;
	SVertexRHWD(float _x, float _y, float _z, float _rhw, DWORD _c) :
	x(_x), y(_y), z(_z), rhw(_rhw), c(_c) {}
};

struct SVertexNT2 {
	float x, y, z;
	float nx, ny, nz;
	float u, v;
	float u2, v2;
	static const int FVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX2;
	SVertexNT2(float _x, float _y, float _z, float _nx, float _ny, float _nz,
		float _u, float _v)
		:x(_x), y(_y), z(_z), nx(_nx), ny(_ny), nz(_nz), u(_u), v(_v), u2(_u), v2(_v) {}
};

struct SVertexNT
{
	float x, y, z;
	float nx, ny, nz;
	float u, v;
	static const int FVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;
	SVertexNT(float _x, float _y, float _z, float _nx, float _ny, float _nz, 
		float _u, float _v) 
		:x(_x), y(_y), z(_z), nx(_nx), ny(_ny), nz(_nz), u(_u), v(_v) {}
};
#endif
