
// Transformation
float4x4 g_mWorld;
float4x4 g_mView;
float4x4 g_mWorldViewProj;
float3 g_vEye;

// Parameter
bool g_bShadows = true;
float g_fDepth = 0.2;
float g_fTiling = 1;
float g_iMin = 10;
float g_iMax = 100;
float2 g_vTexSize = float2(512, 512);
float3 g_vLight = float3(0, 0, 1);

// Material
float4 g_cAmbient = float4(0.15, 0.15, 0.15, 0);
float4 g_cDiffuse = float4(1, 1, 1, 1);
float4 g_cSpecular = float4(1, 1, 1, 1);
float g_fSpecularExponent = 400;

// Texture
texture2D tex;
texture2D texNH;

sampler sp = sampler_state {
	Texture = <tex>;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
};

sampler spNH = sampler_state {
	Texture = <texNH>;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
};

struct VS_INPUT {
	float3 pos		: POSITION;
	float3 normal	: NORMAL;
	float2 uv		: TEXCOORD;
	float3 tangent	: TANGENT;
	float3 binormal : BINORMAL;
};

//        Direction --- Space
// L, N, V, R, T, B --- o, w, t
struct VS_OUTPUT {
	float4 pos		: POSITION;
    float2 uv		: TEXCOORD0;
    float3 Lt		: TEXCOORD1;	// Light in tangent space
    float3 Vt		: TEXCOORD2;	// Viewer in tangent space
    float2 offset	: TEXCOORD5;	// Pre-computed parallax offset
};

struct PS_OUTPUT {
	float4 color : COLOR;
};

VS_OUTPUT VSMain(VS_INPUT i) {
	VS_OUTPUT o;
    o.pos = mul(float4(i.pos, 1.0), g_mWorldViewProj);
    o.uv = i.uv * g_fTiling;
    
    float3 Tw = mul(i.tangent, (float3x3) g_mWorld);
    float3 Bw = mul(i.binormal, (float3x3) g_mWorld);
    float3 Nw = mul(i.normal, (float3x3) g_mWorld);
    
    Tw = normalize(Tw);
    Bw = normalize(Bw);
    Nw = normalize(Nw);
    float3x3 mW2T = float3x3( Tw, Bw, Nw ); // World to Tangent Space Transformation
    o.Lt = mul(mW2T, -g_vLight);
    o.Vt = mul(mW2T, g_vEye - mul(float4(i.pos, 1.0), g_mWorld).xyz);
    o.offset = o.Vt.xy / o.Vt.z * g_fDepth; // Default Parallax Mapping
	return o;
}

float4 ComputeIllumination(float2 uv, float3 Lt, float3 Vt, float fShadowFactor) {
	float3 Nt = normalize(tex2D(spNH, uv).rgb * 2 - 1);
	float4 color = tex2D(sp, uv);
	float4 cDiffuse = saturate(dot(Lt, Nt)) * g_cDiffuse;

	/* Phong Reflection Model *
	float3 Rt = normalize(2 * dot(Lt, Nt) * Nt - Lt);
	float fRdotV = saturate(dot(Rt, Vt));
	float4 cSpecular = saturate(pow(fRdotV, g_fSpecularExponent)) * g_cSpecular;
	
	/* Blinn-Phong Reflection Model */
	float3 Ht = normalize(Lt + Vt);
	float fNdotH = saturate(dot(Nt, Ht));
	float4 cSpecular = saturate(pow(fNdotH, g_fSpecularExponent)) * g_cSpecular;
	/**/

	return ((g_cAmbient + cDiffuse) * color + cSpecular) * fShadowFactor;
}   

PS_OUTPUT PSMain(VS_OUTPUT i) {
	PS_OUTPUT o;
	float2 fUV = i.uv * g_vTexSize;
   
	float3 Lt = normalize(i.Lt);
	float3 Vt = normalize(i.Vt);
	
	// Sample the HeightMap according to the angle between V and N in Tangent Space
	// (for the viewer: the more oblique the more samples taken)
	int iTotal = (int) lerp(g_iMin, g_iMax, Vt.z);
    
	int iCnt = 0;
	float fCur = 1.0;
    float fStep = 1.0 / iTotal;
	float2 vCur = i.uv;
	float2 vStep = fStep * i.offset;
	float fCurH = 0.0;
	float fPrevH = 1.0;
	
	float2 cur = 0;
	float2 prev = 0;
	
	// Steep Parallax Mapping
	while (iCnt < iTotal) {
		vCur -= vStep; // Vt反向延长线
		fCurH = tex2Dlod(spNH, float4(vCur, 0, 1)).a;
		fCur -= fStep;
		if (fCurH > fCur) {
			cur = float2(fCur, fCurH);
			prev = float2(fCur + fStep, fPrevH);
			fPrevH = fCurH;
			iCnt = iTotal;
		} else {
			fPrevH = fCurH;
			iCnt++;
		}
	}
	
	// POM interpolation
	float fPrevDelta = prev.x - prev.y; // Positive
	float fCurDelta = cur.x - cur.y; // Negative
	float fWeight = fCurDelta / (fPrevDelta - fCurDelta);
	float2 uv = vCur - vStep * fWeight;
	
	// Soft Self-Shadowing
	float fShadowFactor = 1;
	if (g_bShadows && Lt.z > 0) {
		float fTemp = 0;
		iTotal = 10;
		iCnt = 0;
		fStep = 1 / iTotal + 0.1;
		fCur = tex2Dlod(spNH, float4(uv, 0, 1)).a + fStep;
		vStep = fStep * Lt.xy * g_fDepth;
		vCur = uv + vStep;
		while (iCnt < iTotal) {
			vCur += vStep; // Lt延长线
			fCurH = tex2Dlod(spNH, float4(vCur, 0, 1)).a;
			fCur += fStep;
			fTemp = max((fCurH - fCur) * (1- iCnt / iTotal) * 5, fTemp);
			iCnt++;
		}
		fShadowFactor = 1 - fTemp;
	}
	
	o.color = ComputeIllumination(uv, Lt, Vt, fShadowFactor);
	return o;
}

// Parallax Occlusion Mapping
technique POM {
	Pass P0 {
		VertexShader = compile vs_3_0 VSMain();
		PixelShader = compile ps_3_0 PSMain();
	}
}
