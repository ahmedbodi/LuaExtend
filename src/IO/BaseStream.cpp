#include "stdafx.h"
#include "IO/BaseStream.h"

 

short BaseStream::readShort() const
{
	short v;
	read((char*)&v, sizeof(short));
	return v;
}

void BaseStream::writeShort(short v)
{
	write((char*)&v, sizeof(short));
}
  

bool BaseStream::rOrw() const
{
    return false;
}
