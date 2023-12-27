// Copyright Epic Games, Inc. All Rights Reserved.


#include "QA/Perfomance/PerfomanceTestUtils.h"

#include "JsonObjectConverter.h"

DEFINE_LOG_CATEGORY_STATIC(LogPerfomanceTestSerialization, All, All)

namespace
{
	const int32 Precision = 3;
}

namespace Displacement
{
	namespace Test
	{
		void ConvertFloatToDisplayString(FString& _targetString, float _inValue)
		{
			_targetString.Reset();

			const FString valueString = FString::SanitizeFloat(_inValue, Precision);
			int32 foundIndex = -1;

			if(valueString.FindChar(TEXT('.'), foundIndex))
			{
				_targetString = valueString.Mid(0, foundIndex + Precision + 1);
			}
		}

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

		FString GetDiplayNameOfTransition(const FName& _fromName, const FName& _toName)
		{
			return FString::Printf(TEXT("Transition:%s--%s"), *_fromName.ToString(), *_toName.ToString());
		}
	}
}