function CopyTable(src)
    local t2 = {}
    -- print(type(src))
    for k, v in pairs(src) do
        t2[k] = v
    end
    return t2
end
