#pragma once

#include "Vertex.h"
#include <vector>


class VertexLayout
{
public:
    VertexLayout(std::vector<int> format);

    inline int getAttribNum() { return attrib_num; }
    inline int getStride() { return stride; }
	int getCount(const int index);
	int getOffset(const int index);

private:
    int attrib_num;
    int stride;
    std::vector<int> count;
    std::vector<int> offset;
};