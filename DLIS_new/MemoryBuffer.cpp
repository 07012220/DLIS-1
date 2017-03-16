#include "MemoryBuffer.h"
#include "windows.h"
#include "new.h"

bool MemoryBuffer::Resize(size_t new_max_size)
{

    // ���� ������� ������ �������, ������� - �������� ������ �� ����
    if (new_max_size < max_size)
        return true;
    
    char   *buf = NULL;
    size_t  cap = new_max_size / 4;

    // ��������� ������ ������ ������ �� ������������ � ������������� ��������
    if (cap < 16)    
        cap = 16;
    
    if (cap > 1024 * 1024)
        cap  = 32 * 1024;

    // ����� ������� ������
    cap += new_max_size;
    
    // ������� ������ ��� �����
    buf = new(std::nothrow) char[cap];
    if (!buf)
        return false;
    
    // �������� ������ ������
    memcpy(buf, data, size);
    max_size = cap;
     
    // ��������� ������ ������ � �������� ����� �����
    delete data;
    data = buf;

    return true;
}


void MemoryBuffer::Free()
{
    if (data)
        delete data;
    
    data     = NULL;
    size     = 0;
    max_size = 0;
}

