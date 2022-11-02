--[IsComponent]
function C_BobSpin()
    local var = {
        BobRate = 2.0,
        BobCounter = 0.0,
        SpinRate = 100.0
    }
    return function()
        return var
    end
end

-- bobspin_global_variable = 0

-- function OnLoad_BobSpin(e)
--     bobspin_global_variable = 0
-- end

-- function FrameStart_BobSpin(e)
--     bobspin_global_variable = bobspin_global_variable + 1;
-- end

--[IsSystem]
function S_BobSpin(e)
    local bob_spin = GetComponent(e, "C_BobSpin")
    local transform = GetTransform(e)
    local dt = GetDeltaTime()
    transform.rotation.y = transform.rotation.y + bob_spin.SpinRate * dt
    bob_spin.BobCounter = bob_spin.BobCounter + bob_spin.BobRate * dt
    transform.position.y = math.sin(bob_spin.BobCounter) / 2.0
	
	if bob_spin.BobCounter > 1.0 then
		InstancePrefab("SFX_Jab",0,0,0)
		bob_spin.BobCounter = 0.0
	end
end

-- function FrameEnd_BobSpin(e)
--     print(bobspin_global_variable)
-- end

-- function OnLoad_Character(e)
--     print(e)
-- end
