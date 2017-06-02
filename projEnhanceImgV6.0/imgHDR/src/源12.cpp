#include "..\..\Utility.h"



IS_RET __stdcall FloatToImageUseLinearMap(TMatrix *Src, TMatrix *Dest)
{
	if (Src == NULL || Dest == NULL) return IS_RET_ERR_NULLREFERENCE;
	if (Src->Data == NULL || Dest->Data == NULL) return IS_RET_ERR_NULLREFERENCE;
	if (Src->Width != Dest->Width || Src->Height != Dest->Height || Src->Channel != Dest->Channel) return IS_RET_ERR_PARAMISMATCH;
	if (Src->Depth != IS_DEPTH_32F && Dest->Depth != IS_DEPTH_8U) return IS_RET_ERR_NOTSUPPORTED;
	if (Src->Channel != 1 && Src->Channel != 3) return IS_RET_ERR_ARGUMENTOUTOFRANGE;

	float *LinePS;
	unsigned char *LinePD;
	int X, Y, Width = Src->Width, Height = Src->Height;

	if (Src->Channel == 1)
	{
		float Max = MinValueF, Min = MaxValueF, Value, InvDelta;
		for (Y = 0; Y < Height; Y++)
		{
			LinePS = (float *)(Src->Data + Y * Src->WidthStep);
			for (X = 0; X < Width; X++)
			{
				Value = LinePS[X];
				if (Min > Value) Min = Value;
				if (Max < Value) Max = Value;
			}
		}
		if (Max == Min)
		{
			memset(Dest->Data, 0, Dest->Height * Dest->WidthStep);
		}
		else
		{
			InvDelta = 255 / (Max - Min);
			for (Y = 0; Y < Height; Y++)
			{
				LinePS = (float *)(Src->Data + Y * Src->WidthStep);
				LinePD = Dest->Data + Y * Dest->WidthStep;
				for (X = 0; X < Width; X++)
				{
					LinePD[X] = (LinePS[X] - Min) * InvDelta;
				}
			}
		}
	}
	else
	{
		float MaxB = MinValueF, MinB = MaxValueF, ValueB, InvDeltaB;
		float MaxG = MinValueF, MinG = MaxValueF, ValueG, InvDeltaG;
		float MaxR = MinValueF, MinR = MaxValueF, ValueR, InvDeltaR;

		for (Y = 0; Y < Height; Y++)
		{
			LinePS = (float *)(Src->Data + Y * Src->WidthStep);
			for (X = 0; X < Width; X++)
			{
				ValueB = LinePS[0]; ValueG = LinePS[1]; ValueR = LinePS[2];
				if (MinB > ValueB) MinB = ValueB;
				if (MaxB < ValueB) MaxB = ValueB;
				if (MinG > ValueG) MinG = ValueG;
				if (MaxG < ValueG) MaxG = ValueG;
				if (MinR > ValueR) MinR = ValueR;
				if (MaxR < ValueR) MaxR = ValueR;
				LinePS += 3;
			}
		}
		if (MaxB == MinB)
			InvDeltaB = 0;
		else
			InvDeltaB = 255 / (MaxB - MinB);
		if (MaxG == MinG)
			InvDeltaG = 0;
		else
			InvDeltaG = 255 / (MaxG - MinG);
		if (MaxR == MinR)
			InvDeltaR = 0;
		else
			InvDeltaR = 255 / (MaxR - MinR);

		for (Y = 0; Y < Height; Y++)
		{
			LinePS = (float *)(Src->Data + Y * Src->WidthStep);
			LinePD = Dest->Data + Y * Dest->WidthStep;
			for (X = 0; X < Width; X++)
			{
				LinePD[0] = (LinePS[0] - MinB) * InvDeltaB;
				LinePD[1] = (LinePS[1] - MinG) * InvDeltaG;
				LinePD[2] = (LinePS[2] - MinR) * InvDeltaR;
				LinePS += 3;
				LinePD += 3;
			}
		}
	}
	return IS_RET_OK;
}