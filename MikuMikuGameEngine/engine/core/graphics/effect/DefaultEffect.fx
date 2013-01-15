float4x4 g_mWorldViewProjection;
float4x4 g_mWorldView;
float4x4 g_mLightViewProjection;
float4 g_materialDiffuse;
float4 g_materialSpecular;
float4 g_materialAmbient;
float4 g_materialToon;
float g_materialSpecularPower;

float g_texAlphaTestRef;

float3 g_eyePos;

float3 g_lightDir;
float3 g_lightColor;

float4 g_edgeColor;

static float4 DiffuseColor = saturate(g_materialDiffuse * float4(g_lightColor,1.0f) + g_materialAmbient);
static float4 SpecularColor = g_materialSpecular*float4(g_lightColor,1.0f);

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

//=========================================================================================
// Edge
//=========================================================================================

float4 VS_PosEdge( float4 vPos : POSITION ) : POSITION
{
	return mul( vPos,g_mWorldViewProjection );
}

float4 PS_Edge() : COLOR
{
	return g_edgeColor;
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
// Model
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

	//float3 diffuse = lerp(g_materialToon,float3(1.0f,1.0f,1.0),saturate(LightNormal * 16 + 0.5) );
	float3 diffuse = tex2D(ToonSampler, float2(0, 0.5 - LightNormal * 0.5) ).rgb;
	
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
	float4 vSpecularColor : COLOR0;
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

	float3 N = normalize(Out.vNormal);
	float3 lightDir = normalize(g_lightDir);

	float3 vViewDir = normalize(-Out.vViewPos);
	float s = ps_BlinnPhong( N,lightDir,vViewDir,g_materialSpecularPower );
	Out.vSpecularColor = saturate(float4(SpecularColor.rgb*s,0.0f));

	return Out;
}

//=========================================================================================
// PixelShader
//=========================================================================================

struct sPS_Input
{
	float3 vNormal : TEXCOORD0;
	float3 vViewPos : TEXCOORD1;
	float2 tex : TEXCOORD2;
	float4 vZCalcTex : TEXCOORD3;
	float4 vSpecularColor : COLOR0;
};

float4 PS_Scene(sPS_Input In,uniform bool useToon,uniform bool useTexture,uniform bool useSphere,uniform bool useSphereAdd) : COLOR
{
	float4 color=DiffuseColor;

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

	if( useToon )
	{
		color *= ps_LightToonDiffuse(N,lightDir);
	}
	else
	{
		color *= ps_LightDiffuse(N,lightDir);
	}

	color += In.vSpecularColor;

	return color;
}

//=========================================================================================
// technique 3D
//=========================================================================================

// useTexture=false,useSphere=false,useSphereAdd=false
technique TechDiffuse
{
	// useToon=false
	pass P0
	{
		VertexShader = compile vs_2_0 VS_Scene();
		PixelShader  = compile ps_2_0 PS_Scene(false,false,false,false);
	}

	// useToon=true
	pass P1
	{
		VertexShader = compile vs_2_0 VS_Scene();
		PixelShader  = compile ps_2_0 PS_Scene(true,false,false,false);
	}
}

// useTexture=false,useSphere=true,useSphereAdd=false
technique TechDiffuseSphereMul
{
	// useToon=false
	pass P0
	{
		VertexShader = compile vs_2_0 VS_Scene();
		PixelShader  = compile ps_2_0 PS_Scene(false,false,true,false);
	}

	// useToon=true
	pass P1
	{
		VertexShader = compile vs_2_0 VS_Scene();
		PixelShader  = compile ps_2_0 PS_Scene(true,false,true,false);
	}
}

// useTexture=false,useSphere=true,useSphereAdd=true
technique TechDiffuseSphereAdd
{
	// useToon=false
	pass P0
	{
		VertexShader = compile vs_2_0 VS_Scene();
		PixelShader  = compile ps_2_0 PS_Scene(false,false,true,true);
	}

	// useToon=true
	pass P1
	{
		VertexShader = compile vs_2_0 VS_Scene();
		PixelShader  = compile ps_2_0 PS_Scene(true,false,true,true);
	}
}

// useTexture=true,useSphere=false,useSphereAdd=false
technique TechDiffuseTexture
{
	// useToon=false
	pass P0
	{
		VertexShader = compile vs_2_0 VS_Scene();
		PixelShader  = compile ps_2_0 PS_Scene(false,true,false,false);
	}

	// useToon=true
	pass P1
	{
		VertexShader = compile vs_2_0 VS_Scene();
		PixelShader  = compile ps_2_0 PS_Scene(true,true,false,false);
	}
}

// useTexture=true,useSphere=true,useSphereAdd=false
technique TechDiffuseTextureSphereMul
{
	// useToon=false
	pass P0
	{
		VertexShader = compile vs_2_0 VS_Scene();
		PixelShader  = compile ps_2_0 PS_Scene(false,true,true,false);
	}

	// useToon=true
	pass P1
	{
		VertexShader = compile vs_2_0 VS_Scene();
		PixelShader  = compile ps_2_0 PS_Scene(true,true,true,false);
	}
}

// useTexture=true,useSphere=true,useSphereAdd=true
technique TechDiffuseTextureSphereAdd
{
	// useToon=false
	pass P0
	{
		VertexShader = compile vs_2_0 VS_Scene();
		PixelShader  = compile ps_2_0 PS_Scene(false,true,true,true);
	}

	// useToon=true
	pass P1
	{
		VertexShader = compile vs_2_0 VS_Scene();
		PixelShader  = compile ps_2_0 PS_Scene(true,true,true,true);
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