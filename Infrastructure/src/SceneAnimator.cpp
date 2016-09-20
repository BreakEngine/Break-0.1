#include "SceneAnimator.h"

using namespace Break::Infrastructure;


// ------------------------------------------------------------------------------------------------
// Constructor for a given scene.
SceneAnimator::SceneAnimator( const aiScene* pScene, size_t pAnimIndex)
{
	mScene = pScene;
	mCurrentAnimIndex = -1;
	mAnimEvaluator = NULL;
	mRootNode = NULL;

	// build the nodes-for-bones table
	for (unsigned int i = 0; i < pScene->mNumMeshes;++i)
	{
		const aiMesh* mesh = pScene->mMeshes[i];
		for (unsigned int n = 0; n < mesh->mNumBones;++n)
		{
			const aiBone* bone = mesh->mBones[n]; 

			mBoneNodesByName[bone->mName.data] = pScene->mRootNode->FindNode(bone->mName);
		}
	}

	// changing the current animation also creates the node tree for this animation
	SetAnimIndex( pAnimIndex);
}

// ------------------------------------------------------------------------------------------------
// Destructor
SceneAnimator::~SceneAnimator()
{
	delete mRootNode;
	delete mAnimEvaluator;
}

// ------------------------------------------------------------------------------------------------
// Sets the animation to use for playback.
void SceneAnimator::SetAnimIndex( size_t pAnimIndex)
{
	// no change
	if( pAnimIndex == mCurrentAnimIndex)
		return;

	// kill data of the previous anim
	delete mRootNode;  mRootNode = NULL;
	delete mAnimEvaluator;  mAnimEvaluator = NULL;
	mNodesByName.clear();

	mCurrentAnimIndex = pAnimIndex;

	// create the internal node tree. Do this even in case of invalid animation index
	// so that the transformation matrices are properly set up to mimic the current scene
	mRootNode = CreateNodeTree( mScene->mRootNode, NULL);

	// invalid anim index
	if( mCurrentAnimIndex >= mScene->mNumAnimations)
		return;

	// create an evaluator for this animation
	mAnimEvaluator = new AnimEvaluator( mScene->mAnimations[mCurrentAnimIndex]);
}

// ------------------------------------------------------------------------------------------------
// Calculates the node transformations for the scene.
void SceneAnimator::Calculate( double pTime)
{
	// invalid anim
	if( !mAnimEvaluator)
		return;

	// calculate current local transformations
	mAnimEvaluator->Evaluate( pTime);

	// and update all node transformations with the results
	UpdateTransforms( mRootNode, mAnimEvaluator->GetTransformations());
}

// ------------------------------------------------------------------------------------------------
// Retrieves the most recent local transformation matrix for the given node.
const aiMatrix4x4& SceneAnimator::GetLocalTransform( const aiNode* node) const
{
	NodeMap::const_iterator it = mNodesByName.find( node);
	if( it == mNodesByName.end())
		return mIdentityMatrix;

	return it->second->mLocalTransform;
}

// ------------------------------------------------------------------------------------------------
// Retrieves the most recent global transformation matrix for the given node.
const aiMatrix4x4& SceneAnimator::GetGlobalTransform( const aiNode* node) const
{
	NodeMap::const_iterator it = mNodesByName.find( node);
	if( it == mNodesByName.end())
		return mIdentityMatrix;

	return it->second->mGlobalTransform;
}

// ------------------------------------------------------------------------------------------------
// Calculates the bone matrices for the given mesh.
const std::vector<aiMatrix4x4>& SceneAnimator::GetBoneMatrices( const aiNode* pNode, size_t pMeshIndex /* = 0 */)
{
	ai_assert( pMeshIndex < pNode->mNumMeshes);
	size_t meshIndex = pNode->mMeshes[pMeshIndex];
	ai_assert( meshIndex < mScene->mNumMeshes);
	const aiMesh* mesh = mScene->mMeshes[meshIndex];

	// resize array and initialise it with identity matrices
	mTransforms.resize( mesh->mNumBones, aiMatrix4x4());

	// calculate the mesh's inverse global transform
	aiMatrix4x4 globalInverseMeshTransform = GetGlobalTransform( pNode);
	globalInverseMeshTransform.Inverse();

	// Bone matrices transform from mesh coordinates in bind pose to mesh coordinates in skinned pose
	// Therefore the formula is offsetMatrix * currentGlobalTransform * inverseCurrentMeshTransform
	for( size_t a = 0; a < mesh->mNumBones; ++a)
	{
		const aiBone* bone = mesh->mBones[a];
		const aiMatrix4x4& currentGlobalTransform = GetGlobalTransform( mBoneNodesByName[ bone->mName.data ]);
		mTransforms[a] = globalInverseMeshTransform * currentGlobalTransform * bone->mOffsetMatrix;
	}

	// and return the result
	return mTransforms;
}

// ------------------------------------------------------------------------------------------------
// Recursively creates an internal node structure matching the current scene and animation.
SceneAnimNode* SceneAnimator::CreateNodeTree( aiNode* pNode, SceneAnimNode* pParent)
{
	// create a node
	SceneAnimNode* internalNode = new SceneAnimNode( pNode->mName.data);
	internalNode->mParent = pParent;
	mNodesByName[pNode] = internalNode;

	// copy its transformation
	internalNode->mLocalTransform = pNode->mTransformation;
	CalculateGlobalTransform( internalNode);

	// find the index of the animation track affecting this node, if any
	if( mCurrentAnimIndex < mScene->mNumAnimations)
	{
		internalNode->mChannelIndex = -1;
		const aiAnimation* currentAnim = mScene->mAnimations[mCurrentAnimIndex];
		for( unsigned int a = 0; a < currentAnim->mNumChannels; a++)
		{
			if( currentAnim->mChannels[a]->mNodeName.data == internalNode->mName)
			{
				internalNode->mChannelIndex = a;
				break;
			}
		}
	}

	// continue for all child nodes and assign the created internal nodes as our children
	for( unsigned int a = 0; a < pNode->mNumChildren; a++)
	{
		SceneAnimNode* childNode = CreateNodeTree( pNode->mChildren[a], internalNode);
		internalNode->mChildren.push_back( childNode);
	}

	return internalNode;
}

// ------------------------------------------------------------------------------------------------
// Recursively updates the internal node transformations from the given matrix array
void SceneAnimator::UpdateTransforms( SceneAnimNode* pNode, const std::vector<aiMatrix4x4>& pTransforms)
{
	// update node local transform
	if( pNode->mChannelIndex != -1)
	{
		ai_assert( pNode->mChannelIndex < pTransforms.size());
		pNode->mLocalTransform = pTransforms[pNode->mChannelIndex];
	}

	// update global transform as well
	CalculateGlobalTransform( pNode);

	// continue for all children
	for( std::vector<SceneAnimNode*>::iterator it = pNode->mChildren.begin(); it != pNode->mChildren.end(); ++it)
		UpdateTransforms( *it, pTransforms);
}

// ------------------------------------------------------------------------------------------------
// Calculates the global transformation matrix for the given internal node
void SceneAnimator::CalculateGlobalTransform( SceneAnimNode* pInternalNode)
{
	// concatenate all parent transforms to get the global transform for this node
	pInternalNode->mGlobalTransform = pInternalNode->mLocalTransform;
	SceneAnimNode* node = pInternalNode->mParent;
	while( node)
	{
		pInternalNode->mGlobalTransform = node->mLocalTransform * pInternalNode->mGlobalTransform;
		node = node->mParent;
	}
}

