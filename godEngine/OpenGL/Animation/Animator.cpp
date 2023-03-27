#include "Animator.h"

namespace god
{
	namespace Animation3D
	{
		Animator::Animator ( Animation* animation )
			:
			m_FinalBoneMatrices { 100 } ,
			m_CurrentAnimation { animation } ,
			m_CurrentTime { 0.0f } ,
			m_DeltaTime { 0.0f }
		{
			fill ( m_FinalBoneMatrices.begin () , m_FinalBoneMatrices.end () , glm::mat4 ( 0.0f ) );
		}

		void Animator::UpdateAnimation ( float dt , std::vector<std::vector<glm::mat4>> const& cachedTransforms )
		{
			m_DeltaTime = dt;
			if ( m_CurrentAnimation )
			{
				m_played = false;
				float og_current_time = m_CurrentTime;
				m_CurrentTime += m_CurrentAnimation->GetTicksPerSecond () * dt;
				//m_CurrentTime = fmod ( m_CurrentTime , m_CurrentAnimation->GetDuration () );
				if ( m_CurrentTime > m_endTime || m_CurrentTime < m_startTime )
				{
					if ( m_repeat )
					{
						m_CurrentTime = m_startTime;
					}
					else
					{
						m_CurrentTime = og_current_time;
					}
					m_played = true;
				}
				uint32_t frame = static_cast< uint32_t >( m_CurrentTime / m_CurrentAnimation->GetTicksPerSecond () * 60.0f );
				CalculateBoneTransform ( frame , &m_CurrentAnimation->GetRootNode () , cachedTransforms );
			}
		}

		void Animator::PlayAnimation ( Animation* pAnimation )
		{
			assert ( pAnimation != nullptr );
			m_CurrentAnimation = pAnimation;
			m_CurrentTime = 0.0f;
		}

		void Animator::CalculateBoneTransform ( uint32_t frame , const AssimpNodeData* node ,
			std::vector<std::vector<glm::mat4>> const& cachedTransforms )
		{
			auto boneInfoMap = m_CurrentAnimation->GetBoneIDMap ();
			if ( boneInfoMap.find ( node->name ) != boneInfoMap.end () )
			{
				int index = boneInfoMap[ node->name ].id;
				m_FinalBoneMatrices[ index ] = cachedTransforms[index][ frame ];
			}
			
			for ( int i = 0; i < node->childrenCount; i++ )
				CalculateBoneTransform ( frame , &node->children[ i ] , cachedTransforms );
		}

		std::vector<glm::mat4> Animator::GetFinalBoneMatrices ()
		{
			return m_FinalBoneMatrices;
		}
	}
}