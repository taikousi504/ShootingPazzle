#include "PostEffectTest.hlsli"

Texture2D<float4> tex0 : register(t0);	//0番スロットに設定されたテクスチャ
Texture2D<float4> tex1 : register(t1);	//1番スロットに設定されたテクスチャ
Texture2D<float> depthTex : register(t2); //1番スロットに設定されたテクスチャ

SamplerState smp : register(s0);		//0番スロットに設定されたサンプラー

float GetRand(float2 texCoord, int Seed)
{
	return frac(sin(dot(texCoord.xy, float2(12.9898, 78.233)) + Seed) * 43758.5453);
}

float4 GetBlur(Texture2D<float4> tex, float2 uv, float windowSizeX, float windowSizeY, int roopCount)
{
	float amountX = 1.0f / windowSizeX;
	float amountY = 1.0f / windowSizeY;

	float4 col = float4(0, 0, 0, 0);

	//縦
	for (int i = 0; i < roopCount; i++) {
		//横
		for (int j = 0; j < roopCount; j++) {
			col += tex.Sample(smp, uv + float2(amountX * (j - (roopCount / 2)), amountY * (i - (roopCount / 2))));
		}
	}

	return col / (roopCount * roopCount);
}

float4 main(VSOutput input) : SV_TARGET
{
	float4 colortex[1];
	colortex[0] = float4(tex0.Sample(smp, input.uv).xyz, 1);
	//colortex[1] = tex1.Sample(smp, input.uv);
	return colortex[0];
	
	//float dsp = pow(tex1.Sample(smp, input.uv), 20);
	//return float4(dsp, dsp, dsp, 1);
	
	//float4 colortex1;

	////色反転
	//colortex0 = float4(1 - colortex0.rgb, 1);
	////平均ぼかし
	//colortex1 = GetBlur(tex1, input.uv, 1280, 720, 10);

	//float4 color = colortex0;
	//if (fmod(input.uv.y, 0.1f) < 0.05f) {
	//	color = colortex1;
	//}


}

float4 main_depth(VSOutput input) : SV_TARGET
{
	float4 colortex[3];
	colortex[0] = float4(tex0.Sample(smp, input.uv).xyz, 1);
	colortex[1] = float4(tex1.Sample(smp, input.uv).xyz, 1);
	
	//深度値テクスチャを1/4スケールで出力
	float2 uv = float2(input.uv.x * 4, input.uv.y * 4);
	float dep = depthTex.Sample(smp, uv);
	colortex[2] = float4(dep, 0, 0, 1);

	float4 output = (uv.x < 1.0f && uv.y < 1.0f) ? colortex[2] : colortex[0];
	return output;
	
	//float dsp = pow(tex1.Sample(smp, input.uv), 20);
	//return float4(dsp, dsp, dsp, 1);
	
	//float4 colortex1;

	////色反転
	//colortex0 = float4(1 - colortex0.rgb, 1);
	////平均ぼかし
	//colortex1 = GetBlur(tex1, input.uv, 1280, 720, 10);

	//float4 color = colortex0;
	//if (fmod(input.uv.y, 0.1f) < 0.05f) {
	//	color = colortex1;
	//}


}