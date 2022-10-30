#include "Animation.h"

namespace god
{
	namespace Animation3D
	{
		Animation::Animation( const std::string& animationPath, Model* model )
		{
			Assimp::Importer importer;
			const aiScene* scene = importer.ReadFile( animationPath, aiProcess_Triangulate );
			//const aiScene* scene = importer.ReadFile( animationPath, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs );
			assert( scene );
			assert( scene->mRootNode );
			assert( scene->HasAnimations() );

			if ( scene )
			{
				std::cout << "This" << animationPath << "has anim" << scene->HasAnimations() << std::endl;
				auto animation = scene->mAnimations[0];
				m_Duration = animation->mDuration;
				m_TicksPerSecond = animation->mTicksPerSecond;
				aiMatrix4x4 globalTransformation = scene->mRootNode->mTransformation;
				globalTransformation = globalTransformation.Inverse();
				ReadHeirarchyData( m_RootNode, scene->mRootNode );
				ReadMissingBones( animation, *model );
			}
			else
			{
				std::cout << "This" << animationPath << "has no scene !" << std::endl;
			}
		}

		Bone* Animation::FindBone( const std::string& name )
		{
			auto iter = std::find_if( m_Bones.begin(), m_Bones.end(),
									  [&] ( const Bone& Bone )
									  {
										  return Bone.GetBoneName() == name;
									  }
			);
			if ( iter == m_Bones.end() ) return nullptr;
			else return &( *iter );
		}

		float Animation::GetTicksPerSecond()
		{
			return m_TicksPerSecond;
		}

		float Animation::GetDuration()
		{
			return m_Duration;
		}

		const AssimpNodeData& Animation::GetRootNode()
		{
			return m_RootNode;
		}

		const std::map<std::string, BoneInfo>& Animation::GetBoneIDMap()
		{
			return m_BoneInfoMap;
		}

		void Animation::ReadMissingBones( const aiAnimation* animation, Model& model )
		{
			int size = animation->mNumChannels;

			auto& boneInfoMap = model.GetBoneInfoMap();//getting m_BoneInfoMap from Model class
			int& boneCount = model.GetBoneCount(); //getting the m_BoneCounter from Model class

			//reading channels(bones engaged in an animation and their keyframes)
			for ( int i = 0; i < size; i++ )
			{
				auto channel = animation->mChannels[i];
				std::string boneName = channel->mNodeName.data;

				if ( boneInfoMap.find( boneName ) == boneInfoMap.end() )
				{
					boneInfoMap[boneName].id = boneCount;
					boneCount++;
				}
				m_Bones.push_back( Bone( channel->mNodeName.data,
								   boneInfoMap[channel->mNodeName.data].id, channel ) );
			}

			m_BoneInfoMap = boneInfoMap;
		}

		void Animation::ReadHeirarchyData( AssimpNodeData& dest, const aiNode* src )
		{
			assert( src );

			dest.name = src->mName.data;
			dest.transformation = AssimpGLMHelpers::ConvertMatrixToGLMFormat( src->mTransformation );
			dest.childrenCount = src->mNumChildren;

			for ( int i = 0; i < src->mNumChildren; i++ )
			{
				AssimpNodeData newData;
				ReadHeirarchyData( newData, src->mChildren[i] );
				dest.children.push_back( newData );
			}
		}
	}
}