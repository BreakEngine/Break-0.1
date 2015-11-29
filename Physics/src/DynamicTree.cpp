#include "DynamicTree.hpp"

using namespace Break;
using namespace Break::Infrastructure;
using namespace Break::Physics;


DynamicTree::DynamicTree()
{
	m_root = _nullNode;

	m_nodeCapacity = 16;
	m_nodeCount = 0;
	m_nodes = (TreeNode*)malloc(m_nodeCapacity * sizeof(TreeNode));
	memset(m_nodes, 0, m_nodeCapacity * sizeof(TreeNode));

	// Build a linked list for the free list.
	for (s32 i = 0; i < m_nodeCapacity - 1; ++i)
	{
		m_nodes[i].next = i + 1;
		m_nodes[i].height = -1;
	}
	m_nodes[m_nodeCapacity-1].next = _nullNode;
	m_nodes[m_nodeCapacity-1].height = -1;
	m_freeList = 0;

	m_path = 0;

	m_insertionCount = 0;
}

DynamicTree::~DynamicTree()
{
	// This frees the entire tree in one shot.
	free(m_nodes);
}

// Allocate a node from the pool. Grow the pool if necessary.
s32 DynamicTree::AllocateNode()
{
	// Expand the node pool as needed.
	if (m_freeList == _nullNode)
	{
		assert(m_nodeCount == m_nodeCapacity);

		// The free list is empty. Rebuild a bigger pool.
		TreeNode* oldNodes = m_nodes;
		m_nodeCapacity *= 2;
		m_nodes = (TreeNode*)malloc(m_nodeCapacity * sizeof(TreeNode));
		memcpy(m_nodes, oldNodes, m_nodeCount * sizeof(TreeNode));
		free(oldNodes);

		// Build a linked list for the free list. The parent
		// pointer becomes the "next" pointer.
		for (s32 i = m_nodeCount; i < m_nodeCapacity - 1; ++i)
		{
			m_nodes[i].next = i + 1;
			m_nodes[i].height = -1;
		}
		m_nodes[m_nodeCapacity-1].next = _nullNode;
		m_nodes[m_nodeCapacity-1].height = -1;
		m_freeList = m_nodeCount;
	}

	// Peel a node off the free list.
	s32 nodeId = m_freeList;
	m_freeList = m_nodes[nodeId].next;
	m_nodes[nodeId].parent = _nullNode;
	m_nodes[nodeId].child1 = _nullNode;
	m_nodes[nodeId].child2 = _nullNode;
	m_nodes[nodeId].height = 0;
	m_nodes[nodeId].userData = NULL;
	++m_nodeCount;
	return nodeId;
}

// Return a node to the pool.
void DynamicTree::FreeNode(s32 nodeId)
{
	assert(0 <= nodeId && nodeId < m_nodeCapacity);
	assert(0 < m_nodeCount);
	m_nodes[nodeId].next = m_freeList;
	m_nodes[nodeId].height = -1;
	m_freeList = nodeId;
	--m_nodeCount;
}

// Create a proxy in the tree as a leaf node. We return the index
// of the node instead of a pointer so that we can grow
// the node pool.
s32 DynamicTree::CreateProxy(const AABB& aabb, void* userData)
{
	s32 proxyId = AllocateNode();

	// Fatten the aabb.
	glm::vec2 r(aabbExtension, aabbExtension);   ///<<<-----aabb extinsion use..

	m_nodes[proxyId].aabb.lowerBound = aabb.lowerBound - r;
	m_nodes[proxyId].aabb.upperBound = aabb.upperBound + r;
	m_nodes[proxyId].userData = userData;
	m_nodes[proxyId].height = 0;

	InsertLeaf(proxyId);

	return proxyId;
}

void DynamicTree::DestroyProxy(s32 proxyId)
{
	assert(0 <= proxyId && proxyId < m_nodeCapacity);
	assert(m_nodes[proxyId].IsLeaf());

	RemoveLeaf(proxyId);
	FreeNode(proxyId);
}

bool DynamicTree::MoveProxy(s32 proxyId, const AABB& aabb, const glm::vec2& displacement)
{
	assert(0 <= proxyId && proxyId < m_nodeCapacity);

	assert(m_nodes[proxyId].IsLeaf());

	if (m_nodes[proxyId].aabb.Contains(aabb))
	{
		return false;
	}

	RemoveLeaf(proxyId);

	// Extend AABB.
	AABB b = aabb;
	glm::vec2 r(aabbExtension, aabbExtension);
	b.lowerBound = b.lowerBound - r;
	b.upperBound = b.upperBound + r;

	// Predict AABB displacement.
	glm::vec2 d = aabbMultiplier * displacement;

	if (d.x < 0.0f)
	{
		b.lowerBound.x += d.x;
	}
	else
	{
		b.upperBound.x += d.x;
	}

	if (d.y < 0.0f)
	{
		b.lowerBound.y += d.y;
	}
	else
	{
		b.upperBound.y += d.y;
	}

	m_nodes[proxyId].aabb = b;

	InsertLeaf(proxyId);
	return true;
}

void DynamicTree::InsertLeaf(s32 leaf)
{
	++m_insertionCount;

	if (m_root == _nullNode)
	{
		m_root = leaf;
		m_nodes[m_root].parent = _nullNode;
		return;
	}

	// Find the best sibling for this node
	AABB leafAABB = m_nodes[leaf].aabb;
	s32 index = m_root;
	while (m_nodes[index].IsLeaf() == false)
	{
		s32 child1 = m_nodes[index].child1;
		s32 child2 = m_nodes[index].child2;

		real32 area = m_nodes[index].aabb.GetPerimeter();

		AABB combinedAABB;
		combinedAABB.Combine(m_nodes[index].aabb, leafAABB);
		real32 combinedArea = combinedAABB.GetPerimeter();

		// Cost of creating a new parent for this node and the new leaf
		real32 cost = 2.0f * combinedArea;

		// Minimum cost of pushing the leaf further down the tree
		real32 inheritanceCost = 2.0f * (combinedArea - area);

		// Cost of descending into child1
		real32 cost1;
		if (m_nodes[child1].IsLeaf())
		{
			AABB aabb;
			aabb.Combine(leafAABB, m_nodes[child1].aabb);
			cost1 = aabb.GetPerimeter() + inheritanceCost;
		}
		else
		{
			AABB aabb;
			aabb.Combine(leafAABB, m_nodes[child1].aabb);
			real32 oldArea = m_nodes[child1].aabb.GetPerimeter();
			real32 newArea = aabb.GetPerimeter();
			cost1 = (newArea - oldArea) + inheritanceCost;
		}

		// Cost of descending into child2
		real32 cost2;
		if (m_nodes[child2].IsLeaf())
		{
			AABB aabb;
			aabb.Combine(leafAABB, m_nodes[child2].aabb);
			cost2 = aabb.GetPerimeter() + inheritanceCost;
		}
		else
		{
			AABB aabb;
			aabb.Combine(leafAABB, m_nodes[child2].aabb);
			real32 oldArea = m_nodes[child2].aabb.GetPerimeter();
			real32 newArea = aabb.GetPerimeter();
			cost2 = newArea - oldArea + inheritanceCost;
		}

		// Descend according to the minimum cost.
		if (cost < cost1 && cost < cost2)
		{
			break;
		}

		// Descend
		if (cost1 < cost2)
		{
			index = child1;
		}
		else
		{
			index = child2;
		}
	}

	s32 sibling = index;

	// Create a new parent.
	s32 oldParent = m_nodes[sibling].parent;
	s32 newParent = AllocateNode();
	m_nodes[newParent].parent = oldParent;
	m_nodes[newParent].userData = NULL;
	m_nodes[newParent].aabb.Combine(leafAABB, m_nodes[sibling].aabb);
	m_nodes[newParent].height = m_nodes[sibling].height + 1;

	if (oldParent != _nullNode)
	{
		// The sibling was not the root.
		if (m_nodes[oldParent].child1 == sibling)
		{
			m_nodes[oldParent].child1 = newParent;
		}
		else
		{
			m_nodes[oldParent].child2 = newParent;
		}

		m_nodes[newParent].child1 = sibling;
		m_nodes[newParent].child2 = leaf;
		m_nodes[sibling].parent = newParent;
		m_nodes[leaf].parent = newParent;
	}
	else
	{
		// The sibling was the root.
		m_nodes[newParent].child1 = sibling;
		m_nodes[newParent].child2 = leaf;
		m_nodes[sibling].parent = newParent;
		m_nodes[leaf].parent = newParent;
		m_root = newParent;
	}

	// Walk back up the tree fixing heights and AABBs
	index = m_nodes[leaf].parent;
	while (index != _nullNode)
	{
		index = Balance(index);

		s32 child1 = m_nodes[index].child1;
		s32 child2 = m_nodes[index].child2;

		assert(child1 != _nullNode);
		assert(child2 != _nullNode);

		m_nodes[index].height = 1 + glm::max(m_nodes[child1].height, m_nodes[child2].height);
		m_nodes[index].aabb.Combine(m_nodes[child1].aabb, m_nodes[child2].aabb);

		index = m_nodes[index].parent;
	}

	//Validate();
}

void DynamicTree::RemoveLeaf(s32 leaf)
{
	if (leaf == m_root)
	{
		m_root = _nullNode;
		return;
	}

	s32 parent = m_nodes[leaf].parent;
	s32 grandParent = m_nodes[parent].parent;
	s32 sibling;
	if (m_nodes[parent].child1 == leaf)
	{
		sibling = m_nodes[parent].child2;
	}
	else
	{
		sibling = m_nodes[parent].child1;
	}

	if (grandParent != _nullNode)
	{
		// Destroy parent and connect sibling to grandParent.
		if (m_nodes[grandParent].child1 == parent)
		{
			m_nodes[grandParent].child1 = sibling;
		}
		else
		{
			m_nodes[grandParent].child2 = sibling;
		}
		m_nodes[sibling].parent = grandParent;
		FreeNode(parent);

		// Adjust ancestor bounds.
		s32 index = grandParent;
		while (index != _nullNode)
		{
			index = Balance(index);

			s32 child1 = m_nodes[index].child1;
			s32 child2 = m_nodes[index].child2;

			m_nodes[index].aabb.Combine(m_nodes[child1].aabb, m_nodes[child2].aabb);
			m_nodes[index].height = 1 + glm::max(m_nodes[child1].height, m_nodes[child2].height);

			index = m_nodes[index].parent;
		}
	}
	else
	{
		m_root = sibling;
		m_nodes[sibling].parent = _nullNode;
		FreeNode(parent);
	}

	//Validate();
}

// Perform a left or right rotation if node A is imbalanced.
// Returns the new root index.
s32 DynamicTree::Balance(s32 iA)
{
	assert(iA != _nullNode);

	TreeNode* A = m_nodes + iA;
	if (A->IsLeaf() || A->height < 2)
	{
		return iA;
	}

	s32 iB = A->child1;
	s32 iC = A->child2;
	assert(0 <= iB && iB < m_nodeCapacity);
	assert(0 <= iC && iC < m_nodeCapacity);

	TreeNode* B = m_nodes + iB;
	TreeNode* C = m_nodes + iC;

	s32 balance = C->height - B->height;

	// Rotate C up
	if (balance > 1)
	{
		s32 iF = C->child1;
		s32 iG = C->child2;
		TreeNode* F = m_nodes + iF;
		TreeNode* G = m_nodes + iG;
		assert(0 <= iF && iF < m_nodeCapacity);
		assert(0 <= iG && iG < m_nodeCapacity);

		// Swap A and C
		C->child1 = iA;
		C->parent = A->parent;
		A->parent = iC;

		// A's old parent should point to C
		if (C->parent != _nullNode)
		{
			if (m_nodes[C->parent].child1 == iA)
			{
				m_nodes[C->parent].child1 = iC;
			}
			else
			{
				assert(m_nodes[C->parent].child2 == iA);
				m_nodes[C->parent].child2 = iC;
			}
		}
		else
		{
			m_root = iC;
		}

		// Rotate
		if (F->height > G->height)
		{
			C->child2 = iF;
			A->child2 = iG;
			G->parent = iA;
			A->aabb.Combine(B->aabb, G->aabb);
			C->aabb.Combine(A->aabb, F->aabb);

			A->height = 1 + glm::max(B->height, G->height);
			C->height = 1 + glm::max(A->height, F->height);
		}
		else
		{
			C->child2 = iG;
			A->child2 = iF;
			F->parent = iA;
			A->aabb.Combine(B->aabb, F->aabb);
			C->aabb.Combine(A->aabb, G->aabb);

			A->height = 1 + glm::max(B->height, F->height);
			C->height = 1 + glm::max(A->height, G->height);
		}

		return iC;
	}

	// Rotate B up
	if (balance < -1)
	{
		s32 iD = B->child1;
		s32 iE = B->child2;
		TreeNode* D = m_nodes + iD;
		TreeNode* E = m_nodes + iE;
		assert(0 <= iD && iD < m_nodeCapacity);
		assert(0 <= iE && iE < m_nodeCapacity);

		// Swap A and B
		B->child1 = iA;
		B->parent = A->parent;
		A->parent = iB;

		// A's old parent should point to B
		if (B->parent != _nullNode)
		{
			if (m_nodes[B->parent].child1 == iA)
			{
				m_nodes[B->parent].child1 = iB;
			}
			else
			{
				assert(m_nodes[B->parent].child2 == iA);
				m_nodes[B->parent].child2 = iB;
			}
		}
		else
		{
			m_root = iB;
		}

		// Rotate
		if (D->height > E->height)
		{
			B->child2 = iD;
			A->child1 = iE;
			E->parent = iA;
			A->aabb.Combine(C->aabb, E->aabb);
			B->aabb.Combine(A->aabb, D->aabb);

			A->height = 1 + glm::max(C->height, E->height);
			B->height = 1 + glm::max(A->height, D->height);
		}
		else
		{
			B->child2 = iE;
			A->child1 = iD;
			D->parent = iA;
			A->aabb.Combine(C->aabb, D->aabb);
			B->aabb.Combine(A->aabb, E->aabb);

			A->height = 1 + glm::max(C->height, D->height);
			B->height = 1 + glm::max(A->height, E->height);
		}

		return iB;
	}

	return iA;
}

s32 DynamicTree::GetHeight() const
{
	if (m_root == _nullNode)
	{
		return 0;
	}

	return m_nodes[m_root].height;
}

//
real32 DynamicTree::GetAreaRatio() const
{
	if (m_root == _nullNode)
	{
		return 0.0f;
	}

	const TreeNode* root = m_nodes + m_root;
	real32 rootArea = root->aabb.GetPerimeter();

	real32 totalArea = 0.0f;
	for (s32 i = 0; i < m_nodeCapacity; ++i)
	{
		const TreeNode* node = m_nodes + i;
		if (node->height < 0)
		{
			// Free node in pool
			continue;
		}

		totalArea += node->aabb.GetPerimeter();
	}

	return totalArea / rootArea;
}

// Compute the height of a sub-tree.
s32 DynamicTree::ComputeHeight(s32 nodeId) const
{
	assert(0 <= nodeId && nodeId < m_nodeCapacity);
	TreeNode* node = m_nodes + nodeId;

	if (node->IsLeaf())
	{
		return 0;
	}

	s32 height1 = ComputeHeight(node->child1);
	s32 height2 = ComputeHeight(node->child2);
	return 1 + glm::max(height1, height2);
}

s32 DynamicTree::ComputeHeight() const
{
	s32 height = ComputeHeight(m_root);
	return height;
}

void DynamicTree::ValidateStructure(s32 index) const
{
	if (index == _nullNode)
	{
		return;
	}

	if (index == m_root)
	{
		assert(m_nodes[index].parent == _nullNode);
	}

	const TreeNode* node = m_nodes + index;

	s32 child1 = node->child1;
	s32 child2 = node->child2;

	if (node->IsLeaf())
	{
		assert(child1 == _nullNode);
		assert(child2 == _nullNode);
		assert(node->height == 0);
		return;
	}

	assert(0 <= child1 && child1 < m_nodeCapacity);
	assert(0 <= child2 && child2 < m_nodeCapacity);

	assert(m_nodes[child1].parent == index);
	assert(m_nodes[child2].parent == index);

	ValidateStructure(child1);
	ValidateStructure(child2);
}

void DynamicTree::ValidateMetrics(s32 index) const
{
	if (index == _nullNode)
	{
		return;
	}

	const TreeNode* node = m_nodes + index;

	s32 child1 = node->child1;
	s32 child2 = node->child2;

	if (node->IsLeaf())
	{
		assert(child1 == _nullNode);
		assert(child2 == _nullNode);
		assert(node->height == 0);
		return;
	}

	assert(0 <= child1 && child1 < m_nodeCapacity);
	assert(0 <= child2 && child2 < m_nodeCapacity);

	s32 height1 = m_nodes[child1].height;
	s32 height2 = m_nodes[child2].height;
	s32 height;
	height = 1 + glm::max(height1, height2);
	assert(node->height == height);

	AABB aabb;
	aabb.Combine(m_nodes[child1].aabb, m_nodes[child2].aabb);

	assert(aabb.lowerBound == node->aabb.lowerBound);
	assert(aabb.upperBound == node->aabb.upperBound);

	ValidateMetrics(child1);
	ValidateMetrics(child2);
}

void DynamicTree::Validate() const
{
	ValidateStructure(m_root);
	ValidateMetrics(m_root);

	s32 freeCount = 0;
	s32 freeIndex = m_freeList;
	while (freeIndex != _nullNode)
	{
		assert(0 <= freeIndex && freeIndex < m_nodeCapacity);
		freeIndex = m_nodes[freeIndex].next;
		++freeCount;
	}

	assert(GetHeight() == ComputeHeight());

	assert(m_nodeCount + freeCount == m_nodeCapacity);
}

s32 DynamicTree::GetMaxBalance() const
{
	s32 maxBalance = 0;
	for (s32 i = 0; i < m_nodeCapacity; ++i)
	{
		const TreeNode* node = m_nodes + i;
		if (node->height <= 1)
		{
			continue;
		}

		assert(node->IsLeaf() == false);

		s32 child1 = node->child1;
		s32 child2 = node->child2;
		s32 balance = glm::abs(m_nodes[child2].height - m_nodes[child1].height);
		maxBalance = glm::max(maxBalance, balance);
	}

	return maxBalance;
}

void DynamicTree::RebuildBottomUp()
{
	s32* nodes = (s32*)malloc(m_nodeCount * sizeof(s32));
	s32 count = 0;

	// Build array of leaves. Free the rest.
	for (s32 i = 0; i < m_nodeCapacity; ++i)
	{
		if (m_nodes[i].height < 0)
		{
			// free node in pool
			continue;
		}

		if (m_nodes[i].IsLeaf())
		{
			m_nodes[i].parent = _nullNode;
			nodes[count] = i;
			++count;
		}
		else
		{
			FreeNode(i);
		}
	}

	while (count > 1)
	{
		real32 minCost = FLT_MAX;
		s32 iMin = -1, jMin = -1;
		for (s32 i = 0; i < count; ++i)
		{
			AABB aabbi = m_nodes[nodes[i]].aabb;

			for (s32 j = i + 1; j < count; ++j)
			{
				AABB aabbj = m_nodes[nodes[j]].aabb;
				AABB b;
				b.Combine(aabbi, aabbj);
				real32 cost = b.GetPerimeter();
				if (cost < minCost)
				{
					iMin = i;
					jMin = j;
					minCost = cost;
				}
			}
		}

		s32 index1 = nodes[iMin];
		s32 index2 = nodes[jMin];
		TreeNode* child1 = m_nodes + index1;
		TreeNode* child2 = m_nodes + index2;

		s32 parentIndex = AllocateNode();
		TreeNode* parent = m_nodes + parentIndex;
		parent->child1 = index1;
		parent->child2 = index2;
		parent->height = 1 + glm::max(child1->height, child2->height);
		parent->aabb.Combine(child1->aabb, child2->aabb);
		parent->parent = _nullNode;

		child1->parent = parentIndex;
		child2->parent = parentIndex;

		nodes[jMin] = nodes[count-1];
		nodes[iMin] = parentIndex;
		--count;
	}

	m_root = nodes[0];
	free(nodes);

	Validate();
}

void DynamicTree::ShiftOrigin(const glm::vec2& newOrigin)
{
	// Build array of leaves. Free the rest.
	for (s32 i = 0; i < m_nodeCapacity; ++i)
	{
		m_nodes[i].aabb.lowerBound -= newOrigin;
		m_nodes[i].aabb.upperBound -= newOrigin;
	}
}
