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
		bool WritePerfomanceTestData(const FString& _fileName, const FPerfomanceTestLevelData& _testResult)
		{
			FString serializedData{};
			if(!FJsonObjectConverter::UStructToJsonObjectString(_testResult, serializedData)) return false;

			const FString fullPath{ PerfomanceTestReportsDirectory + FDateTime::Now().GetDate().ToString() + "/" + _fileName + OutputFileExtension };
			FFileHelper::SaveStringToFile(serializedData, *fullPath);
			
			return true;
		}
	}
}