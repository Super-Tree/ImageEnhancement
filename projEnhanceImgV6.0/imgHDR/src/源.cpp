/// <summary>
///  ʵ�ֹ��ܣ���ɫ�ʻָ��Ķ�߶�Retinexͼ����ǿ,����ʱ��2014.10.14
///  �ο����ģ�A Multiscale Retinex for Bridging the Gap Between Color Images and the Human Observation of Scenes
///  �ο����룺Contrast-Retinex.c in Gimp. 
/// </summary>
/// <param name="Src">��Ҫ�����Դͼ������ݽṹ��</param>
/// <param name="Dest">���洦����ͼ������ݽṹ��</param>
/// <param name="MaxScale"> ���ĳ߶�,���鷶Χ[16-500]�� </param>
/// <param name="ScaleAmount"> ��߶ȵ�����,��Ч��Χ[1-8]��Ϊ1ʱ��ΪSSR��. </param>
/// <param name="Dynamic"> ����ͼ��ԱȶȵĲ���������С��Աȶ�ǿ,���鷶Χ[0.1, 4]��</param>
/// <remarks> 1: ֻ�ܴ���8λ����24λͼ��</remarks>
/// <remarks> 2: Src��Dest������ͬ����ͬ�Ͳ�ͬʱ�ٶ��޲��졣</remarks>
///	<remarks> 3: ��Ҫ�ĺ�ʱ�Ǹ�˹ģ�����֡�</remarks>

IS_RET __stdcall MSRCRGIMP(TMatrix *Src, TMatrix *Dest, float MaxScale, int ScaleAmount, float Dynamic)
{
	if (Src == NULL || Dest == NULL) return IS_RET_ERR_NULLREFERENCE;
	if (Src->Data == NULL || Dest->Data == NULL) return IS_RET_ERR_NULLREFERENCE;
	if (Src->Width != Dest->Width || Src->Height != Dest->Height || Src->Channel != Dest->Channel || Src->Depth != Dest->Depth || Src->WidthStep != Dest->WidthStep) return IS_RET_ERR_PARAMISMATCH;
	if (Src->Depth != IS_DEPTH_8U || Dest->Depth != IS_DEPTH_8U) return IS_RET_ERR_NOTSUPPORTED;
	if (Src->Channel != 3 && Src->Channel != 1) return IS_RET_ERR_ARGUMENTOUTOFRANGE;
	if (MaxScale <= 0 || ScaleAmount < 1 || ScaleAmount >8 || Dynamic < 0.1) return IS_RET_ERR_ARGUMENTOUTOFRANGE;

	TMatrix *Sum = NULL;
	IS_RET Ret = IS_CreateMatrix(Src->Width, Src->Height, IS_DEPTH_32F, Src->Channel, &Sum);
	if (Ret != IS_RET_OK) goto Done;
	Ret = Retinex(Src, Sum, MaxScale, ScaleAmount);
	if (Ret != IS_RET_OK) goto Done;
	Ret = FloatToImageUseLinearMap(Sum, Dest);
	if (Ret != IS_RET_OK) goto Done;
Done:
	IS_FreeMatrix(&Sum);
	return Ret;
}