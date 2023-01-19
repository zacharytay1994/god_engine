-- Forecasts Dummee's attack.

--[IsComponent]
function C_EnemyForecastAttackDummee()
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
function S_EnemyForecastAttackDummee(e)

    local forecastComponent = GetComponent(e, "C_EnemyForecastAttackDummee")
    local enemyController = GetComponent(e, "C_EnemyController")

    -- check if enemy is dead or alive
    if (GetComponent(e, "C_Character").isDead) then
        return
    end

    -- if told to perform attack forecast by EnemyController, then set drawLine to true
    if (forecastComponent.performAttackForecast == true) then
            
        -- if forecastIndicator is in the same lane as the Player, then forecast attack
        if (EnemyForecastAttackDummeeSameLane(e, enemyController.indicatorEntity, GetEntity("Player")) == true) then
            print("[EnemyForecastAttackDummee.lua] Indicator in the same lane as Player, proceeding to forecast attack.")

            forecastComponent.drawLine = true
            enemyController.attackForecast = false
            forecastComponent.performAttackForecast = false
        
        -- else, enemy cannot attack player in the following turn, don't need to forecast anything
        else
            print("[EnemyForecastAttackDummee.lua] Indicator NOT in the same lane as Player, nothing to forecast attack.")
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