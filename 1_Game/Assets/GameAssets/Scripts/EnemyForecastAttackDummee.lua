-- Forecasts Dummee's attack.

--[IsComponent]
function C_EnemyForecastAttackDummee()
    local var = {

        performAttackForecast = false,

        -- Vector3 
        attackDirection = { }
        
    }
    return function()
        return var
    end
end

--[IsSystem]
function S_EnemyForecastAttackDummee(e)

    local forecastComponent = GetComponent(e, "C_EnemyForecastAttackDummee")
    local enemyController = GetComponent(e, "C_EnemyController")

    if (forecastComponent.performAttackForecast == true) then
            
        -- if forecastIndicator is in the same lane as the Player, then forecast attack
        if (EnemyForecastAttackDummeeSameLane(e, enemyController.indicatorEntity, GetEntity("Player")) == true) then
            print("[EnemyForecastAttackDummee.lua] Indicator in the same lane as Player, proceeding to forecast attack.")

            -- SHOW ATTACK FORECAST GRAPHIC HERE
            EnemyForecastAttackDummeeDrawArrow(e, enemyController.indicatorEntity, GetEntity("Player"))

            enemyController.attackForecast = false
            forecastComponent.performAttackForecast = false
        
        -- else, enemy cannot attack player in the following turn, don't need to forecast anything
        else
            print("[EnemyForecastAttackDummee.lua] Indicator NOT in the same lane as Player, nothing to forecast attack.")
            enemyController.attackForecast = false
            forecastComponent.performAttackForecast = false
        end
    end
end

function EnemyForecastAttackDummeeSameLane(e, forecastIndicator, player)
    
    -- init result
    local result = false

    -- get forecastIndicator and player locations
    local indicatorGrid = GetGridCell(forecastIndicator)
    local playerGrid = GetGridCell(player)

    if (indicatorGrid.y ~= playerGrid.y) then 
        print("[EnemyForecastAttackDummee.lua] Indicator not in same y-axis as player, nothing to forecast.")
        return result
    end
    
    -- for setting attackDirection
    local forecastComponent = GetComponent(e, "C_EnemyForecastAttackDummee")

    -- Dummee and player on the same x-axis
    if (indicatorGrid.x == playerGrid.x) then

        -- player is behind Dummee
        if (indicatorGrid.z > playerGrid.z) then 
            result = true 
            forecastComponent.attackDirection = { 0, 0, -1 }
            -- print("[EnemyAttackDummee.lua] Player is behind Dummee, dummeeRotation = 180.")

        -- player is in front of Dummee
        else
            result = true 
            forecastComponent.attackDirection = { 0, 0, 1 }
            -- print("[EnemyAttackDummee.lua] Player is in front of Dummee, dummeeRotation = 0.")
        end

    -- enemy and player on the same z-axis
    elseif (indicatorGrid.z == playerGrid.z) then

        -- player is to Dummee's right
        if (indicatorGrid.x > playerGrid.x) then
            result = true 
            forecastComponent.attackDirection = { 1, 0, 0 }  
            -- print("[EnemyAttackDummee.lua] Player is to Dummee's right, dummeeRotation = 270.")
        
        -- player is to Dummee's left
        else
            result = true 
            forecastComponent.attackDirection = { -1, 0, 0 }  
            -- print("[EnemyAttackDummee.lua] Player is to Dummee's left, dummeeRotation = 90.")
        end   
    end

    return result
end

-- draw a line connecting the forecast indicator and the player
function EnemyForecastAttackDummeeDrawArrow(e, forecastIndicator, player)

    -- calculate attackIndicator's position and scale
    local forecastIndicatorPosition = GetTransform(forecastIndicator).position
    local playerPosition = GetTransform(player).position
    local attackIndicatorPosition = { }
    attackIndicatorPosition[1] = (forecastIndicatorPosition.x + playerPosition.x) / 2
    attackIndicatorPosition[2] = (forecastIndicatorPosition.y + playerPosition.y) / 2
    attackIndicatorPosition[3] = (forecastIndicatorPosition.z + playerPosition.z) / 2
    -- print("[EnemyForecastAttackDummee.lua] Check attackIndicatorPosition:", attackIndicatorPosition[1], attackIndicatorPosition[2], attackIndicatorPosition[3])
    
    -- create an instance of an attackIndicator
    local attackIndicator = InstancePrefabNow("AttackIndicator", attackIndicatorPosition[1], attackIndicatorPosition[2], attackIndicatorPosition[3])

    -- add the instance to EnemyForecast's indicatorsList
    local enemyForecastEntity = GetEntity("EnemyForecast")
    if (enemyForecastEntity ~= -1) then
        local enemyForecastComponent = GetComponent(enemyForecastEntity, "C_EnemyForecast")
        enemyForecastComponent.indicatorsList[#enemyForecastComponent.indicatorsList + 1] = attackIndicator
    else
        print("[EnemyForecastAttackDummee.lua] ERROR: EnemyForecast entity cannot be found!")
    end

    -- scale the attackIndicator (a scale.x or scale.z of 1.0 is exactly the length of one tile)
    if (forecastIndicatorPosition.x - playerPosition.x ~= 0) then
        -- if forecastIndicator and player are on different x-axis then scale according to x-axis distance between each other
        GetTransform(attackIndicator).scale.x = (forecastIndicatorPosition.x - playerPosition.x) / 2
    else
        -- if forecastIndicator and player are on different z-axis then scale according to z-axis distance between each other
        GetTransform(attackIndicator).scale.z = (forecastIndicatorPosition.z - playerPosition.z) / 2
    end

end