// Copyright Epic Games, Inc. All Rights Reserved.


#include "QA/Perfomance/PerfomanceTestUtils.h"
#include "JsonObjectConverter.h"

DEFINE_LOG_CATEGORY_STATIC(LogPerfomanceTestSerialization, All, All)

namespace
{
	const FString PerfomanceTestReportsDirectory = FPaths::ProjectDir() + TEXT("/QA/PerfomanceReports/");
	const FString OutputFileExtension = TEXT(".json");
}

namespace Displacement
{
	namespace Test
	{
		bool WritePerfomanceTestData(const FString& _fileName, const FPerfomanceTestData& _testResult)
		{
			FString serializedData{};
			if(!FJsonObjectConverter::UStructToJsonObjectString(_testResult, serializedData)) return false;

			const FString fullPath{ PerfomanceTestReportsDirectory + _fileName + OutputFileExtension };
			FFileHelper::SaveStringToFile(serializedData, *fullPath);
			
			return true;
		}
		
		bool ReadPerfomanceTestData(const FString& _fileName, FPerfomanceTestData& _testResult)
		{
			FString fileContentString{};
			const FString fullPath{ PerfomanceTestReportsDirectory + _fileName + OutputFileExtension };
			
			if (!FFileHelper::LoadFileToString(fileContentString, *fullPath)) return false;

			TSharedPtr<FJsonObject> jsonObject{};
			TSharedRef<TJsonReader<>> jsonReader = TJsonReaderFactory<>::Create(fileContentString);
			
			if (!FJsonSerializer::Deserialize(jsonReader, jsonObject)) return false;

			return FJsonObjectConverter::JsonObjectToUStruct(jsonObject.ToSharedRef(), &_testResult, 0, 0);
		}
	}
}