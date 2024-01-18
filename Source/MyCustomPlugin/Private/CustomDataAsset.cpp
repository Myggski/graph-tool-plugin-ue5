#include "CustomDataAsset.h"

const TArray<UCustomGraphNodeAsset*>& UCustomDataAsset::GetRootNodes() const
{
	return RootNodes;
}

int UCustomDataAsset::GetLevelNum() const
{
	int Level = 0;
	TArray<UCustomGraphNodeAsset*> CurrLevelNodes = RootNodes; TArray<UCustomGraphNodeAsset*> NextLevelNodes;

	while (CurrLevelNodes.Num() != 0)
	{
		for (int i = 0; i < CurrLevelNodes.Num(); ++i)
		{
			UCustomGraphNodeAsset* Node = CurrLevelNodes[i];
			check(Node != nullptr);

			for (int j = 0; j < Node->ChildrenNodes.Num(); ++j)
			{
				NextLevelNodes.Add(Node->ChildrenNodes[j]);
			}
		}

		CurrLevelNodes = NextLevelNodes;
		NextLevelNodes.Reset();
		++Level;
	}

	return Level;
}

const TArray<UCustomGraphNodeAsset*> UCustomDataAsset::GetNodesByLevel(int Level)
{
	int CurrLEvel = 0;
	TArray<UCustomGraphNodeAsset*> Nodes;
	TArray<UCustomGraphNodeAsset*> NextLevelNodes;

	Nodes = RootNodes;

	while (Nodes.Num() != 0)
	{
		if (CurrLEvel == Level)
			break;

		for (int i = 0; i < Nodes.Num(); ++i)
		{
			UCustomGraphNodeAsset* Node = Nodes[i];
			check(Node != nullptr);

			for (int j = 0; j < Node->ChildrenNodes.Num(); ++j)
			{
				NextLevelNodes.Add(Node->ChildrenNodes[j]);
			}
		}

		Nodes = NextLevelNodes;
		NextLevelNodes.Reset();
		++CurrLEvel;
	}

	return Nodes;
}

void UCustomDataAsset::ClearGraph()
{
	for (int i = 0; i < AllNodes.Num(); ++i)
	{
		UCustomGraphNodeAsset* Node = AllNodes[i];
		if (Node)
		{
			Node->ParentNodes.Empty();
			Node->ChildrenNodes.Empty();
		}
	}

	AllNodes.Empty();
	RootNodes.Empty();
}