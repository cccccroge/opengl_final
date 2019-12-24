#include "VertexLayout.h"
#include "GLEW/glew.h"

VertexLayout::VertexLayout(std::vector<int> format)
{
    attrib_num = format.size();

    int d = 0;
    for (int i = 0; i < attrib_num; ++i) {
        count.push_back(format[i]);
        offset.push_back(d);
        d += format[i] * sizeof(GLfloat);
    }

    stride = d;
}

int VertexLayout::getCount(const int index)
{
    return count[index];
}

int VertexLayout::getOffset(const int index)
{
    return offset[index];
}
