/// <summary>
///  实现功能：带色彩恢复的多尺度Retinex图像增强,整理时间2014.10.14
///  参考论文：A Multiscale Retinex for Bridging the Gap Between Color Images and the Human Observation of Scenes
///  参考代码：Contrast-Retinex.c in Gimp. 
/// </summary>
/// <param name="Src">需要处理的源图像的数据结构。</param>
/// <param name="Dest">保存处理后的图像的数据结构。</param>
/// <param name="MaxScale"> 最大的尺度,建议范围[16-500]。 </param>
/// <param name="ScaleAmount"> 多尺度的数量,有效范围[1-8]，为1时则为SSR。. </param>
/// <param name="Dynamic"> 调整图像对比度的参数，数字小则对比度强,建议范围[0.1, 4]。</param>
/// <remarks> 1: 只能处理8位或者24位图像。</remarks>
/// <remarks> 2: Src和Dest可以相同，相同和不同时速度无差异。</remarks>
///	<remarks> 3: 主要的耗时是高斯模糊部分。</remarks>

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