#pragma once
#include "engine.h"
#include "../FMOD/include/fmod.hpp"

namespace engine
{
	class sound
	{
	protected:
		std::string         m_name{};
		FMOD::Sound *       m_sound{ nullptr };
		FMOD::Channel*      m_channel{ nullptr };
		engine::sound_type  m_type{};
	public:
		virtual ~sound()
		{
			// Channels belong to the FMOD system and are not released separately.
			SAFE_RELEASE(m_sound);
		}
		virtual bool play() = 0;

		engine::sound_type type() const { return m_type; }
		void set_type(engine::sound_type  type) { m_type = type; }
	};
}

