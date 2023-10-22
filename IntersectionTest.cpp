#include "poolgame.h"

void CIntersectionTest::Clear()
{
}

void CIntersectionTest::Destroy()
{
}

const bool CIntersectionTest::SpherePlaneSweep(
							const float fRadius, //sphere radius
                            const D3DXVECTOR3& vC0, //previous position of sphere
                            const D3DXVECTOR3& vC1, //current position of sphere
                            const CPlane& plane, //the plane
                            D3DXVECTOR3& vCi, //position of sphere when it first touched the plane
                            float& fCollisionTime //normalized time of collision
							)
{
	const float fD0 = plane.distanceToPoint( vC0 );
	const float fD1 = plane.distanceToPoint( vC1 );

	//check if it was touching on previous frame
	if( fabs(fD0) <= fRadius )
	{
		vCi = vC0;
		fCollisionTime = 0;
		return true;
	}

	//check if the sphere penetrated during this frame
	if( fD0>fRadius && fD1<fRadius )
	{
		fCollisionTime = (fD0-fRadius)/(fD0-fD1); //normalized time
		vCi = (1-fCollisionTime)*vC0 + fCollisionTime*vC1; //point of first contact
		return true;
	}

	return false;
}


template< class T >

inline void SWAP( T& a, T& b )
//swap the values of a and b
{
	const T temp = a;
	a = b;
    b = temp;
}

// Return true if r1 and r2 are real
inline bool CIntersectionTest::QuadraticFormula( const float fA,
                             const float fB,
                             const float fC,
                             float& fR1, //first
                             float& fR2 //and second roots
                             )
{
	const float fQ = (fB*fB) - (4.0f*fA*fC); 

	if( fQ >= 0 )
	{
		const float fSq = sqrt(fQ);
		const float fD = 1.0f / (2.0f*fA);
		fR1 = ( -fB + fSq ) * fD;
		fR2 = ( -fB - fSq ) * fD;
		return true;//real roots
	}
	else
	{
		return false;//complex roots
	}
}

const bool CIntersectionTest::SphereSphereSweep( const float fRa, //radius of sphere A
                             const D3DXVECTOR3& vA0, //previous position of sphere A
                             const D3DXVECTOR3& vA1, //current position of sphere A
                             const float fRb, //radius of sphere B
                             const D3DXVECTOR3& vB0, //previous position of sphere B
                             const D3DXVECTOR3& vB1, //current position of sphere B
                             float& fU0, //normalized time of first collision
                             float& fU1 //normalized time of second collision
                             )
{
	const D3DXVECTOR3 vA = vA1 - vA0;
	//vector from A0 to A1

    const D3DXVECTOR3 vB = vB1 - vB0;
	//vector from B0 to B1

	const D3DXVECTOR3 vA0B0 = vB0 - vA0;
	//vector from A0 to B0

	const D3DXVECTOR3 vAB = vB - vA;
	//relative velocity (in normalized time)

	const float fRab = fRa + fRb;

	const float fA = D3DXVec3Dot(&vAB,&vAB);
	//u*u coefficient

	const float fB = 2*D3DXVec3Dot(&vAB,&vA0B0);
	//u coefficient

	const float fC = D3DXVec3Dot(&vA0B0,&vA0B0) - (fRab*fRab);
	//constant term
	//check if they're currently overlapping
	if( D3DXVec3Dot(&vA0B0,&vA0B0) <= (fRab*fRab) )
	{
		fU0 = 0;
		fU1 = 0;
		return true;
	}

	//check if they hit each other 
	// during the frame
	if( QuadraticFormula( fA, fB, fC, fU0, fU1 ) )
	{
		if( fU0 > fU1 )
			SWAP( fU0, fU1 );
		return true;
	}

	return false;
}