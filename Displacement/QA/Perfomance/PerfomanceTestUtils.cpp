// Copyright Epic Games, Inc. All Rights Reserved.


#include "QA/Perfomance/PerfomanceTestUtils.h"

#include "JsonObjectConverter.h"

DEFINE_LOG_CATEGORY_STATIC(LogPerfomanceTestSerialization, All, All)

namespace Displacement
{
	namespace Test
	{
		bool WritePerfomanceTestData(const FString& _fileName, const FPerfomanceTestLevelMetrics& _testResult)
		{
			FString serializedData{};
			if(!FJsonObjectConverter::UStructToJsonObjectString(_testResult, serializedData)) return false;

			const FString fullPath{ PerfomanceTestReportsDirectory + "/" + _fileName + "/" + _fileName + FDateTime::Now().ToString() + OutputFileExtension };
			FFileHelper::SaveStringToFile(serializedData, *fullPath);
			
			return true;
		}

		bool ReadPerfomanceTestList(FPerfomanceTestRequestCollection& _request)
		{
			FString stringFile{};
			
			if (!FFileHelper::LoadFileToString(stringFile, *PerfomanceTestsList))
			{
				UE_LOG(LogPerfomanceTestSerialization, Error, TEXT("File opening error"));
				return false;
			}

			const TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(stringFile);
			TSharedPtr<FJsonObject> MainJsonObject{};
			if (!FJsonSerializer::Deserialize(JsonReader, MainJsonObject))
			{
				UE_LOG(LogPerfomanceTestSerialization, Error, TEXT("JSON deserialization error"));
				return false;
			}

			return FJsonObjectConverter::JsonObjectToUStruct(MainJsonObject.ToSharedRef(), &_request, 0, 0);
		}

		FString GetDiplayNameOfTransition(const ADP_PerfomancePoint_Actor* _fromPoint, const ADP_PerfomancePoint_Actor* _toPoint)
		{
			const FString fromPointName = _fromPoint->GetRegionName().ToString();
			const FString toPointName = _toPoint->GetRegionName().ToString();
			return FString::Printf(TEXT("Transition:%s--%s"), *fromPointName, *toPointName);
		}
	}
}