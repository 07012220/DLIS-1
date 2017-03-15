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

        MAX_ATTRIBUTE_LABEL       = 64,
        MAX_TEMPLATE_ATTRIBUTES   = 32,
        //
        STATE_PARSER_FIRST        = 0x00,
        STATE_PARSER_SET          = 0x01,
        STATE_PARSER_OBJECT       = 0x02,
        STATE_PARSER_TEMPLATE_ATTRIBUTE = 0x04,
        STATE_PARSER_ATTRIBUTE    = 0x08, 
        STATE_PARSER_ALL          = STATE_PARSER_SET | STATE_PARSER_OBJECT | STATE_PARSER_TEMPLATE_ATTRIBUTE | STATE_PARSER_ATTRIBUTE,
        //
        REP_CODE_VARIABLE_SIMPLE  = -1,
        REP_CODE_VARIABLE_COMPLEX = -2,
    };
    
    struct TemplateAttributes
    {
        RepresentationCodes  code;
        UINT                count;
    };

    struct RepresentaionCodesLenght
    {
        RepresentationCodes  code;
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

    struct FrameData
    {
        DlisValueObjName  obj_key;
        DlisChannelInfo  *channels;
        int               channel_count; 
        size_t            len;

        FrameData        *next;
    };


    VisibleRecord      m_visible_record;
    SegmentRecord      m_segment;

    SegmentHeader      m_segment_header;
    ComponentHeader    m_component_header;

    FrameData         *m_frame_data;

    UINT               m_state;
    // �������� ���� (root)
    DlisSet           *m_sets;
    // �������� ������ ��������� (next) ����� � ������
    // ���������� ��� ������� ������� � ������ DLIS
    DlisSet           **m_set_tail;
    DlisObject        **m_object_tail;
    DlisAttribute     **m_attribute_tail;
    DlisAttribute     **m_column_tail;
    DlisFrameData     **m_frame_tail;

    // ���������� (��������� ����������) �������  
    // ����� ��� �������� ���������� ������� �������� ��� ���������� ������ DLIS
    DlisSet           *m_last_set;
	DlisSet			  *m_last_root_set;
    DlisObject        *m_last_object;
    DlisAttribute     *m_last_attribute;
    DlisAttribute     *m_last_column;
    DlisFrameData     *m_last_frame;

    CDLISAllocator     m_allocator;
    size_t             m_pull_id_strings;
    size_t             m_pull_id_objects;
    size_t             m_pull_id_frame_data;
    
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
    
    DlisSet        *GetRoot()     {  return m_sets;  }

    char           *AttrGetString(DlisAttribute *attr, char *buf, size_t buf_len);
    int             AttrGetInt(DlisAttribute *attr);

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
    inline void     Big2LittelEndian(void *dst, size_t len);
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
    bool            ReadCodeSimple(RepresentationCodes code, void **dst, size_t *len);
    bool            ReadCodeComplex(RepresentationCodes code, void *dst);

    bool            ReadIndirectlyFormattedLogicalRecord();
    // ������ ��������� ���������� DLIS
    bool            ReadSet();
    bool            ReadObject();
    bool            ReadAttribute();

    bool            ReadAttributeValue(DlisValue *attr_val, RepresentationCodes code, int type);

    void            SetAdd(DlisSet *set);
    void            ObjectAdd(DlisObject *obj);
    void            ColumnAdd(DlisAttribute *obj);
    void            AttributeAdd(DlisAttribute *obj);
    void            FrameAdd(DlisFrameData *frame); 
    bool            ObjectNameCompare(DlisValueObjName *left, DlisValueObjName *rigth);

    void            FlagsParserSet(UINT flag);
    char           *StringTrim(char *str, size_t *len);

    DlisAttribute  *FindColumnTemplate(DlisSet *set, DlisObject *object, DlisAttribute *attr);
    DlisAttribute  *FindAttribute(DlisSet *set, DlisObject *object, char *name_column);
    DlisSet        *FindSubSet(char *name_sub_set, DlisSet *root);
    DlisObject     *FindObject(DlisValueObjName *obj, DlisSet *set);

    FrameData      *FrameDataBuild(DlisValueObjName *obj_name);
    bool            FrameDataParse(FrameData *frame);
    FrameData      *FrameDataFind(DlisValueObjName *obj_name);

    // ���������� code representation
    void            DebugPrintRepCode(RepresentationCodes code, char *str_rep_code, size_t size);
    void            DebugPrintAttrCode(UINT attr_code, char *str_attr_code, size_t size);
};