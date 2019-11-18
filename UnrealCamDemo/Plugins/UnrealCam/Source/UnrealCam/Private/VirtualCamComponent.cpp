// Fill out your copyright notice in the Description page of Project Settings.
#include "VirtualCamComponent.h"
#include "Misc/Paths.h"

typedef bool(*_SendTexture)(const unsigned char* data, int width, int height);
_SendTexture m_SendTextureFromDLL;
void* v_dllHandle;

bool UVirtualCamComponent::importDLL(FString folder, FString name)
{
	FString filePath = *FPaths::GamePluginsDir() + folder + "/" + name;

	if (FPaths::FileExists(filePath))
	{
		v_dllHandle = FPlatformProcess::GetDllHandle(*filePath);
		if (v_dllHandle != NULL)
		{
			return true;
		}
	}
	return false;
}

bool UVirtualCamComponent::importMethodSendTexture()
{
	if (v_dllHandle != NULL)
	{
		m_SendTextureFromDLL = NULL;
		FString procName = "SendTexture";
		m_SendTextureFromDLL = (_SendTexture)FPlatformProcess::GetDllExport(v_dllHandle, *procName);
		if (m_SendTextureFromDLL != NULL)
		{
			return true;
		}
	}
	return false;
}

bool UVirtualCamComponent::SendTextureFromDLL(const unsigned char* data, int width, int height)
{
	if (m_SendTextureFromDLL != NULL)
	{
		bool out = bool(m_SendTextureFromDLL(data, width, height));
		return out;
	}
	return false;
}

void UVirtualCamComponent::freeDLL()
{
	if (v_dllHandle != NULL)
	{
		m_SendTextureFromDLL = NULL;

		FPlatformProcess::FreeDllHandle(v_dllHandle);
		v_dllHandle = NULL;
	}
}

UVirtualCamComponent::UVirtualCamComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


void UVirtualCamComponent::SetWebcamTextureRender(UTextureRenderTarget2D* TextureRender)
{
	WebcamRenderTarget = TextureRender;
}

void UVirtualCamComponent::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	freeDLL();
}


// Called every frame
void UVirtualCamComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!loadedDLL)
	{
		GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Red, TEXT("Loading DLL"));
		loadedDLL = importDLL("ExternalDLL", "UnrealWebcam.dll");
	}
	else if (!loadedMethod)
	{
		GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Red, TEXT("Loading Method"));
		loadedMethod = importMethodSendTexture();
	}
	else
	{
		if (WebcamRenderTarget)
		{
			if (WebcamRenderTarget->Resource->bSRGB)
			{
				unsigned char* _data = new UCHAR[WebcamRenderTarget->SizeX * WebcamRenderTarget->SizeY * 3];
				UCHAR* src = (UCHAR*)WebcamRenderTarget->Resource;
				UCHAR* ptr = (UCHAR*)_data;

				GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Red, TEXT("sRGB"));
			}
			else
			{
				SendTextureFromDLL((UCHAR*)WebcamRenderTarget->Resource, WebcamRenderTarget->SizeX, WebcamRenderTarget->SizeY);
				GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Green, TEXT("TextureSent"));
			}
		}
	}
}

