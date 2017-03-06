#pragma once 

#include    "new.h"
#include    "DlisCommon.h"
#include    "FileBin.h"
#include    "DlisAllocator.h"

// ���������� ������ ���� (��� �������)
extern CFileBin *g_global_log;


// ����� ����������� ������
struct MemoryBuffer
{
    char            *data;
    size_t           size;
    size_t           max_size;

    bool             Resize(size_t new_len); 
    void             Free();    
};


class CDLISParser
{
private:
    enum constants
    {
        Kb         = 1024,
        Mb         = Kb * Kb,
        FILE_CHUNK = 16 * Mb,

        MAX_TEMPLATE_ATTRIBUTES   = 32,
        //
        STATE_PARSER_FIRST        = 0x00,
        STATE_PARSER_SET          = 0x01,
        STATE_PARSER_OBJECT       = 0x02,
        STATE_PARSER_TEMPLATE_ATTRIBUTE = 0x04,
        STATE_PARSER_ATTRIBUTE    = 0x08, 
        STATE_PARSER_ALL          = STATE_PARSER_SET | STATE_PARSER_OBJECT | STATE_PARSER_TEMPLATE_ATTRIBUTE | STATE_PARSER_ATTRIBUTE,

        STATE_FIRST_SEGMEN_LOGICAL_FILE   = 0x10,
        STATE_SECOND_SEGMENT_LOGICAL_FILE = 0x20,        
        //
        REP_CODE_VARIABLE_SIMPLE  = -1,
        REP_CODE_VARIABLE_COMPLEX = -2,
    };
    
    struct TemplateAttributes
    {
        RepresentaionCodes  code;
        UINT                count;
    };

    struct RepresentaionCodesLenght
    {
        RepresentaionCodes  code;
        int                 length;
    };

    typedef unsigned char byte;
    //  ����� ����� 
    HANDLE              m_file;
    // ��������� DLIS
    StorageUnitLabel    m_storage_unit_label;

    // ���������� ������ ������ �� ����� DLIS    
    struct FileChunk : MemoryBuffer
    {
        size_t      pos;
        size_t      remaind;
        size_t      size_chunk;
        UINT64      file_remaind;
    };
    
    FileChunk        m_file_chunk;

    // ��������� DLIS visible record
    struct VisibleRecord
    {
        char      *current;
        char      *end; 
        size_t     len;
    };

    // ��������� DLIS �������� 
    struct SegmentRecord
    {
        char            *current;
        char            *end;
        size_t           len;
    };
    

    VisibleRecord      m_visible_record;
    SegmentRecord      m_segment;

    SegmentHeader      m_segment_header;
    ComponentHeader    m_component_header;

    UINT               m_state;

    TemplateAttributes m_template_attributes[MAX_TEMPLATE_ATTRIBUTES];
    UINT               m_template_attributes_count;
    UINT               m_attributes_count;
    UINT               m_object_num;

    // �������� ���� (root)
    DlisSet           *m_sets;
    // �������� ������ ��������� (next) ����� � ������
    // ���������� ��� ������� ������� � ������ DLIS
    DlisSet           **m_set;
    DlisObject        **m_object;
    DlisAttribute     **m_attribute;
    DlisAttribute     **m_column;

    // ���������� (��������� ����������) �������  
    // ����� ��� �������� ���������� ������� �������� ��� ���������� ������ DLIS
    DlisSet           *m_last_set;
    DlisObject        *m_last_object;
    DlisAttribute     *m_last_attribute;
    DlisAttribute     *m_last_column;

    CDLISAllocator     m_allocator;
    size_t             m_pull_id_strings;
    size_t             m_pull_id_objects;
    
private:
   static RepresentaionCodesLenght s_rep_codes_length[RC_LAST];
 
public:
    CDLISParser();
    ~CDLISParser();

    // ������� DLIS
    bool            Parse(const char *file_name);

    // �������������, �������� ���������� ������� � ������ �� �������
    bool            Initialize();
    void            Shutdown();

private:
    //  ������ �����
    bool            FileOpen(const char *file_name);
    bool            FileClose();
    bool            FileRead(char *data, DWORD len);
    bool            FileWrite(const char *data, DWORD len);
    UINT64          FileSeekGet();
    void            FileSeekSet(UINT64 offset);
    UINT64          FileSize();
    // �������������� bit 2 littel endian
    void            Big2LittelEndian(void *dst, size_t len);
    void            Big2LittelEndianByte(byte *byte);

    // ������ ��������� � ���������� ������
    bool            ReadStorageUnitLabel();
    bool            ReadLogicalFiles();

    // ������ ����������� ������
    bool            BufferNext(char **data, size_t len);
    bool            BufferInitialize();
    bool            BufferIsEOF();
    bool            VisibleRecordNext();

    // ������ �������� DLIS ������ 
    bool            SegmentGet();
    bool            SegmentProcess(); 

    // ������ ���������� DLIS
    bool            ComponentRead();
    bool            ComponentHeaderGet();

    // ������ ����� ������ DLIS
    bool            ReadRawData(void *dst, size_t len);
    bool            ReadCodeSimple(RepresentaionCodes code, void **dst, size_t *len, int count = 1);
    bool            ReadCodeComplex(RepresentaionCodes code, void *dst);

    // ������ ��������� ���������� DLIS
    bool            ReadSet();
    bool            ReadObject();
    bool            ReadAttribute();

    bool            ReadAttributeValue(DlisValue *attr_val, RepresentaionCodes code, int type);

    void            SetAdd(DlisSet *set);
    void            ObjectAdd(DlisObject *obj);
    void            ColumnAdd(DlisAttribute *obj);
    void            AttributeAdd(DlisAttribute *obj);
    
    void            FlagsParserSet(UINT flag);
    void            FlagAttrSet(UINT flag);

    char           *StringTrim(char *str, size_t *len);
    DlisAttribute  *AttrRepresentationCodeFind(DlisSet *set, DlisObject *object, DlisAttribute *attr);
    // ���������� code representation
    void            DebugPrintRepCode(RepresentaionCodes code, char *str_rep_code, size_t size);
    void            DebugPrintAttrCode(UINT attr_code, char *str_attr_code, size_t size);
    void            DebugPrintTables(DlisSet *root, bool is_child, int ident);
};