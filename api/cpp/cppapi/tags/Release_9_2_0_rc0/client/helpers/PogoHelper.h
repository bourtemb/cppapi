//+=============================================================================
//
// project :      TANGO Utility
// file :         PogoHelper.h:
// $Revision$
// $Author$
//
// Copyright (C) :      2004,2005,2006,2007,2008,2009,2010,2011,2012,2013,2014,2015
//						Synchrotron SOLEIL
//                		L'Orme des Merisiers
//                		Saint-Aubin - BP 48 - France
//
// This file is part of Tango.
//
// Tango is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Tango is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with Tango.  If not, see <http://www.gnu.org/licenses/>.
//
//   description :  This utility class helps to allocate and desallocate READ attributes
//	as POGO only generates pointers and let the Device developper manage memory allocation
//
//		Note : these functions also initialise to a default value the allocated attributes
// Usage examples :
//
//		CREATE_SCALAR_ATTRIBUTE(attr_hCoordinate_read);	// by default initialised to 0
//		CREATE_SCALAR_ATTRIBUTE(attr_hCoordinate_read,1);	// initialised to 1
//		DELETE_SCALAR_ATTRIBUTE(attr_hCoordinate_read)
//
//		CREATE_DEVSTRING_ATTRIBUTE(attr_currentUMatrix_read,MAX_STRING_LENGTH);// by default initialised to ""
//		CREATE_DEVSTRING_ATTRIBUTE(attr_currentUMatrix_read,MAX_STRING_LENGTH,"No U Matrix available");
//		DELETE_DEVSTRING_ATTRIBUTE(attr_currentUMatrix_read)
//
//		CREATE_SPECTRUM_ATTRIBUTE(attr_currentspectrum_read,MAX_LENGTH);// by default initialised to 0
//		CREATE_SPECTRUM_ATTRIBUTE(attr_currentspectrum_read,MAX_LENGTH, 1);
//		DELETE_SPECTRUM_ATTRIBUTE(attr_currentspectrum_read);

//-----------------------------------------------------------------------------
//
// function to delete a scalar attribut : You must pass the pointer to the attributs
// Usage : 		DELETE_SCALAR_ATTRIBUTE(attr_hCoordinate_read);

//-----------------------------------------------------------------------------
template <class _SCALAR>  void DELETE_SCALAR_ATTRIBUTE(_SCALAR* attributeName)

{
	if(attributeName)
	{
		delete attributeName;
		attributeName = 0;
	}
}
//=============================================================================
// function to allocate and initialise a read SCALAR attribute as generated by POGO
// Usage : ex : CREATE_SCALAR_ATTRIBUTE(attr_hCoordinate_read);
//=============================================================================
template <class _SCALAR> void CREATE_SCALAR_ATTRIBUTE(_SCALAR*&  attributeName,_SCALAR default_value=0)
{
	attributeName = new _SCALAR;

	if(attributeName == 0)
	{
		TangoSys_OMemStream o;
		o <<  "[" << __FILE__ << "::" << __LINE__ << "]" << std::ends;
		Tango::Except::throw_exception (
			(const char *)"OUT OF MEMORY",
			(const char *)"out of memory error",
			(const char *)(o.str().c_str()));
	}
	// then initialise scalar
	(*attributeName) = (_SCALAR) default_value;

}


//=============================================================================
// function to allocate and initialise a DevString SCALAR attribute as generated by POGO
// Usage : ex : CREATE_DEVSTRING_ATTRIBUTE(attr_currentUMatrix_read,MAX_STRING_LENGTH,"No U Matrix available");
//=============================================================================
inline void CREATE_DEVSTRING_ATTRIBUTE(Tango::DevString*& attributeName, const int length, const char* init_string="")
{
	attributeName = new char*;

	if(attributeName == 0)
	{
		TangoSys_OMemStream o;
		o <<  "[" << __FILE__ << "::" << __LINE__ << "]" << std::ends;
		Tango::Except::throw_exception (
			(const char *)"OUT OF MEMORY",
			(const char *)"out of memory error",
			(const char *)(o.str().c_str()));
	}

	//std::cout << length << std::endl;
	(*attributeName) = new char[length];
	if((*attributeName) == 0)
	{
		TangoSys_OMemStream o;
		o <<  "[" << __FILE__ << "::" << __LINE__ << "]" << std::ends;
		Tango::Except::throw_exception (
			(const char *)"OUT OF MEMORY",
			(const char *)"out of memory error",
			(const char *)(o.str().c_str()));
	}
	::strcpy(*attributeName, init_string);

}
//-----------------------------------------------------------------------------
//
// function to delete a DevString attribut : You must pass the pointer to the attributs
// Usage : 		DELETE_DEVSTRING_ATTRIBUTE(attr_currentUMatrix_read)

//-----------------------------------------------------------------------------
inline void DELETE_DEVSTRING_ATTRIBUTE(Tango::DevString* attributeName)
{

 if(attributeName)
    {
        if(*attributeName)
            delete [] (*attributeName);
        delete attributeName;
        attributeName = 0;
    }

}
//=============================================================================
// function to allocate and initialise a read SPECTRUM attribute as generated by POGO
// Usage : ex : CREATE_SPECTRUM_ATTRIBUTE(attr_currentspectrum_read,MAX_LENGTH);
//=============================================================================
//
template <class _SPECTRUM> void CREATE_SPECTRUM_ATTRIBUTE(_SPECTRUM *& attributeName, const int size, _SPECTRUM default_value=0)
{
	attributeName = new _SPECTRUM[size];

	if(attributeName == 0)
	{
		TangoSys_OMemStream o;
		o <<  "[" << __FILE__ << "::" << __LINE__ << "]" << std::ends;
		Tango::Except::throw_exception (
			(const char *)"OUT OF MEMORY",
			(const char *)"out of memory error",
			(const char *)(o.str().c_str()));
	}
  std::fill( attributeName, attributeName + size, default_value );

}
//=============================================================================
// function to desallocate a read SPECTRUM attribute as generated by POGO
// Usage : ex : DELETE_SPECTRUM_ATTRIBUTE(attr_currentspectrum_read);
//=============================================================================
//
template <class _SPECTRUM> void DELETE_SPECTRUM_ATTRIBUTE(_SPECTRUM * attributeName )
{
	if(attributeName)
	{
		delete [] (attributeName);
	}

}
