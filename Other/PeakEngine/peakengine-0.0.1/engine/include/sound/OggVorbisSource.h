/*
Copyright (C) 2008  Mathias Gottschlag

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in the
Software without restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the
Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "sound/SoundSource.h"

#ifndef _OGGVORBISSOURCE_H_
#define _OGGVORBISSOURCE_H_

struct OggVorbis_File;

//tolua_begin
namespace peak
{
	class OggVorbisSource : public SoundSource
	{
		public:
			OggVorbisSource();
			virtual ~OggVorbisSource();
			
			bool init(std::string filename);
			
			virtual int getLength(void);
			
			virtual bool isSeekable(void);
			virtual bool isStreamed(void);
			
			bool fillBuffer(unsigned int buffer);
			
			void update(void);
		private:
			//tolua_end
			OggVorbis_File *ovfile;
			bool stereo;
			int rate;
			//tolua_begin
	};
};
//tolua_end

#endif

