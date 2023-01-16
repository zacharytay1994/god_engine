--[IsComponent]
function C_350TileDrop()
    local var = {
        --[SerializeFloat]
        DropSpeed = 5.0
    }
    return function()
        return var
    end
end

--[IsSystem]
function S_350TileDrop(e)
    -- move transform back to 0
    local C_350TileDrop = GetComponent(e, "C_350TileDrop")
    local transform = GetTransform(e)
    local dt = GetDeltaTime()
    transform.position = LerpVec3(transform.position.x, transform.position.y, transform.position.z, 0, 0, 0, dt*C_350TileDrop.DropSpeed)
end
