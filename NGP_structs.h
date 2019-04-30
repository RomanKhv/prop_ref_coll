/**********************************************
  Copyright Mentor Graphics Corporation 2018

    All Rights Reserved.

 THIS WORK CONTAINS TRADE SECRET
 AND PROPRIETARY INFORMATION WHICH IS THE
 PROPERTY OF MENTOR GRAPHICS
 CORPORATION OR ITS LICENSORS AND IS
 SUBJECT TO LICENSE TERMS. 
**********************************************/

/**
*/

#pragma once

#include <vector>
//#include <memory.h>
//#include <guid_utils.h>
//#include <wchar_ngp.h>
#include "ngp_shared_array.h"
//#include <enums.h>
//#include <postenums2.h>
//#include <CNGPStream.h>
#include "CNGPString.h"
//#include <vector3.h>
//#include <float.h>
//#include <math_utils_basic.h>
/**
\brief template structure to hold array of fixed size of specified data type
*/
template<typename T, int N> struct SimpleStruct
{
	SimpleStruct()
	{
		memset ( _data, 0, sizeof (_data ) );
	}

	T& operator [] ( int n )
	{
		return _data[n];
	}
	const T& operator [] ( int n ) const
	{
		return _data[n];
	}

	template<class S> void load(S& s)
	{
		for(auto & value : _data)
			s >> value;
	}
	template<class S> void save(S& s) const
	{
		for(const auto & value : _data)
			s << value;
	}
	void show(std::ostream& s) const
	{
		s << "SimpleStruct:";
		for(const auto & value : _data)
			s << " " << value;
	}

	T _data[N];

	static const size_t NElements = N;
};

/**
\brief three-dimension point

	_data[0] x
	_data[1] y
	_data[2] z
*/
struct NGP3DPoint : public SimpleStruct<double, 3> 
{
public:
	NGP3DPoint(){};

	NGP3DPoint(double x, double y, double z )
	{
		Set( x, y, z );
	}

	void Set(double x, double y, double z)
	{
		_data[0] = x;
		_data[1] = y;
		_data[2] = z;
	}

	bool operator==( const NGP3DPoint& rhs ) const
	{
		return _data[0]==rhs._data[0] && _data[1]==rhs._data[1] && _data[2]==rhs._data[2];
	}
};

/**
\brief 3D vector
*/
struct NGP3DVector : public SimpleStruct<double, 3>  
{
public:
	NGP3DVector(){};
	
	NGP3DVector (double x, double y, double z )
	{
		operator[](0) = x;
		operator[](1) = y;
		operator[](2) = z;
	}

	NGP3DVector (const double x[3])
	{
		operator[](0) = x[0];
		operator[](1) = x[1];
		operator[](2) = x[2];
	}

	void Set(double x, double y, double z)
	{
		_data[0] = x;
		_data[1] = y;
		_data[2] = z;
	}

	void operator*=(double a)
	{
		operator[](0) *= a;
		operator[](1) *= a;
		operator[](2) *= a;
	}
};

/**
\brief two-dimension point

	_data[0] x
	_data[1] y
*/
struct NGP2DPoint  : public SimpleStruct<double, 2>
{
public:
	NGP2DPoint(){};

	NGP2DPoint( double x, double y )
	{
		operator[](0) = x;
		operator[](1) = y;
	}
};

/**
\brief 2D vector
*/
struct NGP2DVector : public SimpleStruct<double, 2>
{
public:
	NGP2DVector(){};
	
	NGP2DVector ( double x, double y )
	{
		operator[](0) = x;
		operator[](1) = y;		
	}
};

/**
\brief two-dimension point
	_data[0] x
	_data[1] y
*/
struct NGP2DSize   : public SimpleStruct<long, 2> {};
struct NGP2DPointD : public SimpleStruct<double, 2> {
	NGP2DPointD() {}
	NGP2DPointD(double x, double y) {
		_data[0] = x;
		_data[1] = y;
	}
};

struct NGPCAD3DBox;

/** Box - "NIKA" specification [Xmin Xmax Ymin Ymax Zmin Zmax]. It must be no implicit conversions from NGPCAD3DBox box. */
struct NGPNIK3DBox : public SimpleStruct<double, 6>
{
	NGPNIK3DBox() {}
	explicit NGPNIK3DBox( const NGPCAD3DBox& sNIKBox ) 
	{
		ConvertDataFromCADBox( sNIKBox );
	}
	NGPNIK3DBox(double x_min, double x_max, double y_min, double y_max,
		double z_min, double z_max) {
		_data[0] = x_min;
		_data[1] = x_max;
		_data[2] = y_min;
		_data[3] = y_max;
		_data[4] = z_min;
		_data[5] = z_max;
	}

	void ConvertDataFromCADBox( const double adblCADBox[6] )
	{
		_data[0] = adblCADBox[0]; _data[2] = adblCADBox[1]; _data[4] = adblCADBox[2]; 
		_data[1] = adblCADBox[3]; _data[3] = adblCADBox[4]; _data[5] = adblCADBox[5];
	};
	inline void ConvertDataFromCADBox( const struct NGPCAD3DBox& sCADBox );
};

/** Box - common CAD specification [Xmin Ymin Zmin Xmax Ymax Zmax]. It must be no implicit conversions from NIK box. */
struct NGPCAD3DBox : public SimpleStruct<double, 6>
{
	NGPCAD3DBox() {};
	explicit NGPCAD3DBox( const double adblCADBox[6] ) { memcpy( _data, adblCADBox, 6 * sizeof( double )); };
	explicit NGPCAD3DBox( const NGPNIK3DBox& sNIKBox )
	{
		ConvertDataFromNIKBox( sNIKBox );
	}
	NGPCAD3DBox ( double x_min, double y_min, double z_min, double x_max, double y_max, double z_max )
	{
		_data[0] = x_min;
		_data[1] = y_min;
		_data[2] = z_min;
		_data[3] = x_max;
		_data[4] = y_max;
		_data[5] = z_max;
	}

	void Set ( double x_min, double y_min, double z_min, double x_max, double y_max, double z_max )
	{
		_data[0] = x_min;
		_data[1] = y_min;
		_data[2] = z_min;
		_data[3] = x_max;
		_data[4] = y_max;
		_data[5] = z_max;
	}

	void ConvertDataFromNIKBox( const NGPNIK3DBox& sNIKBox )
	{
		_data[0] = sNIKBox._data[0];		_data[3] = sNIKBox._data[1];
		_data[1] = sNIKBox._data[2];		_data[4] = sNIKBox._data[3];
		_data[2] = sNIKBox._data[4];		_data[5] = sNIKBox._data[5];
	};
};

void NGPNIK3DBox::ConvertDataFromCADBox( const NGPCAD3DBox& sCADBox )
{
	_data[0] = sCADBox._data[0]; _data[2] = sCADBox._data[1]; _data[4] = sCADBox._data[2]; 
	_data[1] = sCADBox._data[3]; _data[3] = sCADBox._data[4]; _data[5] = sCADBox._data[5];
};

struct NGPCAD2DBox : public SimpleStruct<double, 4> {};

/**
\brief {distance, refinement level} pair
*/
struct NGPDistanceRefinementLevel
{
	NGPDistanceRefinementLevel()
	{

	}
	NGPDistanceRefinementLevel(double _distance, int _level)
	{
		distance = _distance;
		level = _level;
	}

	bool operator ==(const NGPDistanceRefinementLevel& dl) const
	{
		return distance == dl.distance && level == dl.level;
	}

	bool operator <(const NGPDistanceRefinementLevel& dl) const
	{
		return distance < dl.distance;
	}

	double	distance;
	int		level;
};

/**
\brief Transformation matrix
*/
struct NGPTransformationMatrix : public SimpleStruct<double, 16> {
	NGPTransformationMatrix ( )
	{
		_data[0] =  _data[4] = _data[8] = _data[12]= 1.;
	}
	double* R() { return &_data[0]; }
	const double* R() const { return &_data[0]; }
	double* T() { return &_data[9]; }
	const double* T() const { return &_data[9]; }

	bool HasTransform() const
	{
		return _data[0]!=1. || _data[4]!=1. || _data[8]!=1. || 
			_data[9]!=0. || _data[10]!=0. || _data[11]!=0. || 
			_data[12]!=1.;
	}
}; 


/**
\brief Planar surface definition parameters.
*/
struct NGPPlaneSurfaceParams : public SimpleStruct<double, 6> {}; 


/**
\brief Cylinder surface definition parameters.
*/
struct NGPCylinderSurfaceParams : public SimpleStruct<double, 7> {}; 

/**
\brief This struct contain an array of 7 doubles that define Rotation feature.

Elements 0,1,2 - define origin,
Elements 3,4,5 - define rotation axis,
Element 6 - sliding mesh flag : 0 - OFF, 1 - ON
*/
struct NGPRotationRegionParams : public SimpleStruct<double, 7> {}; 

/**
\brief Cone surface definition parameters.
*/
struct NGPConeSurfaceParams : public SimpleStruct<double, 8> {}; 

/**
\brief Sphere surface definition parameters.
*/
struct NGPSphere
	: public SimpleStruct<double, 4>
{
	NGPSphere() {}
	NGPSphere(double cx, double cy, double cz, double radius) {
		_data[0] = cx;
		_data[1] = cy;
		_data[2] = cz;
		_data[3] = radius;
	}
	const double* CenterCoords() const { return _data; }
	NGP3DPoint Center() const { return NGP3DPoint( _data[0], _data[1], _data[2] ); }
	double Radius() const { return _data[3]; }
};


struct NGPPlaneData
{
	NGP3DPoint Origin;
	NGP3DVector Normal;
	NGP3DVector Xvector;
};
