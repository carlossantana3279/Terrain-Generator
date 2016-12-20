#ifndef __VECTOR3D_H__	//guard against cyclic dependancy
#define __VECTOR3D_H__

class Vector3D{
public:
	Vecot3D(float x1, float y1, float z1);
	void hello();	//hello function

	float x;
	float y;
	float z; 
private:
	float privateVar;
};

#endif