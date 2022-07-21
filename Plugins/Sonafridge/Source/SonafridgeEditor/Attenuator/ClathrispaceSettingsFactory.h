#pragma once

#include "Factories/Factory.h"
#include "ClathrispaceSettingsFactory.generated.h"

UCLASS(MinimalAPI, hidecategories = Object)
class UClathrispaceSettingsFactory : public UFactory
{
	GENERATED_UCLASS_BODY()
	virtual UObject* FactoryCreateNew(UClass*           Class,
	                                  UObject*          InParent,
	                                  FName             Name,
	                                  EObjectFlags      Flags,
	                                  UObject*          Context,
	                                  FFeedbackContext* Warn) override;

	virtual uint32 GetMenuCategories() const override;
};
