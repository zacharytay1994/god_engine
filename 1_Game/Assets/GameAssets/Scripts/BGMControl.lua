--[IsComponent]
function C_BGMFadeControl()
	local var = {
	}
	return function()
		return var
	end
end

--[IsSystem]
function S_BGMFadeControl(e)
	local level1BGM = 38
	local level1Combat = 36
	
	local audio_source = GetAudioSource(e)
	
	-- Press Q
	if CheckKeyPress(81) then 
		audio_source.sound_id = level1BGM
		audio_source.stop = true
		-- audio_source.fade_out = true
		-- audio_source.played = false
	end
	
	-- Press W
	if CheckKeyPress(87) then
		audio_source.sound_id = level1Combat
		audio_source.stop = true
		-- audio_source.fade_out = true
		-- audio_source.played = false
	end
end