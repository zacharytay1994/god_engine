--[IsComponent]
function C_BGMPlayback()
	local var = {
	}
	return function()
		return var
	end
end

local updatedTime = false

--[IsSystem]
function S_BGMPlayback(e)
	local audio_source = GetAudioSource(e)
	
	if updatedTime == false then
		audio_source.update_playtime = true
		audio_source.new_playtime = bgmTimeStamp
		-- print(bgmTimeStamp)
		updatedTime = true
	end
end