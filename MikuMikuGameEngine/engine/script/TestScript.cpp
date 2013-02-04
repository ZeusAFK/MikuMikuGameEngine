#include "stdafx.h"

#include "../core/util/util.h"

#include "squirrel/squirrel.h"
#include "squirrel/sqstdaux.h"
#include "squirrel/sqstdio.h"
#include "squirrel/sqstdblob.h"
#include "squirrel/sqstdmath.h"
#include "squirrel/sqstdstring.h"

#include "ScriptBehavior.h"

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
						sq_pushstring( sqvm,_SC("type"),-1 );

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

										bool serializable = false;

										sq_pushobject(sqvm, classObject);

										sq_pushstring( sqvm,key,-1 );
										if( SQ_SUCCEEDED( sq_getattributes(sqvm,-2 ) ) )
										{
											sq_pushstring( sqvm,_SC("serializable"),-1 );
											if( SQ_SUCCEEDED( sq_get(sqvm,-2 ) ) )
											{
												SQBool value;

												if( SQ_SUCCEEDED( sq_getbool( sqvm,-1,&value) ) )
												{
													serializable = value!=SQFalse;
												}

												sq_pop(sqvm,1);
											}
											
											if( serializable )
											{
												tstring serializeType;

												sq_pushstring( sqvm,_SC("type"),-1 );
												if( SQ_SUCCEEDED( sq_get(sqvm,-2 ) ) )
												{
													const SQChar* value;

													if( SQ_SUCCEEDED( sq_getstring( sqvm,-1,&value) ) )
													{
														serializeType = value;

														OutputDebugStringFormat( _T("serializable %s %s\n"),value,key );

														if( serializeType==_T("int") )
														{
															ScriptParameterInteger parameter;
															parameter.SetName( tstring(key) );
															parameter.SetValue( sq_objtointeger(&val) );
														}
													}

													sq_pop(sqvm,1);
												}
											}
										}

										sq_settop(sqvm,top);
									}
								}

								sq_pop( sqvm,2 );
							}

							sq_settop( sqvm,top );
						}

						ScriptBehavior behavior( sqvm,classObject );

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