--[IsComponent]
function C_ArcToParent()
    local var = {
        --[SerializeFloat]
        LerpSpeed = 1.0
    }
    return function()
        return var
    end
end

--[IsSystem]
function S_ArcToParent(e)
    local dt = GetDeltaTime()
    local transform = GetTransform(e)
    local arc_to_parent = GetComponent(e,"C_ArcToParent")
    local lerp_position = LerpVec3(transform.position.x, transform.position.y, transform.position.z, 0, 0, 0, dt * arc_to_parent.LerpSpeed);
    transform.position.x = lerp_position.x
    transform.position.y = lerp_position.y
    transform.position.z = lerp_position.z
end
