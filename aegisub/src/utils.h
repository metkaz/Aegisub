// Copyright (c) 2005-2009, Rodrigo Braz Monteiro, Niels Martin Hansen
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//   * Redistributions of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//   * Redistributions in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//   * Neither the name of the Aegisub Group nor the names of its contributors
//     may be used to endorse or promote products derived from this software
//     without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//
// Aegisub Project http://www.aegisub.org/
//
// $Id$

/// @file utils.h
/// @see utils.cpp
/// @ingroup utility
///


#pragma once

#ifndef AGI_PRE
#include <stdint.h>

#include <algorithm>
#include <utility>
#include <vector>

#include <wx/icon.h>
#include <wx/menuitem.h>
#include <wx/thread.h>
#endif

class wxMouseEvent;
class wxWindow;

/// DOCME
typedef std::vector<std::pair<int,int> > IntPairVector;

#ifndef __LINUX__
int64_t abs64(int64_t input);
#endif
int CountMatches(wxString parent,wxString child);
wxString MakeRelativePath(wxString path,wxString reference);
wxString DecodeRelativePath(wxString path,wxString reference);
wxString AegiFloatToString(double value);
wxString AegiIntegerToString(int value);
wxString PrettySize(int bytes);
int SmallestPowerOf2(int x);
void GetWordBoundaries(const wxString text,IntPairVector &results,int start=0,int end=-1);
bool IsWhitespace(wchar_t c);
bool StringEmptyOrWhitespace(const wxString &str);
int AegiStringToInt(const wxString &str,int start=0,int end=-1);
int AegiStringToFix(const wxString &str,size_t decimalPlaces,int start=0,int end=-1);
wxIcon BitmapToIcon(wxBitmap bmp);
void RestartAegisub();

/// Forward a mouse wheel event to the window under the mouse if needed
/// @param source The initial target of the wheel event
/// @param evt The event
/// @return Should the calling code process the event?
bool ForwardMouseWheelEvent(wxWindow *source, wxMouseEvent &evt);


/// @brief Templated abs() function
template <typename T> T tabs(T x) { return x < 0 ? -x : x; }

template<typename T> inline T mid(T a, T b, T c) { return std::max(a, std::min(b, c)); }

#ifndef FORCEINLINE
#ifdef __VISUALC__

/// DOCME
#define FORCEINLINE __forceinline
#else

/// DOCME
#define FORCEINLINE inline
// __attribute__((always_inline)) gives me errors on g++ ~amz
#endif
#endif

/// Polymorphic delete functor
struct delete_ptr {
	template<class T>
	void operator()(T* ptr) const {
		delete ptr;
	}
};

/// Delete all of the items in a container of pointers and clear the container
template<class T>
void delete_clear(T& container) {
	if (!container.empty()) {
		std::for_each(container.begin(), container.end(), delete_ptr());
		container.clear();
	}
}

/// Helper class for background_delete_clear
template<class Container>
class BackgroundDeleter : public wxThread {
	Container cont;
	wxThread::ExitCode Entry() {
		delete_clear(cont);
		return (wxThread::ExitCode)0;
	}
public:
	BackgroundDeleter(Container &source)
	: wxThread(wxTHREAD_DETACHED)
	{
		using std::swap;
		swap(cont, source);

		SetPriority(WXTHREAD_MIN_PRIORITY);
		Create();
		Run();
	}
};

/// Clear a container of pointers and delete the pointed to members on a
/// background thread
template<class T>
void background_delete_clear(T& container) {
	if (!container.empty())
		new BackgroundDeleter<T>(container);
}


template<class Out>
struct cast {
	template<class In>
	Out operator()(In in) const {
		return dynamic_cast<Out>(in);
	}
};
