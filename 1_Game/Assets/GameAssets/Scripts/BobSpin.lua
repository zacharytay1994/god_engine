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

--[IsSystem]
function S_BobSpin(e)
    local bob_spin = GetComponent(e, "C_BobSpin")
    local transform = GetTransform(e)
    local dt = GetDeltaTime()
    transform.rotation.y = transform.rotation.y + bob_spin.SpinRate * dt
    bob_spin.BobCounter = bob_spin.BobCounter + bob_spin.BobRate * dt
    transform.position.y = math.sin(bob_spin.BobCounter) / 2.0

    if (HasEngineComponent(e, "Transform")) then
        print("has transform")
    end

    if (HasScriptComponent(e, "C_BobSpin")) then
        print("has bobspin")
    end
end
