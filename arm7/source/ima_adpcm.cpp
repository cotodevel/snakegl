/***************************
 * ima-adpcm decoder by Discostew
 ***************************/
#include "typedefsTGDS.h"
#include "dsregs.h"
#include "dsregs_asm.h"
#include "pff.h"
#include "ima_adpcm.h"
#include "soundTGDS.h"
#include "main.h"
#include "ipcfifoTGDSUser.h"
#include "timerTGDS.h"

#define tell() pf_tell()
#define seek(x) pf_lseek (x)
#define skip(x) pf_lseek(x + tell())

IMA_Adpcm_Player player;	//Actual PLAYER Instance. See ima_adpcm.cpp -> [PLAYER: section
int ADPCMchunksize = 0;
int ADPCMchannels = 0;

extern "C" {
	void decode_mono_ima_adpcm( IMA_Adpcm_Data *data, u8 *src, s16 *dest, int iterations );
	void decode_stereo_ima_adpcm( IMA_Adpcm_Data *data, u8 *src, s16 *dest, int iterations );
}

IMA_Adpcm_Stream::IMA_Adpcm_Stream()	
{
	
}

IMA_Adpcm_Stream::~IMA_Adpcm_Stream()	
{
	
}

#if (defined(__GNUC__) && !defined(__clang__))
__attribute__((optimize("O0")))
#endif
#if (!defined(__GNUC__) && defined(__clang__))
__attribute__ ((optnone))
#endif
int IMA_Adpcm_Stream::reset(bool loop )	
{
	close();
	if( fget32() != 0x46464952 )		// "RIFF"
	{
		return IMA_ADPCM_ERROR_NOTRIFFWAVE;
	}
	int size = fget32();
	if( fget32() != 0x45564157 )		// "WAVE"
	{
		return IMA_ADPCM_ERROR_NOTRIFFWAVE;
	}
	
	// parse WAV structure
	while( tell() < size )
	{
		u32 code = fget32();
		u32 csize = fget32();
		switch( code )
		{
		case 0x20746D66: 	// format chunk
			
			// catch invalid format
			format = fget16();
			if(( format != WAV_FORMAT_PCM ) && ( format != WAV_FORMAT_IMA_ADPCM ))
			{
				return IMA_ADPCM_ERROR_UNSUPPORTED;
			}
			
			channels = fget16();
			
			// catch invalid channels
			if(( channels < 1 ) || ( channels > 2 ))
			{
				return IMA_ADPCM_ERROR_INVALIDCHANNELS;
			}
				
			sampling_rate = fget32();// sample rate
			skip( 4 );	// avg bytes/second
			block = fget16();
			
			sampBits = fget16();

			if((( format == WAV_FORMAT_PCM ) && (( sampBits != 8 ) && ( sampBits != 16 ))) ||
				(( format == WAV_FORMAT_IMA_ADPCM ) && ( sampBits != 4 )))
			{
				return IMA_ADPCM_ERROR_UNSUPPORTED;
			}
			
			skip( csize - 0x10 );
			break;
			
		case 0x61746164:	// data chunk
			wave_data = tell();
			loop1 = 0;
			skip( csize );
			wave_end = tell();
			if( format == WAV_FORMAT_PCM )
				loop2 = csize >> ( sampBits == 16 ? channels : ( channels - 1 ));
			else
				loop2 = csize << ( 2 - channels );
			break;
			
		case 0x6C706D73:	// sampler chunk
		{
			int s;
			skip( 28 );
			int nl = fget32();
			skip(4);
			s = 36;
			if( nl && loop) 
			{
				skip( 8 );
				loop1 = fget32() >> ( 2 - channels );
				loop2 = fget32() >> ( 2 - channels );
				s += 8+4+4;
			}
			skip( csize - s );
		}
			break;
		default:
			skip( csize );
		}
	}
	wave_loop = loop;
	oddnibble = 0;
	data.curSamps = 0;
	position = 0;
	seek( wave_data );
	currentblock = tell();
	state = state_beginblock;
	return IMA_ADPCM_OKAY;
}

#if (defined(__GNUC__) && !defined(__clang__))
__attribute__((optimize("O0")))
#endif
#if (!defined(__GNUC__) && defined(__clang__))
__attribute__ ((optnone))
#endif
int IMA_Adpcm_Stream::stream( s16 *target, int length )	
{
	if( format == WAV_FORMAT_PCM )
		return stream_pcm( target, length );
	else
	return decode_ima( target, length );
}

#if (defined(__GNUC__) && !defined(__clang__))
__attribute__((optimize("O0")))
#endif
#if (!defined(__GNUC__) && defined(__clang__))
__attribute__ ((optnone))
#endif
int IMA_Adpcm_Stream::stream_pcm( s16 *target, int length )	
{
	if( !wave_loop && ( currentblock >= wave_end ))
		return 1;
	while( length )
	{
		if( !wave_loop ) {
			if( position >= loop2 )
			{
				int i = length * channels;
				if( sampBits == 8 ) i >>= 1;
				for( ; i; i-- )
					*target++ = 0;
				length = 0;
				break;
			}
		}
		if( position == loop1 ) loop_cblock = currentblock;

		int iterations, cpysize;

		iterations = length;
		if( position < loop1 )
		{
			if( position + iterations >= loop1 )
				iterations = loop1-position;
		}
		else if( position < loop2 )
		{
			if( position + iterations >= loop2 )
				iterations = loop2-position;
		}
		cpysize = iterations << ( sampBits == 16 ? channels : ( channels - 1 ));
		
		UINT nbytes_read;
		pf_read(target, cpysize, &nbytes_read);			
		length -= iterations;
		position += iterations;
		currentblock += cpysize;
		if( sampBits == 8 )
			target += iterations >> ( 2 - channels );
		else
			target += iterations * channels;

		
		if(( position == loop2 ) && wave_loop ) {
			seek( loop_cblock );
			currentblock = loop_cblock;
			position = loop1;
		}	
	}
	return 0;
}

#if (defined(__GNUC__) && !defined(__clang__))
__attribute__((optimize("O0")))
#endif
#if (!defined(__GNUC__) && defined(__clang__))
__attribute__ ((optnone))
#endif
int IMA_Adpcm_Stream::decode_ima( s16 *target, int length )	
{
	if( !wave_loop && ( currentblock >= wave_end ))
		return 1;
	while(length)
	{
		switch( state )
		{
			case state_beginblock:
			{
				getblock();
				if( !wave_loop ) {
					if( position >= loop2 )
					{
						int i = length * channels;
						for( ; i; i-- ) {
							*target++ = 0;
						}
						length = 0;
						break;
					}
				}
				else
				{
					if( position == loop1 ) capture_frame();
					if( position >= loop2 ) restore_frame();
				}

				data.samp1 = (short int)get16();
				data.step1 = (short int)get16();

				*target++ = data.samp1;
				if( channels == 2 )
				{
					data.samp2 = (short int)get16();
					data.step2 = (short int)get16();
					*target++ = data.samp2;
				}
									
				blockremain -= 8;
				state = state_decode;
				
				length--;
				position++;
				
				if( position == loop1 ) capture_frame();
				if( position == loop2 ) restore_frame();
				
				break;
			}
			case state_decode:
			{
				int iterations;
				
				iterations = (length > blockremain ? blockremain : length);
				
				if( position < loop1 )
				{
					if( position + iterations >= loop1 )
						iterations = loop1-position;
				}
				else if( position < loop2 )
				{
					if( position + iterations >= loop2 )
						iterations = loop2-position;
				}
				
				if( channels == 2 )
				{
					src = srcb + data.curSamps;
					decode_stereo_ima_adpcm( &data, src, target, iterations );
					srcb += iterations;
					position += iterations;
				}
				else
				{
					src = srcb + ( data.curSamps >> 1 );
					decode_mono_ima_adpcm( &data, src, target, iterations );
					srcb += ( iterations >> 1 );
					position += ( iterations >> 1 );

					if( iterations & 0x1 )
					{
							oddnibble = !oddnibble;
							if( oddnibble )
							{
								srcb++;
								position++;
							}
					}
				}
				
				length -= iterations;
				blockremain -= iterations;
				target += ( iterations * channels );
				
				if( position == loop1 ) capture_frame();
				if( position == loop2 )
				{
					restore_frame();
					break;
				}
				
				if( blockremain == 0 )
					state = state_beginblock;
				break;
			}
		}
	}
	return 0;
}

#if (defined(__GNUC__) && !defined(__clang__))
__attribute__((optimize("O0")))
#endif
#if (!defined(__GNUC__) && defined(__clang__))
__attribute__ ((optnone))
#endif
void IMA_Adpcm_Stream::close() 	{
	pf_lseek(0);
}

#if (defined(__GNUC__) && !defined(__clang__))
__attribute__((optimize("O0")))
#endif
#if (!defined(__GNUC__) && defined(__clang__))
__attribute__ ((optnone))
#endif
void IMA_Adpcm_Stream::capture_frame()	
{
	loop_data = data;
	loop_src = srcb;
	loop_oddnibble = oddnibble;
	loop_state = state;
	loop_br = blockremain;
	loop_cblock = currentblock;
}

#if (defined(__GNUC__) && !defined(__clang__))
__attribute__((optimize("O0")))
#endif
#if (!defined(__GNUC__) && defined(__clang__))
__attribute__ ((optnone))
#endif
void IMA_Adpcm_Stream::restore_frame()	
{
	seek( loop_cblock );
	getblock();
	data = loop_data;
	srcb = loop_src;
	oddnibble = loop_oddnibble;
	state = loop_state;
	blockremain = loop_br;
	position = loop1;
}

#if (defined(__GNUC__) && !defined(__clang__))
__attribute__((optimize("O0")))
#endif
#if (!defined(__GNUC__) && defined(__clang__))
__attribute__ ((optnone))
#endif
int IMA_Adpcm_Stream::fget8() 	{
	u8 a[1];
	UINT nbytes_read;
	pf_read(a, 1, &nbytes_read);
	return a[0];
}

#if (defined(__GNUC__) && !defined(__clang__))
__attribute__((optimize("O0")))
#endif
#if (!defined(__GNUC__) && defined(__clang__))
__attribute__ ((optnone))
#endif
int IMA_Adpcm_Stream::fget16() 	{
	return fget8() + (fget8() << 8);
}

#if (defined(__GNUC__) && !defined(__clang__))
__attribute__((optimize("O0")))
#endif
#if (!defined(__GNUC__) && defined(__clang__))
__attribute__ ((optnone))
#endif
u32 IMA_Adpcm_Stream::fget32() 	{
	return fget16() | (fget16() << 16);
}

#if (defined(__GNUC__) && !defined(__clang__))
__attribute__((optimize("O0")))
#endif
#if (!defined(__GNUC__) && defined(__clang__))
__attribute__ ((optnone))
#endif
int IMA_Adpcm_Stream::get8() 	{
	return *srcb++;
}

#if (defined(__GNUC__) && !defined(__clang__))
__attribute__((optimize("O0")))
#endif
#if (!defined(__GNUC__) && defined(__clang__))
__attribute__ ((optnone))
#endif
int IMA_Adpcm_Stream::get16()		{
	return get8() | (get8() << 8);
}

#if (defined(__GNUC__) && !defined(__clang__))
__attribute__((optimize("O0")))
#endif
#if (!defined(__GNUC__) && defined(__clang__))
__attribute__ ((optnone))
#endif
u32 IMA_Adpcm_Stream::get32()		{
	return get16() | (get16() << 16);
}

#if (defined(__GNUC__) && !defined(__clang__))
__attribute__((optimize("O0")))
#endif
#if (!defined(__GNUC__) && defined(__clang__))
__attribute__ ((optnone))
#endif
void IMA_Adpcm_Stream::getblock()	
{
	currentblock = tell();
	blockremain = block << ( 2 - channels );
	UINT nbytes_read;
	pf_read(datacache, block, &nbytes_read);
	srcb = datacache;
}

#if (defined(__GNUC__) && !defined(__clang__))
__attribute__((optimize("O0")))
#endif
#if (!defined(__GNUC__) && defined(__clang__))
__attribute__ ((optnone))
#endif
int IMA_Adpcm_Stream::get_channels()		{
	return channels;
}

#if (defined(__GNUC__) && !defined(__clang__))
__attribute__((optimize("O0")))
#endif
#if (!defined(__GNUC__) && defined(__clang__))
__attribute__ ((optnone))
#endif
int IMA_Adpcm_Stream::get_sampling_rate()		{
	return sampling_rate;
}

#if (defined(__GNUC__) && !defined(__clang__))
__attribute__((optimize("O0")))
#endif
#if (!defined(__GNUC__) && defined(__clang__))
__attribute__ ((optnone))
#endif
int IMA_Adpcm_Stream::get_format()		{
	return (( format == WAV_FORMAT_PCM ? (( sampBits >> 4 ) << 1 ) : WAV_FORMAT_PCM ) + ( channels - 1 ));
}

/*********************************************
 *
 * [PLAYER: These are the implementation, not the INSTANCE. It must be instanced first from PLAYER object implementation]
 *
 *********************************************/

#if (defined(__GNUC__) && !defined(__clang__))
__attribute__((optimize("O0")))
#endif
#if (!defined(__GNUC__) && defined(__clang__))
__attribute__ ((optnone))
#endif
int on_stream_request( int length, void* dest, int format )		{
	return player.i_stream_request( length, dest, format );
}

#if (defined(__GNUC__) && !defined(__clang__))
__attribute__((optimize("O0")))
#endif
#if (!defined(__GNUC__) && defined(__clang__))
__attribute__ ((optnone))
#endif
IMA_Adpcm_Player::IMA_Adpcm_Player()		{
	active=false;
}

#if (defined(__GNUC__) && !defined(__clang__))
__attribute__((optimize("O0")))
#endif
#if (!defined(__GNUC__) && defined(__clang__))
__attribute__ ((optnone))
#endif
int IMA_Adpcm_Player::play(bool loop_audio, bool automatic_updates, int buffer_length, closeSoundHandle closeHandle )		{
	active = false;
	autofill = automatic_updates;
	int result = stream.reset(loop_audio );
	if( result ){
		//strcpy((char*)0x02000000, "reset fail!");
		return result;
	}
	else{
		//strcpy((char*)0x02000000, "reset OK!"); //so far ok
	}
	
	stream.closeCb = closeHandle;
	paused = false;
	active=true;
	setvolume( 4 );
	// open stream
	
	// IMA-ADPCM file
	int fsize = pf_size();
	ADPCMchunksize = buffer_length;
	soundData.channels = headerChunk.wChannels = ADPCMchannels = stream.get_channels();
	
	headerChunk.dwSamplesPerSec = stream.get_sampling_rate();
	headerChunk.wFormatTag = 1;
	headerChunk.wBitsPerSample = 16;	//Always signed 16 bit PCM out
	
	soundData.len = fsize;
	soundData.loc = 0;
	soundData.dataOffset = pf_tell();
	multRate = 1;
	sndRate = headerChunk.dwSamplesPerSec;
	sampleLen = ADPCMchunksize;
	soundData.sourceFmt = SRC_WAV;
	
	//ARM7 sound code
	setupSoundTGDSVideoPlayerARM7();
	IMAADPCMDecode((s16 *)strpcmL0,(s16 *)strpcmR0);
	
	strpcmL0 = (s16*)lBufferARM7;	//VRAM_D;
	strpcmL1 = (s16*)lBufferARM7 + (((ADPCM_SIZE*2) >> 1) );	//strpcmL0 + (size >> 1);
	strpcmR0 = (s16*)rBufferARM7;	//strpcmL1 + (size >> 1);
	strpcmR1 = (s16*)rBufferARM7 + (((ADPCM_SIZE*2) >> 1) );	//strpcmR0 + (size >> 1);
	
	return 0;
}

#if (defined(__GNUC__) && !defined(__clang__))
__attribute__((optimize("O0")))
#endif
#if (!defined(__GNUC__) && defined(__clang__))
__attribute__ ((optnone))
#endif
void IMA_Adpcm_Player::pause()		{
	if( active )
		paused = true;
}

#if (defined(__GNUC__) && !defined(__clang__))
__attribute__((optimize("O0")))
#endif
#if (!defined(__GNUC__) && defined(__clang__))
__attribute__ ((optnone))
#endif
void IMA_Adpcm_Player::resume()		{
	if( active )
		paused = false;
}

#if (defined(__GNUC__) && !defined(__clang__))
__attribute__((optimize("O0")))
#endif
#if (!defined(__GNUC__) && defined(__clang__))
__attribute__ ((optnone))
#endif
void IMA_Adpcm_Player::stop()		{
	stream.close();
	active=false;
	setvolume( 0 );
	//Audio stop here....
	playerStopARM7();
}

#if (defined(__GNUC__) && !defined(__clang__))
__attribute__((optimize("O0")))
#endif
#if (!defined(__GNUC__) && defined(__clang__))
__attribute__ ((optnone))
#endif
void IMA_Adpcm_Player::setvolume( int vol )		{
	struct sIPCSharedTGDS * TGDSIPC = TGDSIPCStartAddress; 	
	TGDSIPC->soundIPC.volume = vol;
}

#if (defined(__GNUC__) && !defined(__clang__))
__attribute__((optimize("O0")))
#endif
#if (!defined(__GNUC__) && defined(__clang__))
__attribute__ ((optnone))
#endif
int IMA_Adpcm_Player::getvolume()		{
	struct sIPCSharedTGDS * TGDSIPC = TGDSIPCStartAddress; 
	return TGDSIPC->soundIPC.volume;
}

#if (defined(__GNUC__) && !defined(__clang__))
__attribute__((optimize("O0")))
#endif
#if (!defined(__GNUC__) && defined(__clang__))
__attribute__ ((optnone))
#endif
bool IMA_Adpcm_Player::isactive()		{
	return active;
}

#if (defined(__GNUC__) && !defined(__clang__))
__attribute__((optimize("O0")))
#endif
#if (!defined(__GNUC__) && defined(__clang__))
__attribute__ ((optnone))
#endif
IMA_Adpcm_Stream * IMA_Adpcm_Player::getStream()		{
	return &stream;
}

#if (defined(__GNUC__) && !defined(__clang__))
__attribute__((optimize("O0")))
#endif
#if (!defined(__GNUC__) && defined(__clang__))
__attribute__ ((optnone))
#endif
int IMA_Adpcm_Player::i_stream_request( int length, void * dest, int format )		{
	if( !paused ) {
		if( stream.stream( (s16*)dest, length ) != 1)
		{
			
		}
		else{
			if(!stream.wave_loop){ //is bool loop == disabled?
				stop();
			}
		}
		
	} else {
		s16 *d = (s16*)dest;
		int i = length * 2;
		for( ; i; i-- ) {
			*d++ = 0;
		}
	}
	return length;
}

void IMA_Adpcm_Player::update()		{
	
}

#if (defined(__GNUC__) && !defined(__clang__))
__attribute__((optimize("O0")))
#endif
#if (!defined(__GNUC__) && defined(__clang__))
__attribute__ ((optnone))
#endif
u8 adpcmWorkBuffer[ADPCM_SIZE*4];

#if (defined(__GNUC__) && !defined(__clang__))
__attribute__((optimize("O0")))
#endif
#if (!defined(__GNUC__) && defined(__clang__))
__attribute__ ((optnone))
#endif
void IMAADPCMDecode(s16 * lBuf, s16 * rBuf)	{
	s16 * tmpData = (s16 *)&adpcmWorkBuffer[0];
	player.i_stream_request(ADPCMchunksize, (void*)&tmpData[0], WAV_FORMAT_IMA_ADPCM);
	if(soundData.channels == 2)
	{
		uint i=0;
		for(i=0;i<(ADPCMchunksize);++i)
		{					
			lBuf[i] = (s16)checkClipping((int)tmpData[i << 1]);
			rBuf[i] = (s16)checkClipping((int)tmpData[(i << 1) | 1]);
		}
	}
	else
	{
		uint i=0;
		for(i=0;i<(ADPCMchunksize);++i)
		{
			lBuf[i] = (s16)checkClipping((int)tmpData[i]);
			rBuf[i] = (s16)checkClipping((int)tmpData[i]);
		}
	}
}

#if (defined(__GNUC__) && !defined(__clang__))
__attribute__((optimize("O0")))
#endif
#if (!defined(__GNUC__) && defined(__clang__))
__attribute__ ((optnone))
#endif
void setupSoundTGDSVideoPlayerARM7() {
	//Init SoundSampleContext
	initSound();

	sndCursor = 1;
	multRate = 1;
	
	//mallocData(sampleLen * 2 * multRate);
    
	TIMERXDATA(1) = TIMER_FREQ((sndRate));
	TIMERXCNT(1) = TIMER_DIV_1 | TIMER_ENABLE;
  
	TIMERXDATA(2) =  (0x10000) - (sampleLen);
	TIMERXCNT(2) = TIMER_CASCADE | TIMER_IRQ_REQ | TIMER_ENABLE;
	
	//irqSet(IRQ_TIMER1, TIMER1Handler);
	int ch;
	
	for(ch=0;ch<4;++ch)
	{
		SCHANNEL_CR(ch) = 0;
		SCHANNEL_TIMER(ch) = SOUND_FREQ((sndRate * multRate));
		SCHANNEL_LENGTH(ch) = (sampleLen * multRate)>>1;
		SCHANNEL_REPEAT_POINT(ch) = 0;
	}

	//irqSet(IRQ_VBLANK, 0);
	//irqDisable(IRQ_VBLANK);
	REG_IE |= IRQ_TIMER2;
	
	lastL = 0;
	lastR = 0;
}

#if (defined(__GNUC__) && !defined(__clang__))
__attribute__((optimize("O0")))
#endif
#if (!defined(__GNUC__) && defined(__clang__))
__attribute__ ((optnone))
#endif
void timerAudioCallback(){
	
	s16 *bufL, *bufR;
		
		if(sndCursor == 1){
			
			memcpy((void *)strpcmL1, (const void *)strpcmL0 , ADPCM_SIZE>>1); 
			memcpy((void *)strpcmR1, (const void *)strpcmR0 , ADPCM_SIZE>>1);

			IMAADPCMDecode((s16 *)strpcmL1,(s16 *)strpcmR1);
			
			bufL = strpcmL1;
			bufR = strpcmR1;
		}
		else{
			
			memcpy((void *)strpcmL0, (const void *)strpcmL1 , ADPCM_SIZE>>2); 
			memcpy((void *)strpcmR0, (const void *)strpcmR1 , ADPCM_SIZE>>2); 

			IMAADPCMDecode((s16 *)strpcmL0,(s16 *)strpcmR0);
			
			bufL = strpcmL0;
			bufR = strpcmR0;

		}
		
		
		// Left channel
		SCHANNEL_SOURCE((sndCursor << 1)) = (uint32)bufL;
		SCHANNEL_CR((sndCursor << 1)) = SCHANNEL_ENABLE | SOUND_ONE_SHOT | SOUND_VOL(0x7F) | SOUND_PAN(0) | SOUND_16BIT;
		
		// Right channel
		SCHANNEL_SOURCE((sndCursor << 1) + 1) = (uint32)bufR;
		SCHANNEL_CR((sndCursor << 1) + 1) = SCHANNEL_ENABLE | SOUND_ONE_SHOT | SOUND_VOL(0x7F) | SOUND_PAN(0x3FF) | SOUND_16BIT;
		
		sndCursor = 1 - sndCursor;
}