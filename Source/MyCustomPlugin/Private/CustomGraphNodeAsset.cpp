#include "CustomGraphNodeAsset.h"

UCustomGraphNodeAsset::UCustomGraphNodeAsset()
{

}

bool UCustomGraphNodeAsset::IsLeafNode() const
{
	return ChildrenNodes.IsEmpty();
}