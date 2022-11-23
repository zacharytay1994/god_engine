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
	local audio_source = GetAudioSource(e)
	
	-- Press Q
	if CheckKeyPress(81) then
		audio_source.fade_in = true
	end
	
	-- Press W
	if CheckKeyPress(87) then
		audio_source.fade_out = true
	end
end