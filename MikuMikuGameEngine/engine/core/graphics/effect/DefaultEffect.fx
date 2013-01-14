float4x4 g_mWorldViewProjection;
float4x4 g_mWorldView;
float4x4 g_mLightViewProjection;
float4 g_materialDiffuse;
float4 g_materialSpecular;
float4 g_materialAmbient;
float4 g_materialEmissive;
float g_materialSpecularPower;

float g_texAlphaTestRef;

float3 g_eyePos;

float3 g_lightDir;
float3 g_lightColor;

///////////// 定数 ////////////////
#define SKII1	1500
#define	SKII2	8000
#define	Toon	3

texture g_Texture;
sampler TextureSampler = sampler_state
{
	texture = <g_Texture>;
    
	//フィルタ設定
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    MipFilter = LINEAR;    
    //テクスチャラッピング
    AddressU = Wrap;
    AddressV = Wrap;
};

texture g_ToonTexture;
sampler ToonSampler = sampler_state
{
	texture = <g_ToonTexture>;
    
	//フィルタ設定
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    MipFilter = LINEAR;    
    //テクスチャラッピング
    AddressU = Clamp;
    AddressV = Clamp;
};

texture g_SphereMapTexture;
sampler SphereMapSampler = sampler_state
{
	texture = <g_SphereMapTexture>;
    
	//フィルタ設定
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    MipFilter = LINEAR;    
    //テクスチャラッピング
    AddressU = Clamp;
    AddressV = Clamp;
};

struct sVS_Input
{
	float4 vPos : POSITION;
	float3 vNormal : NORMAL;
	float2 vTex : TEXCOORD0;
	float edge : TEXCOORD1;
};

struct sVS_Output
{
	float4 vPos : POSITION;
	float3 vNormal : TEXCOORD0;
	float3 vViewPos : TEXCOORD1;
	float2 vTex : TEXCOORD2;
	float4 vZCalcTex : TEXCOORD3;
};

sVS_Output VS_Scene( sVS_Input In )
{
	sVS_Output Out=(sVS_Output)0;
	Out.vPos = mul( In.vPos,g_mWorldViewProjection );
	Out.vNormal = mul( In.vNormal.xyz,g_mWorldView );
	Out.vViewPos = mul( In.vPos.xyz-g_eyePos,g_mWorldView );
	Out.vTex = In.vTex;
	// ライトの目線によるワールドビュー射影変換をする
	Out.vZCalcTex = mul( In.vPos, g_mLightViewProjection );

	return Out;
}

//=========================================================================================
// PixelShader
//=========================================================================================

float ps_Lambert(float3 normal,float3 lightDir)
{
	return max( 0,dot(normal, lightDir) );
}

float ps_HalfLambert(float3 normal,float3 lightDir)
{
	float n = ps_Lambert(normal,lightDir);
	n = n * 0.5f + 0.5f;
	n = n * n;

	return n;
}

float4 ps_LightDiffuse(float3 N,float3 lightDir)
{
	float n = ps_Lambert(N,lightDir);
	
	float4 diffuse = float4(n,n,n,1.0f );
	
	return float4(diffuse.rgb,1.0f);
}

float4 ps_LightToonDiffuse(float3 N,float3 lightDir)
{
	float LightNormal = dot( N, lightDir );

	float3 toonColor = tex2D( ToonSampler,float2(0.5f,0.75f) ).rgb;

	float3 diffuse = lerp(toonColor,float3(1.0f,1.0f,1.0),saturate(LightNormal * 16 + 0.5) );
	
	return float4(diffuse.rgb,1.0f);
}

float4 ps_Sphere( float3 N )
{
	return tex2D( SphereMapSampler,float2(N.x,N.y)*0.5f+0.5f );
}

float ps_BlinnPhong( float3 normal,float3 lightDir,float3 viewDir,float specularPower )
{
	float3 H = normalize( lightDir+viewDir );
	return pow( max(0,dot(normal,H)),specularPower );
}

struct sPS_Input
{
	float3 vNormal : TEXCOORD0;
	float3 vViewPos : TEXCOORD1;
	float2 tex : TEXCOORD2;
	float4 vZCalcTex : TEXCOORD3;
};

float4 PS_Scene(sPS_Input In,uniform bool useLight,uniform bool useToon,uniform bool useTexture,uniform bool useSphere,uniform bool useSphereAdd) : COLOR
{
	float4 color=saturate(g_materialDiffuse * float4(g_lightColor,1.0f) + g_materialAmbient);

	float3 N = normalize(In.vNormal);
	float3 lightDir = normalize(g_lightDir);

	if( useTexture )
	{
		color *= tex2D( TextureSampler,In.tex );
	}
	if( useSphere )
	{
		float4 sphere = ps_Sphere(N);
		if( useSphereAdd )
		{
			color.rgb += sphere.rgb;
		}
		else
		{
			color.rgb *= sphere.rgb;
		}
	}

	if( useLight )
	{
		if( useToon )
		{
			color *= ps_LightToonDiffuse(N,lightDir);
		}
		else
		{
			color *= ps_LightDiffuse(N,lightDir);
		}

		float4 specularColor = float4(g_lightColor,1.0f)*g_materialSpecular;

		float3 vViewDir = normalize(-In.vViewPos);
		float s = ps_BlinnPhong( N,lightDir,vViewDir,g_materialSpecularPower );
		float4 specular = saturate(float4(specularColor.rgb*s,0.0f));

		color += specular;
	}

	return color;
}

//=========================================================================================
// technique 3D
//=========================================================================================

// useTexture=false,useSphere=false,useSphereAdd=false
technique TechDiffuse
{
	// useLight=false,useToon=false
	pass P0
	{
		VertexShader = compile vs_2_0 VS_Scene();
		PixelShader  = compile ps_2_0 PS_Scene(false,false,false,false,false);
	}

	// useLight=true,useToon=fals
	pass P1
	{
		VertexShader = compile vs_2_0 VS_Scene();
		PixelShader  = compile ps_2_0 PS_Scene(true,false,false,false,false);
	}

	// useLight=true,useToon=true
	pass P2
	{
		VertexShader = compile vs_2_0 VS_Scene();
		PixelShader  = compile ps_2_0 PS_Scene(true,true,false,false,false);
	}
}

// useTexture=false,useSphere=true,useSphereAdd=false
technique TechDiffuseSphereMul
{
	// useLight=false,useToon=false
	pass P0
	{
		VertexShader = compile vs_2_0 VS_Scene();
		PixelShader  = compile ps_2_0 PS_Scene(false,false,false,true,false);
	}

	// useLight=true,useToon=false
	pass P1
	{
		VertexShader = compile vs_2_0 VS_Scene();
		PixelShader  = compile ps_2_0 PS_Scene(true,false,false,true,false);
	}

	// useLight=true,useToon=true
	pass P2
	{
		VertexShader = compile vs_2_0 VS_Scene();
		PixelShader  = compile ps_2_0 PS_Scene(true,true,false,true,false);
	}
}

// useTexture=false,useSphere=true,useSphereAdd=true
technique TechDiffuseSphereAdd
{
	// useLight=false,useToon=false
	pass P0
	{
		VertexShader = compile vs_2_0 VS_Scene();
		PixelShader  = compile ps_2_0 PS_Scene(false,false,false,true,true);
	}

	// useLight=true,useToon=false
	pass P1
	{
		VertexShader = compile vs_2_0 VS_Scene();
		PixelShader  = compile ps_2_0 PS_Scene(true,false,false,true,true);
	}

	// useLight=true,useToon=true
	pass P2
	{
		VertexShader = compile vs_2_0 VS_Scene();
		PixelShader  = compile ps_2_0 PS_Scene(true,true,false,true,true);
	}
}

// useTexture=true,useSphere=false,useSphereAdd=false
technique TechDiffuseTexture
{
	// useLight=false,useToon=false
	pass P0
	{
		VertexShader = compile vs_2_0 VS_Scene();
		PixelShader  = compile ps_2_0 PS_Scene(false,false,true,false,false);
	}

	// useLight=true,useToon=false
	pass P1
	{
		VertexShader = compile vs_2_0 VS_Scene();
		PixelShader  = compile ps_2_0 PS_Scene(true,false,true,false,false);
	}

	// useLight=true,useToon=true
	pass P2
	{
		VertexShader = compile vs_2_0 VS_Scene();
		PixelShader  = compile ps_2_0 PS_Scene(true,true,true,false,false);
	}
}

// useTexture=true,useSphere=true,useSphereAdd=false
technique TechDiffuseTextureSphereMul
{
	// useLight=false,useToon=false
	pass P0
	{
		VertexShader = compile vs_2_0 VS_Scene();
		PixelShader  = compile ps_2_0 PS_Scene(false,false,true,true,false);
	}

	// useLight=true,useToon=false
	pass P1
	{
		VertexShader = compile vs_2_0 VS_Scene();
		PixelShader  = compile ps_2_0 PS_Scene(false,false,true,true,false);
	}

	// useLight=true,useToon=true
	pass P2
	{
		VertexShader = compile vs_2_0 VS_Scene();
		PixelShader  = compile ps_2_0 PS_Scene(true,true,true,true,false);
	}
}

// useTexture=true,useSphere=true,useSphereAdd=true
technique TechDiffuseTextureSphereAdd
{
	// useLight=false,useToon=false
	pass P0
	{
		VertexShader = compile vs_2_0 VS_Scene();
		PixelShader  = compile ps_2_0 PS_Scene(false,false,true,true,true);
	}

	// useLight=true,useToon=false
	pass P1
	{
		VertexShader = compile vs_2_0 VS_Scene();
		PixelShader  = compile ps_2_0 PS_Scene(true,false,true,true,true);
	}

	// useLight=true,useToon=true
	pass P1
	{
		VertexShader = compile vs_2_0 VS_Scene();
		PixelShader  = compile ps_2_0 PS_Scene(true,true,true,true,true);
	}
}

//=========================================================================================
// Edge
//=========================================================================================

float4 VS_PosEdge( sVS_Input In ) : POSITION
{
	return mul( In.vPos+In.vNormal*In.edge*0.001f,g_mWorldViewProjection );
}

float4 PS_Edge() : COLOR
{
	return g_materialDiffuse;
}

technique TechEdge
{
	pass P0
	{
		VertexShader = compile vs_1_1 VS_PosEdge();
		PixelShader  = compile ps_2_0 PS_Edge();
	}
}

//=========================================================================================
// Debug
//=========================================================================================

struct sVS_Input_Line
{
	float4 vPos : POSITION;
	float4 color : COLOR0;
};

struct sVS_Output_Line
{
	float4 vPos : POSITION;
	float4 color : COLOR0;
};

sVS_Output_Line VS_PosLine( sVS_Input_Line In )
{
	sVS_Output_Line Out=(sVS_Output_Line)0;
	Out.vPos = mul( In.vPos,g_mWorldViewProjection );
	Out.color = In.color;

	return Out;
}

float4 PS_Line( float4 color : COLOR0 ) : COLOR
{
	return color;
}

technique TechLine
{
	pass P0
	{
		VertexShader = compile vs_2_0 VS_PosLine();
		PixelShader  = compile ps_2_0 PS_Line();
	}
}


//=========================================================================================
// technique 2D
//=========================================================================================

float4 PS_ScreenDiffuse() : COLOR
{
	return g_materialDiffuse;
}

float4 PS_ScreenDiffuseTexture(float2 tex : TEXCOORD0) : COLOR
{
	float4 texColor = tex2D( TextureSampler,tex );
	if( texColor.a-g_texAlphaTestRef < 0 )
	{
		discard;
	}
	
	float4 color = texColor * g_materialDiffuse;
	return color;
}

technique TechScreenDiffuse
{
	pass P0
	{
		PixelShader  = compile ps_2_0 PS_ScreenDiffuse();
	}
}

technique TechScreenDiffuseTexture
{
	pass P0
	{
		PixelShader  = compile ps_2_0 PS_ScreenDiffuseTexture();
	}
}