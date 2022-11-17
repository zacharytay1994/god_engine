-- EnemyController will be attached to each enemy. 
-- What it does is it makes the enemy move and attack, and then ends its turn.

-- For EnemyController to work, every enemy must have the following components attached:
-- 1) C_EnemyController
-- 2) C_Character
-- 3) an enemy movement script (e.g. MoveDummee)
-- 4) an enemy attack script (e.g. EnemyDummee)

--[IsComponent]
function C_EnemyController()
    local var = {

        -- will be set to true by the movement script attached to this enemy
        hasMoved = false,
        
        -- will be set to true by the attack script attached to this enemy
        hasAttacked = false,

        -- -- can be added in the future, if needed
        -- hasPerformedSpecialBehaviour = false

        -- name of the enemy's movement script (e.g. MoveDummee)
        -- movementScriptName will be initialized by using the inspector
        --[SerializeString]
        movementScriptName = "",

        -- name of the enemy's movement script (e.g. EnemyDummee)
        -- attackScriptName will be initialized by using the inspector
        --[SerializeString]
        attackScriptName = ""

    }
    return function()
        return var
    end
end

--[IsSystem]
function S_EnemyController(e)

    local characterComponent = GetComponent(e, "C_Character")
    local enemyEntity = e

    local enemyController = GetComponent(e, "C_EnemyController")
    
    -- don't run the script if the enemy is not currently active
    if (characterComponent.isActive == false) then
        enemyController.hasMoved = false
        enemyController.hasAttacked = false
        return
    end

    -- -- enemy shouldn't do anything if afflicted with Frozen or Immobilised status
    -- if (characterComponent.statusAilment == "Frozen") then
    --     print("[EnemyController.lua]", EntityName(e), GetEntityData(e).id, "is Frozen. Ending turn.")
    --     characterComponent.endTurn = true
    --     return
    -- elseif (characterComponent.statusAilment == "Immobilised") then
    --     print("[EnemyController.lua]", EntityName(e), GetEntityData(e).id, "is Immobilised. Ending turn.")
    --     characterComponent.endTurn = true
    --     return
    -- end

    -- inform movementScript to start moving the enemy.
    -- once done, movementScript will set hasMoved to true.
    if (enemyController.hasMoved == false) then 
        local movementScript = GetComponent(enemyEntity, enemyController.movementScriptName)
        movementScript.executeMove = true        

    -- after enemy has moved, inform attackScript to try attacking the player.
    -- once done, attackScript will set hasAttacked to true.    
    elseif (enemyController.hasAttacked == false) then
        local attackScript = GetComponent(enemyEntity, enemyController.attackScriptName)
        attackScript.executeAttack = true

    -- after enemy has moved and attacked, reset variables and end turn
    else
        print("[EnemyController.lua] Enemy", EntityName(e), "has moved and attacked, ending turn.")
        enemyController.hasMoved = false
        enemyController.hasAttacked = false
        characterComponent.endTurn = true
    end
end