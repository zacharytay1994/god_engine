--[IsComponent]
function C_BrightnessDepth()
    local var = {
        --[SerializeFloat]
        max_depth = 1000,
        --[SerializeFloat]
        min_depth = -1000
    }
    return function()
        return var
    end
end

--[IsSystem]
function S_BrightnessDepth(e)
    local directionalLight = GetDirectionalLight(e);
    local camera_position = GetCameraPosition();
    local brightness_depth = GetComponent(e, "C_BrightnessDepth");
    -- print(camera_position.y);
    local val = math.max(0,math.min(1, (camera_position.y - brightness_depth.min_depth) / (brightness_depth.max_depth - brightness_depth.min_depth))) * 0.7;
    -- print((camera_position.y - brightness_depth.min_depth) / (brightness_depth.max_depth - brightness_depth.min_depth));
    -- local val = (camera_position.y - brightness_depth.min_depth) / (brightness_depth.max_depth - brightness_depth.min_depth);
    local dl_colour = directionalLight.colour;
    dl_colour.x = val;
    dl_colour.y = val;
    dl_colour.z = val;

    local dl_diffuse = directionalLight.diffuse;
    dl_diffuse.x = val;
    dl_diffuse.y = val;
    dl_diffuse.z = val;

    print(val);
end
