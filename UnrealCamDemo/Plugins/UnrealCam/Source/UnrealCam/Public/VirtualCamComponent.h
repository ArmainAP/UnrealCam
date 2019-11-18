// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/Texture2D.h"
#include "VirtualCamComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UNREALCAM_API UVirtualCamComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UVirtualCamComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* WebcamRenderTexture;

protected:
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	   
private:
	static bool importDLL(FString folder, FString name);
	static void freeDLL();

	static bool importMethodSendTexture();
	static bool SendTextureFromDLL(const unsigned char* data, int width, int height);

	bool loadedDLL = false;
	bool loadedMethod = false;
};
