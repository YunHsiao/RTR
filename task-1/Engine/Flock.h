#ifndef CFlock_H_
#define CFlock_H_

#include "SkeletonMesh.h"
#include "Boid.h"

class CFlock
{
public:
	CFlock();
	~CFlock();

	void onInit(const char* strFilePath, const D3DXVECTOR3& vLowerBound, 
		const D3DXVECTOR3& vUpperBound);
	void onTick(float fElapsedTime);
	void onRender();
protected:
	std::vector<CBoid*> m_boids;
	D3DXVECTOR3 m_vInitPos, m_vLowerBound, m_vUpperBound;
	
	CSkeletonMesh m_mesh;
	D3DXMATRIX m_matAdjust;
};

#endif
