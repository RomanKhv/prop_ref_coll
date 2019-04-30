/**********************************************
  Copyright Mentor Graphics Corporation 2017

    All Rights Reserved.

 THIS WORK CONTAINS TRADE SECRET
 AND PROPRIETARY INFORMATION WHICH IS THE
 PROPERTY OF MENTOR GRAPHICS
 CORPORATION OR ITS LICENSORS AND IS
 SUBJECT TO LICENSE TERMS. 
**********************************************/

#pragma once

#include <ios>
#include "CNGPString.h"
#include "ngp_shared_array.h"

typedef long NGPColor;

namespace ngp_file_io
{
	class INGPFileIO
	{
	public:
		typedef boost::int32_t dft_id_t;
        typedef ngp_shared_array<char> memory_buffer_t;

		virtual int open(const wchar_ngp* file_name, const wchar_ngp* mode) = 0;
		virtual bool open_tmp_file4write(const wchar_ngp* prefix, CNGPString& file_name) = 0;
        		
		virtual void seekr(boost::int64_t offset, std::ios::seekdir origin) = 0;
		virtual void seekw(boost::int64_t offset, std::ios::seekdir origin) = 0;
		virtual boost::uint64_t tellr() = 0;
		virtual boost::uint64_t tellw() = 0;

		virtual long read(float *buff, int elem_n) = 0;
		virtual long read(double *buff, int elem_n) = 0;
		virtual long read(int *buff, int elem_n) = 0;
		virtual long read(long *buff, int elem_n) = 0;
		        long read(unsigned int *buff, int elem_n)				{ return uint_read(buff, elem_n); }
		virtual long read(unsigned long *buff, int elem_n) = 0;
		virtual long read(short *buff, int elem_n) = 0;
		virtual long read(char *buff, int elem_n) = 0;
		virtual long read(bool* buff, int elem_n) = 0;
		virtual long read(CNGPString* buff, int elem_n) = 0;

		virtual long write(const float *buff, int elem_n) = 0;
		virtual long write(const double *buff, int elem_n) = 0;
		virtual long write(const int *buff,	int elem_n) = 0;
		virtual long write(const long *buff, int elem_n) = 0;
		        long write(const unsigned int *buff, int elem_n)		{ return uint_write(buff, elem_n); }
		virtual long write(const unsigned long *buff, int elem_n) = 0;
		virtual long write(const short *buff, int elem_n) = 0;
		virtual long write(const char *buff, int elem_n) = 0;
		virtual long write(const bool* buff, int elem_n) = 0;
		virtual long write(const CNGPString* buff, int elem_n) = 0;

		virtual long read(CNGPString& str) = 0;
		virtual long write(const CNGPString& str) = 0;

        virtual long read(GUID& uuid) = 0;
        virtual long write(const GUID& uuid) = 0;

		virtual long size_write(boost::uint64_t val) { return uint64_write(&val, 1); }
		virtual long size_read(boost::uint64_t& val) { return uint64_read(&val, 1); }

		virtual long bool_write(bool val) = 0;
		virtual long bool_read(bool& val) = 0;

		virtual long dbl_write(double val) = 0;
		virtual long dbl_write(const double *buff,	int elem_n) = 0;
		virtual long dbl_read(double *buff, int elem_n) = 0;

		virtual long int_write(int val) = 0;
		virtual long int_read(int *buff, int elem_n) = 0;
		virtual long int_write(const int *buff, int elem_n) = 0;

		virtual long uint_read(unsigned int* buff, int elem_n) = 0;
		virtual long uint_write(const unsigned int* buff, int elem_n) = 0;

		virtual long int64_read(boost::int64_t *buff, int elem_n) = 0;
		virtual long int64_write(boost::int64_t val) = 0;
		virtual long int64_write(const boost::int64_t *buff, int elem_n) = 0;

		virtual long uint64_read(boost::uint64_t *buff, int elem_n) = 0;
		virtual long uint64_write(const boost::uint64_t *buff, int elem_n) = 0;

		virtual long long_read(long *buff, int elem_n) = 0;
		virtual long long_write(const long *buff, int elem_n) = 0;
		virtual long long_write(long val) = 0;

		virtual long ulong_read(unsigned long *buff, int elem_n) = 0;
		virtual long ulong_write(const unsigned long *buff, int elem_n) = 0;

		virtual long chr_read(char *buff, int elem_n) = 0;
		virtual long chr_write(const char *buff, int elem_n) = 0;

		virtual long flt_read(float *buff, int elem_n) = 0;
		virtual long flt_write(const float *buff, int elem_n) = 0;

		//helper aliases
		long str_read(CNGPString& s)		{ return read(s); }
		long str_write(const CNGPString& s)	{ return write(s); }
		long flt_read(float& val)			{ return flt_read( &val, 1 ); }
		long flt_write(float val)			{ return flt_write( &val, 1 ); }
		long dbl_read(double& val)			{ return dbl_read( &val, 1 ); }
		long int_read(int& val)				{ return int_read( &val, 1 ); }
		long long_read(long& val)			{ return long_read( &val, 1 ); }
		long ulong_read(unsigned long& val)	{ return ulong_read( &val, 1 ); }
		long ulong_write(unsigned long val)	{ return ulong_write( &val, 1 ); }
		long uint64_read(boost::uint64_t& val)	{ return uint64_read(&val, 1); }
		long uint64_write(boost::uint64_t val)	{ return uint64_write(&val, 1); }
		long enum_read(long& e)				{ return long_read(e); }
		long enum_write(long e)				{ return long_write(e); }
        long color_write(NGPColor color) { return int_write((int*)&color, 1); }
        long color_read(NGPColor& color) { return int_read((int*)&color, 1); }

        virtual void open_memory_buffer() = 0;
        virtual void open_memory_buffer(const memory_buffer_t& src) = 0;
        virtual void close_memory_buffer() = 0;
        virtual void dump_memory_buffer(memory_buffer_t& dst) = 0;

		virtual void begin_write_ZFILE() = 0;
		virtual void begin_read_ZFILE() = 0;
		virtual void end_io_ZFILE() = 0;

		virtual void dft_begin_w(const dft_id_t dft_id) = 0;
		virtual void dft_end_w() = 0;
		virtual void dft_begin_r(const dft_id_t dft_id) = 0;
		virtual void dft_end_r() = 0;

		virtual void flush() = 0;
		virtual void close() = 0;

		virtual void set_HugeBlockMode(bool bHugeBlockMode) = 0;
		virtual void set_FLDDataBegin(boost::uint64_t fldDataBegin) = 0;
	};

	typedef boost::shared_ptr<INGPFileIO> INGPFileIOPtr;

	inline INGPFileIOPtr CreateFileIO() { return {}; }

	class ZIP_write_scope
	{
	public:
		ZIP_write_scope(INGPFileIO* f) :
            m_file_io(f)
		{
			if (m_file_io)
				m_file_io->begin_write_ZFILE();
		}
		~ZIP_write_scope()
		{
			if (m_file_io)
				m_file_io->end_io_ZFILE();
		}
	private:
		INGPFileIO* m_file_io;
	};

	class ZIP_read_scope
	{
	public:
		ZIP_read_scope(INGPFileIO* f) :
            m_file_io(f)
		{
			if (m_file_io)
				m_file_io->begin_read_ZFILE();
		}
		~ZIP_read_scope()
		{
			if (m_file_io)
				m_file_io->end_io_ZFILE();
		}
	private:
		INGPFileIO* m_file_io;
	};

	class DFT_write_scope
	{
	public:
		DFT_write_scope(INGPFileIO* f, INGPFileIO::dft_id_t dft_id) :
			m_file_io(f)
		{
			if (m_file_io)
				m_file_io->dft_begin_w(dft_id);
		}
		~DFT_write_scope()
		{
			if (m_file_io)
				m_file_io->dft_end_w();
		}
	private:
		INGPFileIO* m_file_io;
	};

	class DFT_read_scope
	{
	public:
		DFT_read_scope(INGPFileIO* f, INGPFileIO::dft_id_t dft_id) :
			m_file_io(f)
		{
			if (m_file_io)
				m_file_io->dft_begin_r(dft_id);
		}
		~DFT_read_scope()
		{
			if (m_file_io)
				m_file_io->dft_end_r();
		}
	private:
		INGPFileIO* m_file_io;
	};

    template<class Container>
    class mem_read_scope
    {
    public:
        mem_read_scope(INGPFileIO* f, const Container& src) :
            m_file_io(f)
        {
            if (m_file_io)
                m_file_io->open_memory_buffer(src);
        }
        ~mem_read_scope()
        {
            if (m_file_io)
                m_file_io->close_memory_buffer();
        }
    private:
        INGPFileIO* m_file_io;
    };

    template<class Container>
    inline mem_read_scope<Container> make_mem_read_scope(INGPFileIO* f, const Container& src) { return mem_read_scope<Container>(f, src); }

    template<class Container>
    class mem_write_scope
    {
    public:
        mem_write_scope(INGPFileIO* f, Container& dst) :
            m_dst(dst),
            m_file_io(f)
        {
            if (m_file_io)
                m_file_io->open_memory_buffer();
        }
        ~mem_write_scope()
        {
            if (m_file_io)
            {
                m_file_io->dump_memory_buffer(m_dst);
                m_file_io->close_memory_buffer();
            }
        }
    private:
        Container& m_dst;
        INGPFileIO* m_file_io;
    };

    template<class Container>
    inline mem_write_scope<Container> make_mem_write_scope(INGPFileIO* f, Container& src) { return mem_write_scope<Container>(f, src); }

	class utils	
	{
	public:
		static size_t serialization_size_in_bytes( const CNGPString& s )
		{
			return sizeof( int ) + (s.empty() ? 0 : (s.GetLength() + 1) * sizeof( wchar_ngp ));
		}
	};
}
