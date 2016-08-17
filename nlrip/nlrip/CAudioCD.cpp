// Written by Michel Helms (MichelHelms@Web.de).
// Parts of code were taken from Idael Cardoso (http://www.codeproject.com/csharp/csharpripper.asp)
//   and from Larry Osterman (http://blogs.msdn.com/larryosterman/archive/2005/05.aspx).
// Finished at 26th of September in 2006
// Of course you are allowed to cut this lines off ;)

#include "stdafx.h"



#include "CAudioCD.h"
#include "AudioCD_Helpers.h"




// Constructor / Destructor
CAudioCD::CAudioCD( char Drive )
{
	m_hCD = NULL;
	if ( Drive != '\0' )
		Open( Drive );
}


CAudioCD::~CAudioCD()
{
	Close();
}




// Open / Close access
BOOL CAudioCD::Open( char Drive )
{
	Close();

	// Open drive-handle
	char Fn[8] = { '\\', '\\', '.', '\\', Drive, ':', '\0' };
	if ( INVALID_HANDLE_VALUE == ( m_hCD = CreateFile( Fn, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL ) ) )
	{
		m_hCD = NULL;
		return FALSE;
	}

	// Lock drive
	if ( ! LockCD() )
	{
		UnlockCD();
		CloseHandle( m_hCD );
		m_hCD = NULL;
		return FALSE;
	}

	// Get track-table and add it to the intern array
	ULONG BytesRead;
	CDROM_TOC Table;
	if ( 0 == DeviceIoControl( m_hCD, IOCTL_CDROM_READ_TOC, NULL, 0, &Table, sizeof(Table), &BytesRead, NULL ) )
	{
		UnlockCD();
		CloseHandle( m_hCD );
		m_hCD = NULL;
		return FALSE;
	}
	for ( ULONG i=Table.FirstTrack-1; i<Table.LastTrack; i++ )
	{
		CDTRACK NewTrack;
		NewTrack.Address = AddressToSectors( Table.TrackData[i].Address );
		NewTrack.Length = AddressToSectors( Table.TrackData[i+1].Address ) - NewTrack.Address;
		m_aTracks.push_back( NewTrack );
	}

	// Return if track-count > 0
	return m_aTracks.size() > 0;
}


BOOL CAudioCD::IsOpened()
{
	return m_hCD != NULL;
}


void CAudioCD::Close()
{
	UnlockCD();
	m_aTracks.clear();
	CloseHandle( m_hCD );
	m_hCD = NULL;
}




// Read / Get track-data
ULONG CAudioCD::GetTrackCount()
{
	if ( m_hCD == NULL )
		return 0xFFFFFFFF;
	return m_aTracks.size();
}


ULONG CAudioCD::GetTrackTime( ULONG Track )
{
	if ( m_hCD == NULL )
		return 0xFFFFFFFF;
	if ( Track >= m_aTracks.size() )
		return 0xFFFFFFFF;

	CDTRACK& Tr = m_aTracks.at(Track);
	return Tr.Length / 75;
}


ULONG CAudioCD::GetTrackSize( ULONG Track )
{
	if ( m_hCD == NULL )
		return 0xFFFFFFFF;
	if ( Track >= m_aTracks.size() )
		return 0xFFFFFFFF;

	CDTRACK& Tr = m_aTracks.at(Track);
	return Tr.Length * RAW_SECTOR_SIZE;
}


BOOL CAudioCD::ReadTrack( ULONG TrackNr, CBuf<char>* pBuf )
{
	ULONG i;
	if ( m_hCD == NULL )
		return FALSE;

	if ( TrackNr >= m_aTracks.size() )
		return FALSE;
	CDTRACK& Track = m_aTracks.at(TrackNr);

	pBuf->Alloc( Track.Length*RAW_SECTOR_SIZE );

	RAW_READ_INFO Info;
	Info.TrackMode = CDDA;
	Info.SectorCount = SECTORS_AT_READ;

	for ( i=0; i<Track.Length/SECTORS_AT_READ; i++ )
	{
		Info.DiskOffset.QuadPart = (Track.Address + i*SECTORS_AT_READ) * CD_SECTOR_SIZE;
		ULONG Dummy;
		if ( 0 == DeviceIoControl( m_hCD, IOCTL_CDROM_RAW_READ, &Info, sizeof(Info), pBuf->Ptr()+i*SECTORS_AT_READ*RAW_SECTOR_SIZE, SECTORS_AT_READ*RAW_SECTOR_SIZE, &Dummy, NULL ) )
		{
			pBuf->Free();
			return FALSE;
		}
	}

	Info.SectorCount = Track.Length % SECTORS_AT_READ;
	Info.DiskOffset.QuadPart = (Track.Address + i*SECTORS_AT_READ) * CD_SECTOR_SIZE;
	ULONG Dummy;
	if ( 0 == DeviceIoControl( m_hCD, IOCTL_CDROM_RAW_READ, &Info, sizeof(Info), pBuf->Ptr()+i*SECTORS_AT_READ*RAW_SECTOR_SIZE, SECTORS_AT_READ*RAW_SECTOR_SIZE, &Dummy, NULL ) )
	{
		pBuf->Free();
		return FALSE;
	}

	return TRUE;
}


char* stereoSamples = NULL;

ULONG averageChannels( char * stereoSamples, ULONG stereoByteLength  )
{
	ULONG stereoShortLength = stereoByteLength / 2;
	//printf("stereoByteLength %d, stereoShortLength %d", stereoByteLength, stereoShortLength);

	short* shrtBuf = (short *)stereoSamples;
	ULONG shrtBufIndex = 0;
	ULONG leftBufIndex = 0;
	ULONG rightBufIndex = 1;
	for (leftBufIndex = 0; leftBufIndex < stereoShortLength; shrtBufIndex += 1, leftBufIndex += 2, rightBufIndex += 2)
	{
		shrtBuf[shrtBufIndex] = (shrtBuf[leftBufIndex] /2) + (shrtBuf[rightBufIndex] /2);
	}
	return shrtBufIndex * sizeof(unsigned short);
}

BOOL CAudioCD::ExtractAllTracks( LPCTSTR Path)
{
	ULONG i;
	if (m_hCD == NULL)
		return FALSE;

	ULONG Dummy;

	HANDLE hFile = CreateFile(Path, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return FALSE;

	ULONG TotalLength = 0;
	for (ULONG track = 0; track < m_aTracks.size(); ++track)
	{
		TotalLength += m_aTracks.at(track).Length;
	}

	CWaveFileHeader WaveFileHeader(44100, 16, 1, TotalLength*RAW_SECTOR_SIZE/2);
	WriteFile(hFile, &WaveFileHeader, sizeof(WaveFileHeader), &Dummy, NULL);


	for (ULONG track = 0; track < m_aTracks.size(); ++track )
	{  
		 
		CDTRACK& Track = m_aTracks.at(track);
		ULONG trackTime = Track.Length / 75;
		ULONG trackSize = Track.Length * RAW_SECTOR_SIZE;

		stereoSamples = (char*) malloc(trackSize);
		ULONG stereoIndex = 0;


		printf("Track %i: %i:%.2i;  %i bytes of size\n", track + 1, trackTime / 60, trackTime % 60, trackSize);


		CBuf<char> Buf(SECTORS_AT_READ * RAW_SECTOR_SIZE);

		RAW_READ_INFO Info;
		Info.TrackMode = CDDA;
		Info.SectorCount = SECTORS_AT_READ;

		for (i = 0; i < Track.Length / SECTORS_AT_READ; i++)
		{
			Info.DiskOffset.QuadPart = (Track.Address + i*SECTORS_AT_READ) * CD_SECTOR_SIZE;
			if (DeviceIoControl(m_hCD, IOCTL_CDROM_RAW_READ, &Info, sizeof(Info), Buf, SECTORS_AT_READ*RAW_SECTOR_SIZE, &Dummy, NULL))
			{
				memcpy(&stereoSamples[stereoIndex], Buf, Dummy);
				stereoIndex += Dummy;
				//WriteFile(hFile, Buf, Buf.Size()/2, &Dummy, NULL);
			}
		}

		Info.SectorCount = Track.Length % SECTORS_AT_READ;
		Info.DiskOffset.QuadPart = (Track.Address + i*SECTORS_AT_READ) * CD_SECTOR_SIZE;
		if (DeviceIoControl(m_hCD, IOCTL_CDROM_RAW_READ, &Info, sizeof(Info), Buf, Info.SectorCount*RAW_SECTOR_SIZE, &Dummy, NULL))
		{
			memcpy(&stereoSamples[stereoIndex], Buf, Dummy);
			stereoIndex += Dummy; 
			//WriteFile(hFile, Buf, Info.SectorCount*RAW_SECTOR_SIZE  / 2, &Dummy, NULL);
		}
		ULONG newLength = averageChannels(stereoSamples, stereoIndex);
		WriteFile(hFile, stereoSamples, newLength,  &Dummy, NULL);
		free(stereoSamples);
	}
	return CloseHandle(hFile);
}


// Lock / Unlock CD-Rom Drive
BOOL CAudioCD::LockCD()
{
	if ( m_hCD == NULL )
		return FALSE;
	ULONG Dummy;
	PREVENT_MEDIA_REMOVAL pmr = { TRUE };
	return 0 != DeviceIoControl( m_hCD, IOCTL_STORAGE_MEDIA_REMOVAL, &pmr, sizeof(pmr), NULL, 0, &Dummy, NULL );
}


BOOL CAudioCD::UnlockCD()
{
	if ( m_hCD == NULL )
		return FALSE;
	ULONG Dummy;
	PREVENT_MEDIA_REMOVAL pmr = { FALSE };
	return 0 != DeviceIoControl( m_hCD, IOCTL_STORAGE_MEDIA_REMOVAL, &pmr, sizeof(pmr), NULL, 0, &Dummy, NULL );
}




// General operations
BOOL CAudioCD::InjectCD()
{
	if ( m_hCD == NULL )
		return FALSE;
	ULONG Dummy;
	return 0 != DeviceIoControl( m_hCD, IOCTL_STORAGE_LOAD_MEDIA, NULL, 0, NULL, 0, &Dummy, NULL );
}


BOOL CAudioCD::IsCDReady( char Drive )
{
	HANDLE hDrive;
	if ( Drive != '\0' )
	{
		// Open drive-handle if a drive is specified
		char Fn[8] = { '\\', '\\', '.', '\\', Drive, ':', '\0' };
		if ( INVALID_HANDLE_VALUE == ( hDrive = CreateFile( Fn, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL ) ) )
			return FALSE;
	}
	else
	{
		// Otherwise, take our open handle
		if ( m_hCD == NULL )
			return FALSE;
		hDrive = m_hCD;
	}

	ULONG Dummy;
	BOOL Success = DeviceIoControl( hDrive, IOCTL_STORAGE_CHECK_VERIFY2, NULL, 0, NULL, 0, &Dummy, NULL );

	if ( m_hCD != hDrive )
		CloseHandle( hDrive );

	return Success;
}


BOOL CAudioCD::EjectCD()
{
	if ( m_hCD == NULL )
		return FALSE;
	ULONG Dummy;
	BOOL rc;
	rc &= DeviceIoControl( m_hCD, IOCTL_STORAGE_EJECT_MEDIA, NULL, 0, NULL, 0, &Dummy, NULL );
	return rc;
}