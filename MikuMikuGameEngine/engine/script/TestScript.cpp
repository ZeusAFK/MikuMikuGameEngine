#include "stdafx.h"

#include "../core/util/util.h"

#include "squirrel/squirrel.h"
#include "squirrel/sqstdaux.h"
#include "squirrel/sqstdio.h"
#include "squirrel/sqstdblob.h"
#include "squirrel/sqstdmath.h"
#include "squirrel/sqstdstring.h"

#include "ScriptBehavior.h"

#include <map>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

void PrintFunc( HSQUIRRELVM vm,const SQChar* format,... )
{
	va_list va;
	va_start( va,format );

	tstring tszDebug;
	tstring_format_va( tszDebug,format,va );

	va_end( va );

	tszDebug+=_T("\n");

	OutputDebugString( tszDebug.c_str() );
}

void TestScript()
{
	tstring fileName = _T("project\\assets\\Script\\test.nut");

	FILE* fp=NULL;
	if( _tfopen_s(&fp,fileName.c_str(),_T("r"))!=0 )
	{
		OutputDebugStringFormat( _T("スクリプトファイル(%s)が開けませんでした\n"),fileName.c_str() );
		return;
	}

	fpos_t fsize = 0;
	fseek(fp,0,SEEK_END);
	fgetpos(fp,&fsize);

	fseek(fp,0,SEEK_SET); 

	size_t fileSize=(size_t)fsize;

	char* fileBuffer = new char[fileSize+1];
	ZeroMemory( fileBuffer,sizeof(char)*(fileSize+1) );
	fread( fileBuffer,1,sizeof(char)*fileSize,fp );

	fclose( fp );

	tstring script = to_tstring( fileBuffer );

	delete[] fileBuffer;

	HSQUIRRELVM sqvm = sq_open(1024);

	sqstd_seterrorhandlers( sqvm );

	sq_setprintfunc( sqvm,PrintFunc,PrintFunc );

	sq_pushroottable(sqvm);

	sqstd_register_iolib(sqvm);
	sqstd_register_bloblib(sqvm);
	sqstd_register_mathlib(sqvm);
	sqstd_register_stringlib(sqvm);

	sq_pop(sqvm,1);

	SQRESULT result = sq_compilebuffer( sqvm,script.c_str(),script.length(),fileName.c_str(),SQTrue );
	if( SQ_SUCCEEDED(result) )
	{
		int top = sq_gettop(sqvm);

		sq_pushroottable(sqvm);

		result = sq_call( sqvm,1,SQFalse,SQTrue );
		if( SQ_FAILED(result) )
		{
			OutputDebugStringFormat( _T("スクリプトの実行に失敗しました(%x)\n"),result );
		}

		sq_settop( sqvm,top );

		top = sq_gettop(sqvm);

		sq_pushroottable(sqvm);

		sq_pushnull(sqvm);

		while( SQ_SUCCEEDED( sq_next(sqvm,-2) ) )
		{
			const SQChar* key = NULL;
			HSQOBJECT classObject;

			if( SQ_SUCCEEDED( sq_getstring( sqvm,-2,&key) ) &&
				SQ_SUCCEEDED( sq_getstackobj(sqvm, -1, &classObject) ) )
			{
				tstring className = key;

				sq_pushobject(sqvm, classObject);

				tstring value;

				{
					const SQChar* valString;
					sq_tostring(sqvm, -1);
					sq_getstring(sqvm, -1, (const SQChar**)&valString);
					value = valString;
					sq_pop(sqvm,1);
				}

				sq_pop(sqvm, 1);

				OutputDebugStringFormat( _T("key=%s , val = %s\n"),key,value.c_str() );

				if( classObject._type==OT_CLASS )
				{
					int top = sq_gettop(sqvm);

					sq_pushobject(sqvm, classObject);

					sq_pushnull(sqvm);

					bool behaviorClass = false;
					
					if( SQ_SUCCEEDED( sq_getattributes(sqvm,-2 ) ) )
					{
						sq_pushstring( sqvm,_SC("Type"),-1 );

						if( SQ_SUCCEEDED( sq_get(sqvm,-2 ) ) )
						{
							const SQChar* value = NULL;

							if( SQ_SUCCEEDED( sq_getstring( sqvm,-1,&value) ) )
							{
								tstring typeValue = value;
								if( typeValue==_T("Behavior") )
								{
									behaviorClass = true;
								}
							}
						}
					}

					sq_settop( sqvm,top );

					if( behaviorClass )
					{
						// classのメンバー変数を列挙する
						std::map< tstring_symbol,ScriptParameterInterfacePtr > scriptParameters;
						{
							int top = sq_gettop(sqvm);

							sq_pushobject(sqvm, classObject);

							sq_pushnull(sqvm);

							while( SQ_SUCCEEDED( sq_next(sqvm,-2) ) )
							{
								const SQChar* key = NULL;
								HSQOBJECT val;

								if( SQ_SUCCEEDED( sq_getstring( sqvm,-2,&key) ) &&
									SQ_SUCCEEDED( sq_getstackobj(sqvm, -1, &val) ) )
								{
									const SQChar* valString;
									sq_tostring(sqvm, -1);
									sq_getstring(sqvm, -1, (const SQChar**)&valString);

									OutputDebugStringFormat( _T("%s::%s value=%s\n"),className.c_str(),key,valString );

									sq_pop(sqvm,1);

									if( val._type!=OT_CLOSURE )
									{
										int top = sq_gettop(sqvm);

										sq_pushobject(sqvm, classObject);

										sq_pushstring( sqvm,key,-1 );
										if( SQ_SUCCEEDED( sq_getattributes(sqvm,-2 ) ) )
										{
											sq_pushstring( sqvm,_SC("SerializeType"),-1 );
											if( SQ_SUCCEEDED( sq_get(sqvm,-2 ) ) )
											{
												const SQChar* value;

												if( SQ_SUCCEEDED( sq_getstring( sqvm,-1,&value) ) )
												{
													tstring serializeType = value;

													OutputDebugStringFormat( _T("SerializeType %s %s\n"),value,key );

													if( serializeType==_T("int") )
													{
														ScriptParameterInteger* parameter = new ScriptParameterInteger;
														tstring_symbol nameSymbol = tstring(key);
														parameter->SetName( nameSymbol );
														parameter->SetValue( sq_objtointeger(&val) );

														scriptParameters.insert( std::map< tstring_symbol,ScriptParameterInterfacePtr >::value_type( nameSymbol,ScriptParameterInterfacePtr(parameter) ) );
													}
													else if( serializeType==_T("bool") )
													{
														ScriptParameterBool* parameter = new ScriptParameterBool;
														tstring_symbol nameSymbol = tstring(key);
														parameter->SetName( nameSymbol );
														parameter->SetValue( sq_objtobool(&val)!=SQFalse?true:false );

														scriptParameters.insert( std::map< tstring_symbol,ScriptParameterInterfacePtr >::value_type( nameSymbol,ScriptParameterInterfacePtr(parameter) ) );
													}
													else if( serializeType==_T("float") )
													{
														ScriptParameterFloat* parameter = new ScriptParameterFloat;
														tstring_symbol nameSymbol = tstring(key);
														parameter->SetName( nameSymbol );
														parameter->SetValue( sq_objtofloat(&val) );

														scriptParameters.insert( std::map< tstring_symbol,ScriptParameterInterfacePtr >::value_type( nameSymbol,ScriptParameterInterfacePtr(parameter) ) );
													}
													else if( serializeType==_T("string") )
													{
														ScriptParameterString* parameter = new ScriptParameterString;
														tstring_symbol nameSymbol = tstring(key);
														parameter->SetName( nameSymbol );
														parameter->SetValue( tstring(sq_objtostring(&val)) );

														scriptParameters.insert( std::map< tstring_symbol,ScriptParameterInterfacePtr >::value_type( nameSymbol,ScriptParameterInterfacePtr(parameter) ) );
													}
												}

												sq_pop(sqvm,1);
											}
										}

										sq_settop(sqvm,top);
									}
								}

								sq_pop( sqvm,2 );
							}

							sq_settop( sqvm,top );
						}

						ScriptBehavior behavior( sqvm,classObject,scriptParameters );

						behavior.Awake();
						behavior.Start();
						behavior.Update( 0.01f );
					}
				}
			}

			sq_pop( sqvm,2 );
		}

		sq_pop( sqvm,1 );

		sq_settop( sqvm,top );
	}
	else
	{
		OutputDebugStringFormat( _T("スクリプトのコンパイルに失敗しました(%x)\n"),result );
	}

	sq_close( sqvm );
}