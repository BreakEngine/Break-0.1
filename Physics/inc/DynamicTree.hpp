#pragma once
#include "Collision.hpp"
#include "GrowableStack.hpp"

namespace Break
{
	namespace Physics
	{

#define _nullNode (-1)

		/// A node in the dynamic tree. The client does not interact with this directly.
		struct BREAK_API TreeNode
		{
			bool IsLeaf() const
			{
				return child1 == _nullNode;
			}

			/// Enlarged AABB
			AABB aabb;

			void* userData;

			union
			{
				s32 parent;
				s32 next;
			};

			s32 child1;
			s32 child2;

			// leaf = 0, free node = -1
			s32 height;
		};

		/// A dynamic AABB tree broad-phase, inspired by Nathanael Presson's btDbvt.
		/// A dynamic tree arranges data in a binary tree to accelerate
		/// queries such as volume queries and ray casts. Leafs are proxies
		/// with an AABB. In the tree we expand the proxy AABB by _fatAABBFactor
		/// so that the proxy AABB is bigger than the client object. This allows the client
		/// object to move by small amounts without triggering a tree update.
		///
		/// Nodes are pooled and relocatable, so we use node indices rather than pointers.
		class BREAK_API DynamicTree
		{
		public:
			/// Constructing the tree initializes the node pool.
			DynamicTree();

			/// Destroy the tree, freeing the node pool.
			~DynamicTree();

			/// Create a proxy. Provide a tight fitting AABB and a userData pointer.
			s32 CreateProxy(const AABB& aabb, void* userData);

			/// Destroy a proxy. This asserts if the id is invalid.
			void DestroyProxy(s32 proxyId);

			/// Move a proxy with a swepted AABB. If the proxy has moved outside of its fattened AABB,
			/// then the proxy is removed from the tree and re-inserted. Otherwise
			/// the function returns immediately.
			/// @return true if the proxy was re-inserted.
			bool MoveProxy(s32 proxyId, const AABB& aabb1, const glm::vec2& displacement);

			/// Get proxy user data.
			/// @return the proxy user data or 0 if the id is invalid.
			void* GetUserData(s32 proxyId) const;

			/// Get the fat AABB for a proxy.
			const AABB& GetFatAABB(s32 proxyId) const;

			/// Query an AABB for overlapping proxies. The callback class
			/// is called for each proxy that overlaps the supplied AABB.
			template <typename T>
			void Query(T* callback, const AABB& aabb) const;

			/// Ray-cast against the proxies in the tree. This relies on the callback
			/// to perform a exact ray-cast in the case were the proxy contains a shape.
			/// The callback also performs the any collision filtering. This has performance
			/// roughly equal to k * log(n), where k is the number of collisions and n is the
			/// number of proxies in the tree.
			/// @param input the ray-cast input data. The ray extends from p1 to p1 + maxFraction * (p2 - p1).
			/// @param callback a callback class that is called for each proxy that is hit by the ray.
			template <typename T>
			void RayCast(T* callback, const RayCastInput& input) const;

			/// Validate this tree. For testing.
			void Validate() const;

			/// Compute the height of the binary tree in O(N) time. Should not be
			/// called often.
			s32 GetHeight() const;

			/// Get the maximum balance of an node in the tree. The balance is the difference
			/// in height of the two children of a node.
			s32 GetMaxBalance() const;

			/// Get the ratio of the sum of the node areas to the root area.
			real32 GetAreaRatio() const;

			/// Build an optimal tree. Very expensive. For testing.
			void RebuildBottomUp();

			/// Shift the world origin. Useful for large worlds.
			/// The shift formula is: position -= newOrigin
			/// @param newOrigin the new origin with respect to the old origin
			void ShiftOrigin(const glm::vec2& newOrigin);

		private:

			s32 AllocateNode();
			void FreeNode(s32 node);

			void InsertLeaf(s32 node);
			void RemoveLeaf(s32 node);

			s32 Balance(s32 index);

			s32 ComputeHeight() const;
			s32 ComputeHeight(s32 nodeId) const;

			void ValidateStructure(s32 index) const;
			void ValidateMetrics(s32 index) const;

			s32 m_root;

			TreeNode* m_nodes;
			s32 m_nodeCount;
			s32 m_nodeCapacity;

			s32 m_freeList;

			/// This is used to incrementally traverse the tree for re-balancing.
			u32 m_path;

			s32 m_insertionCount;
		};



		inline void* DynamicTree::GetUserData(s32 proxyId) const
		{
			assert(0 <= proxyId && proxyId < m_nodeCapacity);
			return m_nodes[proxyId].userData;
		}

		inline const AABB& DynamicTree::GetFatAABB(s32 proxyId) const
		{
			assert(0 <= proxyId && proxyId < m_nodeCapacity);
			return m_nodes[proxyId].aabb;
		}

		template <typename T>
		inline void DynamicTree::Query(T* callback, const AABB& aabb) const
		{
			GrowableStack<s32, 256> stack;
			stack.Push(m_root);

			while (stack.GetCount() > 0)
			{
				s32 nodeId = stack.Pop();
				if (nodeId == _nullNode)
				{
					continue;
				}

				const TreeNode* node = m_nodes + nodeId;

				if (TestOverlap(node->aabb, aabb))
				{
					if (node->IsLeaf())
					{
						bool proceed = callback->QueryCallback(nodeId);
						if (proceed == false)
						{
							return;
						}
					}
					else
					{
						stack.Push(node->child1);
						stack.Push(node->child2);
					}
				}
			}
		}

		template <typename T>
		inline void DynamicTree::RayCast(T* callback, const RayCastInput& input) const
		{
			glm::vec2 p1 = input.p1;
			glm::vec2 p2 = input.p2;
			glm::vec2 r = p2 - p1;
			assert( MathUtils::LengthSquared(r) > 0.0f);
			r = glm::normalize(r);

			// v is perpendicular to the segment.
			glm::vec2 v = MathUtils::Cross2(1.0f, r);
			glm::vec2 abs_v = glm::abs(v);

			// Separating axis for segment (Gino, p80).
			// |dot(v, p1 - c)| > dot(|v|, h)

			real32 maxFraction = input.maxFraction;

			// Build a bounding box for the segment.
			AABB segmentAABB;
			{
				glm::vec2 t = p1 + maxFraction * (p2 - p1);
				segmentAABB.lowerBound = glm::min(p1, t);
				segmentAABB.upperBound = glm::max(p1, t);
			}

			GrowableStack<s32, 256> stack;
			stack.Push(m_root);

			while (stack.GetCount() > 0)
			{
				s32 nodeId = stack.Pop();
				if (nodeId == _nullNode)
				{
					continue;
				}

				const TreeNode* node = m_nodes + nodeId;

				if (TestOverlap(node->aabb, segmentAABB) == false)
				{
					continue;
				}

				// Separating axis for segment (Gino, p80).
				// |dot(v, p1 - c)| > dot(|v|, h)
				glm::vec2 c = node->aabb.GetCenter();
				glm::vec2 h = node->aabb.GetExtents();
				real32 separation = glm::abs(glm::dot(v, p1 - c)) - glm::dot(abs_v, h);
				if (separation > 0.0f)
				{
					continue;
				}

				if (node->IsLeaf())
				{
					RayCastInput subInput;
					subInput.p1 = input.p1;
					subInput.p2 = input.p2;
					subInput.maxFraction = maxFraction;

					real32 value = callback->RayCastCallback(subInput, nodeId);

					if (value == 0.0f)
					{
						// The client has terminated the ray cast.
						return;
					}

					if (value > 0.0f)
					{
						// Update segment bounding box.
						maxFraction = value;
						glm::vec2 t = p1 + maxFraction * (p2 - p1);
						segmentAABB.lowerBound = glm::min(p1, t);
						segmentAABB.upperBound = glm::max(p1, t);
					}
				}
				else
				{
					stack.Push(node->child1);
					stack.Push(node->child2);
				}
			}
		}


	}
}