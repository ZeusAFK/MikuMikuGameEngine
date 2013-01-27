#include "stdafx.h"

#include "TextureAtlas.h"

#include "Graphics.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

TextureAtlas::TextureAtlas()
{
}

TextureAtlas::~TextureAtlas()
{
	std::map<tstring_symbol,sChipInfo*>::iterator it = m_chipList.begin();
	while( it!=m_chipList.end() )
	{
		delete it->second;
		++it;
	}
	m_chipList.clear();
}

bool TextureAtlas::Load( IXmlReaderPtr reader )
{
	HRESULT hr;
	LPCWSTR pwszLocalName;
	XmlNodeType nodeType;
    while( S_OK == reader->Read(&nodeType) )
	{
		switch(nodeType)
		{
			case XmlNodeType_Element:
				{
					hr = reader->GetLocalName( &pwszLocalName,NULL );
					if( FAILED(hr) )
					{
						return false;
					}

					tstring elementName = to_tstring( pwszLocalName );

					tstring debugText;
					tstring_format( debugText,_T("element %s\n"),elementName.c_str() );
					::OutputDebugString( debugText.c_str() );

					if( elementName==_T("chiplist") )
					{
						while( S_OK == reader->MoveToNextAttribute() )
						{
							LPCWSTR pwszAttributeName;
							LPCWSTR pwszAttributeValue;

							hr = reader->GetLocalName(&pwszAttributeName, NULL);
							if( FAILED(hr) )
							{
								return false;
							}
							hr = reader->GetValue(&pwszAttributeValue, NULL);
							if( FAILED(hr) )
							{
								return false;
							}

							tstring attributeName = to_tstring( pwszAttributeName );
							tstring value = to_tstring( pwszAttributeValue );

							tstring debugText;
							tstring_format( debugText,_T("attribute %s=%s\n"),attributeName.c_str(),value.c_str() );
							::OutputDebugString( debugText.c_str() );

							if( attributeName==_T("version") )
							{
							}
						}
					}
					else if( elementName == _T("chip") )
					{
						tstring chipName;

						sChipInfo* chipInfo = new sChipInfo;
						ZeroMemory( chipInfo,sizeof(sChipInfo) );

						while( S_OK == reader->MoveToNextAttribute() )
						{
							LPCWSTR pwszAttributeName;
							LPCWSTR pwszAttributeValue;

							hr = reader->GetLocalName(&pwszAttributeName, NULL);
							if( FAILED(hr) )
							{
								delete chipInfo;
								return false;
							}
							hr = reader->GetValue(&pwszAttributeValue, NULL);
							if( FAILED(hr) )
							{
								delete chipInfo;
								return false;
							}

							tstring attributeName = to_tstring( pwszAttributeName );
							tstring value = to_tstring( pwszAttributeValue );

							tstring debugText;
							tstring_format( debugText,_T("attribute %s=%s\n"),attributeName.c_str(),value.c_str() );
							::OutputDebugString( debugText.c_str() );

							if( attributeName==_T("id") )
							{
								chipName = value;
							}
							else if( attributeName==_T("x") )
							{
								chipInfo->x = _tcstol( value.c_str(),NULL,10 );
							}
							else if( attributeName==_T("y") )
							{
								chipInfo->y = _tcstol( value.c_str(),NULL,10 );
							}
							else if( attributeName==_T("width") )
							{
								chipInfo->width = _tcstol( value.c_str(),NULL,10 );
							}
							else if( attributeName==_T("height") )
							{
								chipInfo->height = _tcstol( value.c_str(),NULL,10 );
							}
							else if( attributeName==_T("cx") )
							{
								chipInfo->cx = _tcstol( value.c_str(),NULL,10 );
							}
							else if( attributeName==_T("cy") )
							{
								chipInfo->cy = _tcstol( value.c_str(),NULL,10 );
							}
						}

						m_chipList.insert( std::map<tstring_symbol,sChipInfo*>::value_type(tstring_symbol(chipName),chipInfo) );
					}
				}
				break;
			case XmlNodeType_EndElement:
				{
					hr = reader->GetLocalName( &pwszLocalName,NULL );
					if( FAILED(hr) )
					{
						return false;
					}

					tstring elementName = to_tstring( pwszLocalName );

					tstring debugText;
					tstring_format( debugText,_T("end element %s\n"),elementName.c_str() );
					::OutputDebugString( debugText.c_str() );
				}
				break;
		}
	}

	return true;
}

bool TextureAtlas::CreateFromFile( const tstring& filePath,TexturePtr texture )
{
	HRESULT hr;
	IXmlReaderPtr reader;
	IStreamPtr stream;

	m_texture = texture;

	hr = CreateXmlReader( IID_IXmlReader, reinterpret_cast<void**>(&reader), 0);
	if( FAILED(hr) )
	{
        return false;
    }

	hr = SHCreateStreamOnFile( filePath.c_str(), STGM_READ, &stream );
	if( FAILED(hr) )
	{
		return false;
	}

	hr = reader->SetInput(stream);
	if( FAILED(hr) )
	{
		return false;
	}

	return Load( reader );
}

bool TextureAtlas::CreateFromMemory( const BYTE* data,DWORD size,TexturePtr texture )
{
	HRESULT hr;
	IXmlReaderPtr reader;
	IStreamPtr stream;

	m_texture = texture;

	hr = CreateXmlReader( IID_IXmlReader, reinterpret_cast<void**>(&reader), 0);
	if( FAILED(hr) )
	{
        return false;
    }

	hr = ::CreateStreamOnHGlobal( NULL,TRUE,&stream );
	if( FAILED(hr) )
	{
		return false;
	}

	hr = stream->Write( data,size,NULL );
	if( FAILED(hr) )
	{
		return false;
	}

	LARGE_INTEGER dlibMove;
	dlibMove.QuadPart = 0;

	hr = stream->Seek( dlibMove,STREAM_SEEK_SET,NULL );
	if( FAILED(hr) )
	{
		return false;
	}

	hr = reader->SetInput(stream);
	if( FAILED(hr) )
	{
		return false;
	}

	return Load( reader );
}

bool TextureAtlas::CreateFromResource( int resourceID,TexturePtr texture )
{
	HRSRC hRsrc = ::FindResource( NULL,MAKEINTRESOURCE( resourceID ),RT_RCDATA );
	if( hRsrc==NULL )
	{
		return false;
	}

	HANDLE hRes = ::LoadResource( NULL,hRsrc );
	if( hRes==NULL )
	{
		return false;
	}

	BYTE* data = (BYTE*)::LockResource( hRes );
	if( data==NULL )
	{
		return false;
	}
	DWORD size = SizeofResource( NULL,hRsrc );
	if( size==0 )
	{
		return false;
	}

	// TODO:Create From Memory

	return CreateFromMemory( data,size,texture );
}

const sChipInfo* TextureAtlas::GetChipInfo( const tstring_symbol& name )
{
	std::map< tstring_symbol,sChipInfo* >::iterator it = m_chipList.find( name );
	if( it!=m_chipList.end() )
	{
		return it->second;
	}
	return NULL;
}

TexturePtr TextureAtlas::GetTexture()
{
	return m_texture;
}
