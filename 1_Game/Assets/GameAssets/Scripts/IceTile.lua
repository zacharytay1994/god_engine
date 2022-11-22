-- This script makes ice tiles melt when it's time is up.
-- Duration depends on the blue, pink or gold.
-- 

--[IsComponent]
function C_IceTile()
    local var = {

    }
    return function()
        return var
    end
end


--[IsSystem]
function S_IceTile(e)

end