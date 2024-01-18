#include "AssetEditor/EdCustomGraph.h"
#include "CustomDataAsset.h"

UCustomDataAsset* UEdCustomGraph::GetCustomData() const
{
	return CastChecked<UCustomDataAsset>(GetOuter());
}

bool UEdCustomGraph::Modify(bool bAlwaysMarkDirty /*= true*/)
{
	bool Rtn = Super::Modify(bAlwaysMarkDirty);

	GetCustomData()->Modify();

	for (int32 i = 0; i < Nodes.Num(); ++i)
	{
		Nodes[i]->Modify();
	}

	return Rtn;
}


void UEdCustomGraph::PostEditUndo()
{
	Super::PostEditUndo();

	NotifyGraphChanged();
}