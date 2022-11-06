--[IsComponent]
function C_SoundCheck()
	local var = {
		a = 0
	}
	return function()
		return var
	end
end

--[IsSystem]
function S_SoundCheck(e)
	local audio_source = GetAudioSource(e)
	
	print(audio_source.played)
	if (CheckLeftMousePress() == true)
	then
		print("pressed")
		audio_source.played = false
	end
	print(audio_source.played)
end