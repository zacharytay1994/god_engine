#include "../pch.h"
#include "WindowsDialog.h"

#include "FolderHelper.h"

#include <vector>

namespace god
{
	namespace WindowsDialog
	{
		std::string DialogBoxToDirectory ( wchar_t const* fileType , wchar_t const* fileExt ,
			MODE mode , const char* dir )
		{
			HRESULT hr = CoInitializeEx ( NULL , COINIT_APARTMENTTHREADED |
				COINIT_DISABLE_OLE1DDE );

			std::wstringstream ss;
			if ( SUCCEEDED ( hr ) )
			{
				COMDLG_FILTERSPEC ComDlgFS[ 1 ] = { {fileType, fileExt} };
				IFileOpenDialog* p_dialog;

				hr = CoCreateInstance ( CLSID_FileOpenDialog , NULL , CLSCTX_ALL ,
					IID_IFileOpenDialog , reinterpret_cast< void** >( &p_dialog ) );

				p_dialog->SetFileTypes ( 1 , ComDlgFS );
				p_dialog->SetTitle ( L"god Window Dialog" );

				if ( SUCCEEDED ( hr ) )
				{
					// Show the Open dialog box.
					hr = p_dialog->Show ( NULL );

					// Get the file name from the dialog box.
					if ( SUCCEEDED ( hr ) )
					{
						IShellItem* pItem;
						hr = p_dialog->GetResult ( &pItem );
						if ( SUCCEEDED ( hr ) )
						{
							PWSTR pszFilePath;
							hr = pItem->GetDisplayName ( SIGDN_FILESYSPATH , &pszFilePath );

							// Display the file name to the user.
							if ( SUCCEEDED ( hr ) )
							{
								ss << pszFilePath;
								switch ( mode )
								{
								case ( MODE::ASSETS ):
									FolderHelper::CopyFileToAssets ( ss.str ().c_str () , dir );
									break;
								}
							}
							pItem->Release ();
						}
					}
					p_dialog->Release ();
				}
				CoUninitialize ();
			}

			std::wstring file { ss.str ().c_str () };
			std::string sfile;
			std::transform ( file.begin () , file.end () , std::back_inserter ( sfile ) ,
				[]( wchar_t c )
				{
					return ( char ) c;
				}
			);
			return sfile;
		}

		std::vector<std::string> DialogBoxToDirectoryMulti ( wchar_t const* fileType , wchar_t const* fileExt ,
			MODE mode , const char* dir )
		{
			HRESULT hr = CoInitializeEx ( NULL , COINIT_APARTMENTTHREADED |
				COINIT_DISABLE_OLE1DDE );

			std::wstringstream ss;
			std::vector<std::string> out;
			if ( SUCCEEDED ( hr ) )
			{
				COMDLG_FILTERSPEC ComDlgFS[ 1 ] = { {fileType, fileExt} };
				IFileOpenDialog* p_dialog;

				hr = CoCreateInstance ( CLSID_FileOpenDialog , NULL , CLSCTX_ALL ,
					IID_IFileOpenDialog , reinterpret_cast< void** >( &p_dialog ) );

				p_dialog->SetFileTypes ( 1 , ComDlgFS );
				p_dialog->SetTitle ( L"god Window Dialog" );
				p_dialog->SetOptions ( FOS_ALLOWMULTISELECT );

				if ( SUCCEEDED ( hr ) )
				{
					// Show the Open dialog box.
					hr = p_dialog->Show ( NULL );

					// Get the file name from the dialog box.
					if ( SUCCEEDED ( hr ) )
					{
						IShellItemArray* pItemArray;
						hr = p_dialog->GetResults ( &pItemArray );
						if ( SUCCEEDED ( hr ) )
						{
							uint32_t count { 0 };
							pItemArray->GetCount ( reinterpret_cast< DWORD* >( &count ) );
							out.resize ( count );
							for ( uint32_t i = 0; i < count; ++i )
							{
								IShellItem* pItem;
								pItemArray->GetItemAt ( i , &pItem );

								PWSTR pszFilePath;
								hr = pItem->GetDisplayName ( SIGDN_FILESYSPATH , &pszFilePath );

								// Display the file name to the user.
								if ( SUCCEEDED ( hr ) )
								{
									ss << pszFilePath;
									switch ( mode )
									{
									case ( MODE::ASSETS ):
										FolderHelper::CopyFileToAssets ( ss.str ().c_str () , dir );
										break;
									}
								}
								pItem->Release ();

								// convert to string
								std::wstring file { ss.str ().c_str () };
								std::string sfile;
								std::transform ( file.begin () , file.end () , std::back_inserter ( sfile ) ,
									[]( wchar_t c )
									{
										return ( char ) c;
									}
								);
								//std::string sfile ( file.begin () , file.end () );
								out[ i ] = sfile;
								ss.str ( L"" );
							}
						}
					}
					p_dialog->Release ();
				}
				CoUninitialize ();
			}

			return out;
		}
	}
}