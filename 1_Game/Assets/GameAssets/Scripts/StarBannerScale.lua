--[IsComponent]
function C_StarBannerScale()
    local var = {
        --[SerializeFloat]
        ScaleTo = 0.5,
        --[SerializeFloat]
        ScaleSpeed = 8.0,
        --[SerializeInt]
        Next = -1,
    }
    return function()
        return var
    end
end

--[IsSystem]
function S_StarBannerScale(e)
    local star_banner = GetComponent(e, "C_StarBannerScale")
    local gui_object = GetGUIObject(e)
    local dt = GetDeltaTime()
    if (gui_object.size.y < star_banner.ScaleTo - 0.01) then
        gui_object.size.y = Lerp(gui_object.size.y, star_banner.ScaleTo, dt * star_banner.ScaleSpeed)
    elseif (star_banner.Next ~= -1) then
        -- start star
        local star = GetEntity(star_banner.Next.."star")
        if (star ~= -1) then
            local star1_scale = GetComponent(star, "C_StarScale")
            star1_scale.Start = true
        end
    end
end
