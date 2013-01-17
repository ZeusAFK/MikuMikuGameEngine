float4x4 g_mWorldViewProjection;
float4x4 g_mWorldView;
float4x4 g_mWorld;
float4x4 g_mView;
float4 g_materialDiffuse;
float4 g_materialAmbient;
float4 g_materialEmissive;
float4 g_materialSpecular;
float4 g_materialToon;
float g_materialSpecularPower;

float3 g_eyePos;

float3 g_lightDir;
float3 g_lightColorDiffuse;
float3 g_lightColorAmbient;
float3 g_lightColorSpecular;

float4 g_edgeColor;

static float4 DiffuseColor  = g_materialDiffuse * float4(g_lightColorDiffuse, 1.0f);
static float3 AmbientColor  = saturate(g_materialAmbient.rgb * g_lightColorAmbient + g_materialEmissive.rgb);
static float3 SpecularColor = g_materialSpecular.rgb * g_lightColorSpecular;

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
	
	return float4(n,n,n,1.0f);
}

float4 ps_LightToonDiffuse(float3 N,float3 lightDir)
{
	float LightNormal = dot( N, lightDir );

	//float3 diffuse = lerp(g_materialToon,float3(1.0f,1.0f,1.0),saturate(LightNormal * 16 + 0.5) );
	float3 diffuse = tex2D(ToonSampler, float2(0, 0.5 - LightNormal * 0.5) ).rgb;
	
	return float4(diffuse.rgb,1.0f);
}

float4 ps_Sphere( float2 SpTex )
{
	return tex2D( SphereMapSampler,SpTex );
}

float ps_BlinnPhong( float3 normal,float3 lightDir,float3 viewDir,float specularPower )
{
	float3 H = normalize( viewDir+lightDir );
	return pow( max(0,dot(H,normal)),specularPower );
}

struct sVS_Input
{
	float4 vPos : POSITION;
	float3 vNormal : NORMAL;
	float2 vTex : TEXCOORD0;
};

struct sVS_Output
{
	float4 vPos : POSITION;
	float3 vNormal : TEXCOORD0;
	float2 vTex : TEXCOORD1;
	float2 SpTex    : TEXCOORD2;
	float4 Color : COLOR0;
	float4 vSpecularColor : COLOR1;
};

sVS_Output VS_Scene( sVS_Input In,uniform bool useToon,uniform bool useSphere )
{
	sVS_Output Out=(sVS_Output)0;
	Out.vPos = mul( In.vPos,g_mWorldViewProjection );
	Out.vNormal = normalize( mul( In.vNormal.xyz,(float3x3)g_mWorld ) );
	Out.vTex = In.vTex;

	if( useSphere )
	{
		Out.SpTex = mul( Out.vNormal, (float3x3)g_mView );
	}

	Out.Color.rgb=AmbientColor;
	if( !useToon ) 
	{
		Out.Color.rgb += ps_LightDiffuse(Out.vNormal,-g_lightDir) * DiffuseColor.rgb;
	}
	Out.Color.a = DiffuseColor.a;
	Out.Color = saturate( Out.Color );

	float3 vViewDir = normalize( g_eyePos-mul( In.vPos.xyz,g_mWorld ) );
	float s = ps_BlinnPhong( Out.vNormal,-g_lightDir,vViewDir,g_materialSpecularPower );
	Out.vSpecularColor = saturate(float4(SpecularColor.rgb*s,0.0f));

	return Out;
}

//=========================================================================================
// PixelShader
//=========================================================================================

struct sPS_Input
{
	float3 vNormal : TEXCOORD0;
	float2 tex : TEXCOORD1;
	float2 SpTex    : TEXCOORD2;
	float4 Color : COLOR0;
	float4 vSpecularColor : COLOR1;
};

float4 PS_Scene(sPS_Input In,uniform bool useToon,uniform bool useTexture,uniform bool useSphere,uniform bool useSphereAdd) : COLOR
{
	float3 N = normalize(In.vNormal);

	float4 color = In.Color;

	if( useTexture )
	{
		color *= tex2D( TextureSampler,In.tex );
	}
	if( useSphere )
	{
		// スフィアマップ座標計算
		float2 SpTex = In.SpTex;
		SpTex.x = SpTex.x * 0.5f + 0.5f;
		SpTex.y = SpTex.y * -0.5f + 0.5f;

		float4 sphere = ps_Sphere(SpTex);
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
		color *= ps_LightToonDiffuse(N,-g_lightDir );
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
		VertexShader = compile vs_2_0 VS_Scene(false,false);
		PixelShader  = compile ps_2_0 PS_Scene(false,false,false,false);
	}

	// useToon=true
	pass P1
	{
		VertexShader = compile vs_2_0 VS_Scene(true,false);
		PixelShader  = compile ps_2_0 PS_Scene(true,false,false,false);
	}
}

// useTexture=false,useSphere=true,useSphereAdd=false
technique TechDiffuseSphereMul
{
	// useToon=false
	pass P0
	{
		VertexShader = compile vs_2_0 VS_Scene(false,true);
		PixelShader  = compile ps_2_0 PS_Scene(false,false,true,false);
	}

	// useToon=true
	pass P1
	{
		VertexShader = compile vs_2_0 VS_Scene(true,true);
		PixelShader  = compile ps_2_0 PS_Scene(true,false,true,false);
	}
}

// useTexture=false,useSphere=true,useSphereAdd=true
technique TechDiffuseSphereAdd
{
	// useToon=false
	pass P0
	{
		VertexShader = compile vs_2_0 VS_Scene(false,true);
		PixelShader  = compile ps_2_0 PS_Scene(false,false,true,true);
	}

	// useToon=true
	pass P1
	{
		VertexShader = compile vs_2_0 VS_Scene(true,true);
		PixelShader  = compile ps_2_0 PS_Scene(true,false,true,true);
	}
}

// useTexture=true,useSphere=false,useSphereAdd=false
technique TechDiffuseTexture
{
	// useToon=false
	pass P0
	{
		VertexShader = compile vs_2_0 VS_Scene(false,false);
		PixelShader  = compile ps_2_0 PS_Scene(false,true,false,false);
	}

	// useToon=true
	pass P1
	{
		VertexShader = compile vs_2_0 VS_Scene(true,false);
		PixelShader  = compile ps_2_0 PS_Scene(true,true,false,false);
	}
}

// useTexture=true,useSphere=true,useSphereAdd=false
technique TechDiffuseTextureSphereMul
{
	// useToon=false
	pass P0
	{
		VertexShader = compile vs_2_0 VS_Scene(false,true);
		PixelShader  = compile ps_2_0 PS_Scene(false,true,true,false);
	}

	// useToon=true
	pass P1
	{
		VertexShader = compile vs_2_0 VS_Scene(true,true);
		PixelShader  = compile ps_2_0 PS_Scene(true,true,true,false);
	}
}

// useTexture=true,useSphere=true,useSphereAdd=true
technique TechDiffuseTextureSphereAdd
{
	// useToon=false
	pass P0
	{
		VertexShader = compile vs_2_0 VS_Scene(false,true);
		PixelShader  = compile ps_2_0 PS_Scene(false,true,true,true);
	}

	// useToon=true
	pass P1
	{
		VertexShader = compile vs_2_0 VS_Scene(true,true);
		PixelShader  = compile ps_2_0 PS_Scene(true,true,true,true);
	}
}
