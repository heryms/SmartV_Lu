#include "stdafx.h"
#include "Interface.h"
#include "MapPath.h"
IMapInterface * CreateInstance()
{
	IMapInterface *pI= new MapCreater();
	return pI;
}