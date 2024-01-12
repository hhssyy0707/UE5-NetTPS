// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HealthBar.generated.h"

/**
 * 
 */
UCLASS()
class NETTPS_API UHealthBar : public UUserWidget
{
	GENERATED_BODY()
	
	
public:
	//�� ü��
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "HP")
	float PB_HPValue = 1.0f;
};
