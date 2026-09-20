#include "GameReadyAssetCheckerWindow.h"

void SGameReadyAssetCheckerWindow::Construct(const FArguments& InArgs)
{
	ChildSlot
		[
			SNew(STextBlock)
				.Text(FText::FromString(TEXT("Game-Ready Asset Checker")))
		];
}