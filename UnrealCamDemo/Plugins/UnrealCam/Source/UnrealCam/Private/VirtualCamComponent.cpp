// Fill out your copyright notice in the Description page of Project Settings.
#include "VirtualCamComponent.h"
#include "Misc/Paths.h"

typedef bool(*_getSendTexture)(const unsigned char* data, int width, int height);
_getSendTexture m_getSendTextureFromDLL;
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
		m_getSendTextureFromDLL = NULL;
		FString procName = "SendTexture";
		m_getSendTextureFromDLL = (_getSendTexture)FPlatformProcess::GetDllExport(v_dllHandle, *procName);
		if (m_getSendTextureFromDLL != NULL)
		{
			return true;
		}
	}
	return false;
}

bool UVirtualCamComponent::SendTextureFromDLL(const unsigned char* data, int width, int height)
{
	if (m_getSendTextureFromDLL != NULL)
	{
		bool out = bool(m_getSendTextureFromDLL(data, width, height));
		return out;
	}
	return false;
}

void UVirtualCamComponent::freeDLL()
{
	if (v_dllHandle != NULL)
	{
		m_getSendTextureFromDLL = NULL;

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

// Called when the game starts
void UVirtualCamComponent::BeginPlay()
{
	Super::BeginPlay();
	
	importDLL("ExternalDLL", "UnrealWebcam.dll");
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

	if (!loadedMethod)
	{
		loadedMethod = importMethodSendTexture();
		GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Red, TEXT("Loading DLL"));
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

