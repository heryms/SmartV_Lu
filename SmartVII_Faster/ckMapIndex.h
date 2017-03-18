#pragma once

struct Rect
{
	Rect()  {}

	Rect(int a_minX, int a_minY, int a_maxX, int a_maxY)
	{
		min[0] = a_minX;
		min[1] = a_minY;

		max[0] = a_maxX;
		max[1] = a_maxY;
	}
	int min[2];
	int max[2];
};
class ckMapIndex
{
public:
	ckMapIndex(void);
	~ckMapIndex(void);
};

