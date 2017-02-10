
// ������ ��������� ������ � ������ ������ � ������� DLIS
//	������ ��������������� ��������
//	�����: ��������� ��������� (TGT Oil & Gas Services)
//	����: C++03
//	������������ ����������: ���
//	������: 0.0
//	����: 03.07.2015

#ifndef DLIS_ITER
#define DLIS_ITER

#pragma warning(push)
#pragma warning(disable : 4150)  

// ����� - ������ ��������� ��� ������������ �����������, ������� ������
//	 � ����� ��������� ����� ���������.
//	 ������� ��� ������������ ���������� � ������������ ����������
// - NB: � ����������� ����������� ���������� ������������� ���������� ��������
//		 ������������� ����������, �� �� ���������, �� ������� ��� ���������
template <class Container, typename Element, typename IteratorData,
		  const Element *(Container:: *get)(const IteratorData &) const,
		  void (Container:: *iterate)(IteratorData &, bool fwd) const,
		  IteratorData *(Container:: *copy)(const IteratorData &) const,
		  bool (Container:: *same)(const IteratorData &,
								   const IteratorData &) const>
class ConstPtrContainerIterator
	: public std::iterator<std::forward_iterator_tag, Element *>
{
public:
	ConstPtrContainerIterator() : m_rc(NULL), m_pd(NULL) {}
	ConstPtrContainerIterator(const Container *c, const IteratorData &d)
		: m_rc(c), m_pd((m_rc->*copy)(d)) {}
	ConstPtrContainerIterator(const ConstPtrContainerIterator &other)
		: m_rc(other.m_rc), m_pd((m_rc->*copy)(*other.m_pd)) {}
	~ConstPtrContainerIterator() { delete m_pd; }
	ConstPtrContainerIterator &operator=(const ConstPtrContainerIterator &other)
	{
		m_rc = other.m_rc;
		m_pd = (m_rc->*copy)(*other.m_pd);
		return *this;
	}
	ConstPtrContainerIterator &operator++()
		{ (m_rc->*iterate)(*m_pd, true); return *this; }
	ConstPtrContainerIterator operator++(int) {
		ConstPtrContainerIterator tmp(*this);
		operator++();
		return tmp;
	}
	bool operator==(const ConstPtrContainerIterator &other) {
		return (m_rc->*same)(*m_pd, *other.m_pd) && m_rc == other.m_rc;
	}
	bool operator!=(const ConstPtrContainerIterator &other) {
		return !(*this == other);
	}
	const Element *operator*() const { return (m_rc->*get)(*m_pd); }
//	const IteratorData *data() const { return m_pd; }
// - ���� �� �����
private:
	const Container *m_rc;
	IteratorData *m_pd;
};

// ������� ��� ������������ ���������� � �������������� ����������
template <class Container, typename Element, typename IteratorData,
		  Element *(Container:: *get)(const IteratorData &),
		  void (Container:: *iterate)(IteratorData &, bool fwd) const,
		  IteratorData *(Container:: *copy)(const IteratorData &) const,
		  bool (Container:: *same)(const IteratorData &,
								   const IteratorData &) const>
class PtrContainerIterator
	: public std::iterator<std::bidirectional_iterator_tag, Element *>
{
public:
	PtrContainerIterator() : m_rc(NULL), m_pd(NULL) {}
	PtrContainerIterator(Container *c, const IteratorData &d)
		: m_rc(c), m_pd((m_rc->*copy)(d)) {}
	PtrContainerIterator(const PtrContainerIterator &other)
		: m_rc(other.m_rc), m_pd((m_rc->*copy)(*other.m_pd)) {}
	~PtrContainerIterator() { delete m_pd; }
	PtrContainerIterator &operator=(const PtrContainerIterator &other) {
		m_rc = other.m_rc;
		m_pd = (m_rc->*copy)(*other.m_pd);
		return *this;
	}
	PtrContainerIterator &operator++()
		{ (m_rc->*iterate)(*m_pd, true); return *this; }
	PtrContainerIterator operator++(int) {
		PtrContainerIterator tmp(*this);
		operator++();
		return tmp;
	}
	PtrContainerIterator &operator--()
		{ (m_rc->*iterate)(*m_pd, false); return *this; }
	PtrContainerIterator operator--(int) {
		PtrContainerIterator tmp(*this);
		operator--();
		return tmp;
	}
	bool operator==(const PtrContainerIterator &other) {
		return (m_rc->*same)(*m_pd, *other.m_pd) && m_rc == other.m_rc;
	}
	bool operator!=(const PtrContainerIterator &other) {
		return !(*this == other);
	}
	Element *operator*() const { return (m_rc->*get)(*m_pd); }
	const IteratorData *data() const { return m_pd; }
private:
	Container *m_rc;
	IteratorData *m_pd;
};

#pragma warning(pop)
#endif // DLIS_ITER
