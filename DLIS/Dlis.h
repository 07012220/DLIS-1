/*! \file */

/*! \mainpage
	������ ��������� ������ � ������ ������ � ������� DLIS 1.0 \n\n
	�����: ��������� ���������
			(<a href="http://tgtoil.com"> TGT Oil & Gas Services</a>) \n
	����: C++03 \n
	������������ ����������: STL \n
	������: 0.1 \n
	����: 31.07.2015 \n\n
	�������� ������� DLIS 1.0 ��. � ���������
		<a href="http://w3.energistics.org/RP66/V1/Toc/main.html">
		RP66 V1</a>. */
// Doxy: ����� ������ ������ ������ URL-������ �� ���������� ���������, ���
//		 ������ ���� �� ��� �� ������

// ������� URL ��� Doxygen:
/*
		<a href="URL">
		LINKTEXT</a>
*/

//	������ ����������

#ifndef DLIS
#define DLIS

#include <string>
#include <vector>
#include <list>
#include <map>
#include <valarray>
#include <sstream>
#include <iostream>
#include <fstream>

#include "dlis_iter.h"

namespace Dlis {

using std::string;
using std::vector;

typedef unsigned char byte;		//!< 8-��������� ����� ��� �����
typedef signed char   int8;		//!< 8-��������� ����� �� ������

/*! \name Platform-dependent definitions
	�����������, ��������� �� ����������. \n
	��� ������������� ������� �������� �������� littleEndian � ����,
	��� ������� ������� ������,	�� ���������� ����, � ����� �����������������
	������. */
///@{
const bool littleEndian = true;
//!< ������� ����. ������� �� ����������: ��������, true ��� Intel ��� VAX � false ��� Motorola ��� IBM
// - NB: ������������� brief-�������� ���������� ��������� �� ��������
typedef unsigned short uint16;	//!< 16-��������� ����� ��� �����
typedef short int16;			//!< 16-��������� ����� �� ������
typedef unsigned int uint32;	//!< 32-��������� ����� ��� �����
typedef int int32;				//!< 32-��������� ����� �� ������
typedef float ieeeSingle;		//!< ����� ��������� �������� � ������� IEEE 754
typedef double ieeeDouble;		//!< ����� ������� �������� � ������� IEEE 754
// doxy kludge (����� Doxygen �� ����� ���������� brief-��������)
///@}
// - NB: ��� detailed-��������� ���������� ��������� Doxygen ����������
//		 ������������ HTML-���
// - NB: ����� ������ ����������� �������� �������� ��������� �����

//! �������� � ������� ���������� ���������� �������� (Return Info).
/*! ����������� ������� ������� �������� ����� ���� ���� � ������������
	��������, ���� � �������������� �������� */
class RI {
public:
	//! ���� �������� (������������ �� �������������).
	/*!	� ������� ������� ���, ��������, ����� �������� � ������ RI ��
		���������� ����� ����� (� ���� ����������� ���� ����� ����������
		� ������ ����������) */
	enum RetCode {
		Ok,
	// ������ ��������������� �����
		RdSet,
//	// ������ ����� ��������������
			MinWarn = 100,
		NoVal, NonFrameData,
		BadFrameNum, /*NoFrames,*/
		NoLFs, NonStdLRSH, NonStdSet, RepSet, NonStdFHLR, NonStdFH,

		LFWritten, DupObj, DupAttr,

		/*SULIdTrunc, FHNumFixed,*/ FHIdTrunc,
		FromRawPrec, WrongAttr, ManyObjAttrs, AmbigObjs,
		ToRawPrec,
		BadObj, ShortFrame,
			MaxWarn,
//	// ������ ����� ����������� ������
//			MinError = 500,
		BadChan, BadFrameObj, NoChan, BadFrame, NoFrame,
		/*BadValIndex,*/ BigValIndex, NonContig,
		BadRaw, FromRawNone, FromRawValErr, FromRawTypeErr,

		ToRawNone, ToRawValErr, ToRawTypeErr,

		AmbigChans,
		BadHdr, BadVRH, BadLRSH, BadLR, BadFHLRPos, BadSet, NoSetType, BadTpl,
		NoObjName, BadAttr, BadCompDescr, BadFH,

		NonStdAttr, BadRepr, BadUnits, BadDTime, BadDim, WrongCount, WrongCode,
		WrongValType,
		SULLongId, FHBadNum, BadDefOriId, NoUserFH, BadObjType, BadObjName,
		BadAttrLabel,
		AddObjErr, /*BadOriId,*/ LFNotWritten, NoFrameType, NotAllChans,

		ReadErr, WriteErr, OpenErr, DataTrunc,
		OutOfMem,
		Platform,
		ProgErr,
			MaxError,
		LastRC
	};
	// - ���� ��������� �������������� �� ������� ����������� ������
	//	 (���� ������ ����� ��������������� �� ����� ���������� ��������)
/* - NB: ����� ������ ����������� ���� policy, � ������� ���������,
		 ����� �� �������� ��������� �������� ��� ������������� ������
		 (��� �������� ������� ��� �������, �.�. ������������� ������
		 �� �������� DLIS-������, ������ �����, ������� �� ������� ��������� */
	RI() :
		m_rc(Ok), m_rcc(0), m_crit(false) {}
//! \cond
	RI(RetCode retCode, unsigned rcCase = 0) :
		m_rc(retCode), m_rcc(rcCase), m_crit(false) {}
//! \endcond
	static const string &message(RetCode retCode) {
		return msgTable().message(retCode);
	}
	//!< ��������� �� ������ ��� ���� retCode
//! \cond
	friend bool operator<(const RI &ri1, const RI &ri2);
//! \endcond
	RetCode retCode() const { return m_rc; }
	//!< ��� ��������
	string getMessage() const;
	//!< ���������� ���������� ��������� �� ������.
	/*!< ���������� ��������� �� ������ � ���������� ����� (Case), �����������
		 ���������� ���������� ����� ������ � ��������� */
	bool ok() const { return m_rc < MinWarn ; }
	//!< ���������� true ��� ���������� ������
//	operator bool() const { return ok(); }
// - NB: �������� ��������, ��� ��� ������������ ������� �������
	bool critical() const { return m_crit; }
	//!< ������� ����������� ������.
	/*!< ������������� � ���, ��� ��-�� ���� ������ ���������� �������� ����
		 ���������� */
	void clear() {
		m_rc = Ok;
		m_rcc = 0;
		m_crit = false;
	}
//! \cond
	void setOk() { clear(); }
	RI &toCritical() { m_crit = true; return *this; }
	RI &upTo(const RI &ri);
//! \endcond
private:
	struct MsgTable : std::map<RetCode, string> {
		MsgTable() {
			add(Ok,			"Ok");
			add(NoVal,		"Absent Value");
			add(NonFrameData,
				"Non-FDATA IFLR not supported");
//			add(BadValIndex,
//				"Illegal value index");
			add(BigValIndex,
				"Specified value index exceeds count characteristics");
			add(NonContig,	"Non-contiguous source/destination container");
			add(AmbigChans,	"Duplicate Channel Object names");
			add(BadHdr,		"Invalid DLIS header");
			add(BadVRH,		"Invalid Visible Record Header");
			add(BadLRSH,	"Invalid Logical Record Segment Header");
			add(BadLR,		"Invalid Logical Record format");
			add(BadFHLRPos,
				"File Header Logical Record not found or found at wrong place");
			add(BadSet,		"Invalid Set format");
			add(NoSetType,	"Null Set Type identifier");
			add(BadTpl,		"Invalid Set Template");
			add(NoObjName,	"Null Object name identifier");
			add(BadAttr,	"Invalid Object Attribute");
			add(BadCompDescr,
				"Invalid Component Descriptor");
			add(BadFH,		"None or invalid File Header");
			add(BadRaw,		"Invalid DLIS value format");
			add(FromRawNone,
				"DLIS format cannot be converted to this type");
			add(FromRawValErr,
				"Actual DLIS value cannot be loaded into this type");
			add(FromRawTypeErr,
				"This DLIS format cannot be generally loaded into this type");
			add(ToRawNone,
				"Type cannot be converted to this DLIS format");
			add(ToRawValErr,
				"Actual value cannot be saved in this format");
			add(ToRawTypeErr,
				"Values of this type cannot be generally saved in this format");
//			add(NoFrames,	"No frames of this type found");
			add(BadFrameNum,
				"Invalid Frame number");
			add(NoLFs,		"No Logical Files found");
			add(DupAttr,	"Attribute already exists");
			add(DupObj,		"Object already exists");
			add(LFWritten,	"Current Logical File already written");
			add(NonStdLRSH, "Non-standard Logical Record Segment Header");
			add(NonStdSet,	"Non-standard Set structure");
			add(RepSet,		"Replacement Set not supported");
			add(NonStdFHLR,
				"Non-standard File Header Logical Record structure");
			add(NonStdFH,	"Non-standard File Header Object");
			add(BadUnits,	"Non-standard units specified");
			add(BadDTime,	"Invalid DateTime value format");
			add(BadDim,		"Invalid Dimension specified");
			add(WrongCount,
				"Channel value count does not agree with DIMENSION value");
			add(WrongValType,
				"Suitable Representation Code for this value not found");
			add(BadRepr,	"Invalid Representation Code specified");
			add(SULLongId,	"Too long Storage Set Identifier");
			add(FHBadNum,	"Wrong Logical File SEQUENCE-NUMBER");
			add(FHIdTrunc,	"Too long Logical File ID - truncated");
			add(FromRawPrec,
				"Loss of precision while conversion from DLIS format");
			add(ToRawPrec,
				"Loss of precision while conversion to DLIS format");
			add(WrongAttr,	"Invalid Object Attribute skipped");
			add(ManyObjAttrs,
				"Out-of-template Object Attributes discarded");
			add(AmbigObjs,	"Duplicate Object Names");
			add(BadObj,		"Invalid Object discarded");
			add(ShortFrame, "Unused bytes left in Frame Data IFLR");
			add(NoChan,		"Frame Object with lacking channel discarded");
			add(BadChan,
				"Channel Object with invalid main attributes discarded");
			add(BadFrameObj,
				"Frame Object with invalid channel references discarded");
			add(BadFrame,	"Frame with lacking Frame Object discarded");
			add(NoFrame,	"Frame with specified number not found");
			add(ProgErr,	"Program error");
			add(Platform,	"Compiler or platform incompatibility");
//			add(BadOriId,	"Invalid Origin Id parameter");
			add(LFNotWritten,
							"Current Logical File metadata is not yet written");
			add(NoFrameType,
							"Frame Type not found in current Logical File");
			add(NotAllChans,
							"Not all channels found for this Frame Object");
			add(BadDefOriId,
				"Too big Defining Origin Id");
			add(NonStdAttr, "Non-standard attribute");
			add(BadAttrLabel,
				"Invalid Attribute Label");
			add(NoUserFH,	"File Header object cannot be created directly");
			add(BadObjType, "Uncertain Object Type");
			add(BadObjName, "Invalid Object Name");
			add(AddObjErr,	"Could not add new object");
			add(ReadErr,	"Read error");
			add(WriteErr,	"Write error");
			add(OpenErr,	"File open error");
			add(DataTrunc,	"Unexpected end of data");
			add(OutOfMem,	"Out of memory");
// - NB: ���� �� �������� �������� �������������� ������ add ��� ������� ����
//		 � ������� ������ ��� ����� (���� �� �� ����������), �������
//		 �� ��������� ������ ���������
		}
		const string &message(RetCode retCode) const {
			const_iterator it = find(retCode);
			return it != end() ? it->second : strEmpty;
		}
		void add(RetCode rc, const string s)	// - ��� ��������
			{ insert(std::make_pair(rc, s)); }
		string strEmpty;						// - ��� ��������
	}; // struct MsgTable
	static const MsgTable &msgTable() { static MsgTable t; return t; }
	RetCode m_rc;
	unsigned m_rcc;
	// - ������������� (case) ���� ��������; ��������� ����� ����� � ���������
	bool m_crit;
}; // class RI

//! "������ ������".
/*! ����� �������� � ��������� (�����������) ������, �� ������� ����������
	�������� ���������� (��� �������� �������� ����� �������������� ����� RI),
	������������� (���� ������) ���������� ������ � ���� ������
	<���, �������>. */
class ErrorLog : public RI {
public:
//	RI getRetInfo() const { RI ri = *this; return ri; }
	RI getRetInfo() const { return static_cast<const RI &>(*this); }
	//!< ���������� �������� � ��������� ������
	virtual unsigned frequency(RI::RetCode retCode) const = 0;
	//!< ���������� ������� ���� ������ retCode
	virtual unsigned total() const = 0;
	//!< ���������� ����� ����� ������
};


//! DLIS-������ ������������� ��������
namespace Representation {

//! ���� �������������.
/*! DLIS-���� ������������� �������� (%Representation codes). \n
	�������� �������� �������� ��� ���� ����� ��.
		<a href="http://w3.energistics.org/RP66/V1/rp66v1_appb.html">
		�����</a>. \n
	N/A - � ������ ������ ��� �� ��������������. \n
	����������. �������� ������������ ����� C++ ������� �� �����������. */
enum Code {
	Undefined = 0, Auto = 0,  //!< ��� �� ��������� (� ��������� DLIS ��� ���)
	FSHORT =  1, //!< N/A
	FSINGL =  2, //!< ������ ������������� float, 4 ����� (������ IEEE)
	FSING1 =  3, //!< N/A
	FSING2 =  4, //!< N/A
	ISINGL =  5, //!< N/A
	VSINGL =  6, //!< N/A
	// - float, 4 ����� (������ VAX), ����� ��������� ������
	FDOUBL =  7, //!< double, 8 ���� (������ IEEE)
	FDOUB1 =  8, //!< N/A
	FDOUB2 =  9, //!< N/A
	CSINGL = 10, //!< N/A
	CDOUBL = 11, //!< N/A
	SSHORT = 12, //!< signed char, 1 ����
	SNORM  = 13, //!< short, 2 �����
	SLONG  = 14, //!< int, 4 �����
	USHORT = 15, //!< unsigned char, 1 ����
	UNORM  = 16, //!< unsigned int, 2 ����
	ULONG  = 17, //!< unsigned long, 4 �����
	UVARI  = 18, //!< unsigned long, 1, 2 ��� 4 �����
	IDENT  = 19, //!< string, ���������� ����� (�������������, ����� Ident)
	ASCII  = 20, //!< string, ���������� �����
	DTIME  = 21, //!< ���� � �����, 8 ���� (����� DateTime)
	ORIGIN = 22, //!< ������ �� Origin-������, ���������� �����
	OBNAME = 23, //!< ��� �������, ���������� ����� (����� ObjectName)
	OBJREF = 24, //!< ������ �� ������, ���������� ����� (����� ObjectReference)
	ATTREF = 25, //!< N/A
	STATUS = 26, //!< bool, 1 ����
	UNITS  = 27, //!< string, ���������� ����� (������� ���������, ����� Units)
	Count
};

//! \cond
const uint32 maxUVARI = 0x3FFFFFFF;

// ������� ����������� ����������� Representation code �� ���� ��������
//TODO: ������� ���� �� ������ ������� ���� 'bool isInteger(Code code)'
bool valid(Code code);
template <class T>
Code getProperCode();
template<> Code getProperCode<int>();
template<> Code getProperCode<double>();
//bool isInt(Code code) { /*...*/ }
//bool isFloat(Code code) /*...*/ }
//bool isStr(Code code) { /*...*/ }
// ...
//! \endcond

unsigned getSize(Code code);
//!< ���������� ������ DLIS-�������� � ������ (��� -1 ��� ���������� �����)

// ������� ��������������� �������� ��������� ����� �� ���������� ������ DLIS
//	 � ������� �������� Representation code
//	 (ptrRawValue - ��������� �� ������ ���� DLIS-������������� ��������)
//RI toRawValue(const int &value, Code code, byte *ptrRawValue);
//RI fromRawValue(int &value, Code code, byte *ptrRawValue);
//// ...

}; // namespace Representation


//! ���� � �����.
struct DateTime 
{
	//! ������� ��������� ������� (���������: "������� ����")
	enum TimeZone {
		LST,	//!< Local Standard Time
		LDT,	//!< Local Daylight Savings Time
		GMT		//!< Greenwich Mean Time
	};
	DateTime() :
		year(1900), timeZone(LST), month(1), day(1),
		hour(0), min(0), sec(0), msec(0) {}
	void setDate(uint16 yr, byte m = 1, byte d = 1) {
		year = yr;
		month = m;
		day = d;
	}
	//!< ������������� ���� (����� ��� �� ��������)
	void setTime(byte h, byte mn = 0, byte s = 0, uint16 ms = 0) {
		hour = h;
		min = mn;
		sec = s;
		msec = ms;
	}
	//!< ������������� ����� ��� (���� �� ��������)
	bool valid() const;	//!< ��������� �������� �� ������������ ��������� DLIS
// Data
	uint16 year;		//!< ��� (1900..2155)
	TimeZone timeZone;	//!< ������� ���� (��. TimeZone)
	byte month;			//!< ����� (1..12)
	byte day;			//!< ���� ������ (1..31)
	byte hour;			//!< ���� (0..23)
	byte min;			//!< ������ (0..59)
	byte sec;			//!< ������� (0..59)
	uint16 msec;		//!< ������������ (0..999)
};

//! ������� ����� ��� ������� Ident � Units.
/*! ����� ��� ������������� "��������" ����� (����� 256 ��������).
	������� ������ ��������� ������������� (� ������������ � ������� set()). */
class Symbol {
public:
	Symbol(const string &s = "") { set(s); }
	Symbol(const char *s) { set(s); }
	bool empty() const { return m_s.empty(); }
	const string &str() const { return m_s; }
	void set(const string &s) { m_s = s.substr(0, 255); }
	void clear() { set(""); }
	friend bool operator==(const Symbol &s1, const Symbol &s2) {
		return s1.m_s == s2.m_s;
	}
// - NB: ���� �������� operator== ��� ���� ������, ��������� �������� ���
//		 ���������� ��������� � ������� Attribute::Impl::Component::setCharc
private:
	string m_s;
};

//! �������������.
/*! ����� �� ������ "��������" �����, ������������ ��� ������������� �����
	(����) DLIS-���������, ��� ����� ���� DLIS-�������� (����� ObjectName)
	� �.�. ������������� ���� ������������� IDENT. */
class Ident : public Symbol {
public:
	static bool valid(const string &s);
	Ident(const string &s = "") : Symbol(s) {}
	Ident(const char *s) : Symbol(s) {}
// - NB: ������������ (����� ������������� �� ��������� � ��������� �������)
//		 �� �����������
	bool valid() const { return valid(str()); }
	//!< ������� valid ��������� �������� �� ������������ ��������� DLIS.
	/*!< ��������� �������� ������ �� ������� ��������, ������������ ��� ����
		 IDENT (��.
			 <a href="http://w3.energistics.org/RP66/V1/rp66v1_appb.html#B_19">
			 �������� DLIS</a>).
*/
};

//! ������� ���������.
/*! ����� �� ������ "��������" ����� ��� ������������� ���������� �����������
	������ ���������. ������������� ���� ������������� UNITS. */
class Units : public Symbol 
{
public:
	static bool valid(const string &s);
	Units(const string &s = "") : Symbol(s) {}
	Units(const char *s) : Symbol(s) {}
	bool valid() const { return valid(str()); }
	//!< ������� valid ��������� �������� �� ������������ ��������� DLIS.
	/*!< ��������� �������� ������ �� ������� ��������, ������������ ��� ����
		 UNITS (��.
			 <a href="http://w3.energistics.org/RP66/V1/rp66v1_appb.html#B_27">
			 �������� DLIS</a>).
		 ������������ ���������� ������� ����������� ����������� �������
		 ��������� � ������ ������ ������ �� �����������. */
};

class Origin;

//! ��� DLIS-�������.
/*! ����� ��� ������������� ����� DLIS-������� (��� �������� ��� ����).
	������������ ��� ������ �� �������, ��� ������� �������� �� ���������.
	������������� ���� ������������� OBNAME. */
class ObjectName 
{
public:
	ObjectName() { clear(); }
	/*!< ������� ��������� ������ � ������� ������ */
	ObjectName(const string &idstr	/*!< ��������� ������������� ������� */,
			   uint32 originId		/*!< ������������� ORIGIN-������� */,
			   byte copyNum = 0		/*!< ����� ����� ������� */) {
		set(Ident(idstr), originId, copyNum);
	}
	/*!< ������� ��������� ������, ��������� ��������� ������������� idStr. */
	ObjectName(const Ident &id, uint32 originId, byte copyNum) {
		set(id, originId, copyNum);
	}
	/*!< ������� ��������� ������, ��������� ������������� id ���� Ident
	(��. ��. ���������� �����������) */
	bool valid() const 
    {
		return !m_id.empty() && m_id.valid() &&
			   m_ori <= Representation::maxUVARI;
	}

	//!< ��������� �������� �� ������������.
	/*!< ��������� ������������ ���� %Origin � �������������� �������,
		 � ����� ��, �� �������� �� �� ������ �������. */
	Ident ident() const { return m_id; }
	//!< ���������� ������������� �������
	uint32 originId() const { return m_ori; }
	//!< ���������� (��������) ������������� ���������������� ORIGIN-�������
	byte copyNum() const { return m_cpy; }
	//!< ���������� ����� ����� �������
	bool operator==(const ObjectName &other) const {
		return m_ori == other.m_ori && m_cpy == other.m_cpy &&
			   m_id.str() == other.m_id.str();
		// - NB: ��. ���������� � ObjectReference::operator==
	}
	void set(const Ident &id, uint32 originId, byte copyNum) {
		m_ori = originId;
		m_cpy = copyNum;
		m_id = id;
	}
	/*!< ������������� ���� ���������� ������ (��. �����������) */
	void clear() { set(Ident(""), 0, 0); }
private:
	uint32 m_ori;
	byte m_cpy;
	Ident m_id;
};

//! ������ �� DLIS-������.
/*! ������ �� DLIS-������ � ��������� ��� ����. ������������� ����
	������������� OBJREF. */
class ObjectReference 
{
public:
	ObjectReference(const string &typeId /*!< ������������� ���� �������*/,
					const ObjectName &obn /*!< ��� �������*/) :
		m_t(typeId), m_o(obn) {}
	const string &typeId() const { return m_t; }
	//!< ���������� ��� �������
	const ObjectName &objectName() const { return m_o; }
	//!< ���������� ��� �������
	bool valid() const { return !m_t.empty() && m_o.valid(); }
	//!< ��������� �������� �� ������������.
	/*!< ��������� ������������ ����� �������, � ����� ��, �� �������� ��
		 �������� ���� ������ �������. */
	bool operator==(const ObjectReference &other) const 
    {
		return m_t == other.m_t && m_o == other.m_o;
		// - NB: ��� �������, ��� ������������ ���������� ObjectReference
		//		 ��������� �����, ������� ��������� ������ (m_t � m_o)
		//		 �� �������������� ����������� �� ������
		//		 (������������ ��� ������������)
	}
private:
	string m_t;
	ObjectName m_o;
};


//! ��������� ������� � �������� �������� ��� �������� �������� ������.
/*! ������� get � getVector ���������� �������� ��������/������ ���
	���� �� ��������� ����� �������� \n\n
	������� set � setVector ������������� �������� ��������/������. \n
	������������� � �������� (���������� Attribute) ��� ������� �������������
	�������� �������� ������ � ��� ���������������� Count (����� ���������)
	� %Representation Code (��� �������������). ����� ��������� ��������
	�������� �������� ���������� val ��� ��������������� � 1, ���� �������� val
	����� ��������� ��� (��� ��� std::string). ��� ������������� ��������
	���������� code. ��� ���� �������� ����� ���������� ������ ���������������
	������������ �������� ��������� DLIS ��� ������� ��������. ���� ��������
	code ����� Representation::Auto, ��� ������������� ���������� �������������
	�� ������ ���� ��������� val � � ������ ����������� ��������. \n
	������������� � ������ (���������� Channel) ��� ������� �������������
	������ ��� ������� ��������, �. �. ��� ������������� � ����� ���������
	�������� ������ ������������� ��� ���������� REPRESENTATION-CODE �
	DIMENSION. ������� ��� ��������� �������� �������� ������ �����������
	���������� ��������� code �������� Representation::Auto. */
class ValueInterface {
public:
	typedef Representation::Code RCode;
	static const RCode rAuto = Representation::Auto;
/*! \name Read functions */
///@{
// doxy kludge
//! ������ ��� ���������� ��������
	template <typename T>
	RI get(T &val) const {
		if (count() != 1) return RI(RI::FromRawNone, 7).toCritical();
		return get(&val, 0, 1);
	}
//! ������ ��� ������ ���������� �������� ������������ �������� � �������� pos
	template <typename T>
	RI get(T &val, size_t &pos) const {
		if (pos >= count()) return RI(RI::BigValIndex).toCritical();
		return get(&val, pos, 1);
	}
//! ������ ��� ����������� std::vector
	template <class T>
	RI get(vector<T> &vec) const {
		vec.resize(count());
		return get(&vec[0], 0, vec.size());
	}
//! ������ ��� �������� ����������� � 1 ���������� (std::valarray, QVector)
	template <class T, template<class> class Vector>
	RI get(Vector<T> &vec) const {
		vec.resize(count());
		if (!contiguous(vec)) return RI(RI::NonContig, 2).toCritical();
		return get(&vec[0], 0, vec.size());
	}
//! ������ ��� �������� ����������� ����� ��� � 1 ���������� (��� std::vector)
	template <class Vector>
	RI getVector(Vector &vec) const {
		vec.resize(count());
		if (!contiguous(vec)) return RI(RI::NonContig, 1).toCritical();
		return get(&vec[0], 0, vec.size());
	}
//	long asInt(size_t index = -1) const;
//	// - ��� ����� USHORT, SHORT, ...
//	double asFloat(size_t index = -1) const;
//	// - ��� ����� FSINGL, FDOUBL, ...
//	string asStr(size_t index = -1) const;
//	// - ��� ����� IDENT, ASCII, ...
///@}
/*! \name Write functions */
///@{
// doxy kludge
//! ������ ��� ��������� ��������
	template <typename T>
	RI set(const T &val, RCode code = rAuto) {
		return set(&val, 1, code);
	}
//! ������� ��� ����������� ��������� ��������
	RI set(const char *val, RCode code = rAuto) {
		return set(&(string)val, 1, code);
	}
//! ������ ��� ����������� std::vector
	template <class T>
	RI set(const vector<T> &vec, RCode code = rAuto) {
		return set(&vec[0], vec.size(), code);
	}
//! ������ ��� �������� ����������� � 1 ���������� (std::valarray, QVector)
	template <class T, template<class> class Vector>
	RI set(const Vector<T> &vec, RCode code = rAuto) {
		if (!contiguous(vec)) return RI(RI::NonContig, 2).toCritical();
		return set(&vec[0], vec.size(), code);
	}
//! ������ ��� �������� ����������� ����� ��� � 1 ���������� (��� std::vector)
	template <class Vector>
	RI setVector(const Vector &vec, RCode code = rAuto) {
		if (!contiguous(vec)) return RI(RI::NonContig, 2).toCritical();
		return set(&vec[0], vec.size(), code);
	}
///@}
private:
//! \cond
	friend class Attribute;
	friend class Channel;
	class        Dispatcher;

	template <class Container>
	static bool contiguous(const Container &c) 
    {
		if (c.size() <= 1) return true;
		size_t lastpos = c.size() - 1;
		return &c[0] + lastpos == &c[lastpos];
	}

	ValueInterface(const Dispatcher *vd);
	~ValueInterface();
	size_t count() const;
#define GET_SET(type)								\
	RI get(type *p, size_t pos, size_t cnt) const;	\
	RI set(const type *p, size_t cnt, RCode c);
	GET_SET(int8)
	GET_SET(byte)
	GET_SET(uint16)
	GET_SET(int16)
	GET_SET(uint32)
	GET_SET(int32)
	GET_SET(ieeeSingle)
	GET_SET(ieeeDouble)
	GET_SET(string)
	GET_SET(bool)
	GET_SET(Representation::Code)
	GET_SET(Ident)
	GET_SET(Units)
	GET_SET(ObjectName)
	GET_SET(ObjectReference)
	GET_SET(DateTime)
#undef GET_SET
	const Dispatcher *const m_pvd;
//! \endcond
}; // class ValueInterface

class   AttributeDefinition;

class   AttributeParent;

//! ������� DLIS-�������.
class Attribute {
public:
	static const string &labelStrOf(const Attribute *pa);
//	const AttributeRestrictions &getRestrictions() const;
// ������� ������
	const Ident &label() const;	//!< ����� (���) ��������
//	const string &labelStr() const { return label().str(); }
	uint32 count() const;
	//!< ����� ��������� �������� (�������������� Count)
	Representation::Code representationCode() const;
	//!< ��� ������������� �������� (�������������� %Representation Code)
	const Units &units() const;
	//!< ������� ��������� �������� (�������������� %Units)
	const ValueInterface &cvalue() const;	//!< ��������� ������ ��������.
	/*!< ������������� ������� ��������� �������� ��������. */
//	template <class T> RI getValues(vector<T> &values) const;
//	std::valarray<byte> &rawValue() const {/*...*/}
//	// - ������������� DLIS-�������� � ����������� ������� DLIS
// ������� ������
//	RI setRepresentationCode(Representation::Code code);
// - NB: Representation Code ��������������� ����� value
	RI setUnits(const Units &units);
	//!< ������������� ������� ��������� �������� (�������������� %Units)
	ValueInterface &value();	//!< ��������� ������ ��������.
	/*!< ������������� ������� ��������� �������� �������� ������
		 � ���������������� Count � %Representation Code. */
//	template <class T> RI setValue(const T &value,
//	  Representation::Code code = Representation::Undefined) {/*...*/}
//	template <class T> RI setValue(const vector<T> &value,
//	  Representation::Code code = Representation::Undefined) {/*...*/}
//protected:
//// ������� �������������� ������ ������������� DLIS-�������� (��� ��������
////	 �� ��������� ������������
//	RI setValueCount(uint32 count) {/*...*/}
//	RI setRepresentationCode(Representation::Code code) {/*...*/}
//	void setRawValue(std::valarray<byte> &val) {/*...*/}
//	// - � �������� setCount � setRepresentationCode ����������� ������
//	//	 ������������ ������������ ��������
private:
//! \cond
	template <class Container>
	friend void erasePtrContainer(Container &c,
								  typename Container::iterator first,
								  typename Container::iterator last);
	friend class ValueInterface;
	friend class BaseObjectImpl;
	friend class Template;
	friend class Object;
	friend class FileHeader;
	friend class Channel;
	friend class FrameType;
	friend class Set;
	static void deletePtr(Attribute *a) { delete a; }
	Attribute(AttributeParent *parent, const Ident &label = Ident(),
			  const AttributeDefinition *ad = NULL);
	~Attribute();
	class Impl;
	Impl *const pim;
//! \endcond
}; // class Attribute

class ObjectParent;

//! ������� ����� ��� ������� DLIS-��������.
class Object {
public:
	//! ���� DLIS-��������.
	/*! �������� ������� ������������ ������������� ���������������
		%Object types ���, ��-�������, Set types, ������� ������������
		��������� Object::typeId. ������ ������ ��������������� ����� ��������,
		�������������� �������� DLIS, ��. �
			<a href="http://w3.energistics.org/RP66/V1/rp66v1_appa.html#A_2">
			�������</a>
		(������� Allowable Set Types). */
	enum Type {
		TypeUndefined,
		TypeAny = TypeUndefined,
		FILE_HEADER,
		ORIGIN,
		WELL_REFERENCE_POINT,
		AXIS,
		CHANNEL,
		FRAME,
		PATH,
		CALIBRATION,
		CALIBRATION_COEFFICIENT,
		CALIBRATION_MEASUREMENT,
		COMPUTATION,
		EQUIPMENT,
		GROUP,
		PARAMETER,
		PROCESS,
		SPLICE,
		TOOL,
		ZONE,
		COMMENT,
		MESSAGE,
//		UPDATE,
		TypeOther,
		TypeCount
	};
	static const string &typeId(Type type);
	static const ObjectName &nameOf(const Object *po);
	static ObjectReference referenceTo(const Object *po);
// ������� ������
	const ObjectName &name() const;	//!< ��� �������
	Type type() const;				//!< ��� �������
	const string &typeId() const;	//!< ��������� ������������� ���� �������
//	const vector<const Attribute *> &cattributes() const {/*...*/}
//	// - ���������� ������ ���� ��������� �������
	unsigned attributeCount() const;	//!< ���������� ��������� �������
	typedef vector<const Attribute *>::const_iterator AttributeConstIt;
	//!< ����������� �������� �� ���������
	AttributeConstIt cbeginAttribute() const; //!< �������� �� ������ �������
	AttributeConstIt cendAttribute() const;	  //!< �������� �� ��������� �������
	const Attribute *cattribute(const string &labelStr) const;
	//!< ������� � ��������� ������ (��� ������)
// ������� ������
	Attribute *attribute(const string &labelStr);
	//!< ������� � ��������� ������ (��� ������).
	/*!< ���� ������� ����������� (�� ��� �������� ��������
		 LogicalFile::introduceAttribute � �� �������� ������������� � ������
		 ������ ��� DLIS-�������� ����� ����), ������� ���������� NULL. */
	Attribute *stdAttribute(const string &labelStr);
	//!< ����������� ������� � ��������� ������ (��� ������).
	/*!< ��� ���������� �������� �� ����������� ������������� (�� ����
		 DLIS-�������� ����� ����). ���� ��, �������� ���������, ��������
		 � ����� ������ � �������� ����� ���� ���, ������� ���������� NULL. \n
		 ��. ��. LogicalFile::introduceAttribute().
		   ���������. �������� ���������� �������� ����� ������������ ���� ���
		 ���������� ������� � ������� ��������� �������� ������� ����. \n */
//	template <class T> RI addAttribute(Attribute * &attr, const Ident &label,
//	  const T &value, const Units &units = "", Representation::Code code = Representation::Undefined)
//	  {/*...*/}
//	/* - ��������� ������� ������ �� ���������, ��������� ���������
//		 � ����� �������������. ��� ���������� ���������� �� ������������
//		 ������������� �� ���� T �������� �������� �������� ������������
//		 �������� ��� ���������� Representation::getProperCode.
//		 ����� �������� (Count) ��������������� � 1 */
//	template <class T> RI addAttribute(Attribute * &attr, const Ident &label,
//	  const vector<T> &value, const Units &units = "",
//	  Representation::Code code = Representation::Undefined) {/*...*/}
//	/* - �� ��, �� ������� �������� � ���������� value.
//		 ����� �������� (Count) �������������� �� ������� value */
protected:
//! \cond
	friend class Attribute;
	friend class Set;
	friend class LogicalFile;
//	Object();
	Object(ObjectParent *parent, const ObjectName &name = ObjectName());
	virtual ~Object();
	RI addAttribute(Attribute * &attr) {/*...*/}
	// - ��������� �������, ��������� �� ��������� ������� ������
	//	 (��. ���� ���������� � protected-������� Writer::startNewLogicalFile)
	class Impl;
	Impl *pim;
private:
	static void deletePtr(Object *o) { delete o; }
//! \endcond
}; // class Object

//! ORIGIN-������ (�������� ������).
/*! �������� DLIS-������� ���� ORIGIN �������� ������� �������� ���������
	������ (��� �������� ������������� ������ Well Information LAS-�����). \n
	������ ������ ��������� ������� ORIGIN ��.
		<a href="http://w3.energistics.org/RP66/V1/rp66v1_sec5.html#5_2_1">
		�����</a>. */
class Origin: public Object {
public:
	static Object::Type type() { return Object::ORIGIN; }
private:
//! \cond
	friend class Set;
	Origin(ObjectParent *parent, const ObjectName &name = ObjectName());
//! \endcond
};

//! CHANNEL-������ (�����).
/*! �������� DLIS-������� ���� CHANNEL �������� �������� � ������ ��������
	������ (��� �������� ������������� �������� ������ � ������
	Curve Information LAS-�����). \n
	�������� REPRESENTATION-CODE, UNITS, DIMENSION � ELEMENT-LIMIT ���������
	�������������. \n
	������ ������ ��������� ������� CHANNEL ��.
		<a href="http://w3.energistics.org/RP66/V1/rp66v1_sec5.html#5_5_1">
		�����</a>. */
class Channel: public Object 
{
public:
	static Object::Type type() { return Object::CHANNEL; }
// ������� ������ �������� ���������
	Representation::Code representationCode() const;
	//!< ��� ������������� �������� ������ (������� REPRESENTATION-CODE)
	Units &units() const;
	//!< ������� ��������� �������� ������ (������� UNITS)
	const vector<uint32> &dimension() const;
	//!< ����������� �������� ������ (������� DIMENSION).
	/*!< dimension().size() ����� ����� ���������: \n
		 1 - ��� ��������� (��������������) � ��������� ��������, \n
		 2 - ��� ��������� ��������, � �.�. \n
		 ��� ��������� �������� dimension()[0]==1. */
// ������� ������������� �������� Dimension
	size_t flatCount() const;	//!< ����� ����� �������� ������.
	/*!< ����� ������������ ���� ��������� �������� Dimension. */
//	bool isScalar() const {/*...*/}
//	// - ���������� true � ������ ���������� ��������
//	bool isVector() const {/*...*/}
//	// - ���������� true � ������ ���������� �������� (��-���������� ��������
//	//	 � ������ ��������� 1)
// ������� ��������� �������� ���������
	RI setRepresentationCode(Representation::Code code);
	//!< ������������� ��� ������������� �������� ������ (������� REPRESENTATION-CODE)
	RI setUnits(const Units &units);
	//!< ������������� ������� ��������� �������� ������ (������� UNITS)
	RI setDimension (uint32 dim);
	//!< ������������� ����������� ���������� �������� ������ (������� DIMENSION)
	RI setDimension (const vector<uint32> &dims);
	//!< ������������� ������������ ����������� �������� ������ (������� DIMENSION)
	// - ������� setDimension ������������� ������������� � �� �� ��������
	//	 � ������� ELEMENT-LIMIT (������ � ������ ������ ��������� ��������
	//	 DIMENSION ����� Attribute::value::set �������� ELEMENT-LIMIT
	//	 �� ���������� )
// ������� ��������� � ��������� ������� �������� ������ (� ������� ������)
//	// - ������� getCurrentValue ���������� ��������� �������� ��� ���� ��
//	//	 ��������� ������������ ��������.
//	/* - �������� flatIndex ��������� ��� ��������� �������� ������� ������
//		 ��������, ��� ��������� �������� - ������ � ������������ �������,
//		 ���������� "�������������" ����� ������� ��������, � �.�. ���� ����
//		 �������� ������ (����� -1), �������� ������ ���� ��������� */
	const ValueInterface &ccurrentValue() const;
	//!< ��������� ������ �������� �������� ������
	ValueInterface &currentValue();
	//!< ��������� ������ �������� �������� ������
private:
//! \cond
	friend class ValueInterface;
	friend class Set;
	friend class FrameType;
	friend class Frame;
//	friend class LogicalFile;
	Channel(ObjectParent *parent, const ObjectName &name = ObjectName());
	~Channel();
	class Impl;
	Impl *const pim;
//! \endcond
}; // class Channel

//! FRAME-������ (��� ������).
/*! �������� DLIS-������� ���� FRAME �������� ������ ������� � �������� �������
	�� �������� � ����� ������ �������. \n
	����������. ������� ���������� ������ �������� ��������� ������� ��� ������
	�������� ������� / ������� ������� (��� �������� ������������� ����� ������
	LAS-�����). \n
	������� CHANNELS ��������� �������������. \n
	������ ������ ��������� ������� FRAME ��.
		<a href="http://w3.energistics.org/RP66/V1/rp66v1_sec5.html#5_7_1">
		�����</a>. \n
	����������. � �������� �� ���� ������ ���� ������: � ������� ���������
	�������� �������� INDEX-TYPE ������ �������������� �������� TIME
	(� ��������� "Index measures elapsed time."). */
class FrameType: public Object {
public:
	static Object::Type type() { return Object::FRAME; }
// ������� ������
	const vector<const Channel *> &cchannels() const;
	//!< ������ ������� (��� ������ ��������)
	const uint32 frameCount() const;	//!< ���������� ������� ������� ����
// ������� ������
	RI setChannels(const vector<ObjectName> &channelNames);
	//!< ������������� ������ ������� (������� CHANNELS)
// doxy kludge
//! \cond
	RI setChannels(const vector<Channel *> &channels);
//! \endcond
	const vector<Channel *> &channels();
	//!< ������ ������� (��� ������ ��������)
private:
//! \cond
	friend class Set;
	friend class LogicalFile;
	friend class Reader;
	FrameType(ObjectParent *parent, const ObjectName &name = ObjectName());
	~FrameType();
	class Impl;
	Impl *const pim;
//! \endcond
};

class LogicalFileParent;

//! ���������� ����.
/*! �������� ���������� (DLIS-�������) � �������� �������. ��������
	������������� LAS-����� ������ 2.0. */
class LogicalFile 
{
private:
	class ObjectIteratorData;
	ObjectIteratorData *copy(const ObjectIteratorData &d) const;
	bool same(const ObjectIteratorData &d1, const ObjectIteratorData &d2) const;
	const Object *cgetObject(const ObjectIteratorData &d) const;
	Object *getObject(const ObjectIteratorData &d);
	void iterateObject(ObjectIteratorData &d, bool fwd /*backwards = false*/) const;
public:
// ������� ������
	uint32 seqNum() const;
	//!< ���������� ����� ����������� �����.
	/*!< ��. ��. Writer::startNewLogicalFile(). */
	const string &id() const;
	//!< �������������-��������� ����������� �����.
	/*!< �������� �������� ID ��� �������� � ����� ������. \n
		 ��. ��. �������� setId(). */
	uint32 definingOriginId() const;
	//!< ������������� Defining Origin.
	/*!< �������� ������������� ORIGIN-������� Defining %Origin. */
	bool anyEncrypted() const;
	//!< ������� ������� ������������� ������
	typedef ConstPtrContainerIterator<LogicalFile, Object, ObjectIteratorData,
									  &cgetObject, &iterateObject,
									  &copy, &same> ObjectConstIt;
	//!< ����������� �������� �� DLIS-��������
	ObjectConstIt cbeginObject(Object::Type type = Object::TypeAny) const;
	//!< �������� �� ������ DLIS-������ ���� type.
	/*!< �������� Object::TypeAny ������������ ��� ������� �� �������� ����
		 �����. */
	ObjectConstIt cendObject() const;
	//!< �������� �� ��������� DLIS-������.
	/*!< ����������. ������������ �������� cendObject �������� ��������� ���
		 ���� ����� DLIS-��������. */
/*! \name Getting objects
	������� cobject ���������� ����������� DLIS-������ ���������� ����
	� � ��������� ������. \n
	��� �������� ���������� type ������� ������� ��� ���������� SpecificObject
	������� ������� (����� ���� ������������ ������ Origin, Channel
	� FrameType). \n
	��� ������� �������� ������� � ��������� name ��� ������������� --
	����������� nameIdStr, originId � copyNum (��. ����� ObjectName).
	���� �������� originId ����� -1, ������������ �������������	ORIGIN-�������
	Defining %Origin. */
///@{
	const Object *cobject(Object::Type type, const ObjectName &name) const;
	const Object *cobject(Object::Type type, const string &nameIdStr,
						  uint32 originId = -1, byte copyNum = 0) const;
	template <class SpecificObject>
	const SpecificObject *cobject(const ObjectName &name) const;
	template <class SpecificObject>
	const SpecificObject *cobject(const string &nameIdStr,
								  uint32 originId = -1, byte copyNum = 0) const;
///@}
	const Origin *cdefiningOrigin() const;
	//!< ������ Defining Origin (��� ������)
// ������� ������
	RI setId(const string &id);
	//!< ������ �������������-��������� ����������� �����.
	/*!< ������ �������� �������� ID ������� File Header. ������������ ����� --
		 65 ��������. */
	Origin *definingOrigin();
	//!< ������ Defining %Origin (��� ������)
/*! \name Adding objects
	������� addNewObject ��������� � ���������� ���� ����� (������) DLIS-������
	���������� ����	� � ��������� ������. \n
	��������� ������� ���������� ���������� ������� cobject. */
///@{
	Object *addNewObject(Object::Type type, const ObjectName &name,
						 RI *ri = NULL);
	Object *addNewObject(Object::Type type, const string &nameIdStr,
						 uint32 originId = -1, byte copyNum = 0, RI *ri = NULL);
	template <class SpecificObject>
	SpecificObject *addNewObject(const ObjectName &name, RI *ri = NULL);
	template <class SpecificObject>
	SpecificObject *addNewObject(const string &nameIdStr, uint32 originId = -1,
								 byte copyNum = 0, RI *ri = NULL);
///@}
	RI introduceAttribute(Object::Type type, const string &labelStr,
						  bool standardOnly = true);
	//!< ��������� ����� �������.
	/*!< ��������� ����� ������� � ������ labelStr ��� ���� DLIS-�������� ����
		 type. ����� �������� ����������� ������� (�� ��������� � ���������),
		 �������� ��������� standardOnly ������ ���� false. \n
		 ��. ��. Object::stdAttribute(). */
protected:
//! \cond
	RI addObject(Object * &object, ObjectName &name) {/*...*/}
	// - ��������� DLIS-������, ��������� �� ��������� ������� ������
	//	 (��. ���� ���������� � protected-������� Writer::startNewLogicalFile)
private:
	friend class FrameType;
	// - ��� ������� � ObjectIt
	friend class Reader;
	friend class Writer;
	static void deletePtr(LogicalFile *lf) { delete lf; }
	LogicalFile(LogicalFileParent *parent);
	LogicalFile(LogicalFileParent *parent,
				uint32 seqNum, const ObjectName &defOri);
	~LogicalFile();
	typedef PtrContainerIterator<LogicalFile, Object, ObjectIteratorData,
								 &getObject, &iterateObject,
								 &copy, &same> ObjectIt;
	// - ����������� �������� �� DLIS-��������
	ObjectIt beginObject(Object::Type type = Object::TypeAny);
	ObjectIt endObject(Object::Type type = Object::TypeAny);
	class Impl;
	Impl *const pim;
//! \endcond
}; // class LogicalFile
// - ����������. �������� ����������� DLIS-�������� �� �������������

template <class SpecificObject>
SpecificObject *LogicalFile::addNewObject(const ObjectName &name, RI *ri) 
{
	Object *po = addNewObject(SpecificObject::type(), name, ri);
	return dynamic_cast<SpecificObject *>(po);
}

template <class SpecificObject>
SpecificObject *LogicalFile::addNewObject(const string &nameStr,
										  uint32 originId, byte copyNum,
										  RI *ri) 
{
	Object *po = addNewObject(SpecificObject::type(), nameStr,
							  originId, copyNum, ri);
	return dynamic_cast<SpecificObject *>(po);
}

template <class SpecificObject>
const SpecificObject *LogicalFile::cobject(const ObjectName &name) const {
	const Object *po = cobject(SpecificObject::type(), name);
	return dynamic_cast<const SpecificObject *>(po);
}

template <class SpecificObject>
const SpecificObject *LogicalFile::cobject(const string &nameIdStr,
										   uint32 originId,
										   byte copyNum) const {
	const Object *po = cobject(SpecificObject::type(), nameIdStr,
							   originId, copyNum);
	return dynamic_cast<const SpecificObject *>(po);
}


//! ����� ��� ������ DLIS-������.
/*! ������������ ������ ������� �������� DLIS-������ (Storage Unit) �� �����
	��� ������. */
class Reader 
{

public:
    Reader();	//!< ������� ������ ��������� Reader
    ~Reader();	//!< � ������ ������ � ������ ��������� ����
/*! \name Open functions
	������� open �������� ������ Storage Unit ������� �� ���������
	(Storage Unit Label). ��. ��. ������� storageSetId(). */
///@{
	RI open(const string &fileName);
	//!< �������� ������ �� ����� � ������ fileName
	RI open(std::istream *stream, std::streamsize byteCount = -1);

	//!< �������� ������ �� ������ *stream.
	/*!< ����� ������ ���� ������ ��� ������ � ���������� �� ������
		 DLIS-������. \n
		 �������� byteCount ��������� ����� ����������� ������.
		 ���� �� ����� -1, ������ ����� ����������� �� ����� ������. */
///@}
	void close();	//!< � ������ ������ � ������ ��������� ����
//	bool isDlis() const;
//	// - ���������� true, ���� ����������� ������ �������� ����������
//	//	 ��������� ������� DLIS ������ 1
	const string &storageSetId() const;
	//!< ������������� Storage Set (�� Storage Unit Label).
	/*!< ������ ������ (�� ��������� DLIS) Storage Set ������� �� ����������
		 ��������� Storage Unit, �� ������, � ����� � ������ ����������,
		 ����� ��������� ����� ������, � ����� ������� Storage Set
		 � Storage Unit ���������. */
	unsigned logicalFileCount() const;	//!< ���������� ���������� ������
	typedef vector<const LogicalFile *>::const_iterator LogicalFileConstIt;
	//!< ����������� �������� �� ���������� ������
	LogicalFileConstIt cbeginLogicalFile() const;
	//!< ���������� ����������� �������� �� ������ ���������� ����
	LogicalFileConstIt cendLogicalFile() const;
	//!< ���������� ����������� �������� �� ��������� ���������� ����
	const ErrorLog &read(bool loadFrames = true);
	//!< ��������� ��� ���������� �����
	/*!< ��������� ��� ���������� ����� �� �����/������, ����� ���������
		 �������� read � ���������� ������ �� "������ ������" -- ������
		 ��������� �� ������� � ����������� ������. \n
		 ���������� ����������� � ������ ���������, � ����� ������ �������
		 (������� �������� ���������� �������� �������) ������������ ����������
		 loadFrames. \n
		 ���� loadFrames=true, ������ ����������� � ������ ���������. \n
		 ���� loadFrames=false, ������� ��������� ��������� ���� �������,
		 ����� ������� �� ������������ ��� ����������� ������������ ��������
		 ������������� ������� ��� ������ ������� readFrame(). */
/* - ����������. ��������� (� ����� �����������) �������� ������ ����������
	 � ������� ����������, �. �. ��� ����������� ������ (� ������ �����
	 �������) ������� ����������� ����� (� ��������� �� ���) ����� "������"
	 ��� ������ ���������������.
		- ����������� ���-���� ��������, �.�. ���������� ����� ����������
		  �� �������� ������� �������, �������, �� ��������� � ��������, ������
		  (�� �� �����������!) ������� (��� ��������� ����� �����������
		  ����������) � �������� �����. */
	RI readFrame(const FrameType *frameType, uint32 frameIndex);
	//!< ��������� ����� ���� frameType � � �������� frameIndex.
	/*!< �� ������������������ ������� � �����, �������� FRAME-��������
		 *frameType, ��������� ����� � �������� frameIndex.
		 �������� ������� ������ ����� �������� �������� �������
		 �urrentFrameValue() ��������������� ����������� Channel. */
private:
	class Impl;
	Impl *const pim;
};

//! ����� ��� ������ DLIS-������.
/*! ������������ ������ ������� �������� DLIS-������ (Storage Unit) � ����
	��� �����. */
class Writer {
public:
	Writer();	//!< ������� ������ ��������� Writer
	~Writer();	//!< � ������ ������ � ������ ��������� ����
/*! \name Open functions
	������� open �������� ������ Storage Unit ������� �� ���������
	(Storage Unit Label). \n
	� ��������� ����� ������� ������������� (��. Reader::storageSetId()),
	�������� ������� id. ����� ������ �� ������ ��������� 60 ��������. */
///@{
	RI open(std::ostream *stream, const string &id);
	//!< �������� ������ � ����� *stream.
	/*!< ����� ������ ���� ������ ��� ������. */
	RI open(const string &fileName, const string &id);
	//!< �������� ������ � ���� � ������ fileName
// doxy kludge
///@}
	void close();
	//!< � ������ ������ � ������ ��������� ����
	LogicalFile *startNewLogicalFile(
			uint32 seqNum = 0,
				//!< ���������� ����� ����������� �����
			const ObjectName &definingOrigin = ObjectName("DEFINING_ORIGIN", 0),
				//!< ��� ORIGIN-������� Defining %Origin
			RI *retInfo = NULL);
	//!< �������� ������ ������ ����������� �����.
	/*!< ������� ����� ���������� ���� � ��������� � ���� ������� File Header
		 � Defining %Origin. \n
		 �������� seqNum ������ �������� �������� SEQUENCE-NUMBER �������
		 File Header. �� ����� ���� ����� ������������� ������,
		 ������� ����������� ������ ����������� �����, ��������������� ���
		 � ������ Storage Unit. ���� �������� seqNum �� ������ (����� 0), ���
		 ��������������� �������������. ������������ ��� �������� �� ����� ����
		 ��������. \n
			���������� 1. ������������ ������� ������ (� ��������� retInfo)
		 ��������� ������ ������� ����������, �������, ���� ��� ����� ��������
		 �� ���������, �������� retInfo �� ����� ���������� � ����� ���� ������
		 (������ � ���������� �����������). \n
			���������� 2. ��������� ����� ����������� ����� �������� ������
		 ����������, ������� � ������� ���������� ������ �������� �����������
		 ����� ��� �������������. */
	RI writeMetadata(/*vector<Attribute *> fwdAttrs = vector<Attribute *>()*/);
	//!< ���������� ���������� (��� DLIS-�������) �������� ����������� �����.
	/*!< ��� ������� ����������� ����� ������� ����� ���� ������� ������ ����
		 ���. */
	/* - ������ fwdAttrs (�� ��������� ������) ������ ��������, ��������
		 ������� � ���������� (� �������� ������ �������) ����� ����
		 ������������ �������� rewriteAttribute. ����������� ����������
		 �������� �������, ��������, ��� ��������� �������� ��������� INDEX-MIN
		 � INDEX-MAX � ��� ���������� FRAME-�������, ����� ��� �������� �������
		 ����������, � ������������ ������ � �������� ������ �������,
		 ��� ������ ��� ������ � �������� �������, �� ���� ����������� ������
		 ��� ��������� ������� */
	RI writeNextFrame(FrameType *frameType);
	//!< ���������� ��������� ����� ���������� ����.
	/*!< ���������� ��������� ����� �� ����� ������� �������� ����������� �����
		 � �����, �������� FRAME-�������� *frameType. �������� ������� ������
		 ���� �������������� ��������� �������� ������� currentValue().set()
		 ��� ��������������� ����������� Channel. \n
		 ����������. ����������� �������� ������� ����� ������ �� ��������� �
		 ����� ������������ � ��������� ������ �� ��� ���, ���� �� �����
		 �������� ������ ����������. */
//! \cond
//	RI rewriteAttribute(Attribute *attr) {/*...*/}
//	// - �������������� �������� ������ �� ���������, ���������� ��� ������
//	//	 writeMetaData. ��������� ����� �������� ����������� ������� ��������.
//	RI rewriteAttributes() {/*...*/}
//	// - �������������� �������� ���� ����� ���������
	size_t writtenByteCount() {/*...*/}
	// - ����� ���������� ����
//! \endcond
//protected:
//	RI startNewLogicalFile(LogicalFile *lgcFile) {/*...*/}
//	/* - ���������� ��������� ������ LogicalFile (��� ��� ����������),
//		 ��������� �� ��������� ������� ������, � �������� ������ �����������
//		 ����� � ��������� � ���� ������ Defining Origin).
//		 ����� �������, ��������� ����� ����� - ��������� Writer, ����� �����
//		 ����� - ��������� LogicalFile � �.�., ����� �������� ������
//		 � protected-������ ������� ������� ������ */
//	RI writeObject(Object *object) {/*...*/}
//	/* - ���������� ��������� DLIS-������. ������� ����� ���� ������������,
//		 ��������, ��� ������ UPDATE-��������, ���������� ��������������
//		 ����� ��������� CHANNEL- � FRAME-��������, "������" ����� �������,
//		 �.�. ����� �������� writeNextFrame */
private:
	class Impl;
	Impl *const pim;
};


//class Generic {
//private:
//	template <class Container>
//	static void erasePtrContainer(Container &c,
//								  typename Container::iterator first,
//								  typename Container::iterator last) {
//		for (Container::iterator it = first; it != last; ++it) delete *it;
//		c.erase(first, last);
//	}
//	template <class Container>
//	static void clearPtrContainer(Container &c) {
//		erasePtrContainer(c, c.begin(), c.end());
//	}
//// - NB: ������ �� ���-�� ������� � ���������� �������� XXXPtrContainer,
////		 ��� ��� ��������� ������ � ����������� ����������
//};

//! \cond
#ifndef NDEBUG
// ������� ����������� ����������� ������������ ������
bool d_test_IntField();
bool d_test_ValueConvertor();
#endif // NDEBUG
//! \endcond


}; // namespace Dlis

#endif // DLIS
