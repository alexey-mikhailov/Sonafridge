// Sonafridge 2022

#include "SonafridgeEditorCommon.h"
#include "Kismet/KismetTextLibrary.h"

FText FloatToText(float                       InValue,
                  int32                       NumDigits,
                  SonafridgeEditor::ESignMode SignMode)
{
	return UKismetTextLibrary::Conv_FloatToText(InValue,
	                                            HalfFromZero,
	                                            SignMode == SonafridgeEditor::ESignMode::Always,
	                                            false,
	                                            1,
	                                            324,
	                                            NumDigits,
	                                            NumDigits);
}
