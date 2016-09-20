#pragma once
#include "Globals.hpp"

#include "../deps/assimp/include/scene.h"
#include "../deps/assimp/include/ai_assert.h"

#include "AnimEvaluator.h"

namespace Break
{
	namespace Infrastructure
	{

		// ---------------------------------------------------------------------------------
		/** A little tree structure to match the scene's node structure, but holding
		*  additional data. Needs to be public to allow using it in templates at
		*  certain compilers.
		*/
		struct BREAK_API SceneAnimNode
		{
			std::string mName;
			SceneAnimNode* mParent;
			std::vector<SceneAnimNode*> mChildren;

			//! most recently calculated local transform
			aiMatrix4x4 mLocalTransform;

			//! same, but in world space
			aiMatrix4x4 mGlobalTransform;

			//!  index in the current animation's channel array. -1 if not animated.
			size_t mChannelIndex;

			//! Default construction
			SceneAnimNode() {
				mChannelIndex = -1; mParent = NULL;
			}

			//! Construction from a given name
			SceneAnimNode( const std::string& pName)
				: mName( pName) {
					mChannelIndex = -1; mParent = NULL;
			}

			//! Destruct all children recursively
			~SceneAnimNode() {
				for( std::vector<SceneAnimNode*>::iterator it = mChildren.begin(); it != mChildren.end(); ++it)
					delete *it;
			}
		};

		// ---------------------------------------------------------------------------------
		/** Calculates the animated node transformations for a given scene and timestamp.
		*
		*  Create an instance for a aiScene you want to animate and set the current animation
		*  to play. You can then have the instance calculate the current pose for all nodes
		*  by calling Calculate() for a given timestamp. After this you can retrieve the
		*  present transformation for a given node by calling GetLocalTransform() or
		*  GetGlobalTransform(). A full set of bone matrices can be retrieved by
		*  GetBoneMatrices() for a given mesh.
		*/
		class BREAK_API SceneAnimator 
		{
		public:

			// ----------------------------------------------------------------------------
			/** Constructor for a given scene.
			*
			* The object keeps a reference to the scene during its lifetime, but
			* ownership stays at the caller.
			* @param pScene The scene to animate.
			* @param pAnimIndex [optional] Index of the animation to play. Assumed to
			*  be 0 if not given.
			*/
			SceneAnimator( const aiScene* pScene, size_t pAnimIndex = 0);

			/** Destructor */
			~SceneAnimator();

			// ----------------------------------------------------------------------------
			/** Sets the animation to use for playback. This also recreates the internal
			* mapping structures, which might take a few cycles.
			* @param pAnimIndex Index of the animation in the scene's animation array
			*/
			void SetAnimIndex( size_t pAnimIndex);

			// ----------------------------------------------------------------------------
			/** Calculates the node transformations for the scene. Call this to get
			* uptodate results before calling one of the getters.
			* @param pTime Current time. Can be an arbitrary range.
			*/
			void Calculate( double pTime);

			// ----------------------------------------------------------------------------
			/** Retrieves the most recent local transformation matrix for the given node.
			*
			* The returned matrix is in the node's parent's local space, just like the
			* original node's transformation matrix. If the node is not animated, the
			* node's original transformation is returned so that you can safely use or
			* assign it to the node itsself. If there is no node with the given name,
			* the identity matrix is returned. All transformations are updated whenever
			* Calculate() is called.
			* @param pNodeName Name of the node
			* @return A reference to the node's most recently calculated local
			*   transformation matrix.
			*/
			const aiMatrix4x4& GetLocalTransform( const aiNode* node) const;

			// ----------------------------------------------------------------------------
			/** Retrieves the most recent global transformation matrix for the given node.
			*
			* The returned matrix is in world space, which is the same coordinate space
			* as the transformation of the scene's root node. If the node is not animated,
			* the node's original transformation is returned so that you can safely use or
			* assign it to the node itsself. If there is no node with the given name, the
			* identity matrix is returned. All transformations are updated whenever
			* Calculate() is called.
			* @param pNodeName Name of the node
			* @return A reference to the node's most recently calculated global
			*   transformation matrix.
			*/
			const aiMatrix4x4& GetGlobalTransform( const aiNode* node) const;

			// ----------------------------------------------------------------------------
			/** Calculates the bone matrices for the given mesh.
			*
			* Each bone matrix transforms from mesh space in bind pose to mesh space in
			* skinned pose, it does not contain the mesh's world matrix. Thus the usual
			* matrix chain for using in the vertex shader is
			* @code
			* boneMatrix * worldMatrix * viewMatrix * projMatrix
			* @endcode
			* @param pNode The node carrying the mesh.
			* @param pMeshIndex Index of the mesh in the node's mesh array. The NODE's
			*   mesh array, not  the scene's mesh array! Leave out to use the first mesh
			*   of the node, which is usually also the only one.
			* @return A reference to a vector of bone matrices. Stays stable till the
			*   next call to GetBoneMatrices();
			*/
			const std::vector<aiMatrix4x4>& GetBoneMatrices( const aiNode* pNode,
				size_t pMeshIndex = 0);


			// ----------------------------------------------------------------------------
			/** @brief Get the current animation index
			*/
			size_t CurrentAnimIndex() const {
				return mCurrentAnimIndex;
			}

			// ----------------------------------------------------------------------------
			/** @brief Get the current animation or NULL
			*/
			aiAnimation* CurrentAnim() const {
				return  mCurrentAnimIndex < mScene->mNumAnimations ? mScene->mAnimations[ mCurrentAnimIndex ] : NULL;
			}

		protected:

			/** Recursively creates an internal node structure matching the
			*  current scene and animation.
			*/
			SceneAnimNode* CreateNodeTree( aiNode* pNode, SceneAnimNode* pParent);

			/** Recursively updates the internal node transformations from the
			*  given matrix array
			*/
			void UpdateTransforms( SceneAnimNode* pNode, const std::vector<aiMatrix4x4>& pTransforms);

			/** Calculates the global transformation matrix for the given internal node */
			void CalculateGlobalTransform( SceneAnimNode* pInternalNode);

		protected:
			/** The scene we're operating on */
			const aiScene* mScene;

			/** Current animation index */
			size_t mCurrentAnimIndex;

			/** The AnimEvaluator we use to calculate the current pose for the current animation */
			AnimEvaluator* mAnimEvaluator;

			/** Root node of the internal scene structure */
			SceneAnimNode* mRootNode;

			/** Name to node map to quickly find nodes by their name */
			typedef std::map<const aiNode*, SceneAnimNode*> NodeMap;
			NodeMap mNodesByName;

			/** Name to node map to quickly find nodes for given bones by their name */
			typedef std::map<const char*, const aiNode*> BoneMap;
			BoneMap mBoneNodesByName;

			/** Array to return transformations results inside. */
			std::vector<aiMatrix4x4> mTransforms;

			/** Identity matrix to return a reference to in case of error */
			aiMatrix4x4 mIdentityMatrix;
		};
	}
}


