-- Forecasts Squinky's attack.

--[IsComponent]
function C_EnemyForecastAttackSquinky()
    local var = {

        -- once movement forecast is done, EnemyController will tell this script to 
        -- perform an attack forecast by setting this boolean to true
        performAttackForecast = false,

        -- Vector3 which stores the direction of the Squinky's attack
        attackDirection = { },

        -- will draw a line (if player is in attack range), but only if
        -- EnemyForecast did not pause drawing of lines.
        drawLine = false
        
    }
    return function()
        return var
    end
end

--[IsSystem]
function S_EnemyForecastAttackSquinky(e)

    local forecastComponent = GetComponent(e, "C_EnemyForecastAttackSquinky")
    local enemyController = GetComponent(e, "C_EnemyController")

    -- if told to perform attack forecast by EnemyController, then set drawLine to true
    if (forecastComponent.performAttackForecast == true) then
            
        -- if forecastIndicator is adjacent to the Player, then forecast attack
        if (EnemyForecastAttackSquinkyAdjacent(e, enemyController.indicatorEntity, GetEntity("Player")) == true) then
            print("[EnemyForecastAttackSquinky.lua] Indicator is adjacent to Player, proceeding to forecast attack.")

            forecastComponent.drawLine = true
            enemyController.attackForecast = false
            forecastComponent.performAttackForecast = false
        
        -- else, enemy cannot attack player in the following turn, don't need to forecast anything
        else
            print("[EnemyForecastAttackSquinky.lua] Indicator is NOT adjacent to Player, nothing to forecast attack.")
            forecastComponent.drawLine = false
            enemyController.attackForecast = false
            forecastComponent.performAttackForecast = false
        end
    end

    -- if can drawLine and EnemyForecast has not paused the drawing of lines, then draw the line between the movement indicator and Player.    
    if (forecastComponent.drawLine and GetComponent(GetEntity("EnemyForecast"), "C_EnemyForecast").pauseDrawLine == false) then
        
        -- safety check
        if (enemyController.indicatorEntity ~= nil) then
            
            -- getting the positions required for drawing lines 
            local forecastIndicatorPosition = GetTransform(enemyController.indicatorEntity).position
            local playerPosition = GetTransform(GetEntity("Player")).position
            
            DrawLine(forecastIndicatorPosition.x, forecastIndicatorPosition.y, forecastIndicatorPosition.z, playerPosition.x, playerPosition.y, playerPosition.z, 1, 0, 0, 0, 10)
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