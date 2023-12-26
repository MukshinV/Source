// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DP_PerfomancePoint_Actor.h"
#include "QA/Perfomance/PerfomanceTestTypes.h"

namespace Displacement
{
	namespace Test
	{
#if WITH_EDITOR
		const FString DisplacementRepositoryDir = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir() + TEXT("/.."));
#else
		const FString DisplacementRepositoryDir = FPaths::ConvertRelativePathToFull(FPaths::LaunchDir() + TEXT("/../../"));
#endif

		const FString PerfomanceTestReportsDirectory = DisplacementRepositoryDir + TEXT("/QA/Jsons/");
		const FString PerfomanceTestsList = DisplacementRepositoryDir + TEXT("/QA/Data/PerfomanceMaps.json");
		const FString OutputFileExtension = TEXT(".json");
		
		FORCEINLINE float SecondsToMilliseconds(float _timeSeconds) { return _timeSeconds * 1000.0f; }
		void ConvertFloatToDisplayString(FString& _targetString, float _inValue);
		bool WritePerfomanceTestData(const FString& _fileName, const FPerfomanceTestLevelMetrics& _testResult);
		bool ReadPerfomanceTestList(FPerfomanceTestRequestCollection& _requests);
		FString GetDiplayNameOfTransition(const ADP_PerfomancePoint_Actor* _fromPoint, const ADP_PerfomancePoint_Actor* _toPoint);
		FString GetDiplayNameOfTransition(const FName& _fromName, const FName& _toName);

	}
}