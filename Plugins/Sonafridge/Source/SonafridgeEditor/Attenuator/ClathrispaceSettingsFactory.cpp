#include "ClathrispaceSettingsFactory.h"
#include "Sonafridge/Attenuator/Clathrispace.h"
#include "UObject/ObjectMacros.h"
#include "UObject/Object.h"

UClathrispaceSettingsFactory::UClathrispaceSettingsFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SupportedClass = UClathrispaceSettings::StaticClass();

	bCreateNew = true;
	bEditorImport = false;
	bEditAfterNew = true;
}

UObject* UClathrispaceSettingsFactory::FactoryCreateNew(UClass*           Class,
                                                        UObject*          InParent,
                                                        FName             InName,
                                                        EObjectFlags      Flags,
                                                        UObject*          Context,
                                                        FFeedbackContext* Warn)
{
	return NewObject<UClathrispaceSettings>(InParent, InName, Flags);
}

uint32 UClathrispaceSettingsFactory::GetMenuCategories() const
{
	return EAssetTypeCategories::Sounds;
}


	
