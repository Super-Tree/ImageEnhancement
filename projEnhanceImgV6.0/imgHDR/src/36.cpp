// <summary>
/// ʵ�ֹ��ܣ�����Retinex�㷨�ĸ������
///	�ο����ģ�A Multiscale Retinex for Bridging the Gap Between Color Images and the Human Observation of Scenes
///	����ʱ�䣺2014.6.23
/// </summary>
/// <param name="Src">��Ҫ�����Դͼ������ݽṹ��</param>
/// <param name="Dest">���洦���ĸ������ݵ����ݽṹ��</param>
/// <param name="MaxScale">���ĳ߶ȴ�10.14С������˹ģ���İ뾶��</param>
/// <param name="ScaleAmount">�߶ȵ�����������ȡֵ[1,8]��</param>
///	<remarks> SSR = log(Input)-log(GuassBlrurInput)�� </remarks>

IS_RET Retinex(TMatrix *Src, TMatrix *Dest, float MaxScale, int ScaleAmount)
{
	if (Src == NULL || Dest == NULL) return IS_RET_ERR_NULLREFERENCE;
	if (Src->Data == NULL || Dest->Data == NULL) return IS_RET_ERR_NULLREFERENCE;
	if (Src->Width != Dest->Width || Src->Height != Dest->Height || Src->Channel != Dest->Channel) return IS_RET_ERR_PARAMISMATCH;
	if (Src->Depth != IS_DEPTH_8U && Dest->Depth != IS_DEPTH_32F) return IS_RET_ERR_NOTSUPPORTED;
	if (Src->Channel != 1 && Src->Channel != 3) return IS_RET_ERR_ARGUMENTOUTOFRANGE;

	int X, Y, Z, Width = Src->Width, Height = Src->Height;
	float *LinePD;
	unsigned char *LinePS, *LinePB;
	IS_RET Ret = IS_RET_OK;

	TMatrix *Blur = NULL;
	Ret = IS_CreateMatrix(Src->Width, Src->Height, Src->Depth, Src->Channel, &Blur);
	if (Ret != IS_RET_OK) goto Done;
	float *RetinexScale = (float *)IS_AllocMemory(ScaleAmount * sizeof(float));
	if (RetinexScale == NULL) { Ret = IS_RET_ERR_OUTOFMEMORY; goto Done; }
	float *LogTable = (float *)IS_AllocMemory(256 * sizeof(float));
	if (LogTable == NULL) { Ret = IS_RET_ERR_OUTOFMEMORY; goto Done; }

	if (ScaleAmount == 1)
		RetinexScale[0] = MaxScale;
	else if (ScaleAmount == 2)                      //  ����������߶ȣ�����ѡ�����߶Ⱥ��м�߶�
	{
		RetinexScale[0] = MaxScale / 2;
		RetinexScale[1] = MaxScale;
	}
	else
	{
		float Step = MaxScale / ScaleAmount;
		for (Y = 0; Y < ScaleAmount; Y++)
		{
			RetinexScale[Y] = 2 + Y * Step;
		}
	}

	for (Y = 0; Y < 256; Y++) LogTable[Y] = 1.0 / ScaleAmount * log(Y + 1.0);	//log(0)��û�������

	for (Z = 0; Z < ScaleAmount; Z++)											// ��ν��߶�
	{
		GuassBlur(Src, Blur, RetinexScale[Z]);
		for (Y = 0; Y < Height; Y++)
		{
			LinePS = Src->Data + Y * Src->WidthStep;
			LinePB = Blur->Data + Y * Blur->WidthStep;
			LinePD = (float *)(Dest->Data + Y * Dest->WidthStep);
			if (Src->Channel == 1)
			{
				for (X = 0; X < Width; X++)
				{
					LinePD[X] += LogTable[LinePS[X]] - LogTable[LinePB[X]];
				}
			}
			else
			{
				for (X = 0; X < Width; X++)
				{
					LinePD[0] += LogTable[LinePS[0]] - LogTable[LinePB[0]];
					LinePD[1] += LogTable[LinePS[1]] - LogTable[LinePB[1]];
					LinePD[2] += LogTable[LinePS[2]] - LogTable[LinePB[2]];
					LinePS += 3;
					LinePB += 3;
					LinePD += 3;
				}
			}
		}
	}
Done:
	IS_FreeMatrix(&Blur);
	IS_FreeMemory(RetinexScale);
	IS_FreeMemory(LogTable);
	return Ret;
}