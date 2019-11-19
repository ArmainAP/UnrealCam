// Fill out your copyright notice in the Description page of Project Settings.
#include "VirtualCamComponent.h"
#include "Misc/Paths.h"
#include "Engine/Classes/Kismet/KismetRenderingLibrary.h"

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
		GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Red, TEXT("Importing DLL"));
		loadedDLL = importDLL("ExternalDLL", "UnrealWebcam.dll");
	}
	else if (!loadedMethod)
	{
		GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Red, TEXT("Importing Method"));
		loadedMethod = importMethodSendTexture();
	}
	else
	{
		if (WebcamRenderTexture)
		{
			FTextureRenderTarget2DResource* textureResource = (FTextureRenderTarget2DResource*)WebcamRenderTexture->Resource;
			TArray<FColor> ColorBuffer;
			if (textureResource->ReadPixels(ColorBuffer))
			{
				UCHAR* data = new UCHAR[textureResource->GetSizeX() * textureResource->GetSizeY() * 3];
				for (uint32 i = 0; i < textureResource->GetSizeX() * textureResource->GetSizeY(); i++)
				{
					data[i * 3 + 2] = ColorBuffer[i].R;
					data[i * 3 + 1] = ColorBuffer[i].G;
					data[i * 3 + 0] = ColorBuffer[i].B;
				}

				int i = textureResource->GetSizeX() * textureResource->GetSizeY() * 3 - 1;
				int j = 0;
				while (i > j)
				{
					auto temp = data[i];
					data[i] = data[j];
					data[j] = temp;
					i--; j++;
				}

				bool response = SendTextureFromDLL(data, textureResource->GetSizeX(), textureResource->GetSizeY());
			}
		}
	}
}

