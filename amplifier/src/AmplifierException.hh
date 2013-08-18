/*****************************************************************************/ 
/* AmplifierException.hh                                                     */
/* Copyright (c) 2013 Tom Hartman (rokstar83@gmail.com)                      */
/*                                                                           */
/* This program is free software; you can redistribute it and/or             */
/* modify it under the terms of the GNU General Public License               */
/* as published by the Free Software Foundation; either version 2            */
/* of the License, or the License, or (at your option) any later             */
/* version.                                                                  */
/*                                                                           */
/* This program is distributed in the hope that it will be useful,           */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of            */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             */
/* GNU General Public License for more details.                              */
/*****************************************************************************/
#ifndef AMPLIFIEREXCEPTION_HH_
#define AMPLIFIEREXCEPTION_HH_
#include <stdexcept>

namespace BlasterBox {

	 class AmplifierException : public std::runtime_error
	 {
	 public:
			explicit AmplifierException(const std::string & msg);
	 };
}

#endif /* AMPLIFIEREXCEPTION_HH_ */