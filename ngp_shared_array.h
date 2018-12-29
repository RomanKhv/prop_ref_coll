/**********************************************
  Copyright Mentor Graphics Corporation 2018

    All Rights Reserved.

 THIS WORK CONTAINS TRADE SECRET
 AND PROPRIETARY INFORMATION WHICH IS THE
 PROPERTY OF MENTOR GRAPHICS
 CORPORATION OR ITS LICENSORS AND IS
 SUBJECT TO LICENSE TERMS. 
**********************************************/

#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/type_traits.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <vector>
#include <boost/functional/hash_fwd.hpp>
#include <cpp_compiler_features.h>
#include <assert_macro.h>

#ifdef _MSC_VER
#pragma warning(disable : 4996)
#endif

template<class T> inline void ngp_checked_delete(T * x)
{
	// intentionally complex - simplification causes regressions
	typedef char type_must_be_complete[ sizeof(T)? 1: -1 ];
	(void) sizeof(type_must_be_complete);
	::operator delete ( x );
}

/**
\brief template class for collection of NGP objects.

WARNING! It's not really SHARED array. Data is shared only until it's not changed, if you change it => you change your own copy.

This class is designed to provide a maximum performance when passing data across function call stack.
It does not allocate a memory and call copy constructor when copy operator of ngp_shared_array is called.
This class allow to transfer data between modules compiled with different compiler and different version of CRT libraries without  memory leaks and corruption.

When user want to change the data inside array class check that it  is unique and if not it copy data,
so original data will not be changed during editing.
*/
template<class T>
class ngp_shared_array : protected boost::shared_ptr<T>
{
private:
	typedef boost::shared_ptr<T> super_type;

	template<class T2> struct checked_deleter
	{
		typedef void result_type;
		typedef T2 * argument_type;

		void operator()(T2 * x) const
		{
			// boost:: disables ADL
			ngp_checked_delete(x);
		}
	};

public:
    typedef ngp_shared_array<T> this_type;
	typedef T value_type;
	typedef T* iterator;
	typedef const T* const_iterator;
	typedef size_t size_type;
	/**
	constructor
	*/
	ngp_shared_array ( )
	{
		_size = 0;
		_capacity = 0;
	}
	ngp_shared_array ( const this_type& a )
	{
		*this = a;
	}
	/**
	destructor
	*/
	~ngp_shared_array ( )
	{
	}

	const this_type& operator = (const this_type& a )
	{
		if ( &a == this )
			return *this;
		if ( this_type::get() != a.get() )
		{
			clear();
			super_type::operator = ( static_cast<super_type> (a) );
		}
		_size = a._size;
		_capacity = a._capacity;
		m_deleter = a.m_deleter;
		return *this;
	}

#ifdef CPP_INITIALIZER_LISTS_AVAILABLE
	ngp_shared_array( std::initializer_list<T> il )
		: ngp_shared_array()
	{
		assign( il.begin(), il.end() );
	}
	const this_type& operator = ( std::initializer_list<T> il )
	{
		assign( il.begin(), il.end() );
		return *this;
	}
#endif

	/**
	This function set array size to 0, but do not free allocated memory.
	*/
	void clear ( )
	{
		if ( is_shared() ) {
			this_type p;
			swap ( p );
		}
		else {
			m_deleter.reset ( );
			_size = 0;
		}
	}

	/**
	Return constant reference to beginning of array. Use it for reading.
	*/
	const_iterator begin ( ) const
	{
		return this->get();
	}
	const_iterator cbegin ( ) const
	{
		return this->get();
	}
	iterator ncbegin ( )
	{
		backup();
		return this->get();
		// Non-const begin()/end() methods are forbidden (to prevent unintended blowing up of memory)
	}

	/**
	Return constant reference to beginning of array. Use it for reading.
	*/
	const_iterator end ( ) const
	{
		return ( this->get() + _size );
	}
	const_iterator cend ( ) const
	{
		return ( this->get() + _size );
	}
	iterator ncend ( )
	{
		backup();
		return ( this->get() + _size );
	}
	
	/**
	Change size of array. This function allocate additional memory if necessary.
	On growth new elements are default initialized. On reduction extra elements are destroyed.
	*/
	__inline void resize ( size_t new_size )
	{
		const size_t old_size = size();
		const bool shared = is_shared();

		if ( new_size == old_size )
		{
			if ( shared )
			{
				backup();	//break sharing
				//It is very likely that modification of array will follow the resize() call.
				//This is actually bugfix for the situation when a non-empty array is passed to filling method,
				//which populates it to the same size with { arr.resize(); parallel_for(){ arr.at() = ...; } }
			}
			return;
		}

		if ( !shared )		//sole owner
		{
			if ( new_size > old_size  &&  new_size <= capacity() )
			{
				construct( begin()+old_size, begin()+new_size );
				_size = new_size;
				init_deleter();
				return;
			}
			if ( new_size < old_size )
			{
				destroy( begin()+new_size, end() );
				_size = new_size;
				init_deleter();
				return;
			}
		}

		// actual reallocation + break of sharing:
		this_type newArr;
		newArr.alloc( new_size );
		const size_t mutual_subrange = (new_size < old_size) ? new_size : old_size;
		if ( mutual_subrange > 0 )
		{
			newArr.copy_construct( newArr.begin(), newArr.begin()+mutual_subrange, begin() );
		}
		if ( new_size > old_size )
		{
			newArr.construct( newArr.begin()+old_size, newArr.begin()+new_size );
		}
		newArr._size = new_size;
		newArr.init_deleter();

		swap( newArr );
	}
	/**
	Reserves a minimum length of storage, allocating space if necessary.
	*/
	__inline void reserve ( size_t size )
	{
		if ( size > 0 && size > capacity ( ) ){
			ngp_shared_array<T> _temp;
			_temp.alloc ( size );
			_temp.assign ( begin ( ), end ( ) );
			swap ( _temp );
		}
	}
	/**
	Erases an array and copies the specified elements to the empty array.
	*/
	template<class _Iterator>
	void assign ( _Iterator _begin, _Iterator _end )
	{
		const size_t new_size = calculate_size ( _begin, _end );
		if ( new_size==size() && !is_shared() )
		{
			copy( _begin, _end );
		}
		else  {
			clear();
			reserve( new_size );
			copy_construct( begin(), begin()+new_size, _begin );
			_size = new_size;
			init_deleter();
		}
	}
	/**
	Erases an array and copies all the elements from the specified array without memory sharing.
	Should be used when needed to break dependency memory allocator origin.
	*/
	void copy_without_sharing ( const this_type& r )
	{
		assign ( r.begin(), r.end() );
	}

	/**
	Resize array and add new item at the end.
	*/
	void push_back(const T& _Val)
	{
		if ( !is_shared()  &&  size() < capacity() )
		{
			++_size;
			copy_construct ( end()-1, end(), &_Val );
			init_deleter ( );
		}
		else {
			const size_t old_size = size();
			const size_t new_size = old_size + 1;

			this_type newArr;
			newArr.alloc( old_size + old_size/2 + 1 );
			newArr.copy_construct( newArr.begin(), newArr.begin()+old_size, begin() );
			newArr.copy_construct( newArr.begin()+old_size, newArr.begin()+new_size, &_Val );
			newArr._size = new_size;
			newArr.init_deleter ( );

			swap( newArr );
		}
	}

	// position iterator has const type to match begin/end functions return type
	template<class _Iterator>
	void insert ( const_iterator pos, _Iterator first, _Iterator last )
	{
		size_t pos_index = pos - this->begin();
		backup ( );
		pos =  this->begin() + pos_index;

		size_t added_size = calculate_size ( first, last );
		size_t required_size = _size + added_size;
		if ( capacity() < required_size )
		{
			size_t desired_size = _size + _size / 2;
			size_t pos_index = pos - this->begin();
			reserve ( ( required_size > desired_size ) ? required_size : desired_size );
			pos = this->get() + pos_index;
		}
		construct ( this->begin() + _size, this->begin() + required_size );
		_size = required_size;
		init_deleter ( );

		typedef boost::integral_constant < bool,
			! boost::is_class<value_type>::value &&
			//call memcpy if _Iterator is a simple type or if we copy from std::vector
			//because MS has special implementation for vector<bool> (internally it store array of words)
		    //do not use memcpy for that type of data
			( ! boost::is_class<_Iterator>::value || ( boost::is_same<_Iterator, typename std::vector<value_type>::iterator>::value && ! boost::is_same<value_type, bool >::value ) ) 
		> truth_type;
		insert ( const_cast<iterator>(pos), first, last, added_size, truth_type() );
	}

	template <class CONTAINER>
	void append(const CONTAINER& src)
	{
		if ( src.empty() )
			return;
		insert( end(), src.begin(), src.end() );
	}

	/**
	Return true if array size equal 0.
	*/
    bool operator! () const // never throws
    {
        return boost::shared_ptr<T>::operator! ( );
    }
	/**
	Return constant reference to i element of array. Use it for reading.
	*/
 	const T& operator[] (std::ptrdiff_t i) const // never throws
    {
		_ASSERTE ( i >= 0 && i < static_cast<std::ptrdiff_t> ( size ( ) ) );
		return this->get ( )[i];
    }
	/**
	Return reference to i element of array.
	This function check if an array is unique and if not copy the stored data.
	Because of performance use it only when you want to receive a write access.
	*/
 	T& at (std::ptrdiff_t i) // never throws
    {
		backup ( );
		return this->get ( )[i];
    }

	/**
	Returns reference to the 1st element in array. Be sure the array is not empty prior to call this method.
	*/
	const T& front() const
	{
		return operator[](0);
	}

	/**
	Returns reference to the last element in array. Be sure the array is not empty prior to call this method.
	*/
	const T& back() const
	{
		return operator[](_size-1);
	}

	T& back()
	{
		return at(_size-1);
	}

	/**
	Return reference to beginning of array.
	This function check if an array is unique and if not copy the stored data.
	Because of performance use it only when you want to receive a write access.
	*/
	iterator c_array ( )
	{
		backup ( );
		return this->get ( );
	}
	/**
	Return constant reference to beginning of array. Use it for reading.
	*/
	const_iterator data ( ) const
	{
		return this->get ( );
	}
	/**
	Return size of array.
	*/
	const size_t& size ( ) const
	{
		return _size;
	}
	/**
	Return true if array size equal 0.
	*/
	bool empty ( ) const
	{
		return ( size ( ) ) ? false : true;
	}
	/**
	Exchanges the contents of the two smart pointers.
	*/
	void swap ( this_type& sp )
	{
		size_t sz ( _size );
		size_t cap ( _capacity );

		_size = sp._size;
		_capacity = sp._capacity;
		sp._size = sz;
		sp._capacity = cap;

		boost::shared_ptr<T>::swap ( sp );
		m_deleter.swap ( sp.m_deleter );
	}
	template<class BinaryPredicate>
	void sort(const BinaryPredicate &_Comp)
	{
		std::stable_sort(c_array(), c_array()+_size, _Comp);
	}

	/**
	Returns true if array contains at least element equal to the specified value.
	*/
	bool contain ( const T& ref_val ) const
	{
		const const_iterator i_end = end ( );
		for ( const_iterator i = begin(); i != i_end; ++i )
		{
			if ( *i == ref_val )
				return true;
		}
		return false;
	}

	size_t size_in_bytes() const { return size() * sizeof(T); }

	struct hash
	{
		template <class TT>
		size_t operator() (const ngp_shared_array<TT>& arr) const
		{
			return boost::hash_range(arr.begin(), arr.end());
		}
	};
	struct equal 
	{ 
		template <class TT>
		bool operator()( const ngp_shared_array<TT> & x, const ngp_shared_array<TT> & y) const
		{
			if (x.size() != y.size())
				return false;
			for (size_t i =0, i_end = x.size(); i<i_end; i++)
			{
				if (x[i]!=y[i])
					return false;
			}
			return true;
		}
	};

private:
	template <typename I>
	void init_deleter ( const boost::false_type&  )
	{
		boost::function1 < void, char* > deleter = boost::bind ( &ngp_shared_array<T>::static_destroy, _1, this->get ( ), this->get ( ) + size ( )  );
		if ( m_deleter ) {
			boost::function1 < void, char* > *p_deleter = boost::get_deleter < boost::function1 < void, char* > > ( m_deleter );
			_ASSERTE ( p_deleter );
			*p_deleter = deleter;
		}
		else {
			m_deleter.reset ( ( char* ) ( intptr_t )-1 , deleter  );
		}
	}

	template<typename I>
	void init_deleter ( const boost::true_type& )
	{
	}

	//Must be called whenever '_size' is changed (because it is inserted into deleter functor)
	void init_deleter ( )
	{
		typedef boost::integral_constant<bool, ! boost::is_class<value_type>::value > truth_type;
		init_deleter<T> ( truth_type ( ) );
	}

	bool is_shared() const
	{
		return ( this->get() && !this->unique() );
	}

	void backup ( )
	{
		if ( is_shared() ) {
			boost::shared_ptr<T> sp = *this;
			const_iterator _begin = begin ( );
			const_iterator _end = end ( );
			reset ( );
			assign ( _begin, _end );
		}
	}
	void reset ( )
	{
		boost::shared_ptr<T>::reset ( );
		_size = 0;
		_capacity = 0;
	}
	void alloc ( const size_t& size )
	{
		boost::shared_ptr<T>::reset ( ( value_type *)::operator new ( size * sizeof ( value_type ) ), checked_deleter<T> ( ) );
		_ASSERTE ( this->get ( ) );
		_capacity = size;
	}
	template<class _Iterator>
	void copy ( _Iterator _begin, _Iterator _end, const boost::false_type& ) const
	{
		iterator dest_begin = this->get ( );
		while ( _begin != _end && dest_begin != end ( ) ) {
			( *dest_begin ) = ( *_begin );
			_begin ++;
			dest_begin ++;
		}
	}
	template<class _Iterator>
	void copy ( _Iterator _begin, _Iterator _end, const boost::true_type& ) const
	{
		size_t size = ( _end - _begin );
		size = ( size < _size ) ? size : _size;

		if ( size )
			memcpy ( this->get ( ), &( *_begin ), sizeof ( value_type ) * size );
	}
	template<class _Iterator>
	void copy ( _Iterator _begin, _Iterator _end ) const
	{
		typedef boost::integral_constant <
			bool,
			! boost::is_class<value_type>::value
			&&
			//call memcpy if _Iterator is a simple type or if we copy from std::vector
			//because MS has special implementation for vector<bool> (internaly it store array of words)
		    //do not use memcpy for that type of data
			( 
			( ! boost::is_class<_Iterator>::value && boost::is_same<_Iterator, iterator>::value )
			|| ( boost::is_same<_Iterator, typename std::vector<value_type>::iterator>::value && ! boost::is_same<value_type, bool >::value )
			) 
		> 
		truth_type;

		copy ( _begin, _end, truth_type ( ) );
	}

	template<class _Iterator>
	size_t calculate_size ( _Iterator _begin, _Iterator _end, const boost::true_type& ) const
	{
		return _end - _begin;
	}

	template<class _Iterator>
	size_t calculate_size ( _Iterator _begin, _Iterator _end, const boost::false_type& ) const
	{
		size_t size = 0;
		while ( _begin != _end ) {
			size ++;
			_begin ++;
		}
		return size;
	}
	template<class _Iterator>
	size_t calculate_size ( _Iterator _begin, _Iterator _end ) const
	{
		typedef boost::integral_constant <
			bool, ! boost::is_class<_Iterator>::value || boost::is_same<_Iterator, typename std::vector<value_type>::iterator>::value > 
		truth_type;
		return calculate_size ( _begin, _end, truth_type ( ) );
	}

	bool _is_valid_iterator_(iterator i) const { return ( i>=begin() && i<(begin()+_capacity) ); }

	template <typename I>
	static void destroy ( iterator _begin, const_iterator _end, const boost::false_type& )
	{
		while ( _begin != _end ) {
			( _begin )->~value_type ( );
			_begin ++;
		}
	}

	template <typename I>
	static void destroy ( iterator _begin, const_iterator _end, const boost::true_type& )
	{
	}

	static void destroy ( const_iterator _begin, const_iterator _end )
	{
		typedef boost::integral_constant<bool, ! boost::is_class<value_type>::value > truth_type;
		destroy<T> ( const_cast<iterator>(_begin), _end, truth_type ( ) );
	}

	static void static_destroy ( char* data, iterator _begin, iterator _end )
	{
		typedef boost::integral_constant<bool, ! boost::is_class<value_type>::value > truth_type;
		destroy<T> ( _begin, _end, truth_type ( ) );
	}

	template <typename I>
	void construct ( const_iterator const_begin, const_iterator _end, const boost::false_type& ) const
	{
		iterator _begin =  const_cast< iterator >( const_begin );
		while ( _begin != _end ) {
			_ASSERTE( _is_valid_iterator_(_begin) );
			::new ( _begin ) value_type ( );
			_begin ++;
		}
	}
	template <typename I>
	void construct ( const_iterator const_begin, const_iterator _end, const boost::true_type& ) const
	{
	}
	template <typename src_iterator_t>
	void copy_construct ( iterator dest_begin, const_iterator dest_end, src_iterator_t src_begin, const boost::false_type& ) const
	{
		iterator i_dest = dest_begin;
		src_iterator_t i_src = src_begin;
		for (; i_dest != dest_end; ++i_dest, ++i_src ) {
			_ASSERTE( _is_valid_iterator_(i_dest) );
			::new ( i_dest ) value_type ( *i_src );
		}
	}
	template <typename src_iterator_t>
	void copy_construct ( iterator dest_begin, const_iterator dest_end, src_iterator_t src_begin, const boost::true_type& ) const
	{
		iterator i_dest = dest_begin;
		src_iterator_t i_src = src_begin;
		for (; i_dest != dest_end; ++i_dest, ++i_src ) {
			_ASSERTE( _is_valid_iterator_(i_dest) );
			*i_dest = *i_src;
		}
	}
	void construct ( const_iterator _begin, const_iterator _end )
	{
		typedef boost::integral_constant<bool, boost::has_trivial_constructor<value_type>::value > truth_type;
		if ( _begin != _end )
			construct<T> ( _begin, _end, truth_type ( ) );
	}
	template <typename src_iterator_t>
	void copy_construct ( const_iterator _begin, const_iterator _end, src_iterator_t src_begin )
	{
		typedef boost::integral_constant<bool, boost::has_trivial_constructor<value_type>::value > truth_type;
		if ( _begin != _end )
			copy_construct<src_iterator_t> ( const_cast<iterator>(_begin), _end, src_begin, truth_type ( ) );
	}

	size_t capacity ( ) const
	{
		return _capacity;
	}

	template<class _Iterator>
	void insert ( iterator pos, _Iterator first, _Iterator last, size_t added_size, const boost::true_type& )
	{
		size_t moved_size = this->get ( ) + _size - added_size - pos;
		if ( moved_size > 0 )
			memmove ( pos + added_size, pos, sizeof(value_type) * moved_size );
		if ( added_size > 0 )
			memcpy ( pos, &*first, sizeof(value_type) * added_size );
	}

	template<class _Iterator>
	void insert ( iterator pos, _Iterator first, _Iterator last, size_t added_size, const boost::false_type& )
	{
		size_t to_move = this->get ( ) + _size - added_size - pos;
		if ( to_move > 0 )
			for ( iterator i = this->get() + _size - added_size - 1, j = this->get() + _size - 1; to_move > 0; --to_move )
				*( j-- ) = *( i-- );
		if ( added_size > 0 )
			while ( first != last )
				*( pos++ ) = *( first++ );
	}

	size_t _size;
	size_t _capacity;
	boost::shared_ptr<char> m_deleter;
};
