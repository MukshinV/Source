// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "QA/Perfomance/PerfomanceTestTypes.h"

namespace Displacement
{
	namespace Test
	{
		bool WritePerfomanceTestData(const FString& _fileName, const FPerfomanceTestData& _testResult);
		bool ReadPerfomanceTestData(const FString& _fileName, FPerfomanceTestData& _testResult);
	}
}