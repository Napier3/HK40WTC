#include "MathApi.h"


double math_ValueRevised(double dValue, long nDots)
{
	double dInt = (long)dValue;
	double dTemp = dValue - dInt;
	double dCal = 0;
	long nDot = 0;
	double dIntTemp = 0;
	double dDiv = 1;
	long nIntTempPrev = 0;

	if (nDots <= 0)
	{
		nDots = 3;
	}

	for (nDot=0; nDot<nDots; nDot++)
	{
		dDiv *= 10;
		dTemp *= 10;
		dIntTemp = (long)dTemp;
		dCal = dCal * 10 + dIntTemp;
		nIntTempPrev = (long)dIntTemp;
		dTemp = dTemp - dIntTemp;
	}

	dTemp *= 10;
	dIntTemp = (long)dTemp;
	long nIntTemp = (long)dIntTemp;

	if (dIntTemp >= 6)
	{
		dCal += 1;
	}
	else
	{
		if (nIntTemp == 5)
		{
			if (dTemp - 5.0 > PRECISION_COMPARE)
			{
				dCal += 1;
			}
			else
			{
				if (nIntTempPrev % 2 == 1)
				{
					dCal += 1;
				}
			}
		}
	}

	dCal = dInt + dCal / dDiv;
	//dCal += 0.0000000001;

	return dCal;
}

void MakeAngleTo360(double &dAngle)
{
	while (dAngle < 0)
	{
		dAngle += 360;
	}

	while (dAngle >= 360)
	{
		dAngle -= 360;
	}
}

void MakeAngleTo360(float &fAngle)
{
	while (fAngle < 0)
	{
		fAngle += 360;
	}

	while (fAngle > 360)
	{
		fAngle -= 360;
	}
}


void MakeAngleTo180(double &dAngle)
{
	while (dAngle < -180)
	{
		dAngle += 360;
	}

	while (dAngle > 180)
	{
		dAngle -= 360;
	}
}

void MakeAngleTo180(float &fAngle)
{
	while (fAngle < -180)
	{
		fAngle += 360;
	}

	while (fAngle > 180)
	{
		fAngle -= 360;
	}
}


void MakeAngleTo_PI2(double &dAngle)
{
	while (dAngle < 0)
	{
		dAngle += _PI2;
	}

	while (dAngle >= _PI2)
	{
		dAngle -= _PI2;
	}
}

void MakeAngleTo_PI2(float &fAngle)
{
	while (fAngle < 0)
	{
		fAngle += (float)_PI2;
	}

	while (fAngle > _PI2)
	{
		fAngle -= (float)_PI2;
	}
}


void MakeAngleTo_PI(double &dAngle)
{
	while (dAngle < -_PI)
	{
		dAngle += _PI2;
	}

	while (dAngle > _PI)
	{
		dAngle -= _PI2;
	}
}

void MakeAngleTo_PI(float &fAngle)
{
	while (fAngle < -_PI)
	{
		fAngle += (float)_PI2;
	}

	while (fAngle > _PI)
	{
		fAngle -= (float)_PI2;
	}
}

void math_CalError(double dValue, double dRefValue, double &dAbsCal, double &dRelCal, BOOL bIsAngle)
{
	dAbsCal = dValue - dRefValue;

	if (bIsAngle)
	{
		dAbsCal = dAbsCal - ( ((long)(dAbsCal / 360))*360);
	}

	dRelCal = dAbsCal;//fabs(dAbsCal);

	if (!bIsAngle)
	{
		if (dRefValue > 0.0001)
		{
			dRelCal /= dRefValue;
		}
	}
}

void math_CalError(double dValue, double dRefValue, double dRelStdRefValue, double &dAbsCal, double &dRelCal, BOOL bIsAngle)
{
	dAbsCal = dValue - dRefValue;

	if (bIsAngle)
	{
		dAbsCal = dAbsCal - ( ((long)(dAbsCal / 360))*360);
	}

	dRelCal = dAbsCal;//fabs(dAbsCal);

	if (!bIsAngle)
	{
		if (dRefValue > 0.0001)
		{
			dRelCal /= dRelStdRefValue;
		}
	}
}

long math_CalIsInError(double dAbsCal, double dRelCal, double dAbsErr, double dRelErr)
{
        bool bAbsRsltJdg = true;
        bool bRelRsltJdg = true;
	long nRsltJdg = 0;
        bool bRsltJdg = false;

	dAbsCal = fabs(dAbsCal);
	dRelCal = fabs(dRelCal);

	if (dAbsErr > 0)
	{
		if (dAbsCal > dAbsErr)
		{
                        bAbsRsltJdg = false;
		}
		else
		{
                        bRsltJdg = true;
		}
	}

	if (dRelErr > 0)
	{
		if (dRelCal > dRelErr)
		{
                        bRelRsltJdg = false;
		}
		else
		{
                        bRsltJdg = true;
		}
	}

	//if (bAbsRsltJdg && bRelRsltJdg)
	if (bRsltJdg)
	{
		nRsltJdg = 1;
	}
	else
	{
		nRsltJdg = 0;
	}

	return nRsltJdg;
}

bool mathAngle360_IsEqual(double dValue1, double dValue2, double dPrecision)
{
	MakeAngleTo360(dValue1);
	MakeAngleTo360(dValue2);
	double dValue = fabs(dValue2 - dValue1);
	MakeAngleTo360(dValue);//正常上述相减完值应该在0~360返回内,防止精度丢失导致的误差,此处再确认转换一次
        bool bRet = (dValue< dPrecision);

	if (!bRet)//如果差值在360附近,此处再减去360确认一次
	{
		bRet = (fabs(dValue - 360)< dPrecision);
	}

	return bRet;
}
