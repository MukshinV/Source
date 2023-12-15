// Copyright Epic Games, Inc. All Rights Reserved.


#include "QA/Perfomance/PerfomanceTestUtils.h"
#include "JsonObjectConverter.h"

DEFINE_LOG_CATEGORY_STATIC(LogPerfomanceTestSerialization, All, All)

namespace
{
	const FString PerfomanceTestReportsDirectory = FPaths::ProjectDir() + TEXT("/QA/PerfomanceReports/");
	const FString PerfomanceTestsList = FPaths::ProjectDir() + TEXT("/QA/Data/PerfomanceMaps.json");
	const FString OutputFileExtension = TEXT(".json");
}

namespace Displacement
{
	namespace Test
	{
		bool WritePerfomanceTestData(const FString& _fileName, const FPerfomanceTestLevelData& _testResult)
		{
			FString serializedData{};
			if(!FJsonObjectConverter::UStructToJsonObjectString(_testResult, serializedData)) return false;

			const FString fullPath{ PerfomanceTestReportsDirectory + FDateTime::Now().GetDate().ToString() + "/" + _fileName + OutputFileExtension };
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
	}
}