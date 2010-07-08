// PreComp - PreComp file for MediaInfo
// Copyright (C) 2006-2009 Jerome Martinez, Zen@MediaArea.net
//
// This library is free software: you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this library. If not, see <http://www.gnu.org/licenses/>.
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// Helpers for compilers (precompilation)
//
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//---------------------------------------------------------------------------
#ifdef ZENLIB_USEWX
    #include <wx/wxprec.h>
    #ifdef WX_PRECOMP
        #include <string>
        #include <sstream>
        #include <vector>
    #endif
#else //ZENLIB_USEWX
    #if defined(__BORLANDC__) ||defined(__VISUALC__)
        #include <string>
        #include <sstream>
        #include <vector>
    #endif
#endif //ZENLIB_USEWX
//---------------------------------------------------------------------------

