#include "DLISParser.h"
#include "stdio.h"
#include "stdlib.h"
#include "stddef.h"
#include "assert.h"


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


CDLISParser::RepresentaionCodesLenght CDLISParser::s_rep_codes_lenght[] = 
{
    { RC_FSHORT, 2  },
    { RC_FSINGL, 4  },
    { RC_FSING1, 8  },
    { RC_FSING2, 12 },
    { RC_ISINGL, 4  },
    { RC_VSINGL, 4  },
    { RC_FDOUBL, 8  },
    { RC_FDOUB1, 16 },
    { RC_FDOUB2, 24 },
    { RC_CSINGL, 8  },
    { RC_CDOUBL, 16 },
    { RC_SSHORT, 1  },
    { RC_SNORM,  2  },
    { RC_SLONG,  4  },
    { RC_USHORT, 1  },
    { RC_UNORM,  2  },
    { RC_ULONG,  4  },
    { RC_UVARI,  1, },
    { RC_DTIME,  8  },
    { RC_STATUS, 1  },
};


CDLISParser::CDLISParser() : m_file(INVALID_HANDLE_VALUE)
{
    memset(&m_storage_unit_label, 0, sizeof(m_storage_unit_label));
    memset(&m_file_chunk,         0, sizeof(m_file_chunk));
    memset(&m_visible_record,     0, sizeof(m_visible_record));
    memset(&m_segment_header,     0, sizeof(m_segment_header));
    memset(&m_component_header,   0, sizeof(m_component_header));
    memset(&m_segment,            0, sizeof(m_segment));
}


CDLISParser::~CDLISParser()
{

}


bool CDLISParser::Parse(const char *file_name)
{
    if (!file_name)
        return false;

    if (!FileOpen(file_name))
        return false;
    
    if (!ChunkInitialize())
        return false;
        
    if (!StorageUnitLabelRead())
        return false;
    
    if (!ReadLogicalFiles())
        return false;

    return true;
}


bool CDLISParser::Initialize()
{
   memset(&m_file_chunk, 0, sizeof(m_file_chunk)); 
   return true;
}


void CDLISParser::Shutdown()
{
    FileClose();

    m_file_chunk.Free();
    memset(&m_file_chunk, 0, sizeof(m_file_chunk));
}


bool CDLISParser::FileOpen(const char *file_name)
{
    if (!file_name)
        return false;

    FileClose();

    m_file = CreateFile(file_name, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (m_file == INVALID_HANDLE_VALUE)
        return false;
        
    return true;
}


bool CDLISParser::FileClose()
{
    if (m_file != INVALID_HANDLE_VALUE)
    {
        CloseHandle(m_file);
        m_file = INVALID_HANDLE_VALUE;
    }
    return true;
}


bool CDLISParser::FileRead(char *data, DWORD len)
{
    BOOL  r;
    DWORD readed = 0;

    r = ReadFile(m_file, data, len, &readed, NULL);
    if (r == FALSE)
        return false;
    
    if (readed != len)
        return false;

    return true;
}


bool CDLISParser::FileWrite(const char *data, DWORD len)
{
    BOOL    r;
    DWORD   writed = 0;

    r = WriteFile(m_file, data, len, &writed, NULL);
    
    if (r == FALSE)
        return false;
    
    if (writed != len)
        return false;

    return true;
}


UINT64 CDLISParser::FileSeekGet()
{
    UINT64  seek;

    LONG    low_seek  = 0;
    LONG    high_seek = 0;
    
    low_seek = SetFilePointer(m_file, 0, &high_seek, FILE_CURRENT);
    
    if (high_seek)
    {
        seek = ((UINT64)high_seek << 32) | low_seek;
    }
    else
    {
        seek = low_seek;
    } 

    return seek;
}


void CDLISParser::FileSeekSet(UINT64 offset)
{
    LONG  low_seek  = 0;
    LONG  high_seek = 0;

    // �������� � ������� DWORD ���� ��������?
    if (offset & 0XFFFFFFFF00000000)
    {
        low_seek  = (LONG)(offset  & 0X00000000FFFFFFFF);
        high_seek = (LONG)((offset & 0XFFFFFFFF00000000) >> 32);
    }
    else
    {
        low_seek  = (LONG)(offset & 0X00000000FFFFFFFF);
        high_seek = 0;
    }

    SetFilePointer(m_file, low_seek, &high_seek, FILE_BEGIN);
}


UINT64 CDLISParser::FileSize()
{
    UINT64   size;
    DWORD    high_size = 0;
    DWORD    low_size;

    low_size = GetFileSize(m_file, &high_size);
     
    if (high_size)
    {
        size = ((UINT64)high_size << 32) & 0xFFFFFFFF00000000 | ((UINT64)low_size & 0xFFFFFFFF);
    }
    else
    {
        size = (size_t)low_size;
    }

    return size;
}


void CDLISParser::Big2LittelEndian(void *data, size_t len)
{
    static byte tmp[8];

    if (len > sizeof(tmp))
        return;

    byte *src = (byte *)data + len - 1;
    byte *dst = tmp;

    while (src >= data)
    {
        *(dst++) = *(src--);
    }

    memcpy(data, tmp, len);
}


void CDLISParser::Big2LittelEndianByte(byte *bt)
{
    byte dst = 0;
    byte src = *bt;
    
    for (int i = 0; i < 0xf; i++)
        if ((0x1 << i) & src)
            dst |= (0x80 >> i);

    *bt = dst;
}


void CDLISParser::RoleAndFormatFromByte(byte *role, byte *format)
{
    
}


bool CDLISParser::ChunkNextBuffer(char **data, size_t len)
{

    if (len <= m_file_chunk.remaind)      
    {
        *data = m_file_chunk.data + m_file_chunk.pos;

        m_file_chunk.pos       += len;
        m_file_chunk.remaind   -= len;
        return true;
    }


    if (m_file_chunk.remaind) 
    {
        char *src, *dst;
    
        src = m_file_chunk.data + m_file_chunk.pos;
        dst = m_file_chunk.data;
    
        memmove(dst, src, m_file_chunk.remaind);
        m_file_chunk.size = m_file_chunk.remaind;
    }
    else
        m_file_chunk.size = 0;


    DWORD   amout;
    
    amout = min(FILE_CHUNK, (DWORD)(m_file_chunk.file_remaind));    

    if (!m_file_chunk.Resize(amout + m_file_chunk.remaind))
        return false;
 

    if (!FileRead(m_file_chunk.data + m_file_chunk.remaind, amout))
        return false; 


    m_file_chunk.file_remaind -= amout;
    m_file_chunk.remaind      += amout;
    m_file_chunk.pos           = 0;
    m_file_chunk.size_chunk    = m_file_chunk.remaind;
    
    if (len <= m_file_chunk.remaind)      
    {
        *data = m_file_chunk.data;

        m_file_chunk.pos      += len;
        m_file_chunk.remaind  -= len;
        return true;
    }
       
    return false;
}


bool CDLISParser::ChunkInitialize()
{
    m_file_chunk.Free();
    memset(&m_file_chunk, 0, sizeof(m_file_chunk));
    
    m_file_chunk.file_remaind = FileSize();
    return true;
}


bool CDLISParser::ChunkEOF()
{
    return (m_file_chunk.remaind == 0 && m_file_chunk.file_remaind == 0);
}


bool CDLISParser::SegmentIsSet(byte role)
{
    bool r;
    
    r = (role == TypeRole::Redundant_Set) || (role == TypeRole::Replacement_Set) || (role == TypeRole::Set); 
    return r;
}


bool CDLISParser::SegmentIsObject(byte role)
{
    bool r;
    
    r = role == TypeRole::Object;
    return r;
}


bool CDLISParser::SegmentIsAttr(byte role)
{
    bool r;
    
    r = (role == TypeRole::Attribute) || (role == TypeRole::Invariant_Attribute) || (role == TypeRole::Absent_Attribute); 
    return r;
}


bool CDLISParser::VisibleRecordNext()
{

    m_visible_record.current = NULL;
    m_visible_record.end     = NULL;
    m_visible_record.len     = 0;    

    char                 *data;
    VisibleRecordHeader  *header;

    bool r = ChunkNextBuffer(&data, sizeof(VisibleRecordHeader));

    if (r) 
    {
        header = (VisibleRecordHeader *)data;
        Big2LittelEndian(&((*header).length), sizeof(header->length));
    }
    
    if (r)
    {
        char *record;

        m_visible_record.len = header->length - sizeof(VisibleRecordHeader);
        r = ChunkNextBuffer(&record, m_visible_record.len);            

        if (r)
        {
            m_visible_record.current = record;
            m_visible_record.end     = record + m_visible_record.len; 
        }
    }

    return r;
}


bool CDLISParser::StorageUnitLabelRead()
{
    //!!! FileRead((char *)&m_storage_unit_label, sizeof(m_storage_unit_label));
    //return true;

    bool    r;
    char   *data;

    r = ChunkNextBuffer(&data, sizeof(m_storage_unit_label));
    
    if (r)
        m_storage_unit_label = *(StorageUnitLabel *)data;

    return r;
}


bool CDLISParser::ReadLogicalFiles2()
{
    size_t   size = 80;

    SegmentHeader header = { 0 };

    char *data = (char *)&header;
    DWORD len  = sizeof(header);
    
    UINT64  file_size;
    UINT64  file_seek;

    file_size = FileSize();

    bool r = FileRead(data, len);
    size += len;

    if (!r)
        return false;

    Big2LittelEndian(&header.length, sizeof(header.length));
    header.length -= sizeof(header);

    if (header.length)
    {
       do 
       {
            char  *buf;
            
            buf = new(std::nothrow) char[header.length];
            if (!buf)
                return false;
            
            r = FileRead(buf, header.length);
            size += header.length;
            delete buf;

            printf("data len: %d\n", header.length);

            if (r)
            {
                file_seek = FileSeekGet();
                if (file_seek >= file_size)
                    break;

                data = (char *)&header;
                len  = sizeof(header);

                r = FileRead(data, len);
                size += len;
                if (!r)
                    return false;

                Big2LittelEndian(&header.length, sizeof(header.length));
                header.length -= sizeof(header);
            }

            if (!r)
                break;
       } 
       while (header.length);
    }

    return r;
}


bool CDLISParser::ReadLogicalFiles()
{
    static int count = 0;

    bool  r = VisibleRecordNext();
    if (r)
    {
        do
        {
            if (ChunkEOF())
                break;

            count ++;

            r = ReadLogicalFile();
             
            if (r)
                r = VisibleRecordNext();
        }
        while (r);
    }

    return r;
}


bool CDLISParser::ReadLogicalFile()
{
    bool r = true;
    
    while (r)
    {
        r = ReadSegment();
        if (r) 
        {
            // ����� ������� ������ 
            if (m_visible_record.current >= m_visible_record.end)
                break;
        }
    }

    assert(m_visible_record.current == m_visible_record.end);
    return r;
}


bool CDLISParser::ReadSegment()
{

    bool r = HeaderSegmentGet(&m_segment_header);

    if (r) 
    {
        if (m_segment_header.attributes & Logical_Record_Structure)
        {
            // ������ ������� � ���������� ����� 
            if (m_segment_header.type == FHLR)
            {
                int k = 0;
            }

            // ����������� �������� (������ � �� �������) � ������ ���������
            if (m_segment_header.attributes & Successor)
            {
                int k = 0;
            }
            

            // ��������������� ���������� ����������
            r = ReadComponent();
            while (r)
            {
                // ����� ��������, ������� ��� ��������� ����� ������ ������
                if (m_segment.current >= m_segment.end)
                    break;

                r = ReadComponent();
            }


        }
    }
   
    if (r)
    {
        // ��������� �� ����� �������
        m_visible_record.current += m_segment_header.length;
    }

    return r;
}


bool CDLISParser::ReadComponent()
{
    bool r;
    
    r = HeaderComponentGet();
    if (r)
    {
        if (m_component_header.role == Absent_Attribute || m_component_header.role == Attribute || m_component_header.role == Invariant_Attribute)             
        {
            r = ReadComponentAttr();
        }
        else if (m_component_header.role == Object)
        {
            r = ReadComponentObject(); 
        }
        else if (m_component_header.role == Redundant_Set || m_component_header.role == Replacement_Set || m_component_header.role == Set)
        {
            r = ReadComponentSet(); 
        }
    }

    return r;
}


bool CDLISParser::ReadComponentSet()
{
    bool   r = m_component_header.role == Redundant_Set;

    assert(r);

    if (r)
    {

    } 

    return r;
}


bool CDLISParser::ReadComponentAttr()
{
    return true;
}


bool CDLISParser::ReadComponentObject()
{
    return true;
}


bool CDLISParser::HeaderSegmentGet(SegmentHeader *header)
{
    *header = *(SegmentHeader *)m_visible_record.current; 

    //  ������� ������ ������ �������� � ��� ��������
    Big2LittelEndian(&((*header).length), sizeof(header->length));
    Big2LittelEndianByte(&((*header).attributes));
    


    short  size_header = (short)(offsetof(SegmentHeader, length_data));
    // ���������� ���������� ������ �������� (���������� ������  ��� ����������)
    header->length_data = header->length - size_header;

    if (header->attributes & Trailing_Length)
        header->length_data -= sizeof(short);

    if (header->attributes & Checksum)
        header->length_data -= sizeof(short);
    
    if (header->attributes & Padding)    
    {
        byte    *pad_len;
        char    *data;  
        // ���������� ���������� padding (�������������) �������� ��� ����� :
        // ��������� �������� �� ������ 
        // ��������� �������� + ������ ������ - 1 ���� (�.�. � ���� ����� ��������� �������� ���������� padding ����)
        // �� ����� ������ ���������� ���������� padding ��������
        data = m_visible_record.current + size_header;
        
        pad_len = (byte *)(data) + header->length_data - sizeof(byte);
        assert((data + (*pad_len)) <= m_visible_record.end);
        header->length_data -= *pad_len;
    }
    

    // �������� �������� ��������, ��� ���������� ������, ������ � �����
    m_segment.current = m_visible_record.current + size_header;
    m_segment.end     = m_segment.current + m_segment_header.length_data;
    m_segment.len     = m_segment_header.length_data;

    return true;
}


bool CDLISParser::HeaderComponentGet()
{
    byte desc = *(byte *)m_segment.current;
    
    Big2LittelEndianByte(&desc); 

    // ������� ���� � ������ Component-�
    // ��� ��������� role ������� ������� 5 ���
    // ��� �������� format ������� ������ 3 ����
    m_component_header.role   = desc & 0x7;
    m_component_header.format = desc & 0xF8;
    
    m_segment.current += sizeof(byte);
    m_segment.len     -= sizeof(byte);

    assert(m_segment.current <= m_segment.end);

    return true;
}


bool CDLISParser::ReadRawData(void *dst, size_t len)
{
    memcpy(dst, m_segment.current, len);
    
    m_segment.current += len;
    m_segment.len     -= len;

    return true;
}


bool CDLISParser::ReadRepresentationCode(RepresentaionCodes code, void **dst, size_t *len)
{
    static char buf[128] = { 0 };

    switch (code)
    {
        case RC_FSHORT:
        case RC_FSINGL:
        case RC_FSING1:
        case RC_FSING2:
        case RC_ISINGL:
        case RC_VSINGL:
        case RC_FDOUBL:
        case RC_FDOUB1:
        case RC_FDOUB2:
        case RC_CSINGL:
        case RC_CDOUBL:
        case RC_SSHORT:
        case RC_SNORM: 
        case RC_SLONG: 
        case RC_USHORT:
        case RC_UNORM: 
        case RC_ULONG: 
        case RC_DTIME: 
        case RC_STATUS:
            {
            	for (int i = 0; i < _countof(s_rep_codes_lenght); i++)
            	    if (s_rep_codes_lenght[i].code == code)
            	    {
            	        ReadRawData(buf, s_rep_codes_lenght[i].lenght);

                        m_segment.current += s_rep_codes_lenght[i].lenght;
                        m_segment.len     -= s_rep_codes_lenght[i].lenght;

                        *dst = buf;
                        *len = s_rep_codes_lenght[i].lenght;
            		    break;
            	    }

                assert(false);
                break;
            }

        case RC_IDENT:
        case RC_ASCII:
        //case RC_OBNAME:
            {
                size_t    count;
                void     *ptr;
                
                UINT      str_len = 0;
                
                if (RC_ASCII)
                    ReadRepresentationCode(RC_UVARI,  &ptr, &count);
                else
                    ReadRepresentationCode(RC_USHORT, &ptr, &count);

                assert(count <= sizeof(str_len));

                memcpy((((char *)&str_len)) + sizeof(str_len) - count - 1, ptr, count);
                ReadRawData(buf, str_len);
                
                *dst   = buf;
                *len   = str_len;
            }
            break;

        case RC_UVARI:
        case RC_ORIGIN:
            {
                byte  var_len;
                
                memset(&buf[0], 0, sizeof(buf));
                ReadRawData(buf, 1);
                
                if (*buf & 0x80)
                    var_len = 2;
                else if (*buf & 0xC0)
                    var_len = 4;
                else
                    var_len = 1;

                var_len &= ~0xC0;

                if (var_len > 1)
                    ReadRawData(&buf[1], var_len - 1);

                Big2LittelEndian(buf, var_len);

            }

        default:
            assert(false);
            break;
    }

    return true;
}


bool CDLISParser::ReadCharacteristics()
{
    //size_t              val;
    //char                str[128];
    //RepresentaionCodes  rep_code;
    
    //if (m_component_header.format & TypeAttribute::TypeAttrLable)
    //    ReadRepresentationCode(RC_IDENT, &val);

    //if (m_component_header.format & TypeAttribute::TypeAttrCount)
    //    ReadRepresentationCode(RC_UVARI, &val);

    //if (m_component_header.format & TypeAttribute::TypeAttrRepresentationCode)
    //    ReadRepresentationCode(RC_USHORT, &rep_code);

    //if (m_component_header.format & TypeAttribute::TypeAttrUnits)
    //    ReadRepresentationCode(RC_IDENT, str);

    //if (m_component_header.format & TypeAttribute::TypeAttrValue)
    //    ReadRepresentationCode(rep_code, str);
    
    return true;
}


bool CDLISParser::ReadObject()
{
    //char str[128];

    //if (m_component_header.format & TypeObject::TypeObjectName)
    //    ReadRepresentationCode(RC_IDENT, str);

    return true;
}


bool CDLISParser::ReadSet()
{
    //char tmp[128];

    //if (m_component_header.format & TypeSet::TypeSetType)
    //    ReadRepresentationCode(RC_IDENT, tmp);

    //if (m_component_header.format & TypeSet::TypeSetName)
    //    ReadRepresentationCode(RC_IDENT, tmp);

    return true;
}