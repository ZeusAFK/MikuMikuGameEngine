float4x4 g_mWorldViewProjection;
float4x4 g_mWorldView;
float4x4 g_mWorld;
float4x4 g_mView;
float4x4 g_mShadowWorldViewProjection;
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
    MipFilter = NONE;    
    //テクスチャラッピング
    AddressU = Clamp;
    AddressV = Clamp;
	AddressW = Clamp;
};

texture g_ShadowMapTexture;
sampler ShadowMapSampler = sampler_state
{
	texture = <g_ShadowMapTexture>;
    
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
// ZPlot
//=========================================================================================

struct VS_ZValuePlot_OUTPUT 
{
    float4 Pos : POSITION;              // 射影変換座標
    float4 ShadowMapTex : TEXCOORD0;    // Zバッファテクスチャ
};

// 頂点シェーダ
VS_ZValuePlot_OUTPUT ZValuePlot_VS( float4 Pos : POSITION )
{
    VS_ZValuePlot_OUTPUT Out = (VS_ZValuePlot_OUTPUT)0;

    // ライトの目線によるワールドビュー射影変換をする
    Out.Pos = mul( Pos, g_mShadowWorldViewProjection );

    // テクスチャ座標を頂点に合わせる
    Out.ShadowMapTex = Out.Pos;

    return Out;
}

// ピクセルシェーダ
float4 ZValuePlot_PS( float4 ShadowMapTex : TEXCOORD0 ) : COLOR
{
    // R色成分にZ値を記録する
    return float4(ShadowMapTex.z/ShadowMapTex.w,0,0,1);
}

// Z値プロット用テクニック
technique TechZPlot
{
    pass P0
	{
        AlphaBlendEnable = FALSE;
        VertexShader = compile vs_2_0 ZValuePlot_VS();
        PixelShader  = compile ps_2_0 ZValuePlot_PS();
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
	float3 vEye		: TEXCOORD3;
	float4 vZCalcTex	: TEXCOORD4;
	float4 Color : COLOR0;
	float4 vSpecularColor : COLOR1;
};

sVS_Output VS_Scene( sVS_Input In,uniform bool useToon,uniform bool useSphere,uniform bool useShadow )
{
	sVS_Output Out=(sVS_Output)0;
	Out.vPos = mul( In.vPos,g_mWorldViewProjection );
	Out.vNormal = normalize( mul( In.vNormal.xyz,(float3x3)g_mWorld ) );
	Out.vTex = In.vTex;
	Out.vEye = g_eyePos-mul( In.vPos.xyz,g_mWorld );

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

	if( useShadow )
	{
		// ライト視点によるワールドビュー射影変換
		Out.vZCalcTex = mul( In.vPos, g_mShadowWorldViewProjection );
	}
	else
	{
		float3 vViewDir = normalize( Out.vEye );
		float s = ps_BlinnPhong( Out.vNormal,-g_lightDir,vViewDir,g_materialSpecularPower );
		Out.vSpecularColor = saturate(float4(SpecularColor.rgb*s,0.0f));
	}

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
	float3 vEye		: TEXCOORD3;
	float4 vZCalcTex	: TEXCOORD4;
	float4 Color : COLOR0;
	float4 vSpecularColor : COLOR1;
};

float4 PS_Scene(sPS_Input In,uniform bool useToon,uniform bool useTexture,uniform bool useSphere,uniform bool useSphereAdd,uniform bool useShadow) : COLOR
{
	float3 N = normalize(In.vNormal);

	float4 color = In.Color;
	float4 shadowColor = float4(AmbientColor, color.a);  // 影の色

	float4 vSpecularColor = In.vSpecularColor;
	if( useShadow )
	{
		float3 vViewDir = normalize( In.vEye );
		float s = ps_BlinnPhong( N,-g_lightDir,vViewDir,g_materialSpecularPower );
		vSpecularColor = saturate(float4(SpecularColor.rgb*s,0.0f));
	}

	if( useTexture )
	{
		float4 texColor = tex2D( TextureSampler, In.tex );
		color *= texColor;
		if( useShadow )
		{
			shadowColor *= texColor;
		}
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
			if( useShadow )
			{
				shadowColor.rgb += sphere.rgb;
			}
		}
		else
		{
			color.rgb *= sphere.rgb;
			if( useShadow )
			{
				shadowColor.rgb *= sphere.rgb;
			}
		}
	}

	if( useShadow )
	{
		// テクスチャ座標に変換
		In.vZCalcTex /= In.vZCalcTex.w;
		float2 transTexCoord;
		transTexCoord.x = (1.0f + In.vZCalcTex.x)*0.5f;
		transTexCoord.y = (1.0f - In.vZCalcTex.y)*0.5f;

		if( !any( saturate(transTexCoord) != transTexCoord ) )
		{
			float comp;
			////if(parthf)
			//{
			//	// セルフシャドウ mode2
			//	comp=1-saturate(max(In.vZCalcTex.z-tex2D(ShadowMapSampler,transTexCoord).r , 0.0f)*SKII2*transTexCoord.y-0.3f);
			//}
			//else 
			{
				// セルフシャドウ mode1
				comp=1.0f-saturate(max(In.vZCalcTex.z-tex2D(ShadowMapSampler,transTexCoord).r , 0.0f)*SKII1-0.3f);
			}
			if( useToon ) 
			{
				// トゥーン適用
				comp = min(saturate(dot(In.vNormal,-g_lightDir)*Toon),comp);
				shadowColor.rgb *= g_materialToon;
			}
	        
			color = lerp(shadowColor, color, comp);
		}
	}
	else
	{
		if( useToon )
		{
			color *= ps_LightToonDiffuse(N,-g_lightDir );
		}
	}

	color += vSpecularColor;

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
		VertexShader = compile vs_2_0 VS_Scene(false,false,false);
		PixelShader  = compile ps_2_0 PS_Scene(false,false,false,false,false);
	}

	// useToon=true
	pass P1
	{
		VertexShader = compile vs_2_0 VS_Scene(true,false,false);
		PixelShader  = compile ps_2_0 PS_Scene(true,false,false,false,false);
	}
}

// useTexture=false,useSphere=true,useSphereAdd=false
technique TechDiffuseSphereMul
{
	// useToon=false
	pass P0
	{
		VertexShader = compile vs_2_0 VS_Scene(false,true,false);
		PixelShader  = compile ps_2_0 PS_Scene(false,false,true,false,false);
	}

	// useToon=true
	pass P1
	{
		VertexShader = compile vs_2_0 VS_Scene(true,true,false);
		PixelShader  = compile ps_2_0 PS_Scene(true,false,true,false,false);
	}
}

// useTexture=false,useSphere=true,useSphereAdd=true
technique TechDiffuseSphereAdd
{
	// useToon=false
	pass P0
	{
		VertexShader = compile vs_2_0 VS_Scene(false,true,false);
		PixelShader  = compile ps_2_0 PS_Scene(false,false,true,true,false);
	}

	// useToon=true
	pass P1
	{
		VertexShader = compile vs_2_0 VS_Scene(true,true,false);
		PixelShader  = compile ps_2_0 PS_Scene(true,false,true,true,false);
	}
}

// useTexture=true,useSphere=false,useSphereAdd=false
technique TechDiffuseTexture
{
	// useToon=false
	pass P0
	{
		VertexShader = compile vs_2_0 VS_Scene(false,false,false);
		PixelShader  = compile ps_2_0 PS_Scene(false,true,false,false,false);
	}

	// useToon=true
	pass P1
	{
		VertexShader = compile vs_2_0 VS_Scene(true,false,false);
		PixelShader  = compile ps_2_0 PS_Scene(true,true,false,false,false);
	}
}

// useTexture=true,useSphere=true,useSphereAdd=false
technique TechDiffuseTextureSphereMul
{
	// useToon=false
	pass P0
	{
		VertexShader = compile vs_2_0 VS_Scene(false,true,false);
		PixelShader  = compile ps_2_0 PS_Scene(false,true,true,false,false);
	}

	// useToon=true
	pass P1
	{
		VertexShader = compile vs_2_0 VS_Scene(true,true,false);
		PixelShader  = compile ps_2_0 PS_Scene(true,true,true,false,false);
	}
}

// useTexture=true,useSphere=true,useSphereAdd=true
technique TechDiffuseTextureSphereAdd
{
	// useToon=false
	pass P0
	{
		VertexShader = compile vs_2_0 VS_Scene(false,true,false);
		PixelShader  = compile ps_2_0 PS_Scene(false,true,true,true,false);
	}

	// useToon=true
	pass P1
	{
		VertexShader = compile vs_2_0 VS_Scene(true,true,false);
		PixelShader  = compile ps_2_0 PS_Scene(true,true,true,true,false);
	}
}


// useTexture=false,useSphere=false,useSphereAdd=false,useShadow=true
technique TechDiffuseShadow
{
	// useToon=false
	pass P0
	{
		VertexShader = compile vs_2_0 VS_Scene(false,false,true);
		PixelShader  = compile ps_2_0 PS_Scene(false,false,false,false,true);
	}

	// useToon=true
	pass P1
	{
		VertexShader = compile vs_2_0 VS_Scene(true,false,true);
		PixelShader  = compile ps_2_0 PS_Scene(true,false,false,false,true);
	}
}

// useTexture=false,useSphere=true,useSphereAdd=false,useShadow=true
technique TechDiffuseSphereMulShadow
{
	// useToon=false
	pass P0
	{
		VertexShader = compile vs_2_0 VS_Scene(false,true,true);
		PixelShader  = compile ps_2_0 PS_Scene(false,false,true,false,true);
	}

	// useToon=true
	pass P1
	{
		VertexShader = compile vs_2_0 VS_Scene(true,true,true);
		PixelShader  = compile ps_2_0 PS_Scene(true,false,true,false,true);
	}
}

// useTexture=false,useSphere=true,useSphereAdd=true,useShadow=true
technique TechDiffuseSphereAddShadow
{
	// useToon=false
	pass P0
	{
		VertexShader = compile vs_2_0 VS_Scene(false,true,true);
		PixelShader  = compile ps_2_0 PS_Scene(false,false,true,true,true);
	}

	// useToon=true
	pass P1
	{
		VertexShader = compile vs_2_0 VS_Scene(true,true,true);
		PixelShader  = compile ps_2_0 PS_Scene(true,false,true,true,true);
	}
}

// useTexture=true,useSphere=false,useSphereAdd=false,useShadow=true
technique TechDiffuseTextureShadow
{
	// useToon=false
	pass P0
	{
		VertexShader = compile vs_2_0 VS_Scene(false,false,true);
		PixelShader  = compile ps_2_0 PS_Scene(false,true,false,false,true);
	}

	// useToon=true
	pass P1
	{
		VertexShader = compile vs_2_0 VS_Scene(true,false,true);
		PixelShader  = compile ps_2_0 PS_Scene(true,true,false,false,true);
	}
}

// useTexture=true,useSphere=true,useSphereAdd=false,useShadow=true
technique TechDiffuseTextureSphereMulShadow
{
	// useToon=false
	pass P0
	{
		VertexShader = compile vs_2_0 VS_Scene(false,true,true);
		PixelShader  = compile ps_2_0 PS_Scene(false,true,true,false,true);
	}

	// useToon=true
	pass P1
	{
		VertexShader = compile vs_2_0 VS_Scene(true,true,true);
		PixelShader  = compile ps_2_0 PS_Scene(true,true,true,false,true);
	}
}

// useTexture=true,useSphere=true,useSphereAdd=true,useShadow=true
technique TechDiffuseTextureSphereAddShadow
{
	// useToon=false
	pass P0
	{
		VertexShader = compile vs_2_0 VS_Scene(false,true,true);
		PixelShader  = compile ps_2_0 PS_Scene(false,true,true,true,true);
	}

	// useToon=true
	pass P1
	{
		VertexShader = compile vs_2_0 VS_Scene(true,true,true);
		PixelShader  = compile ps_2_0 PS_Scene(true,true,true,true,true);
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