--[IsComponent]
function C_BGMPlaybackTracker()
	local var = {
	}
	return function()
		return var
	end
end

-- Global Variable for BGM
bgmTimeStamp = 0

--[IsSystem]
function S_BGMPlaybackTracker(e)
	local audio_source = GetAudioSource(e)
	
	bgmTimeStamp = audio_source.current_playtime
	-- print(bgmTimeStamp)
end