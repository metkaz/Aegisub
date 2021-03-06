// Copyright (c) 2006, Rodrigo Braz Monteiro
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

/// @file audio_provider_hd.h
/// @see audio_provider_hd.cpp
/// @ingroup audio_input
///

#include "include/aegisub/audio_provider.h"

namespace agi {
	class BackgroundRunner;
	class ProgressSink;
	class read_file_mapping;
}

class HDAudioProvider final : public AudioProviderWrapper {
	/// Name of the file which the decoded audio is written to
	agi::fs::path cache_filename;
	std::unique_ptr<agi::read_file_mapping> file;

	/// Fill the cache with all of the data from the source audio provider
	/// @param src Audio data to cache
	/// @param file File to write to
	/// @param ps Sink for progress reporting
	void FillCache(AudioProvider *src, std::ostream *file, agi::ProgressSink *ps);

	void FillBuffer(void *buf, int64_t start, int64_t count) const override;

public:
	HDAudioProvider(std::unique_ptr<AudioProvider> source, agi::BackgroundRunner *br);
	~HDAudioProvider();
};
