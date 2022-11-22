-- Forecasts Squinky's attack.

--[IsComponent]
function C_EnemyForecastAttackSquinky()
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
function S_EnemyForecastAttackSquinky(e)

    local forecastComponent = GetComponent(e, "C_EnemyForecastAttackSquinky")
    local enemyController = GetComponent(e, "C_EnemyController")

    if (forecastComponent.performAttackForecast == true) then
            
        -- if forecastIndicator is adjacent to the Player, then forecast attack
        if (EnemyForecastAttackSquinkyAdjacent(e, enemyController.indicatorEntity, GetEntity("Player")) == true) then
            print("[EnemyForecastAttackSquinky.lua] Indicator is adjacent to Player, proceeding to forecast attack.")

            -- SHOW ATTACK FORECAST GRAPHIC HERE
            EnemyForecastAttackDummeeDrawArrow(e, enemyController.indicatorEntity, GetEntity("Player"))

            enemyController.attackForecast = false
            forecastComponent.performAttackForecast = false
        
        -- else, enemy cannot attack player in the following turn, don't need to forecast anything
        else
            print("[EnemyForecastAttackSquinky.lua] Indicator is NOT adjacent to Player, nothing to forecast attack.")
            enemyController.attackForecast = false
            forecastComponent.performAttackForecast = false
        end
    end
end

function EnemyForecastAttackSquinkyAdjacent(e, forecastIndicator, player)

    -- init result
    local result = false

    -- get attacker and defenders' locations
    forecastIndicatorGrid = GetGridCell(forecastIndicator)
    playerGrid = GetGridCell(player)

    if (forecastIndicatorGrid.y == playerGrid.y) then 

        -- enemy behind player
        if ((forecastIndicatorGrid.x == playerGrid.x and forecastIndicatorGrid.z == playerGrid.z - 1) or
            (forecastIndicatorGrid.x == playerGrid.x and forecastIndicatorGrid.z == playerGrid.z + 1) or
            (forecastIndicatorGrid.z == playerGrid.z and forecastIndicatorGrid.x == playerGrid.x - 1) or
            (forecastIndicatorGrid.z == playerGrid.z and forecastIndicatorGrid.x == playerGrid.x + 1)) then
            result = true
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