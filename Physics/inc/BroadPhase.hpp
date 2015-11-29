#pragma once
#include <algorithm>
#include "Globals.hpp"
#include "Collision.hpp"
#include "DynamicTree.hpp"

namespace Break
{

	namespace Physics
	{

		struct BREAK_API Pair
		{
			s32 proxyIdA;
			s32 proxyIdB;
		};

		/// The broad-phase is used for computing pairs and performing volume queries and ray casts.
		/// This broad-phase does not persist pairs. Instead, this reports potentially new pairs.
		/// It is up to the client to consume the new pairs and to track subsequent overlap.
		class BREAK_API BroadPhase
		{
		public:

			enum
			{
				nullProxy = -1
			};

			BroadPhase();
			~BroadPhase();

			/// Create a proxy with an initial AABB. Pairs are not reported until
			/// UpdatePairs is called.
			s32 CreateProxy(const AABB& aabb, void* userData);

			/// Destroy a proxy. It is up to the client to remove any pairs.
			void DestroyProxy(s32 proxyId);

			/// Call MoveProxy as many times as you like, then when you are done
			/// call UpdatePairs to finalized the proxy pairs (for your time step).
			void MoveProxy(s32 proxyId, const AABB& aabb, const glm::vec2& displacement);

			/// Call to trigger a re-processing of it's pairs on the next call to UpdatePairs.
			void TouchProxy(s32 proxyId);

			/// Get the fat AABB for a proxy.
			const AABB& GetFatAABB(s32 proxyId) const;

			/// Get user data from a proxy. Returns NULL if the id is invalid.
			void* GetUserData(s32 proxyId) const;

			/// Test overlap of fat AABBs.
			bool TestOverlap(s32 proxyIdA, s32 proxyIdB) const;

			/// Get the number of proxies.
			s32 GetProxyCount() const;

			/// Update the pairs. This results in pair callbacks. This can only add pairs.
			template <typename T>
			void UpdatePairs(T* callback);

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

			/// Get the height of the embedded tree.
			s32 GetTreeHeight() const;

			/// Get the balance of the embedded tree.
			s32 GetTreeBalance() const;

			/// Get the quality metric of the embedded tree.
			real32 GetTreeQuality() const;

			/// Shift the world origin. Useful for large worlds.
			/// The shift formula is: position -= newOrigin
			/// @param newOrigin the new origin with respect to the old origin
			void ShiftOrigin(const glm::vec2& newOrigin);

		private:

			friend class DynamicTree;

			void BufferMove(s32 proxyId);
			void UnBufferMove(s32 proxyId);

			bool QueryCallback(s32 proxyId);

			DynamicTree m_tree;

			s32 m_proxyCount;

			s32* m_moveBuffer;
			s32 m_moveCapacity;
			s32 m_moveCount;

			Pair* m_pairBuffer;
			s32 m_pairCapacity;
			s32 m_pairCount;

			s32 m_queryProxyId;
		};

		/// This is used to sort pairs.
		inline bool PairLessThan(const Pair& pair1, const Pair& pair2)
		{
			if (pair1.proxyIdA < pair2.proxyIdA)
			{
				return true;
			}

			if (pair1.proxyIdA == pair2.proxyIdA)
			{
				return pair1.proxyIdB < pair2.proxyIdB;
			}

			return false;
		}

		inline void* BroadPhase::GetUserData(s32 proxyId) const
		{
			return m_tree.GetUserData(proxyId);
		}

		inline bool BroadPhase::TestOverlap(s32 proxyIdA, s32 proxyIdB) const
		{
			const AABB& aabbA = m_tree.GetFatAABB(proxyIdA);
			const AABB& aabbB = m_tree.GetFatAABB(proxyIdB);
			return Physics::TestOverlap(aabbA, aabbB);
		}

		inline const AABB& BroadPhase::GetFatAABB(s32 proxyId) const
		{
			return m_tree.GetFatAABB(proxyId);
		}

		inline s32 BroadPhase::GetProxyCount() const
		{
			return m_proxyCount;
		}

		inline s32 BroadPhase::GetTreeHeight() const
		{
			return m_tree.GetHeight();
		}

		inline s32 BroadPhase::GetTreeBalance() const
		{
			return m_tree.GetMaxBalance();
		}

		inline real32 BroadPhase::GetTreeQuality() const
		{
			return m_tree.GetAreaRatio();
		}

		template <typename T>
		void BroadPhase::UpdatePairs(T* callback)
		{
			// Reset pair buffer
			m_pairCount = 0;

			// Perform tree queries for all moving proxies.
			for (s32 i = 0; i < m_moveCount; ++i)
			{
				m_queryProxyId = m_moveBuffer[i];
				if (m_queryProxyId == nullProxy)
				{
					continue;
				}

				// We have to query the tree with the fat AABB so that
				// we don't fail to create a pair that may touch later.
				const AABB& fatAABB = m_tree.GetFatAABB(m_queryProxyId);

				// Query tree, create pairs and add them pair buffer.
				m_tree.Query(this, fatAABB);
			}

			// Reset move buffer
			m_moveCount = 0;

			// Sort the pair buffer to expose duplicates.
			std::sort(m_pairBuffer, m_pairBuffer + m_pairCount, PairLessThan);

			// Send the pairs back to the client.
			s32 i = 0;
			while (i < m_pairCount)
			{
				Pair* primaryPair = m_pairBuffer + i;
				void* userDataA = m_tree.GetUserData(primaryPair->proxyIdA);
				void* userDataB = m_tree.GetUserData(primaryPair->proxyIdB);

				callback->AddPair(userDataA, userDataB);
				++i;

				// Skip any duplicate pairs.
				while (i < m_pairCount)
				{
					Pair* pair = m_pairBuffer + i;
					if (pair->proxyIdA != primaryPair->proxyIdA || pair->proxyIdB != primaryPair->proxyIdB)
					{
						break;
					}
					++i;
				}
			}

			// Try to keep the tree balanced.
			//m_tree.Rebalance(4);
		}

		template <typename T>
		inline void BroadPhase::Query(T* callback, const AABB& aabb) const
		{
			m_tree.Query(callback, aabb);
		}

		template <typename T>
		inline void BroadPhase::RayCast(T* callback, const RayCastInput& input) const
		{
			m_tree.RayCast(callback, input);
		}

		inline void BroadPhase::ShiftOrigin(const glm::vec2& newOrigin)
		{
			m_tree.ShiftOrigin(newOrigin);
		}



	}

}