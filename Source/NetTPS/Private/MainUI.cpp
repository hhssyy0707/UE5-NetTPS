// Fill out your copyright notice in the Description page of Project Settings.


#include "MainUI.h"

#include "Components/Image.h"
#include "Components/UniformGridPanel.h"

void UMainUI::ShowCrosshair(bool IsShow)
{
	if (IsShow) {
		IM_Crosshair->SetVisibility(ESlateVisibility::Visible);
	}
	else {
		IM_Crosshair->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UMainUI::AddBullet()
{
	auto BulletWidget = CreateWidget(GetWorld(), BulletUIFactory);
	BulletPanel->AddChildToUniformGrid(BulletWidget, 1, BulletPanel->GetChildrenCount());
}

void UMainUI::PopBullet(int32 BulletIndex)
{
	BulletPanel->RemoveChildAt(BulletIndex);
}

void UMainUI::RemoveAllBullets()
{
	/*for (auto BulltetWidget : BulletPanel->GetAllChildren()) {
		BulletPanel->RemoveChild(BulltetWidget);
	}*/

	BulletPanel->ClearChildren();
}
