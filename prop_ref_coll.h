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
