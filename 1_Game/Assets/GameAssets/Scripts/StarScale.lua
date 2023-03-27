--[IsComponent]
function C_StarScale()
    local var = {
        --[SerializeFloat]
        ScaleToX = 0.1,
        --[SerializeFloat]
        ScaleSpeed = 1.0,
        --[SerializeInt]
        Next = -1,
        Start = false
    }
    return function()
        return var
    end
end

--[IsSystem]
function S_StarScale(e)
    local star_scale = GetComponent(e, "C_StarScale");
    local gui = GetGUIObject(e)
    local dt = GetDeltaTime()
    -- print(star_scale.Start)
    -- print(gui.size.x)
    if (gui.size.x < star_scale.ScaleToX and star_scale.Start) then
        gui.size.x = Lerp(gui.size.x, star_scale.ScaleToX, dt * star_scale.ScaleSpeed)
        if (gui.size.x > star_scale.ScaleToX - 0.03) then
            -- start star
            local star = GetEntity(star_scale.Next .. "star")
            if (star ~= -1) then
                local star1_scale = GetComponent(star, "C_StarScale")
                star1_scale.Start = true
            end
        end
    end
end
