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

    local entities = EntitiesWithComponent("Transform")
    for k=1, #entities do
        print(GetTransform(entities[k]).position.x)
    end
    if (HasComponent(e,"C_BobSpin")) then
        print(GetComponent(e,"C_BobSpin").BobCounter)
    end
end
