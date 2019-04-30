#pragma once

#include <set>

#include <boost/ptr_container/ptr_list.hpp>
#include <boost/iostreams/device/back_inserter.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/archive/text_wiarchive.hpp>
#include <boost/archive/text_woarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/range/size.hpp>
#include <boost/property_tree/ptree.hpp>

#include "INGPFileIO.h"

using boost::property_tree::wptree;

//////////////////////////////////////////////////////////////////////////

class CProperty {
public:
	virtual void CopyFrom(const CProperty&) = 0;
	virtual bool IsEqualTo(const CProperty&) const = 0;
	virtual wptree& SaveToPTree(wptree&) const = 0;
	virtual const wptree* LoadFromPTree(const wptree&) = 0;
	virtual void SaveToBinFile(const ngp_file_io::INGPFileIOPtr&) const = 0;
	virtual void LoadFromBinFile(const ngp_file_io::INGPFileIOPtr&) = 0;

	const std::wstring& GetTag() const { return m_xmlTag; }
	virtual ~CProperty() {}

protected:
	CProperty(const wchar_ngp* xml_tag) :m_xmlTag(xml_tag) {}
	CProperty(const CProperty&) = delete;
	void operator=(const CProperty&) {}

protected:
	const std::wstring m_xmlTag;

public:
	static const wchar_ngp* ItemXMLTag;
};

typedef std::unique_ptr<CProperty> CPropertyPtr;

//////////////////////////////////////////////////////////////////////////

enum EPropertiesFilter {
    PF_All,
    PF_Serializable,
    PF_Assignable,		// a property has both getter and setter, thus is adjustable and can be checked for equality
	PF_AssignableExceptPalettes,
	PF_NonProjectLocal,	// new used in AssignFeatureKeepingProjectDependProperty
	PF_ForEqualityTest,
};

struct CollectOptions
{
    const EPropertiesFilter m_filter;

    CollectOptions() : m_filter(PF_All) { }
    CollectOptions(EPropertiesFilter f) : m_filter(f) { }
};

//////////////////////////////////////////////////////////////////////////

class CPropertyTreeNode
{
public:
    CPropertyTreeNode& Add(CProperty*);
    CPropertyTreeNode& AddSubGroup(const wchar_ngp* group_tag);
    wptree& SaveToPTree(wptree&) const;
    const wptree* LoadFromPTree(const wptree&, bool extractProperty = true);
    void SaveToBinFile(const ngp_file_io::INGPFileIOPtr&) const;
    void LoadFromBinFile(const ngp_file_io::INGPFileIOPtr&);

	EPropertiesFilter getFilter() const { return m_Options.m_filter; }
	CollectOptions& getOptions() { return m_Options; }

protected:
	CPropertyTreeNode(CProperty*, CollectOptions&);

    void VisitAll(const boost::function<void(CProperty*)>&);
    void VisitAllc(const boost::function<void(const CProperty*)>&) const;
    bool IsContaining(const std::wstring& tag) const;
    void UpdateArrayFlag(const std::wstring& propertyTag);

protected:
    CPropertyPtr m_Property;

    bool m_IsArray;

    typedef boost::ptr_list<CPropertyTreeNode> TNodesList;
    TNodesList m_SubNodes;
private:
	CollectOptions& m_Options;
};

//////////////////////////////////////////////////////////////////////////

class CPropertyRefCollection
    : public CPropertyTreeNode
{
public:
	explicit CPropertyRefCollection(const wchar_ngp* rootTag, EPropertiesFilter=PF_All);

    bool operator==(const CPropertyRefCollection&) const;
    void CopyFrom(const CPropertyRefCollection&);

    bool SaveToBinaryFile(const fs::path& filename, const long version = 0) const;
    bool SaveToBinaryFile(const ngp_file_io::INGPFileIOPtr& file, const long version = 0) const;
    bool LoadFromBinaryFile(const fs::path& filename, const long version = 0);
    bool LoadFromBinaryFile(const ngp_file_io::INGPFileIOPtr& file, const long version = 0);

    static bool SaveTreeToFile(const fs::path& filename, const wptree& tag);

private:
	CollectOptions m_Options;
};

//////////////////////////////////////////////////////////////////////////

class CCollectionableData :
	boost::noncopyable
{
public:
	virtual ~CCollectionableData() {}

	virtual bool AssignProperties(const CCollectionableData&, EPropertiesFilter);

	bool operator==(const CCollectionableData&) const;
	bool IsEqual(const CCollectionableData&) const;

	virtual bool SaveToFile(const fs::path& filename) const;
    virtual bool SaveToFile(const ngp_file_io::INGPFileIOPtr& file) const;
	virtual bool LoadFromFile(const fs::path& filename);
    virtual bool LoadFromFile(const ngp_file_io::INGPFileIOPtr& file);

protected:
	bool SerializeAsPtree(bool save_load, ngp_shared_array<char>& data);
	bool SaveToPTree(wptree& tag, EPropertiesFilter f = PF_Serializable) const;
	bool LoadFromPTree(const wptree& tag, EPropertiesFilter f = PF_Serializable);

private:
	virtual CNGPString GetRootTag() const = 0;
	virtual long GetVersion() const = 0;
	virtual void CollectProperties(CPropertyTreeNode&, const CollectOptions&) const = 0;

    bool Save(std::function<bool(const CPropertyRefCollection&)> save, EPropertiesFilter) const;
    bool Load(std::function<bool(CPropertyRefCollection&)> load, EPropertiesFilter);
};

//////////////////////////////////////////////////////////////////////////

template<typename TYPE>
static bool CompareOperator(const TYPE& a, const TYPE& b)
{
	return _CompareOperator(a, b, boost::is_floating_point<TYPE>());
}

template<class TYPE>
bool operator==(const ngp_shared_array<TYPE>& lhs, const ngp_shared_array<TYPE>& rhs)
{
	if (lhs.size() != rhs.size())
		return false;

	return std::equal(lhs.cbegin(), lhs.cend(), rhs.cbegin());
}

template<typename TYPE>
static bool _CompareOperator(const TYPE& a, const TYPE& b, const boost::true_type&) //if floating point value
{
	return std::fabs( a - b ) < 1e-10;
}

template<typename TYPE>
static bool _CompareOperator(const TYPE& a, const TYPE& b, const boost::false_type&)
{
	return a == b;
}

namespace prop_ref_coll
{

template<typename T>
struct default_ops
{
	static bool operator_equal( const T& lhs, const T& rhs )
	{
		return lhs == rhs;
	}

	typedef bool( *static_operator_equal_t )(const T& lhs, const T& rhs);
};

}

//////////////////////////////////////////////////////////////////////////

template<typename PROPERTY_TYPE>
struct CPropertyBaseSaveLoadToTree
{
    static void Save(const PROPERTY_TYPE* pValue, wptree& prop_node)
    {
        prop_node.put_value(*pValue);
    }

    static void Load(PROPERTY_TYPE* pValue, const wptree& prop_node)
    {
        if (boost::optional<PROPERTY_TYPE> val = prop_node.get_value_optional<PROPERTY_TYPE>())
            *pValue = *val;
    }
};

template<typename PROPERTY_TYPE>
struct CPropertyBaseSaveLoadToBinFile
{
    static void Save(const PROPERTY_TYPE* pValue, const ngp_file_io::INGPFileIOPtr& bin_file)
    {
        bin_file->write(pValue, 1);
    }

    static void Load(PROPERTY_TYPE* pValue, const ngp_file_io::INGPFileIOPtr& bin_file)
    {
        bin_file->read(pValue, 1);
    }
};

//Write non-default code with specifics as specializations below:

template<>
struct CPropertyBaseSaveLoadToTree<CNGPString>
{
    static void Save(const CNGPString* pValue, wptree& prop_node)
    {
        const std::wstring s = pValue->c_str();
        prop_node.put_value(s);
    }

    static void Load(CNGPString* pValue, const wptree& prop_node)
    {
        if (boost::optional<std::wstring> s = prop_node.get_value_optional<std::wstring>())
            *pValue = s->c_str();
    }
};

template<>
struct CPropertyBaseSaveLoadToBinFile<std::wstring>
{
    static void Save(const std::wstring* pValue, const ngp_file_io::INGPFileIOPtr& bin_file)
    {
        const CNGPString s = pValue->c_str();
        bin_file->write(s);
    }

    static void Load(std::wstring* pValue, const ngp_file_io::INGPFileIOPtr& bin_file)
    {
        CNGPString s;
        bin_file->read(s);
        *pValue = s.c_str();
    }
};

//////////////////////////////////////////////////////////////////////////

template<typename PROPERTY_TYPE>
class CPropertyBase
	: public CProperty
{
	typedef CPropertyBase<PROPERTY_TYPE> TSelf;

public:
	CPropertyBase(const PROPERTY_TYPE* pVal, const wchar_ngp* xml_tag)
		: CProperty(xml_tag)
		, m_pValue( const_cast<PROPERTY_TYPE*>(pVal) )
	{
	}

	bool HasValue() const
	{
		return !!this->m_pValue;
	}

	virtual void CopyFrom(const CProperty& rval) override
	{
		const TSelf& rprop = static_cast<const TSelf&>(rval);

		if ( !HasValue() || !rprop.HasValue() )
			return;

		*this->m_pValue = *rprop.m_pValue;
	}

	virtual bool IsEqualTo(const CProperty& rval) const override
	{
		const TSelf& rprop = static_cast<const TSelf&>(rval);

		if ( !HasValue() || !rprop.HasValue() )
			return true;

		return CompareOperator( *this->m_pValue, *rprop.m_pValue );
	}

	static void Save(const PROPERTY_TYPE* pValue, wptree& prop_node)
	{
        CPropertyBaseSaveLoadToTree<PROPERTY_TYPE>::Save(pValue, prop_node);
	}

	static void Load(PROPERTY_TYPE* pValue, const wptree& prop_node)
	{
        CPropertyBaseSaveLoadToTree<PROPERTY_TYPE>::Load(pValue, prop_node);
	}

	static void Save(const PROPERTY_TYPE* pValue, const ngp_file_io::INGPFileIOPtr& bin_file)
	{
        CPropertyBaseSaveLoadToBinFile<PROPERTY_TYPE>::Save(pValue, bin_file);
	}

	static void Load(PROPERTY_TYPE* pValue, const ngp_file_io::INGPFileIOPtr& bin_file)
	{
        CPropertyBaseSaveLoadToBinFile<PROPERTY_TYPE>::Load(pValue, bin_file);
	}

protected:
	virtual ~CPropertyBase() {}

protected:
	PROPERTY_TYPE* const m_pValue;
};

//////////////////////////////////////////////////////////////////////////

template <typename PROPERTY_TYPE,
		  typename prop_ref_coll::default_ops<PROPERTY_TYPE>::static_operator_equal_t COMPARATOR = prop_ref_coll::default_ops<PROPERTY_TYPE>::operator_equal >
// 		  typename COMPARATOR = prop_ref_coll::default_op_equal<PROPERTY_TYPE>>
class CPropertyCustom
	: public CPropertyBase<PROPERTY_TYPE>
{
	typedef CPropertyCustom<PROPERTY_TYPE, COMPARATOR> TSelf;
public:
	CPropertyCustom( const PROPERTY_TYPE* pVal, const wchar_ngp* xml_tag )
		: CPropertyBase<PROPERTY_TYPE>( pVal, xml_tag )
	{}

	virtual bool IsEqualTo( const CProperty& rval ) const override
	{
		const TSelf& rprop = static_cast<const TSelf&>(rval);

		if ( !HasValue() || !rprop.HasValue() )
			return true;

		return COMPARATOR( *this->m_pValue, *rprop.m_pValue );
// 		return COMPARATOR{}( *m_pValue, *rprop.m_pValue );
	}

	virtual void CopyFrom( const CProperty& rval ) override {}
	virtual wptree& SaveToPTree( wptree& t ) const { return t; }
	virtual const wptree* LoadFromPTree( const wptree& t ) { return &t; }
	virtual void SaveToBinFile( const ngp_file_io::INGPFileIOPtr& ) const {}
	virtual void LoadFromBinFile( const ngp_file_io::INGPFileIOPtr& ) {}
};

//////////////////////////////////////////////////////////////////////////

template<typename PROPERTY_TYPE>
class CPropertySimple
	: public CPropertyBase<PROPERTY_TYPE>
{
	using base = CPropertyBase<PROPERTY_TYPE>;
	std::unique_ptr<PROPERTY_TYPE> m_def_value;
public:
	CPropertySimple(const PROPERTY_TYPE* pVal, const wchar_ngp* xml_tag)
		: base(pVal, xml_tag)
	{
	}

	CPropertySimple(const PROPERTY_TYPE* pVal, const wchar_ngp* xml_tag, PROPERTY_TYPE def_val)
		: base(pVal, xml_tag)
	{
		m_def_value = std::make_unique<PROPERTY_TYPE>( def_val );
	}

	virtual wptree& SaveToPTree(wptree& owner_node) const
	{
		wptree prop_node;
		base::Save(this->m_pValue, prop_node );
		return owner_node.put_child( this->m_xmlTag, prop_node );
	}

	virtual const wptree* LoadFromPTree(const wptree& owner_node) override
	{
		boost::optional<const wptree&> prop_node = owner_node.get_child_optional( this->m_xmlTag );
		if (prop_node)
		{
			base::Load( this->m_pValue, *prop_node );
			return &prop_node.get();
		}
		else {
			if ( m_def_value )
				*this->m_pValue = *m_def_value;
			return nullptr;
		}
	}

	virtual void SaveToBinFile(const ngp_file_io::INGPFileIOPtr& bin_file) const
	{
		base::Save( this->m_pValue, bin_file );
	}

	virtual void LoadFromBinFile(const ngp_file_io::INGPFileIOPtr& bin_file)
	{
		base::Load( this->m_pValue, bin_file );
	}
};

//////////////////////////////////////////////////////////////////////////

template<typename PROPERTY_TYPE>
class CPropertyEnum
	: public CPropertyBase<PROPERTY_TYPE>
{
	 using base = CPropertyBase<PROPERTY_TYPE>;
public:
	CPropertyEnum(const PROPERTY_TYPE* pVal, const wchar_ngp* xml_tag)
		: base(pVal, xml_tag)
	{
	}

	virtual wptree& SaveToPTree(wptree& owner_node) const
	{
		return owner_node.put( this->m_xmlTag, long(*this->m_pValue) );
	}

	virtual const wptree* LoadFromPTree(const wptree& owner_node) override
	{
		boost::optional<const wptree&> prop_node = owner_node.get_child_optional( this->m_xmlTag );
		if (prop_node)
		{
			if (boost::optional<long> val = prop_node->get_value_optional<long>())
				*this->m_pValue = (PROPERTY_TYPE) *val;

			return &prop_node.get();
		}
		else
			return nullptr;
	}

	virtual void SaveToBinFile(const ngp_file_io::INGPFileIOPtr& bin_file) const
	{
		bin_file->long_write( *this->m_pValue );
	}

	virtual void LoadFromBinFile(const ngp_file_io::INGPFileIOPtr& bin_file)
	{
		long l;
		bin_file->long_read( &l, 1 );
		*this->m_pValue = (PROPERTY_TYPE) l;
	}
};

//////////////////////////////////////////////////////////////////////////

template<typename PROPERTY_TYPE>
class CPropertySimpleArr
	: public CPropertyBase<PROPERTY_TYPE>
{
	using base = CPropertyBase<PROPERTY_TYPE>;
	typedef CPropertySimpleArr<PROPERTY_TYPE> TSelf;

public:
	CPropertySimpleArr(const PROPERTY_TYPE* pVal, int size, const wchar_ngp* xml_tag)
		: base(pVal, xml_tag)
		, m_Size(size)
	{
	}

	virtual void CopyFrom(const CProperty& rval)
	{
		const TSelf& rprop = static_cast<const TSelf&>(rval);

		if ( !HasValue() || !rprop.HasValue() )
			return;

		for (int i=0; i<m_Size; ++i)
			this->m_pValue[i] = rprop.m_pValue[i];
	}

	virtual bool IsEqualTo(const CProperty& rval) const
	{
		const TSelf& rprop = static_cast<const TSelf&>(rval);

		if ( !HasValue() || !rprop.HasValue() )
			return true;

		for (int i=0; i<m_Size; ++i)
		{
			if (!CompareOperator(this->m_pValue[i], rprop.m_pValue[i]))
				return false;
		}
		return true;
	}

	virtual wptree& SaveToPTree(wptree& owner_node) const
	{
		wptree& array_node = owner_node.put( this->m_xmlTag, L"" );
		for (int i=0; i<m_Size; ++i)
			array_node.add( this->ItemXMLTag, this->m_pValue[i] );

		//wptree item;
		//for (int i=0; i<m_Size; ++i)
		//{
		//	item.put_value( this->m_pValue[i] );
		//	array_node.push_back( std::make_pair( std::wstring(), item ) );
		//}

		return array_node;
	}

	virtual const wptree* LoadFromPTree(const wptree& owner_node) override
	{
		boost::optional<const wptree&> prop_node = owner_node.get_child_optional( this->m_xmlTag );
		if (prop_node)
		{
			int i = 0;
			for ( const wptree::value_type& item_node : *prop_node )
			{
				_ASSERTE( item_node.first == this->ItemXMLTag );
				if (boost::optional<PROPERTY_TYPE> val = item_node.second.get_value_optional<PROPERTY_TYPE>())
					this->m_pValue[i++] = *val;
			}
			return &prop_node.get();
		}
		return nullptr;
	}

	virtual void SaveToBinFile(const ngp_file_io::INGPFileIOPtr& bin_file) const
	{
		bin_file->long_write( m_Size );
		bin_file->write( this->m_pValue, m_Size );
	}

	virtual void LoadFromBinFile(const ngp_file_io::INGPFileIOPtr& bin_file)
	{
		long size = 0;
		bin_file->long_read( &size, 1 );
		bin_file->read( this->m_pValue, size );
	}

private:
	const int m_Size;
};

//////////////////////////////////////////////////////////////////////////

template<typename ENUM_TYPE>
class CPropertySetOfEnum
	: public CProperty
{
	typedef CPropertySetOfEnum<ENUM_TYPE> TSelf;
	typedef std::set<ENUM_TYPE> TSet;

public:
	CPropertySetOfEnum(const std::set<ENUM_TYPE>* pSet, const wchar_ngp* xml_tag)
		: CProperty(xml_tag)
		, m_pSet(pSet)
	{
	}

	virtual void CopyFrom(const CProperty& rval)
	{
		const TSelf& rprop = static_cast<const TSelf&>(rval);

		const_cast<TSet&>(*m_pSet) = *rprop.m_pSet;
	}

	virtual bool IsEqualTo(const CProperty& rval) const
	{
		const TSelf& rprop = static_cast<const TSelf&>(rval);

		if ( m_pSet->size() != rprop.m_pSet->size() )
			return false;

		if ( m_pSet->empty() )
			return true;

		return std::equal( m_pSet->begin(), m_pSet->end(), rprop.m_pSet->begin() );
	}

	virtual wptree& SaveToPTree(wptree& owner_node) const
	{
		wptree& array_node = owner_node.put( this->m_xmlTag, L"" );
		for ( const ENUM_TYPE t : *m_pSet )
			array_node.add( this->ItemXMLTag, long(t) );

		return array_node;
	}

	virtual const wptree* LoadFromPTree(const wptree& owner_node) override
	{
		boost::optional<const wptree&> prop_node = owner_node.get_child_optional( this->m_xmlTag );
		if (prop_node)
		{
			const_cast<TSet*>(m_pSet)->clear();

			for ( const wptree::value_type& item_node : *prop_node )
			{
				_ASSERTE( item_node.first == this->ItemXMLTag );
				if (boost::optional<long> val = item_node.second.get_value_optional<long>())
					const_cast<TSet*>(m_pSet)->insert( (ENUM_TYPE) *val );
			}
			return &prop_node.get();
		}
		return nullptr;
	}

	virtual void SaveToBinFile(const ngp_file_io::INGPFileIOPtr& bin_file) const
	{
		bin_file->size_write(m_pSet->size());
		for ( const ENUM_TYPE t : *m_pSet )
			bin_file->long_write( long(t) );
	}

	virtual void LoadFromBinFile(const ngp_file_io::INGPFileIOPtr& bin_file)
	{
		const_cast<TSet*>(m_pSet)->clear();

		boost::uint64_t size = 0;
		bin_file->size_read(size);
		for (size_t i=0; i<size; ++i)
		{
			long value;
			bin_file->long_read( &value, 1 );
			const_cast<TSet*>(m_pSet)->insert( (ENUM_TYPE) value );
		}
	}

private:
	const TSet* const m_pSet;
};

//////////////////////////////////////////////////////////////////////////

template<typename CONTAINER>
class CPropertySetOfSimple
	: public CProperty
{
	typedef CPropertySetOfSimple<CONTAINER> TSelf;
	typedef typename CONTAINER::value_type ITEM_TYPE;

public:
	CPropertySetOfSimple(const CONTAINER* pSet, const wchar_ngp* xml_tag)
		: CProperty(xml_tag)
		, m_pSet(pSet)
	{
	}

	virtual void CopyFrom(const CProperty& rval)
	{
		const TSelf& rprop = static_cast<const TSelf&>(rval);
		const_cast<CONTAINER&>(*m_pSet) = *rprop.m_pSet;
	}

	virtual bool IsEqualTo(const CProperty& rval) const
	{
		const TSelf& rprop = static_cast<const TSelf&>(rval);

		if (m_pSet->size() != rprop.m_pSet->size())
			return false;

		if (m_pSet->empty())
			return true;

		return std::equal(m_pSet->begin(), m_pSet->end(), rprop.m_pSet->begin());
	}

	virtual wptree& SaveToPTree(wptree& owner_node) const
	{
		wptree& array_node = owner_node.put(m_xmlTag, L"");
		for (const ITEM_TYPE i : *m_pSet) {
			wptree prop_node;
			CPropertyBase<ITEM_TYPE>::Save(&i, prop_node);
			array_node.add_child(ItemXMLTag, prop_node);
		}

		return array_node;
	}

	virtual const wptree* LoadFromPTree(const wptree& owner_node) override
	{
		boost::optional<const wptree&> prop_node = owner_node.get_child_optional(m_xmlTag);
		if (prop_node)
		{
			const_cast<CONTAINER*>(m_pSet)->clear();
			for (const wptree::value_type& item_node : *prop_node)
			{
				ITEM_TYPE item;
				CPropertyBase<ITEM_TYPE>::Load(&item, item_node.second);
				const_cast<CONTAINER*>(m_pSet)->insert(item);
			}
			return &prop_node.get();
		}
		return nullptr;
	}

	virtual void SaveToBinFile(const ngp_file_io::INGPFileIOPtr& bin_file) const
	{
		bin_file->size_write(m_pSet->size());
		for (const ITEM_TYPE i : *m_pSet)
			CPropertyBase<ITEM_TYPE>::Save(&i, bin_file);
	}

	virtual void LoadFromBinFile(const ngp_file_io::INGPFileIOPtr& bin_file)
	{
		const_cast<CONTAINER*>(m_pSet)->clear();

		boost::uint64_t size = 0;
		bin_file->size_read(size);
		for (size_t i = 0; i < size; ++i)
		{
			ITEM_TYPE value;
			CPropertyBase<ITEM_TYPE>::Load(&value, bin_file);
			const_cast<CONTAINER*>(m_pSet)->insert(value);
		}
	}

private:
	const CONTAINER* const m_pSet;
};

//////////////////////////////////////////////////////////////////////////

template<typename CONTAINER>
class CPropertyContainerOfEnum
	: public CProperty
{
	typedef CPropertyContainerOfEnum<CONTAINER> TSelf;

public:
	CPropertyContainerOfEnum(const CONTAINER* container, const wchar_ngp* xml_tag):
		CProperty(xml_tag),
		m_container(container)
	{
	}

	virtual void CopyFrom(const CProperty& rval)
	{
		const TSelf& rprop = static_cast<const TSelf&>(rval);

		const_cast<CONTAINER&>(*m_container) = *rprop.m_container;
	}

	virtual bool IsEqualTo(const CProperty& rval) const
	{
		const TSelf& rprop = static_cast<const TSelf&>(rval);

		if ( m_container->size() != rprop.m_container->size() )
			return false;

		if ( m_container->empty() )
			return true;

		return std::equal( m_container->begin(), m_container->end(), rprop.m_container->begin(), CompareOperator<long> );
	}

	virtual wptree& SaveToPTree(wptree& owner_node) const
	{
		wptree& array_node = owner_node.put( this->m_xmlTag, L"" );

		for (typename CONTAINER::const_iterator i = m_container->begin(); i != m_container->end(); ++i) {
			wptree prop_node;
			CPropertyBase<long>::Save( &((long&)*i), prop_node );
			array_node.add_child( this->ItemXMLTag, prop_node );
		}
		return array_node;
	}

	virtual const wptree* LoadFromPTree(const wptree& owner_node) override
	{
		boost::optional<const wptree&> prop_node = owner_node.get_child_optional( this->m_xmlTag );
		if (prop_node)
		{
			const_cast<CONTAINER*>(m_container)->clear();

			for ( const wptree::value_type& item_node : *prop_node )
			{
				_ASSERTE( item_node.first == this->ItemXMLTag );
				//if ( attr.first == this->ItemXMLTag )
				long item;
				CPropertyBase<long>::Load( &item, item_node.second );
				const_cast<CONTAINER*>(m_container)->push_back( (typename CONTAINER::value_type) item );
			}
			return &prop_node.get();
		}
		return nullptr;
	}

	virtual void SaveToBinFile(const ngp_file_io::INGPFileIOPtr& bin_file) const
	{
		bin_file->size_write(m_container->size());
		for (typename CONTAINER::const_iterator i = m_container->begin(); i != m_container->end(); ++i) {
			CPropertyBase<long>::Save( &((long&)*i), bin_file );
		}
	}

	virtual void LoadFromBinFile(const ngp_file_io::INGPFileIOPtr& bin_file)
	{
		boost::uint64_t size = 0;
		bin_file->size_read(size);

		const_cast<CONTAINER*>(m_container)->resize( size );

		for (typename CONTAINER::const_iterator i = m_container->begin(); i != m_container->end(); ++i) {
			CPropertyBase<long>::Load( const_cast<long*>(&((long&)*i)), bin_file );
		}
	}

private:
	const CONTAINER* const m_container;
};

//////////////////////////////////////////////////////////////////////////

template<typename CONTAINER>
class CPropertyContainerOf
	: public CProperty
{
	typedef CPropertyContainerOf<CONTAINER> TSelf;
	typedef typename CONTAINER::value_type ITEM_TYPE;

public:
	CPropertyContainerOf(const CONTAINER* container, const wchar_ngp* xml_tag,
		const boost::function<void(const ITEM_TYPE&, const wchar_ngp*, CPropertyTreeNode&)>& itemDecompositor)
		: CProperty(xml_tag)
		, m_pContainer(container)
		, m_ItemDecompositor(itemDecompositor)
	{
	}

	virtual void CopyFrom(const CProperty& rval)
	{
		const TSelf& rprop = static_cast<const TSelf&>(rval);

		const_cast<CONTAINER&>(*m_pContainer) = *rprop.m_pContainer;
	}

	virtual bool IsEqualTo(const CProperty& rval) const
	{
		const TSelf& rprop = static_cast<const TSelf&>(rval);

		if ( m_pContainer->size() != rprop.m_pContainer->size() )
			return false;

		if ( m_pContainer->empty() )
			return true;

		return std::equal( m_pContainer->begin(), m_pContainer->end(), rprop.m_pContainer->begin(), CompareOperator<ITEM_TYPE> );
	}

	virtual wptree& SaveToPTree(wptree& owner_node) const
	{
		CPropertyRefCollection coll( this->m_xmlTag.c_str(), PF_Serializable );
		for ( const ITEM_TYPE& item : *m_pContainer )
		{
			m_ItemDecompositor(item, this->ItemXMLTag, coll);
		}

		return coll.CPropertyTreeNode::SaveToPTree( owner_node );
	}

	virtual const wptree* LoadFromPTree(const wptree& owner_node) override
	{
		boost::optional<const wptree&> groupNode = owner_node.get_child_optional(this->m_xmlTag);
		if (groupNode)
		{
			const_cast<CONTAINER*>(m_pContainer)->clear();
			const_cast<CONTAINER*>(m_pContainer)->resize( groupNode->size() );

			CPropertyRefCollection coll( this->m_xmlTag.c_str(), PF_Serializable );
			for ( const ITEM_TYPE& item : *m_pContainer ) {
				m_ItemDecompositor(item, this->ItemXMLTag, coll);
			}
			return coll.CPropertyTreeNode::LoadFromPTree( owner_node );
		}
		return nullptr;
	}

	virtual void SaveToBinFile(const ngp_file_io::INGPFileIOPtr& bin_file) const
	{
		CPropertyRefCollection coll( this->m_xmlTag.c_str(), PF_Serializable );
		for ( const ITEM_TYPE& item : *m_pContainer )
		{
			m_ItemDecompositor(item, this->ItemXMLTag, coll);
		}

		bin_file->size_write(m_pContainer->size());
		coll.CPropertyTreeNode::SaveToBinFile( bin_file );
	}

	virtual void LoadFromBinFile(const ngp_file_io::INGPFileIOPtr& bin_file)
	{
		boost::uint64_t nItems = 0;
		bin_file->size_read(nItems);

		const_cast<CONTAINER*>(m_pContainer)->resize( nItems );

		CPropertyRefCollection coll( this->m_xmlTag.c_str(), PF_Serializable );
		for ( const ITEM_TYPE& item : *m_pContainer )
		{
			m_ItemDecompositor(item, this->ItemXMLTag, coll);
		}

		return coll.CPropertyTreeNode::LoadFromBinFile( bin_file );
	}

private:
	const CONTAINER* const m_pContainer;
	const boost::function<void(const ITEM_TYPE&, const wchar_ngp*, CPropertyTreeNode&)> m_ItemDecompositor;
};

//////////////////////////////////////////////////////////////////////////

template<typename CONTAINER>
class CPropertyContainerOfSimple
	: public CProperty
{
	typedef CPropertyContainerOfSimple<CONTAINER> TSelf;
	typedef typename CONTAINER::value_type ITEM_TYPE;

public:
	CPropertyContainerOfSimple(const CONTAINER* container, const wchar_ngp* xml_tag):
	  CProperty(xml_tag),
		  m_container(container)
	  {
	  }

	  virtual void CopyFrom(const CProperty& rval)
	  {
		  const TSelf& rprop = static_cast<const TSelf&>(rval);

		  const_cast<CONTAINER&>(*m_container) = *rprop.m_container;
	  }

	  virtual bool IsEqualTo(const CProperty& rval) const
	  {
		  const TSelf& rprop = static_cast<const TSelf&>(rval);

		  if ( m_container->size() != rprop.m_container->size() )
			  return false;

		  if ( m_container->empty() )
			  return true;

		  return std::equal( m_container->begin(), m_container->end(), rprop.m_container->begin(), CompareOperator<ITEM_TYPE> );
	  }

	  virtual wptree& SaveToPTree(wptree& owner_node) const
	  {
		  wptree& array_node = owner_node.put( this->m_xmlTag, L"" );

		  for (typename CONTAINER::const_iterator i = m_container->begin(); i != m_container->end(); ++i) {
			  wptree prop_node;
			  CPropertyBase<ITEM_TYPE>::Save( &(*i), prop_node );
			  array_node.add_child( this->ItemXMLTag, prop_node );
		  }
		  return array_node;
	  }

	  virtual const wptree* LoadFromPTree(const wptree& owner_node) override
	  {
		  boost::optional<const wptree&> prop_node = owner_node.get_child_optional( this->m_xmlTag );
		  if (prop_node)
		  {
			  const_cast<CONTAINER*>(m_container)->clear();

			  for ( const wptree::value_type& item_node : *prop_node )
			  {
				  _ASSERTE( item_node.first == this->ItemXMLTag );
				  //if ( attr.first == this->ItemXMLTag )
				  ITEM_TYPE item;
				  CPropertyBase<ITEM_TYPE>::Load( &item, item_node.second );
				  const_cast<CONTAINER*>(m_container)->push_back( item );
			  }
			  return &prop_node.get();
		  }
		  return nullptr;
	  }

	virtual void SaveToBinFile(const ngp_file_io::INGPFileIOPtr& bin_file) const
	{
		bin_file->size_write(m_container->size());
		for (typename CONTAINER::const_iterator i = m_container->begin(); i != m_container->end(); ++i) {
			CPropertyBase<ITEM_TYPE>::Save( &(*i), bin_file );
		}
	}

	virtual void LoadFromBinFile(const ngp_file_io::INGPFileIOPtr& bin_file)
	{
		boost::uint64_t size = 0;
		bin_file->size_read(size);

		const_cast<CONTAINER*>(m_container)->resize( size );

		for (typename CONTAINER::const_iterator i = m_container->begin(); i != m_container->end(); ++i) {
			CPropertyBase<ITEM_TYPE>::Load( const_cast<ITEM_TYPE*>(&(*i)), bin_file );
		}
	}

private:
	const CONTAINER* const m_container;
};

//////////////////////////////////////////////////////////////////////////

template<typename CONTAINER, typename COPY_OPERATOR, typename DECOMPOSITOR>
class CPropertyContainerOfPtrs:
	public CProperty
{
	typedef CPropertyContainerOfPtrs<CONTAINER, COPY_OPERATOR, DECOMPOSITOR> SELF;
	typedef typename CONTAINER::value_type::element_type ITEM_TYPE;

public:
	CPropertyContainerOfPtrs(const CONTAINER* container, const wchar_ngp* xml_tag):
	  CProperty(xml_tag),
	  m_container(container)
	{
	}

	virtual void CopyFrom(const CProperty& r)
	{
		const SELF& r_prop = static_cast<const SELF&>(r);
		
		const_cast<CONTAINER&>(*m_container).clear();
		for (typename CONTAINER::const_iterator r_it = r_prop.m_container->begin(); r_it != r_prop.m_container->end(); ++r_it)
		{
			boost::shared_ptr<ITEM_TYPE> obj = ITEM_TYPE::Instance();
			m_copy_operator( obj, (*r_it) );
			const_cast<CONTAINER&>(*m_container).push_back(obj);
		}
	}

	virtual bool IsEqualTo(const CProperty& r) const
	{
		const SELF& r_prop = static_cast<const SELF&>(r);
		if (r_prop.m_container->size() != m_container->size())
			return false;

		if (m_container->empty())
			return true;

		return std::equal(m_container->begin(), m_container->end(), r_prop.m_container->begin(),
			[](const typename CONTAINER::value_type& a, const typename CONTAINER::value_type& b) { return *a == *b; } );
	}

	virtual wptree& SaveToPTree(wptree& owner_node) const
	{
		CPropertyRefCollection coll(this->m_xmlTag.c_str(), PF_Serializable);
		for (typename CONTAINER::const_iterator i = m_container->begin(); i != m_container->end(); ++i) {
			m_decompositor(*(*i), this->ItemXMLTag, coll);
		}
		return coll.CPropertyTreeNode::SaveToPTree(owner_node);
	}

	virtual const wptree* LoadFromPTree(const wptree& owner_node) override
	{
		boost::optional<const wptree&> group_node = owner_node.get_child_optional(this->m_xmlTag);
		if (group_node)
		{
			const_cast<CONTAINER&>(*m_container).clear();

			CPropertyRefCollection coll(this->m_xmlTag.c_str(), PF_Serializable);
			for (size_t i = 0, isz = group_node->size(); i < isz ; ++i)
			{
				boost::shared_ptr<ITEM_TYPE> obj = ITEM_TYPE::Instance();
				m_decompositor(*obj, this->ItemXMLTag, coll);
				const_cast<CONTAINER&>(*m_container).push_back(obj);
			}
			return coll.CPropertyTreeNode::LoadFromPTree(owner_node);
		}
		return nullptr;
	}

	virtual void SaveToBinFile(const ngp_file_io::INGPFileIOPtr& bin_file) const
	{
		CPropertyRefCollection coll(this->m_xmlTag.c_str(), PF_Serializable);
		for (typename CONTAINER::const_iterator i = m_container->begin(); i != m_container->end(); ++i) {
			m_decompositor(*(*i), this->ItemXMLTag, coll);
		}
		bin_file->size_write(m_container->size());
		coll.CPropertyTreeNode::SaveToBinFile(bin_file);
	}

	virtual void LoadFromBinFile(const ngp_file_io::INGPFileIOPtr& bin_file)
	{
		const_cast<CONTAINER&>(*m_container).clear();

		boost::uint64_t size = 0;
		bin_file->size_read(size);

		CPropertyRefCollection coll(this->m_xmlTag.c_str(), PF_Serializable);
		for (size_t i = 0; i < size; ++i)
		{
			boost::shared_ptr<ITEM_TYPE> obj = ITEM_TYPE::Instance();
			m_decompositor(*obj, this->ItemXMLTag, coll);
			const_cast<CONTAINER&>(*m_container).push_back(obj);
		}
		return coll.CPropertyTreeNode::LoadFromBinFile( bin_file );
	}

private:
	const CONTAINER* const m_container;
	const DECOMPOSITOR m_decompositor;
	const COPY_OPERATOR m_copy_operator;
};

//////////////////////////////////////////////////////////////////////////

template<typename MAP>
class CPropertyMapOfStruct
	: public CProperty
{
	typedef CPropertyMapOfStruct<MAP> SELF;
	typedef typename MAP::key_type KEY_TYPE;
	typedef typename MAP::mapped_type VALUE_TYPE;
	typedef void (*VALUE_DECOMPOSITOR)(const VALUE_TYPE&, const wchar_ngp*, class CPropertyTreeNode&);
	typedef std::wstring (*STR_KEY_GENERATOR)(const KEY_TYPE&);
	typedef KEY_TYPE (*STR_KEY_DEGENERATOR)(const std::wstring&);

public:
	CPropertyMapOfStruct(const MAP* map, const wchar_ngp* xml_tag, const VALUE_DECOMPOSITOR& decompositor, const STR_KEY_GENERATOR& key_generator, const STR_KEY_DEGENERATOR& key_degenerator)
		:CProperty(xml_tag)
		,m_map(map)
		,m_fn_decompositor(decompositor)
		,m_fn_key_generator(key_generator)
		,m_fn_key_degenerator(key_degenerator)
	{
	}

	virtual void CopyFrom(const CProperty& r)
	{
		const SELF& r_prop = static_cast<const SELF&>(r);

		const_cast<MAP&>(*m_map) = *r_prop.m_map;
	}

	virtual bool IsEqualTo(const CProperty& r) const
	{
		const SELF& r_prop = static_cast<const SELF&>(r);
		if ( r_prop.m_map->size() != m_map->size() )
			return false;
		if ( m_map->empty() )
			return true;
		return std::equal( m_map->begin(), m_map->end(), r_prop.m_map->begin(),
			[](const typename MAP::value_type& a, const typename MAP::value_type& b) { return a.second == b.second; } );
	}

	virtual wptree& SaveToPTree(wptree& owner_node) const
	{
		wptree map_node;
		for ( const typename MAP::value_type& p : *m_map )
		{
			CPropertyRefCollection coll( m_fn_key_generator(p.first).c_str(), PF_Serializable );	//pair grouping node is here, thus
			m_fn_decompositor( p.second, nullptr, coll );	//decompositor should not create grouping node
			coll.SaveToPTree( map_node );
		}
		return owner_node.put_child( this->m_xmlTag, map_node );
	}

	virtual const wptree* LoadFromPTree(const wptree& owner_node) override
	{
		boost::optional<const wptree&> map_node = owner_node.get_child_optional(this->m_xmlTag);
		if (map_node)
		{
			const_cast<MAP&>(*m_map).clear();

			for ( const wptree::value_type& item_node : *map_node )
			{
				VALUE_TYPE value;

				CPropertyRefCollection coll( item_node.first.c_str(), PF_Serializable );
				m_fn_decompositor( value, nullptr, coll );
				coll.LoadFromPTree( item_node.second, false );

				const_cast<MAP&>(*m_map).emplace( m_fn_key_degenerator(item_node.first), std::move(value) );
			}

			return &map_node.get();
		}
		return nullptr;
	}

	virtual void SaveToBinFile(const ngp_file_io::INGPFileIOPtr& bin_file) const
	{
		bin_file->size_write( m_map->size() );

		for ( const typename MAP::value_type& p : *m_map )
		{
			CPropertyBase<typename MAP::key_type>::Save( &p.first, bin_file );

			CPropertyRefCollection coll( m_fn_key_generator(p.first).c_str(), PF_Serializable );	//pair grouping node is here, thus
			m_fn_decompositor( p.second, nullptr, coll );	//decompositor should not create grouping node
			coll.SaveToBinFile( bin_file );
		}
	}

	virtual void LoadFromBinFile(const ngp_file_io::INGPFileIOPtr& bin_file)
	{
		const_cast<MAP&>(*m_map).clear();

		boost::uint64_t size = 0;
		bin_file->size_read(size);

		for (size_t i = 0; i < size; ++i)
		{
			typename MAP::value_type item_pair;

			CPropertyBase<typename MAP::key_type>::Load( const_cast<typename MAP::key_type*>(&item_pair.first), bin_file );

			CPropertyRefCollection coll( m_fn_key_generator(item_pair.first).c_str(), PF_Serializable );
			m_fn_decompositor( item_pair.second, nullptr, coll );
			coll.LoadFromBinFile( bin_file );

			const_cast<MAP&>(*m_map).emplace( std::move(item_pair) );
		}
	}

private:
	const MAP* const m_map;
	const VALUE_DECOMPOSITOR m_fn_decompositor;
	const STR_KEY_GENERATOR m_fn_key_generator;
	const STR_KEY_DEGENERATOR m_fn_key_degenerator;
};

//////////////////////////////////////////////////////////////////////////

template<typename MAP, typename COPY_OPERATOR>
class CPropertyMapOfPtrs:
	public CProperty
{
	typedef CPropertyMapOfPtrs<MAP, COPY_OPERATOR> SELF;
	typedef typename MAP::mapped_type::element_type ITEM_TYPE;
	typedef typename MAP::mapped_type ITEM_SMART_PTR;
	typedef typename MAP::key_type KEY_TYPE;
	typedef boost::function<void(const ITEM_TYPE&, const wchar_ngp*, class CPropertyTreeNode&)> ITEM_DECOMPOSITOR;
	typedef boost::function<const KEY_TYPE& (const ITEM_TYPE&)> ITEM_KEYGETTER;
	typedef boost::function<ITEM_SMART_PTR()> INSTANCE; 

public:
	CPropertyMapOfPtrs(const MAP* map, const wchar_ngp* xml_tag, const ITEM_DECOMPOSITOR& decomp, const ITEM_KEYGETTER& key_getter, const INSTANCE& inst):
		CProperty(xml_tag),
		m_map(map),
		m_fn_decompositor(decomp),
		m_fn_get_key(key_getter),
		m_fn_instance(inst)
	{
	}

	virtual void CopyFrom(const CProperty& r)
	{
		const SELF& r_prop = static_cast<const SELF&>(r);
		
		const_cast<MAP&>(*m_map).clear();
		for (typename MAP::const_iterator r_it = r_prop.m_map->begin(); r_it != r_prop.m_map->end(); ++r_it)
		{
			ITEM_SMART_PTR obj = m_fn_instance();
			m_copy_operator( obj, r_it->second );
			const_cast<MAP&>(*m_map).insert( typename MAP::value_type( m_fn_get_key(*obj), std::move(obj) ) );
		}
	}

	virtual bool IsEqualTo(const CProperty& r) const
	{
		const SELF& r_prop = static_cast<const SELF&>(r);
		if (r_prop.m_map->size() != m_map->size())
			return false;

		if (m_map->empty())
			return true;

		return std::equal(m_map->begin(), m_map->end(), r_prop.m_map->begin(),
			[](const typename MAP::value_type& a, const typename MAP::value_type& b) { return *a.second == *b.second; } );
	}

	virtual wptree& SaveToPTree(wptree& owner_node) const
	{
		CPropertyRefCollection coll(this->m_xmlTag.c_str(), PF_Serializable);
		for (typename MAP::const_iterator i = m_map->begin(); i != m_map->end(); ++i) {
			m_fn_decompositor(*(i->second), this->ItemXMLTag, coll);
		}
		return coll.CPropertyTreeNode::SaveToPTree(owner_node);
	}

	virtual const wptree* LoadFromPTree(const wptree& owner_node) override
	{
		boost::optional<const wptree&> group_node = owner_node.get_child_optional(this->m_xmlTag);
		if (group_node)
		{
			const_cast<MAP&>(*m_map).clear();

			std::list<ITEM_SMART_PTR> t_list;
			CPropertyRefCollection coll(this->m_xmlTag.c_str(), PF_Serializable);
			for (size_t i = 0, isz = group_node->size(); i < isz; ++i) {
				ITEM_SMART_PTR obj = m_fn_instance();
				m_fn_decompositor(*obj, this->ItemXMLTag, coll);
				t_list.push_back( std::move(obj) );
			}

			//restore map links
			const wptree* ptree = coll.CPropertyTreeNode::LoadFromPTree(owner_node);
			if (ptree) {
				for ( auto& val : t_list )
					const_cast<MAP&>(*m_map).insert( typename MAP::value_type( m_fn_get_key(*val), std::move(val) ) );
			}
			return ptree;
		}
		return nullptr;
	}

	virtual void SaveToBinFile(const ngp_file_io::INGPFileIOPtr& bin_file) const
	{
		CPropertyRefCollection coll(this->m_xmlTag.c_str(), PF_Serializable);
		for (typename MAP::const_iterator i = m_map->begin(); i != m_map->end(); ++i) {
			m_fn_decompositor(*(i->second), this->ItemXMLTag, coll);
		}
		bin_file->size_write(m_map->size());
		coll.CPropertyTreeNode::SaveToBinFile(bin_file);
	}

	virtual void LoadFromBinFile(const ngp_file_io::INGPFileIOPtr& bin_file)
	{
		const_cast<MAP&>(*m_map).clear();

		boost::uint64_t size = 0;
		bin_file->size_read(size);

		std::list<ITEM_SMART_PTR> t_list;
		CPropertyRefCollection coll(this->m_xmlTag.c_str(), PF_Serializable);
		for (size_t i = 0; i < size; ++i)
		{
			ITEM_SMART_PTR obj = m_fn_instance();
			m_fn_decompositor(*obj, this->ItemXMLTag, coll);
			t_list.push_back( std::move(obj) );
		}
		
		coll.CPropertyTreeNode::LoadFromBinFile( bin_file );

		//restore map links
		for ( auto& val : t_list )
			const_cast<MAP&>(*m_map).insert( typename MAP::value_type( m_fn_get_key(*val), std::move(val) ) );
	}

private:
	const MAP* const m_map;
	const ITEM_DECOMPOSITOR m_fn_decompositor;
	const ITEM_KEYGETTER m_fn_get_key;
	const INSTANCE m_fn_instance;
	const COPY_OPERATOR m_copy_operator;
};

//////////////////////////////////////////////////////////////////////////

class CPropertyGroup
	: public CProperty
{
public:
	CPropertyGroup(const wchar_ngp* xml_tag)
		: CProperty(xml_tag)
	{
	}

	virtual void CopyFrom(const CProperty&) {}
	virtual bool IsEqualTo(const CProperty&) const { return true; }

	virtual wptree& SaveToPTree(wptree& owner_node) const
	{
		return owner_node.add( this->m_xmlTag, L"" );		//group
	}

	virtual const wptree* LoadFromPTree(const wptree& owner_node) override
	{
		boost::optional<const wptree&> node = owner_node.get_child_optional( this->m_xmlTag );
		if (node)
			return &node.get();
		else
			return nullptr;
	}

	virtual void SaveToBinFile(const ngp_file_io::INGPFileIOPtr& bin_file) const
	{
		//do nothing
	}

	virtual void LoadFromBinFile(const ngp_file_io::INGPFileIOPtr& bin_file)
	{
		//do nothing
	}
};

//////////////////////////////////////////////////////////////////////////

template<class CONTAINER>
class CPropertyContainerOfBinary
	: public CProperty
{
public:
	typedef typename CONTAINER::value_type ITEM_TYPE;
	typedef CPropertyContainerOfBinary<CONTAINER> TSelf;

	CPropertyContainerOfBinary(const CONTAINER* container, const wchar_ngp* xml_tag)
		: CProperty(xml_tag)
		, m_container(container)
	{
	}

	virtual void CopyFrom(const CProperty& rval) override
	{
		const TSelf& rprop = static_cast<const TSelf&>(rval);

		const_cast<CONTAINER&>(*m_container) = *rprop.m_container;
	}

	virtual bool IsEqualTo(const CProperty& rval) const override
	{ 
		const TSelf& rprop = static_cast<const TSelf&>(rval);

		if (m_container->size() != rprop.m_container->size())
			return false;

		return std::equal(m_container->cbegin(), m_container->cend(), rprop.m_container->cbegin(), CompareOperator<ITEM_TYPE>);
	}

	virtual wptree& SaveToPTree(wptree& owner_node) const override
	{
		std::wstring serialisedContainer;
		SerializeContainerToStr(*const_cast<CONTAINER*>(m_container), serialisedContainer);
		wptree prop_node;
		prop_node.put_value(serialisedContainer);
		return owner_node.put_child(this->m_xmlTag, prop_node);
	}

	virtual const wptree* LoadFromPTree(const wptree& owner_node) override
	{
		boost::optional<const wptree&> prop_node = owner_node.get_child_optional(this->m_xmlTag);
		if (!prop_node)
			return nullptr;

		const boost::optional<std::wstring> serialisedData = prop_node->get_value_optional<std::wstring>();
		if (!serialisedData)
			return nullptr;

		DeserializeContainerFromStr(*serialisedData, *const_cast<CONTAINER*>(m_container));

		return &prop_node.get();
	}

	virtual void SaveToBinFile(const ngp_file_io::INGPFileIOPtr& bin_file) const override
	{
		SaveToBinFileImpl(bin_file, boost::is_arithmetic<ITEM_TYPE>());
	}

	virtual void LoadFromBinFile(const ngp_file_io::INGPFileIOPtr& bin_file) override
	{
		LoadFromBinFileImpl(bin_file, boost::is_arithmetic<ITEM_TYPE>());
	}

private:
	template <typename C = CONTAINER, typename T = ITEM_TYPE>
	static std::enable_if_t<boost::is_same<C, ngp_shared_array<T> >::value, T*>
		dataPtr(C &container)
	{
		return container.c_array();
	}

	template <typename C = CONTAINER, typename T = ITEM_TYPE>
	static std::enable_if_t<!boost::is_same<C, ngp_shared_array<T> >::value, T*>
		dataPtr(C &container)
	{
		return container.data();
	}

	void SaveToBinFileImpl(const ngp_file_io::INGPFileIOPtr& bin_file, const boost::true_type&) const //if ITEM_TYPE is arithmetic
	{
		bin_file->size_write(m_container->size());
		bin_file->write(m_container->data(), m_container->size());
	}

	void SaveToBinFileImpl(const ngp_file_io::INGPFileIOPtr& bin_file, const boost::false_type&) const
	{
		std::string serializedData;
		SerializeContainerToStr(*const_cast<CONTAINER*>(m_container), serializedData);
		bin_file->size_write(serializedData.size());
		bin_file->chr_write(serializedData.data(), serializedData.size());
	}

	void LoadFromBinFileImpl(const ngp_file_io::INGPFileIOPtr& bin_file, const boost::true_type&) //if ITEM_TYPE is arithmetic
	{
		boost::uint64_t containerSize = 0;
		bin_file->size_read(containerSize);
		const_cast<CONTAINER*>(m_container)->resize(containerSize);
		bin_file->read(dataPtr(*const_cast<CONTAINER*>(m_container)), (int)m_container->size());
	}

	void LoadFromBinFileImpl(const ngp_file_io::INGPFileIOPtr& bin_file, const boost::false_type&)
	{
		boost::uint64_t dataSize = 0;
		bin_file->size_read(dataSize);
		std::vector<char> serializedData(dataSize);
		bin_file->chr_read(serializedData.data(), dataSize);
		const_cast<CONTAINER*>(m_container)->clear();
		DeserializeContainerFromStr(serializedData, *const_cast<CONTAINER*>(m_container));
	}

	void DeserializeContainerFromStr(const std::wstring &str, CONTAINER &container) const
	{
		std::vector<ITEM_TYPE> serializedContainerData;

		boost::iostreams::basic_array_source<wchar_t> device(str.data(), str.size());
		boost::iostreams::stream<boost::iostreams::basic_array_source<wchar_t> > stream(device);
		boost::archive::text_wiarchive ia(stream, boost::archive::no_header);
		ia >> serializedContainerData;

		container.clear();
		std::copy(serializedContainerData.cbegin(), serializedContainerData.cend(), std::back_inserter(container));
	}

	void SerializeContainerToStr(const CONTAINER &container, std::wstring &str) const
	{
		boost::iostreams::back_insert_device<std::wstring> inserter(str);
		boost::iostreams::stream<boost::iostreams::back_insert_device<std::wstring> > stream(inserter);
		boost::archive::text_woarchive archive(stream, boost::archive::no_header);

		std::vector<ITEM_TYPE> serializedContainerData(container.cbegin(), container.cend());

		archive << serializedContainerData;
		stream.flush();
	}

	const CONTAINER* const m_container;
};

//////////////////////////////////////////////////////////////////////////

#define PROPERTY_SIMPLE_REF(p, xml_tag, type)			new CPropertySimple< type >( &(p), xml_tag )
#define PROPERTY_SIMPLE_REF_DEFINIT(p, xml_tag, type, defval)	new CPropertySimple< type >( &(p), xml_tag, std::move(defval) )
#define PROPERTY_SIMPLEARR_REF(p, xml_tag, type)		new CPropertySimpleArr<type>( (p), boost::size(p), xml_tag )
#define PROPERTY_ENUM_REF(p, xml_tag, type)				new CPropertyEnum< type >( &(p), xml_tag )
#define PROPERTY_SETOFENUM_REF(p, xml_tag, type)		new CPropertySetOfEnum< type >( &(p), xml_tag )
#define PROPERTY_SETOF_REF(p, xml_tag, container)			new CPropertySetOfSimple< container >( &(p), xml_tag )
#define PROPERTY_LISTOF_REF(p, xml_tag, container, decom)   new CPropertyContainerOf< container >( &(p), xml_tag, decom )
#define PROPERTY_CONTAINEROF_REF(p, xml_tag, container) new CPropertyContainerOfSimple< container >( &(p), xml_tag )
#define PROPERTY_CONTAINEROFENUM_REF(p, xml_tag, type)		new CPropertyContainerOfEnum< type >( &(p), xml_tag )
#define PROPERTY_CONTAINER_OF_PTRS_REF(p, xml_tag, container, copy_operator, decom) new CPropertyContainerOfPtrs< container, copy_operator, decom >( &(p), xml_tag )
#define PROPERTY_MAP_OF_STRUCT_REF(p, xml_tag, decom, key_gen, key_degen)				new CPropertyMapOfStruct< decltype(p) >( &(p), xml_tag, decom, key_gen, key_degen )
#define PROPERTY_MAP_OF_PTRS_REF(p, xml_tag, map, copy_operator, decom, key_get, instance) new CPropertyMapOfPtrs< map, copy_operator >( &(p), xml_tag, decom, key_get, instance)
#define PROPERTY_CONTAINEROFBINARY_REF(p, xml_tag, contType) new CPropertyContainerOfBinary <contType> ( &(p), xml_tag )
