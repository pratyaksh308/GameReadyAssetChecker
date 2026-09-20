#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

class SGameReadyAssetCheckerWindow : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SGameReadyAssetCheckerWindow)
		{
		}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);
};