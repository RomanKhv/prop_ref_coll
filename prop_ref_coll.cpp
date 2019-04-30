
#include "stdafx.h"
#include "prop_ref_coll.h"
#include "xml_tags.h"

//////////////////////////////////////////////////////////////////////////

const wchar_ngp* CProperty::ItemXMLTag = L"item";

//////////////////////////////////////////////////////////////////////////

CPropertyTreeNode::CPropertyTreeNode( CProperty* property, CollectOptions& opt )
	: m_Property( property )
	, m_IsArray( false )
	, m_Options( opt )
{
}

CPropertyTreeNode& CPropertyTreeNode::Add( CProperty* property )
{
	_ASSERTE( !IsContaining( property->GetTag() ) ); //CProperty with the same tag-name already exists
	CPropertyTreeNode* newNode = new CPropertyTreeNode(property, m_Options);

	m_SubNodes.push_back( newNode );

	UpdateArrayFlag( property->GetTag() );

	return *newNode;
}

CPropertyTreeNode& CPropertyTreeNode::AddSubGroup( const wchar_ngp* group_tag )
{
	CPropertyTreeNode* newNode = new CPropertyTreeNode( new CPropertyGroup( group_tag ), m_Options );

	m_SubNodes.push_back( newNode );

	UpdateArrayFlag( newNode->m_Property->GetTag() );

	return *newNode;
}

void CPropertyTreeNode::UpdateArrayFlag(const std::wstring& propertyTag)
{
	if ( !m_IsArray )
		m_IsArray = ( propertyTag == CProperty::ItemXMLTag );
	else
		_ASSERTE( propertyTag == CProperty::ItemXMLTag );
}

void CPropertyTreeNode::VisitAll( const boost::function<void(CProperty*)>& visiter )
{
	if ( m_Property )
		visiter( m_Property.get() );

	for ( CPropertyTreeNode& node : m_SubNodes )
	{
		node.VisitAll( visiter );
	}
}

void CPropertyTreeNode::VisitAllc( const boost::function<void(const CProperty*)>& visiter ) const
{
	if ( m_Property )
		visiter( m_Property.get() );

	for ( const CPropertyTreeNode& node : m_SubNodes )
	{
		node.VisitAllc( visiter );
	}
}

bool CPropertyTreeNode::IsContaining( const std::wstring& tag ) const
{
	for ( const CPropertyTreeNode& subNode : m_SubNodes )
	{
		if ( subNode.m_Property &&
			 subNode.m_Property->GetTag()==tag )
			return true;
	}

	return false;
}

wptree& CPropertyTreeNode::SaveToPTree( wptree& owner_node ) const
{
	wptree* node = &m_Property->SaveToPTree( owner_node );

	if ( !m_SubNodes.empty() )
	{
		for ( const CPropertyTreeNode& subNode : m_SubNodes )
		{
			subNode.SaveToPTree( *node );
		}
	}

	return NGP_VERIFY(node) ? *node : owner_node;
}

const wptree* CPropertyTreeNode::LoadFromPTree( const wptree& owner_node, bool extractProperty )
{
	const wptree* node;

	if ( extractProperty )
		node = m_Property->LoadFromPTree( owner_node );		
	else
		node = &owner_node;		//(it is actually this_node)

	if ( !m_SubNodes.empty() && node )
	{
		if ( m_IsArray )
		{
			//'node' is expected to have only subnodes with tag==ItemXMLTag.
			//get_child() won't work in this case.
			//So we read subnodes here one after another.
			//When a subnode is read, a corresponding subproperty is loading from it.

			_ASSERTE( m_SubNodes.size() == node->size() );
			TNodesList::iterator iProp = m_SubNodes.begin();
			for ( const wptree::value_type& child_node : *node )
			{
				CPropertyTreeNode& subProp = *iProp;

				_ASSERTE( subProp.m_Property->GetTag() == CProperty::ItemXMLTag );
				_ASSERTE( child_node.first             == CProperty::ItemXMLTag );

				subProp.LoadFromPTree( child_node.second, false );
				//(here we set 'extractProperty' to false,
				// because we've already read a corresponding subnode and pass it)

				++iProp;
			}
		}
		else
		{
			for ( CPropertyTreeNode& subNode : m_SubNodes )
			{
				subNode.LoadFromPTree( *node );
			}
		}
	}

	return (node) ? node : &owner_node;
}

void CPropertyTreeNode::SaveToBinFile( const ngp_file_io::INGPFileIOPtr& bin_file ) const
{
	m_Property->SaveToBinFile( bin_file );

	for ( const CPropertyTreeNode& node : m_SubNodes )
		node.SaveToBinFile( bin_file );
}

void CPropertyTreeNode::LoadFromBinFile( const ngp_file_io::INGPFileIOPtr& bin_file )
{
	m_Property->LoadFromBinFile( bin_file );

	for ( CPropertyTreeNode& node : m_SubNodes )
		node.LoadFromBinFile( bin_file );
}

//////////////////////////////////////////////////////////////////////////

CPropertyRefCollection::CPropertyRefCollection(const wchar_ngp* rootTag, EPropertiesFilter filter)
	: CPropertyTreeNode( new CPropertyGroup(rootTag), m_Options )
	, m_Options( filter )
{
}

bool CPropertyRefCollection::operator==( const CPropertyRefCollection& rhs) const
{
	// If any property is supposed to be filtered out for specific operation (copy/compare/serialize), 
	// then the collection should be used for that specific operation only.
	// PF_All is suggested to include all properties (no filtering) =>
	// thus the collection can be used for different consequent operations (f.e., load and then compare).
	_ASSERTE( m_Options.m_filter==PF_ForEqualityTest || m_Options.m_filter==PF_All );
	_ASSERTE( rhs.m_Options.m_filter==PF_ForEqualityTest || rhs.m_Options.m_filter==PF_All );

	std::list<const CProperty*> l_props, r_props;
	VisitAllc(
		[&] (const CProperty* prop) { l_props.push_back( prop ); }
	);
	rhs.VisitAllc(
		[&] (const CProperty* prop) { r_props.push_back( prop ); }
	);

	NGP_REQUIRE_b( l_props.size() == r_props.size() );

	std::list<const CProperty*>::const_iterator li, ri;
	for (li=l_props.begin(), ri=r_props.begin(); li!=l_props.end() && ri!=r_props.end(); ++li, ++ri)
	{
		const CProperty& lp = *(*li);
		const CProperty& rp = *(*ri);
		ASSERT( lp.GetTag() == rp.GetTag() );

		if ( !lp.IsEqualTo(rp) )
		{
//			LOG(Message, (boost::wformat(L"Not equal property: '%s'") % lp.GetTag().c_str()).str().c_str());
			return false;
		}
	}

	return true;
}

void CPropertyRefCollection::CopyFrom( const CPropertyRefCollection& rhs )
{
	_ASSERTE( m_Options.m_filter==PF_Assignable || m_Options.m_filter==PF_AssignableExceptPalettes || m_Options.m_filter==PF_NonProjectLocal || m_Options.m_filter==PF_All );
	_ASSERTE( rhs.m_Options.m_filter==PF_Assignable || rhs.m_Options.m_filter==PF_AssignableExceptPalettes || rhs.m_Options.m_filter==PF_NonProjectLocal || rhs.m_Options.m_filter==PF_All );

	std::list<CProperty*> l_props;
	VisitAll(
		[&] (CProperty* prop) { l_props.push_back( prop ); }
	);
	std::list<const CProperty*> r_props;
	rhs.VisitAllc(
		[&] (const CProperty* prop) { r_props.push_back( prop ); }
	);

	NGP_REQUIRE( l_props.size() == r_props.size() );

	std::list<CProperty*>::iterator li;
	std::list<const CProperty*>::const_iterator ri;
	for (li=l_props.begin(), ri=r_props.begin(); li!=l_props.end() && ri!=r_props.end(); ++li, ++ri)
	{
		CProperty& lp = *(*li);
		const CProperty& rp = *(*ri);
		ASSERT( lp.GetTag() == rp.GetTag() );

		lp.CopyFrom( rp );
	}
}

bool CPropertyRefCollection::SaveToBinaryFile(const ngp_file_io::INGPFileIOPtr& file, const long refVersion) const
{
	_ASSERTE( m_Options.m_filter==PF_Serializable || m_Options.m_filter==PF_All );

    try
    {
        file->long_write(&refVersion, 1);
        CPropertyTreeNode::SaveToBinFile(file);
        return true;
    }
    catch (...)
    {
        return false;
    }
}

bool CPropertyRefCollection::SaveToBinaryFile(const fs::path& filename, const long refVersion) const
{
	_ASSERTE( m_Options.m_filter==PF_Serializable || m_Options.m_filter==PF_All );

    //ngp_file_io::INGPFileIOPtr bin_file = ngp_file_io::CreateFileIO();
    //NGP_REQUIRE_b(bin_file->open(filename.wstring().c_str(), L"wb"));

    //if (!NGP_VERIFY(SaveToBinaryFile(bin_file, refVersion)))
    //{
    //    boost::system::error_code ec;
    //    fs::remove(filename.c_str(), ec);
    //    return false;
    //}

    return true;
}

bool CPropertyRefCollection::LoadFromBinaryFile(const ngp_file_io::INGPFileIOPtr& file, const long refVersion)
{
	_ASSERTE( m_Options.m_filter==PF_Serializable || m_Options.m_filter==PF_All );

    try
    {
        long fileVersion = 0;
        file->long_read(&fileVersion, 1);
        if (fileVersion > refVersion)
            return false;

        CPropertyTreeNode::LoadFromBinFile(file);
        return true;
    }
    catch (...)
    {
        return false;
    }
}

bool CPropertyRefCollection::LoadFromBinaryFile(const fs::path& filename, const long refVersion)
{
	_ASSERTE( m_Options.m_filter==PF_Serializable || m_Options.m_filter==PF_All );

    ngp_file_io::INGPFileIOPtr bin_file = ngp_file_io::CreateFileIO();
    //NGP_VERIFY(bin_file->open(filename.wstring().c_str(), L"rb"));
    //
    //if (!NGP_VERIFY(LoadFromBinaryFile(bin_file, refVersion)))
    //    return false;
    return true;
}

//////////////////////////////////////////////////////////////////////////

bool CCollectionableData::SaveToFile(const fs::path& filename) const
{
	//settings
	wptree tag;
	if (!NGP_VERIFY(SaveToPTree(tag)))
		return false;

	return CPropertyRefCollection::SaveTreeToFile(filename, tag);
}

bool CCollectionableData::LoadFromFile(const fs::path& filename)
{
	std::wifstream input;
	std::locale utf8_locale(std::locale(), new fs::detail::utf8_codecvt_facet);
	input.imbue(utf8_locale);
	input.open(filename.c_str(), std::ios::in);
	if (!NGP_VERIFY(input.is_open()))
		return false;

	wptree tag;
	try {
//		read_xml(input, tag);
	}
	catch (...)	{
		return NGP_VERIFY(false);
	}

	if (!NGP_VERIFY(LoadFromPTree(tag)))
		return false;

	return true;
}

bool CCollectionableData::Save(std::function<bool(const CPropertyRefCollection&)> save, EPropertiesFilter filter) const
{
	CPropertyRefCollection coll(GetRootTag(), filter);

    const long version = GetVersion();
    coll.Add(PROPERTY_SIMPLE_REF(version, tag_version, long));
	CollectProperties(coll, coll.getOptions());

    try {
        save(coll);
    }
    catch (...) {
        return NGP_VERIFY(false);
    }

    return true;
}

bool CCollectionableData::SaveToPTree(wptree& tag, EPropertiesFilter filter) const
{
    return Save(
		[&](const CPropertyRefCollection& coll) {
			coll.SaveToPTree(tag);
			return true;
		},
		filter
	);
}

bool CCollectionableData::SaveToFile(const ngp_file_io::INGPFileIOPtr& file) const
{
    return Save(
		[&](const CPropertyRefCollection& coll) {
			if (!NGP_VERIFY(coll.SaveToBinaryFile(file)))
				return false;
			return true;
		},
		PF_Serializable
	);
}

bool CCollectionableData::Load(std::function<bool(CPropertyRefCollection&)> load, EPropertiesFilter filter)
{
    try {
		CPropertyRefCollection coll(GetRootTag(), filter);
		CollectOptions& opts = coll.getOptions();

        const long version = 0;
        coll.Add(PROPERTY_SIMPLE_REF(version, tag_version, long));

        CollectProperties(coll, opts);
        load(coll);
    }
    catch (...) {
        return NGP_VERIFY(false);
    }

    return true;
}

bool CCollectionableData::LoadFromPTree(const wptree& tag, EPropertiesFilter filter)
{
    return Load(
		[&](CPropertyRefCollection& coll) {
			coll.LoadFromPTree(tag);
			return true;
		},
		filter
	);
}

bool CCollectionableData::LoadFromFile(const ngp_file_io::INGPFileIOPtr& file)
{
    return Load(
		[&](CPropertyRefCollection& coll) {
			return coll.LoadFromBinaryFile(file);
		},
		PF_Serializable
	);
}

bool CPropertyRefCollection::SaveTreeToFile(const fs::path& filename, const wptree& tag)
{
	fs::wofstream output;
	std::locale utf8_locale(std::locale(), new fs::detail::utf8_codecvt_facet);
	output.imbue(utf8_locale);
	output.open(filename.c_str(), std::ios::out);
	if (!NGP_VERIFY(output.is_open()))
		return false;

	try {
//		write_xml(output, tag);
	}
	catch (...) {
		boost::system::error_code ec;
		fs::remove(filename, ec);
		return NGP_VERIFY(false);
	}

	return true;
}

bool CCollectionableData::SerializeAsPtree(bool save_load, ngp_shared_array<char>& data)
{
	if (save_load)
	{
		data.clear();

		wptree tree;
		if (!NGP_VERIFY(SaveToPTree(tree)))
			return false;

		boost::iostreams::stream_buffer<boost::iostreams::back_insert_device<ngp_shared_array<char>>> buff(data);
		std::iostream os(&buff);
		//boost::archive::binary_oarchive ar(os);
		//boost::property_tree::save(ar, tree, 1);

		return true;
	}
	else
	{
		boost::iostreams::stream_buffer<boost::iostreams::basic_array_source<char>> buff(data.begin(), data.size());

		std::iostream os(&buff);
//		boost::archive::binary_iarchive ar(os);

		wptree tree;
//		boost::property_tree::load(ar, tree, 1);

		return LoadFromPTree(tree);
	}
}

bool CCollectionableData::AssignProperties(const CCollectionableData& a, EPropertiesFilter filter)
{
	CPropertyRefCollection a_coll(a.GetRootTag(), filter);
	CPropertyRefCollection coll(GetRootTag(), filter);
	a.CollectProperties(a_coll, a_coll.getOptions());
	CollectProperties(coll, coll.getOptions());
	coll.CopyFrom(a_coll);
	return true;
}

bool CCollectionableData::IsEqual(const CCollectionableData& a) const
{
	CPropertyRefCollection a_coll(a.GetRootTag(), PF_ForEqualityTest);
	CPropertyRefCollection coll(GetRootTag(), PF_ForEqualityTest);
	a.CollectProperties(a_coll, a_coll.getOptions());
	CollectProperties(coll, coll.getOptions());
	return coll == a_coll;
}

bool CCollectionableData::operator==(const CCollectionableData& a) const
{
	return IsEqual(a);
}
